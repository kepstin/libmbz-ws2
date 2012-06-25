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

struct _MbzWs2ConnectionPrivate {
}

G_DEFINE_TYPE(MbzWs2Connection, mbz_ws2_connection, G_TYPE_OBJECT)

static void mbz_ws2_connection_class_init(MbzWs2ConnectionClass *klass)
{
	g_type_class_add_private(klass, sizeof (MbzWs2ConnectionPrivate));
}

static void mbz_ws2_connection_init(MbzWs2Connection *self)
{
	self->priv = MBZ_WS2_CONNECTION_GET_PRIVATE(self);
}
