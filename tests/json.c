#include "json.h"

#include <glib.h>

int main (int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/json/artist", test_json_artist);

	return g_test_run();
}
