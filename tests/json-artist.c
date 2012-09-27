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
 * License along with this library; if not, see <http://www.gnu.org/licenses>
 */

#include "json.h"

#include <glib.h>

void test_json_artist (void)
{
	gboolean ret;
	gchar *artist_json;
	MbzWs2Artist *artist;
	GError *error = NULL;

	ret = g_file_get_contents(TOP_SRCDIR "/tests/json-artist.json", &artist_json, NULL, &error);
	g_assert_no_error(error);

	artist = json_gobject_from_data(MBZ_WS2_TYPE_ARTIST, artist_json, -1, NULL);
	g_assert_no_error(error);

	return;
}
