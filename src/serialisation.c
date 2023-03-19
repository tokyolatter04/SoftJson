
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/internal/serialisation.h"

char* serialise_json_object(JsonObject object) {
	CharList string = char_list_init(0);

	// Start of object

	char_list_add(&string, '{');

	for (uint64 i = 0; i < object.length; i++) {
		// Serialise JsonPair at the index

		JsonPair* pair = json_object_get(&object, i);
		char* pair_str = serialise_json_pair(*pair);

		// Insert character-by-character

		for (uint64 j = 0; j < strlen(pair_str); j++) {
			char_list_add(&string, pair_str[j]);
		}

		// Insert a ',' if it is not the last of multiple pairs

		if (i != object.length - 1) {
			// Pair separator

			char_list_add(&string, ',');
		}

		// Cleanup pair serial, it is now stored in the string

		free(pair_str);
	}

	// End of object

	char_list_add(&string, '}');

	// End string

	char_list_add(&string, '\0');

	return string.buffer;
}

char* serialise_json_list(JsonList list) {
	CharList string = char_list_init(0);

	// Start of list

	char_list_add(&string, '[');

	for (uint64 i = 0; i < list.length; i++) {
		// Serialise JsonValue at an index

		JsonValue* value = json_list_get(&list, i);
		char* value_str = serialise_json_value(*value);

		// Insert character-by-character

		for (uint64 j = 0; j < strlen(value_str); j++) {
			char_list_add(&string, value_str[j]);
		}

		// Insert a ',' if it is not the last of multiple valuse

		if (i != list.length - 1) {
			// Value seperator

			char_list_add(&string, ',');
		}

		// Cleanup value serial, it is now stored in the string

		free(value_str);
	}

	// End of list

	char_list_add(&string, ']');

	// End string

	char_list_add(&string, '\0');

	return string.buffer;
}

char* serialise_keyword(JsonType keyword_type) {
	char* buffer;

	// Create keyword string

	switch (keyword_type) {
		case JSON_DATA_TYPE_TRUE:
			buffer = malloc(sizeof(char) * 5);
			snprintf(buffer, 5, "%s", "true");
			break;
		case JSON_DATA_TYPE_FALSE:
			buffer = malloc(sizeof(char) * 6);
			snprintf(buffer, 6, "%s", "false");
			break;
		case JSON_DATA_TYPE_NULL:
			buffer = malloc(sizeof(char) * 5);
			snprintf(buffer, 5, "%s", "null");
			break;
		default:
			buffer = calloc(sizeof(char), 1);
			break;
	}

	return buffer;
}

char* serialise_json_int(JsonInt value) {
	// Insert int into string
	// Use malloc as caller is expecting heap allocated string

	char* buffer = malloc(sizeof(char) * 16);
	snprintf(buffer, 16, "%lld", value);

	return buffer;
}

char* serialise_json_float(JsonFloat value) {
	// Insert float into string
	// Use malloc as caller is expecting heap allocated string

	char* buffer = malloc(sizeof(char) * 16);
	snprintf(buffer, 16, "%Lg", value);

	return buffer;
}

char* serialise_json_string(JsonString value) {
	CharList string = char_list_init(0);

	// Start of string

	char_list_add(&string, '"');

	// Add character-by-character

	for (uint64 i = 0; i < strlen(value); i++) {
		char_list_add(&string, value[i]);
	}

	// End of string

	char_list_add(&string, '"');

	// End string

	char_list_add(&string, '\0');

	return string.buffer;
}

char* serialise_json_value(JsonValue value) {
	switch (value.type) {
		case JSON_DATA_TYPE_OBJECT:
			return serialise_json_object(value.data._object);
		case JSON_DATA_TYPE_LIST:
			return serialise_json_list(value.data._list);
		case JSON_DATA_TYPE_TRUE:
		case JSON_DATA_TYPE_FALSE:
		case JSON_DATA_TYPE_NULL:
			return serialise_keyword(value.type);
		case JSON_DATA_TYPE_INT:
			return serialise_json_int(value.data._int);
		case JSON_DATA_TYPE_FLOAT:
			return serialise_json_float(value.data._float);
		case JSON_DATA_TYPE_STRING:
			return serialise_json_string(value.data._string);
		default: {
			// Data type not recognised
			// Return malloc string as caller is expecting heap allocated string

			char* buffer = malloc(sizeof(char) * 32);
			snprintf(buffer, 32, "ERROR: INVALID JSON_DATA_TYPE");

			return buffer;
		}
	}
}

char* serialise_json_pair(JsonPair pair) {
	CharList string = char_list_init(0);

	// Start of pair key string

	char_list_add(&string, '"');

	// Add key character-by-character

	for (uint64 i = 0; i < strlen(pair.key); i++) {
		char_list_add(&string, pair.key[i]);
	}

	// End of pair key string

	char_list_add(&string, '"');

	// Key-pair separator

	char_list_add(&string, ':');

	// Serialise value

	char* value_str = serialise_json_value(pair.value);

	// Add value character-by-character

	for (uint64 i = 0; i < strlen(value_str); i++) {
		char_list_add(&string, value_str[i]);
	}

	// End string

	char_list_add(&string, '\0');

	// Cleanup value string, it is now stored in the string

	free(value_str);

	return string.buffer;
}
