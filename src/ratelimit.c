/*
 * libmbz-ws2: Provide a GObject interface to the Musicbrainz webservice.
 * Copyright © 2012 Calvin Walton <calvin.walton@kepstin.ca>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "ratelimit.h"

G_DEFINE_TYPE(MbzWs2Ratelimit, mbz_ws2_ratelimit, G_TYPE_OBJECT)

struct _MbzWs2RatelimitPrivate {
	GQueue in_queue;
	GQueue out_queue;
	guint period;
	guint burst;
	guint burst_period;
	gboolean active;
};

#define MBZ_WS2_RATELIMIT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), MBZ_WS2_TYPE_RATELIMIT, MbzWs2RatelimitPrivate))

typedef struct _RatelimitCallback RatelimitCallback;

struct _RatelimitCallback {
	MbzWs2RatelimitCallback function;
	gpointer user_data;
};

enum {
	RATELIMIT_PERIOD = 1,
	RATELIMIT_BURST,
	RATELIMIT_N_PROPERTIES,
};

static GParamSpec *ratelimit_properties[RATELIMIT_N_PROPERTIES] = { NULL };

static void ratelimit_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	MbzWs2Ratelimit *self = MBZ_WS2_RATELIMIT(object);
	
	switch (property_id) {
	case RATELIMIT_PERIOD:
		self->priv->period = g_value_get_uint(value);
		self->priv->burst_period = self->priv->period * self->priv->burst;
		g_debug("MbzWs2Ratelimit:period:%u - burst_period:%u\n", self->priv->period, self->priv->burst_period);
		break;
	case RATELIMIT_BURST:
		self->priv->burst = g_value_get_uint(value);
		self->priv->burst_period = self->priv->period * self->priv->burst;
		g_debug("MbzWs2Ratelimit:burst:%u - burst_period:%u\n", self->priv->period, self->priv->burst_period);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}
}

static void ratelimit_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	MbzWs2Ratelimit *self = MBZ_WS2_RATELIMIT(object);
	
	switch (property_id) {
	case RATELIMIT_PERIOD:
		g_value_set_uint(value, self->priv->period);
		break;
	case RATELIMIT_BURST:
		g_value_set_uint(value, self->priv->burst);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}
}

static void mbz_ws2_ratelimit_class_init(MbzWs2RatelimitClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
	
	gobject_class->set_property = ratelimit_set_property;
	gobject_class->get_property = ratelimit_get_property;
	
	/**
	 * MBWs2Ratelimit:period:
	 *
	 * The period between ratelimited requests, in milliseconds. The
	 * default is 1100 (1000 plus a fudge factor).
	 */
	ratelimit_properties[RATELIMIT_PERIOD] = g_param_spec_uint(
		"period",
		"Period",
		"Time between requests (ms)",
		0,		/* minimum */
		G_MAXUINT,	/* maximum */
		1100,		/* default */
		G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
	
	/**
	 * MBWs2Ratelimit:burst:
	 *
	 * The number of requests in a row to allow before ratelimiting kicks
	 * in.
	 */
	ratelimit_properties[RATELIMIT_BURST] = g_param_spec_uint(
		"burst",
		"Burst",
		"Number of requests before ratelimiting is enabled",
		1,		/* minimum */
		G_MAXUINT,	/* maximum */
		10,		/* default */
		G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
	
	g_object_class_install_properties(gobject_class, RATELIMIT_N_PROPERTIES, ratelimit_properties);
	
	g_type_class_add_private(klass, sizeof (MbzWs2RatelimitPrivate));
}

static void mbz_ws2_ratelimit_init(MbzWs2Ratelimit *self)
{
	self->priv = MBZ_WS2_RATELIMIT_GET_PRIVATE(self);
	
	g_queue_init(&self->priv->in_queue);
	g_queue_init(&self->priv->out_queue);
}

MbzWs2Ratelimit *mbz_ws2_ratelimit_new(void)
{
	MbzWs2Ratelimit *rl;
	
	rl = g_object_new(MBZ_WS2_TYPE_RATELIMIT, NULL);
	
	return rl;
}

MbzWs2Ratelimit *mbz_ws2_ratelimit_new_custom(guint period, guint burst)
{
	MbzWs2Ratelimit *rl;
	
	rl = g_object_new(MBZ_WS2_TYPE_RATELIMIT,
		"period", period,
		"burst", burst,
		NULL);
	
	return rl;
}

guint mbz_ws2_ratelimit_get_period(MbzWs2Ratelimit *self)
{
	return self->priv->period;
}

guint mbz_ws2_ratelimit_get_burst(MbzWs2Ratelimit *self)
{
	return self->priv->burst;
}

static gboolean ratelimit_run_immediate(gpointer data) {
	RatelimitCallback *rc = data;
	
	rc->function(rc->user_data);
	
	g_free(rc);
	
	return FALSE;
}

static void ratelimit_start_timer(MbzWs2Ratelimit *self);

static gboolean ratelimit_next(gpointer user_data)
{
	MbzWs2Ratelimit *self = user_data;
	
	g_debug("Timer tick\n");
	
	/* If the oldest element in out_queue is more than period * burst old,
	 * remove it. */
	if (g_queue_get_length(&self->priv->out_queue) > 0) {
		if (GPOINTER_TO_UINT(g_queue_peek_head(&self->priv->out_queue))
					+ self->priv->burst_period
				<= g_get_monotonic_time()/1000) {
			g_debug("Dropping old request from out_queue\n");
			g_queue_pop_head(&self->priv->out_queue);
		}
	}
	
	/* Do more requests if space is available in out_queue */
	while (g_queue_get_length(&self->priv->out_queue) < self->priv->burst
			&& g_queue_get_length(&self->priv->in_queue) > 0) {
		g_debug("Performing queued request\n");
		RatelimitCallback *rc = g_queue_pop_head(&self->priv->in_queue);
		
		rc->function(rc->user_data);
		
		g_free(rc);
	}
	
	g_debug("Out Queue: %u, In Queue: %u\n",
		g_queue_get_length(&self->priv->out_queue),
		g_queue_get_length(&self->priv->in_queue));
	
	/* Queue the next timer tick if out_queue isn't empty */
	if (g_queue_get_length(&self->priv->out_queue) > 0) {
		ratelimit_start_timer(self);
	} else {
		g_debug("Stopping timer\n");
		self->priv->active = FALSE;
	}
	
	return FALSE;
}

static void ratelimit_start_timer(MbzWs2Ratelimit *self)
{
	self->priv->active = TRUE;
	guint delay = MAX(
		self->priv->period,
		self->priv->burst_period -
			(g_get_monotonic_time()/1000 -
			GPOINTER_TO_UINT(g_queue_peek_head(&self->priv->out_queue))));
	
	g_timeout_add(delay, ratelimit_next, self);
}

void mbz_ws2_ratelimit_queue(
	MbzWs2Ratelimit *self,
	MbzWs2RatelimitCallback function,
	gpointer user_data)
{
	RatelimitCallback *rc = g_new(RatelimitCallback, 1);
	rc->function = function;
	rc->user_data = user_data;
	
	/* If there is space in the out_queue, do the request immediately. */
	if (g_queue_get_length(&self->priv->out_queue) < self->priv->burst) {
		g_debug("Performing immediate request\n");
		g_main_context_invoke(NULL, ratelimit_run_immediate, rc);
		g_queue_push_tail(&self->priv->out_queue, GUINT_TO_POINTER(g_get_monotonic_time()/1000));
	} else {
		g_debug("Queueing request\n");
		g_queue_push_tail(&self->priv->in_queue, rc);
	}
	
	g_debug("Out Queue: %u, In Queue: %u\n",
		g_queue_get_length(&self->priv->out_queue),
		g_queue_get_length(&self->priv->in_queue));
	
	/* Start the timer if it wasn't previously running. */
	if (!self->priv->active) {
		g_debug("Starting timer\n");
		ratelimit_start_timer(self);
	}
}
