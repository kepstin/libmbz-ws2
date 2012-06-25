#include "src/mbz-ws2-connection.h"

int main(int argc, char *argv[]) {
	MbzWs2Connection *con;
	gchar *user_agent, *base_uri;
	guint ratelimit_period;
	
	g_type_init();
	
	con = mbz_ws2_connection_new("MbzLinkCheck/1");
	
	g_object_get(con,
		"user-agent", &user_agent,
		"base-uri", &base_uri,
		"ratelimit-period", &ratelimit_period,
		NULL);

	g_print("User Agent: %s\n", user_agent);
	g_print("Base URI: %s\n", base_uri);
	g_print("Ratelimit Period: %u\n", ratelimit_period);

	g_free(user_agent);
	g_free(base_uri);
	
	g_object_unref(con);
	
	return 0;
}
