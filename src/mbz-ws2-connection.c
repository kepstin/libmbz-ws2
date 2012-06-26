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

#include "mbz-ws2-connection.h"

#include "mbz-ws2-ratelimit.h"

#include <libsoup/soup.h>

G_DEFINE_TYPE(MbzWs2Connection, mbz_ws2_connection, G_TYPE_OBJECT)

struct _MbzWs2ConnectionPrivate {
	gchar *user_agent;
	gchar *endpoint_uri_string;
	SoupURI *base_uri;
	MbzWs2Ratelimit *ratelimiter;
	gchar *username;
	gchar *password;
	guint ratelimit_period;
};

#define MBZ_WS2_CONNECTION_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), MBZ_WS2_TYPE_CONNECTION, MbzWs2ConnectionPrivate))

enum {
	CONNECTION_USER_AGENT = 1,
	CONNECTION_ENDPOINT_URI,
	CONNECTION_RATELIMIT_PERIOD,
	CONNECTION_RATELIMITER,
	CONNECTION_USERNAME,
	CONNECTION_PASSWORD,
	CONNECTION_N_PROPERTIES,
};

static GParamSpec *connection_properties[CONNECTION_N_PROPERTIES] = { NULL };

static void connection_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	MbzWs2Connection *self = MBZ_WS2_CONNECTION(object);
	
	switch (property_id) {
	case CONNECTION_USER_AGENT:
		g_free(self->priv->user_agent);
		self->priv->user_agent = g_value_dup_string(value);
		g_debug("MbzWs2Connection:user-agent:%s\n", self->priv->user_agent);
		break;
	case CONNECTION_ENDPOINT_URI:
		g_free(self->priv->endpoint_uri_string);
		self->priv->endpoint_uri_string = g_value_dup_string(value);
		g_debug("MbzWs2Connection:endpoint-uri:%s\n", self->priv->endpoint_uri_string);
		break;
	case CONNECTION_RATELIMIT_PERIOD:
		self->priv->ratelimit_period = g_value_get_uint(value);
		g_debug("MbzWs2Connection:ratelimit-period:%u\n", self->priv->ratelimit_period);
		break;
	case CONNECTION_USERNAME:
		g_free(self->priv->username);
		self->priv->username = g_value_dup_string(value);
		g_debug("MbzWs2Connection:username:%s\n", self->priv->username);
		break;
	case CONNECTION_PASSWORD:
		g_free(self->priv->password);
		self->priv->password = g_value_dup_string(value);
		g_debug("MbzWs2Connection:password:%s\n", self->priv->username);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}
}

static void connection_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	MbzWs2Connection *self = MBZ_WS2_CONNECTION(object);
	
	switch (property_id) {
	case CONNECTION_USER_AGENT:
		g_value_set_string(value, self->priv->user_agent);
		break;
	case CONNECTION_ENDPOINT_URI:
		g_value_set_string(value, self->priv->endpoint_uri_string);
		break;
	case CONNECTION_RATELIMIT_PERIOD:
		g_value_set_uint(value, self->priv->ratelimit_period);
		break;
	case CONNECTION_RATELIMITER:
		g_value_set_object(value, self->priv->ratelimiter);
		break;
	case CONNECTION_USERNAME:
		g_value_set_string(value, self->priv->username);
		break;
	case CONNECTION_PASSWORD:
		g_value_set_string(value, self->priv->password);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}
}

static void connection_constructed(GObject *object)
{
	MbzWs2Connection *self = MBZ_WS2_CONNECTION(object);
	
	if (self->priv->user_agent == NULL)
		g_error("User agent string was not set\n");
	
	G_OBJECT_CLASS(mbz_ws2_connection_parent_class)->constructed(object);
	
	self->priv->base_uri = soup_uri_new(self->priv->endpoint_uri_string);

	self->priv->ratelimiter = g_object_new(MBZ_WS2_TYPE_RATELIMIT,
		"period", self->priv->ratelimit_period,
		NULL);
}

static void connection_dispose(GObject *object)
{
	MbzWs2Connection *self = MBZ_WS2_CONNECTION(object);
	
	g_object_unref(self->priv->ratelimiter);
	self->priv->ratelimiter = NULL;
	
	G_OBJECT_CLASS(mbz_ws2_connection_parent_class)->dispose(object);
}

static void connection_finalize(GObject *object)
{
	MbzWs2Connection *self = MBZ_WS2_CONNECTION(object);
	
	g_free(self->priv->user_agent);
	g_free(self->priv->endpoint_uri_string);
	g_free(self->priv->username);
	g_free(self->priv->password);

	if (self->priv->base_uri != NULL) {
		soup_uri_free(self->priv->base_uri);
		self->priv->base_uri = NULL;
	}
	
	G_OBJECT_CLASS(mbz_ws2_connection_parent_class)->finalize(object);
}

static void mbz_ws2_connection_class_init(MbzWs2ConnectionClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
	
	g_type_class_add_private(klass, sizeof (MbzWs2ConnectionPrivate));
	
	gobject_class->set_property = connection_set_property;
	gobject_class->get_property = connection_get_property;
	gobject_class->constructed = connection_constructed;
	gobject_class->dispose = connection_dispose;
	gobject_class->finalize = connection_finalize;
	
	/**
	 * MbzWs2Connection:user-agent:
	 *
	 * The HTTP user agent to identify your application as. See
	 * http://wiki.musicbrainz.org/XML_Web_Service/Rate_Limiting for
	 * details.
	 */
	connection_properties[CONNECTION_USER_AGENT] = g_param_spec_string(
		"user-agent",
		"User Agent",
		"The HTTP user agent to identify your application as",
		NULL, /* default */
		G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
	
	/**
	 * MbzWs2Connection:endpoint-uri:
	 *
	 * The endpoint URI for the Musicbrainz webservice. By default, this
	 * will access the official webservice at http://musicbrainz.org/ws/2,
	 * but it can be overridden to allow using a mirror server.
	 */
	connection_properties[CONNECTION_ENDPOINT_URI] = g_param_spec_string(
		"endpoint-uri",
		"Endpoint URI",
		"The endpoint URI for the Musicbrainz webservice (version 2)",
		"http://musicbrainz.org/ws/2", /* default */
		G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
	
	/**
	 * MbzWs2Connection:ratelimit-period:
	 *
	 * The period between ratelimited requests to the Musicbrainz
	 * webservice, in milliseconds. The default is 1100 (1000 plus a fudge
	 * factor), and it can be set to 0 to disable the ratelimiter.
	 */
	connection_properties[CONNECTION_RATELIMIT_PERIOD] = g_param_spec_uint(
		"ratelimit-period",
		"Ratelimit Period",
		"Time between webservice calls (ms)",
		0,		/* minimum */
		G_MAXUINT,	/* maximum */
		1100,		/* default */
		G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
	
	/**
	 * MbzWs2Connection:ratelimiter:
	 *
	 * The #MbzWs2Ratelimit object used by the connection.
	 */
	connection_properties[CONNECTION_RATELIMITER] = g_param_spec_object(
		"ratelimiter",
		"Ratelimiter",
		"The MbzWs2Ratelimit object used by this connection",
		MBZ_WS2_TYPE_RATELIMIT,
		G_PARAM_READABLE);
	
	/**
	 * MbzWs2Connection:username:
	 *
	 * The username to use when retrieving resources that require
	 * authentication.
	 */
	connection_properties[CONNECTION_USERNAME] = g_param_spec_string(
		"username",
		"Username",
		"The username for retrieving authenticated resources",
		NULL,	/* default */
		G_PARAM_READWRITE);

	/**
	 * MbzWs2Connection:password:
	 *
	 * The password to use when retrieving resources that require
	 * authentication.
	 */
	connection_properties[CONNECTION_PASSWORD] = g_param_spec_string(
		"password",
		"Password",
		"The password for retrieving authenticated resources",
		NULL,	/* default */
		G_PARAM_READWRITE);

	g_object_class_install_properties(gobject_class, CONNECTION_N_PROPERTIES, connection_properties);
}

static void mbz_ws2_connection_init(MbzWs2Connection *self)
{
	self->priv = MBZ_WS2_CONNECTION_GET_PRIVATE(self);
}

MbzWs2Connection *mbz_ws2_connection_new(const gchar *user_agent)
{
	MbzWs2Connection *con;

	g_return_val_if_fail(user_agent != NULL, NULL);

	con = g_object_new(MBZ_WS2_TYPE_CONNECTION, "user-agent", user_agent, NULL);
	
	return con;
}

MbzWs2Connection *mbz_ws2_connection_new_server(
	const gchar *user_agent,
	const gchar *endpoint_uri,
	guint ratelimit_period)
{
	MbzWs2Connection *con;
	
	g_return_val_if_fail(user_agent != NULL, NULL);
	
	con = g_object_new(MBZ_WS2_TYPE_CONNECTION,
		"user-agent", user_agent,
		"endpoint-uri", endpoint_uri,
		"ratelimit-period", ratelimit_period,
		NULL);
	
	return con;
}

const gchar *mbz_ws2_connection_get_user_agent(MbzWs2Connection *self)
{
	return self->priv->user_agent;
}

const gchar *mbz_ws2_connection_get_endpoint_uri(MbzWs2Connection *self)
{
	return self->priv->endpoint_uri_string;
}

MbzWs2Ratelimit *mbz_ws2_connection_get_ratelimiter(MbzWs2Connection *self)
{
	MbzWs2Ratelimit *rl = self->priv->ratelimiter;
	
	g_object_ref(rl);
	
	return rl;
}

void mbz_ws2_connection_set_authentication(
		MbzWs2Connection *self,
		const gchar *username,
		const gchar *password)
{
	g_free(self->priv->username);
	g_free(self->priv->password);

	self->priv->username = g_strdup(username);
	self->priv->password = g_strdup(password);
}
