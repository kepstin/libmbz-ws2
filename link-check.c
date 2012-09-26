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

#include "src/connection.h"
#include "src/ratelimit.h"

int main(int argc, char *argv[]) {
	MbzWs2Connection *con;
	const gchar *user_agent, *endpoint_uri;
	guint ratelimit_period, ratelimit_burst;
	MbzWs2Ratelimit *rl;
	
	g_type_init();
	
	con = mbz_ws2_connection_new("MbzLinkCheck/1");
	
	user_agent = mbz_ws2_connection_get_user_agent(con);
	endpoint_uri = mbz_ws2_connection_get_endpoint_uri(con);

	g_print("User Agent: %s\n", user_agent);
	g_print("Endpoint URI: %s\n", endpoint_uri);
	
	rl = mbz_ws2_connection_get_ratelimiter(con);
	
	ratelimit_period = mbz_ws2_ratelimit_get_period(rl);
	ratelimit_burst = mbz_ws2_ratelimit_get_burst(rl);
	g_print("Ratelimiter: period: %u / burst: %u\n", ratelimit_period, ratelimit_burst);
	
	g_object_unref(rl);
	
	g_object_unref(con);
	
	return 0;
}
