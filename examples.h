
#pragma once

#include <stdio.h>

#include "include/softjson.h"

void print_data_example() {
	char* string = "{				\
		\"name\": \"John\",			\
		\"age\": 20,				\
		\"gender\": \"male\",		\
		\"occupation\": null,		\
		\"home-owner\": true,		\
		\"hobbies\": [				\
			\"Cycling\",			\
			\"Swimming\"			\
		]							\
	}";

	JsonHandler handler = soft_create_handler();
	JsonValue value = soft_load_string(&handler, string);
	JsonObject person = value.data._object;

	if (handler.error.exists) {
		printf("Error: %s\n", handler.error.log);
		return;
	}

	// Print Name

	JsonValue* name_val;

	if (json_object_get_value(&person, "name", &name_val)) {
		if (name_val->type == JSON_DATA_TYPE_INT) {
			JsonString name = name_val->data._string;

			printf("Name: %s\n", name);
		}
	}

	// Print Age

	JsonValue* age_val;

	if (json_object_get_value(&person, "age", &age_val)) {
		if (age_val->type == JSON_DATA_TYPE_INT) {
			JsonInt age = age_val->data._int;

			printf("Age: %d\n", age);
		}
	}

	// Print Occupation

	JsonValue* occupation_val;

	if (json_object_get_value(&person, "occupation", &occupation_val)) {
		if (occupation_val->type == JSON_DATA_TYPE_LIST) {
			// Not ran here, as the object is null
		}
	}

	// Print Home Owner

	JsonValue* home_owner_val;

	if (json_object_get_value(&person, "home-owner", &home_owner_val)) {
		switch (home_owner_val->type) {
			case JSON_DATA_TYPE_TRUE:
				printf("Home Owner: True\n");
				break;
			case JSON_DATA_TYPE_FALSE:
				printf("Home Owner: False\n");
				break;
		}
	}

	// Print Hobbies

	JsonValue* hobbies_val;

	if (json_object_get_value(&person, "hobbies", &hobbies_val)) {
		if (hobbies_val->type == JSON_DATA_TYPE_LIST) {
			JsonList hobbies = hobbies_val->data._list;

			for (uint64 i = 0; i < hobbies.length; i++) {
				JsonValue* value = json_list_get(&hobbies, i);

				if (value->type == JSON_DATA_TYPE_STRING) {
					printf("Hobby %d: %s\n", i, value->data._string);
				}
				else {
					printf("Hobby %d: (Unexpected type)", i);
				}
			}
		}
	}
}

// See tests.h for more examples
