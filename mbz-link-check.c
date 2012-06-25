#include "src/mbz-ws2-connection.h"

int main(int argc, char *argv[]) {
	MbzWs2Connection *con;
	gchar *tmp;
	
	g_type_init();
	
	con = mbz_ws2_connection_new_server("MbzLinkCheck/1", "http://192.168.112.67:5000/ws/2", 0);
	
	g_object_get(con, "user-agent", &tmp, NULL);
	g_print("User Agent: %s\n", tmp);
	g_free(tmp);
	
	g_object_get(con, "base-uri", &tmp, NULL);
	g_print("Base URI: %s\n", tmp);
	g_free(tmp);
	
	g_object_unref(con);
	
	return 0;
}
