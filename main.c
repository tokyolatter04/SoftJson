
#include "include/softjson.h"

int main(void)
{

	// Prioritise memory efficiency over speed
	// (Engine wastes less memory, but can be slower when parsing large data)

	soft_set_engine_mode(JSON_MODE_PRIORITY_MEMORY);

	// Create animal

	JsonObject animal = json_create_object();

	json_object_add(&animal, "name", json_create_string_value("Polar Bear"));
	json_object_add(&animal, "age", json_create_int_value(22));
	json_object_add(&animal, "weight-kg", json_create_int_value(1130));
	json_object_add(&animal, "is-mammal", json_create_bool_value(TRUE));

	// Output animal

	JsonHandler handler = soft_create_handler();
	char *serial = soft_dump_string(&handler, json_create_object_value(animal));

	// Data: {"name":"Polar Bear","age":22,"weight-kg":1130,"is-mammal":true}
	printf("Data: %s\n", serial);

	// Free data

	json_object_free(&animal, FALSE);
}
