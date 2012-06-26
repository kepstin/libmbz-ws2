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

#include "mbz-ws2-ratelimit.h"

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
