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

#ifndef MBZ_WS2_CONNECTION_H
#define MBZ_WS2_CONNECTION_H

#include <glib-object.h>

#define MBZ_WS2_TYPE_CONNECTION			(mbz_ws2_connection_get_type())
#define MBZ_WS2_CONNECTION(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), MBZ_WS2_TYPE_CONNECTION, MbzWs2Connection))
#define MBZ_WS2_IS_CONNECTION(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), MBZ_WS2_TYPE_CONNECTION))
#define MBZ_WS2_CONNECTION_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass), MBZ_WS2_TYPE_CONNECTION, MbzWs2ConnectionClass))
#define MBZ_WS2_IS_CONNECTION_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), MBZ_WS2_TYPE_CONNECTION))
#define MBZ_WS2_CONNECTION_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), MBZ_WS2_TYPE_CONNECTION, MbzWs2ConnectionClass))

typedef struct _MbzWs2Connection	MbzWs2Connection;
typedef struct _MbzWs2ConnectionClass	MbzWs2ConnectionClass;

typedef struct _MbzWs2ConnectionPrivate	MbzWs2ConnectionPrivate;

struct _MbzWs2Connection {
	GObject parent_instance;
	
	MbzWs2ConnectionPrivate *priv;
};

struct _MbzWs2ConnectionClass {
	GObjectClass parent_class;
};

GType mbz_ws2_connection_get_type(void);

/**
 * mbz_ws2_connection_new:
 * @user_agent: The HTTP user agent to identify your application as. See
 * http://wiki.musicbrainz.org/XML_Web_Service/Rate_Limiting for details.
 *
 * Create a new webservice connection using the default settings, which is:
 * - Connect to http://musicbrainz.org/ws/2
 * - Use a ratelimiter configured for 1 request per second.
 *
 * Returns: (transfer full): The connection object, which must be dereferenced
 * when you are finished with it.
 * Since: 1.0
 */
MbzWs2Connection *mbz_ws2_connection_new(const gchar *user_agent);

/**
 * mbz_ws2_connection_new_server:
 * @user_agent: The HTTP user agent to identify your application as.
 * @base_uri: The base URI for the Musicbrainz webservice (version 2).
 * @ratelimit_period: The time between successive webservice requests (in ms).
 * Use 0 to disable the ratelimiter completely.
 *
 * Create a new webservice connection to a non-standard server. The primary
 * purpose of this function is to allow connections to a local Musicbrainz
 * mirror server that mas ratelimiting disabled.
 *
 * Returns: (transfer full): The connection object, which must be dereferenced
 * when you are finished with it.
 * Since: 1.0
 */
MbzWs2Connection *mbz_ws2_connection_new_server(
	const gchar *user_agent,
	const gchar *base_uri,
	guint ratelimit_period);

#endif /* MBZ_WS2_CONNECTION_H */
