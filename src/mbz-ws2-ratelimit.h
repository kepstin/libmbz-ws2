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

#ifndef MBZ_WS2_RATELIMIT_H
#define MBZ_WS2_RATELIMIT_H

#include <glib-object.h>

#define MBZ_WS2_TYPE_RATELIMIT			(mbz_ws2_ratelimit_get_type())
#define MBZ_WS2_RATELIMIT(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), MBZ_WS2_TYPE_RATELIMIT, MbzWs2Ratelimit))
#define MBZ_WS2_IS_RATELIMIT(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), MBZ_WS2_TYPE_RATELIMIT))
#define MBZ_WS2_RATELIMIT_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass), MBZ_WS2_TYPE_RATELIMIT, MbzWs2RatelimitClass))
#define MBZ_WS2_IS_RATELIMIT_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), MBZ_WS2_TYPE_RATELIMIT))
#define MBZ_WS2_RATELIMIT_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), MBZ_WS2_TYPE_RATELIMIT, MbzWs2RatelimitClass))

typedef struct _MbzWs2Ratelimit		MbzWs2Ratelimit;
typedef struct _MbzWs2RatelimitClass	MbzWs2RatelimitClass;

typedef struct _MbzWs2RatelimitPrivate	MbzWs2RatelimitPrivate;

struct _MbzWs2Ratelimit {
	GObject parent_instance;
	
	MbzWs2RatelimitPrivate *priv;
};

struct _MbzWs2RatelimitClass {
	GObjectClass parent_class;
};

GType mbz_ws2_ratelimit_get_type(void);

/**
 * mbz_ws2_ratelimit_new:
 *
 * Create a ratelimiter using the default parameters.
 *
 * Returns: (transfer full): The ratelimiter object, which must be dereferenced
 * when you are finished with it.
 * Since: 1.0
 */
MbzWs2Ratelimit *mbz_ws2_ratelimit_new(void);

/**
 * mbz_ws2_ratelimit_new_custom:
 * @period: The period between ratelimited requests, in milliseconds.
 * @burst: The number of requests in a row to allow before ratelimiting kicks in.
 *
 * Create a ratelimiter using non-standard options.
 *
 * Returns: (transfer full): The ratelimiter object, which must be dereferenced
 * when you are finished with it.
 * Since: 1.0
 */
MbzWs2Ratelimit *mbz_ws2_ratelimit_new_custom(guint period, guint burst);

/**
 * mbz_ws2_ratelimit_get_period:
 *
 * Returns: The period between ratelimited requests, in milliseconds.
 * Since: 1.0
 */
guint mbz_ws2_ratelimit_get_period(MbzWs2Ratelimit *self);

/**
 * mbz_ws2_ratelimit_get_burst:
 *
 * Returns: The number of requests in a row before ratelimiting kicks in.
 * Since: 1.0
 */
guint mbz_ws2_ratelimit_get_burst(MbzWs2Ratelimit *self);
#endif /* MBZ_WS2_RATELIMIT_H */
