#include "src/mbz-ws2-connection.h"
#include "src/mbz-ws2-ratelimit.h"

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
