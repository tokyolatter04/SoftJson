# SoftJson

## About
This is a JSON parser written in C for parsing JSON data. It includes error handling with information about the error that occurred.

## Features

- Parse JSON from a string
- Parse JSON from a file
- Write JSON to a string
- Write JSON to a file
- Error handling / safety
- Efficient and fast parsing
- Memory cleanup
- Customise Hardware Priority

- Currently does NOT support unicode

## Hardware Priority

This JSON parser is optimised to be fast, this is done by allocating a large buffer of memory for different objects and moving data into it. This decreases the total amount of memory allocations while storing JSON data. However, not all of this buffer is filled, which causes wasted memory. The parser supports being set to a memory efficient mode by calling `soft_set_engine_mode(JsonMode mode)`. This will make the parser significantly slower, but will result in less memory being wasted

## Examples

Examples can be found in `tests.h` and `examples.h`

```c
#include "include/softjson.h"

int main(void) {

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
	char* serial = soft_dump_string(&handler, json_create_object_value(animal));

	// Data: {"name":"Polar Bear","age":22,"weight-kg":1130,"is-mammal":true}
	printf("Data: %s\n", serial);

	// Free data

	json_object_free(&animal, FALSE);
}
```
