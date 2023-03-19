
#include <stdlib.h>
#include <string.h>

#include "../include/internal/objects.h"
#include "../include/internal/utils.h"

////////////////////////////////////
/////////// JSON OBJECT ////////////
////////////////////////////////////

JsonObject json_create_object() {
	return json_pair_list_init(0);
}

void json_object_add(JsonObject* object, const char* key, struct _JsonValue value) {
	// Overwrite key if it already exists
	// Will significantly degrade the speed of parsing very large objects with 1000s of values

	JsonValue* existing_val;

	if (json_object_get_value(object, key, &existing_val)) {
		*existing_val = value;
		return;
	}

	// Key does not exist in this object

	JsonPair pair = json_create_pair(key, value);

	json_pair_list_add(object, pair);
}

struct _JsonPair* json_object_get(JsonObject* object, uint64 index) {
	return &object->buffer[index];
}

bool8 json_object_get_value(JsonObject* object, const char* key, struct _JsonValue** value) {
	for (uint64 i = 0; i < object->length; i++) {
		JsonPair* pair = json_object_get(object, i);

		if (string_compare(pair->key, key)) {
			*value = &pair->value;
			return SUCCESS;
		}
	}

	return FAILURE;
}

//////////////////////////////////
/////////// JSON LIST ////////////
//////////////////////////////////

JsonList json_create_list() {
	return json_value_list_init(sizeof(JsonValue), 0);
}

void json_list_add(JsonList* list, struct _JsonValue value) {
	json_value_list_add(list, value);
}

JsonValue* json_list_get(JsonList* list, uint64 index) {
	return &list->buffer[index];
}

///////////////////////////////////
/////////// JSON VALUE ////////////
///////////////////////////////////

JsonValue json_create_value(JsonType type, void* data) {
	JsonValue value = (JsonValue){
		.type = type
	};

	// Assign the union value based on the type

	switch (type) {
		case JSON_DATA_TYPE_OBJECT:
			value.data._object = *(JsonObject*)data;
			break;
		case JSON_DATA_TYPE_LIST:
			value.data._list = *(JsonList*)data;
			break;
		case JSON_DATA_TYPE_INT:
			value.data._int = *(JsonInt*)data;
			break;
		case JSON_DATA_TYPE_FLOAT:
			value.data._float = *(JsonFloat*)data;
			break;
		case JSON_DATA_TYPE_STRING:
			value.data._string = data;
			break;
	}

	return value;
}

JsonValue json_create_object_value(JsonObject object) {
	return json_create_value(JSON_DATA_TYPE_OBJECT, &object);
}

JsonValue json_create_list_value(JsonList list) {
	return json_create_value(JSON_DATA_TYPE_LIST, &list);
}

JsonValue json_create_int_value(JsonInt _int) {
	return json_create_value(JSON_DATA_TYPE_INT, &_int);
}

JsonValue json_create_float_value(JsonFloat _float) {
	return json_create_value(JSON_DATA_TYPE_FLOAT, &_float);
}

JsonValue json_create_string_value(JsonString string) {
	return json_create_value(JSON_DATA_TYPE_STRING, string);
}

JsonValue json_create_bool_value(JsonBool _bool) {
	return _bool
		? json_create_value(JSON_DATA_TYPE_TRUE, NULL)
		: json_create_value(JSON_DATA_TYPE_FALSE, NULL);
}

JsonValue json_create_null_value() {
	return json_create_value(JSON_DATA_TYPE_NULL, NULL);
}

//////////////////////////////////
/////////// JSON PAIR ////////////
//////////////////////////////////

JsonPair json_create_pair(const char* key, JsonValue value) {
	return (JsonPair) {
		.key = key,
		.value = value
	};
}

///////////////////////////////////////
/////////// MEMORY FREEING ////////////
///////////////////////////////////////

/*
	NOTE:

	Set 'loaded' to TRUE if:
		- Item being freed came from:
			- soft_json_load_string()
			- soft_json_load_file()

	Set 'loaded' to FALSE if:
		- Item being freed was not loaded from a string/file

	Do not attempt to free a value if it was the product of a failed parse
*/

void json_object_free(JsonObject* object, bool8 loaded) {
	// Free all JsonPairs in the object buffer

	for (uint64 i = 0; i < object->length; i++) {
		JsonPair* pair = json_object_get(object, i);
		json_pair_free(pair, loaded);
	}

	free(object->buffer);
}

void json_list_free(JsonList* list, bool8 loaded) {
	// Free all JsonValues in the list buffer

	for (uint64 i = 0; i < list->length; i++) {
		JsonValue* value = json_list_get(list, i);
		json_value_free(value, loaded);
	}

	free(list->buffer);
}

void json_string_free(JsonString* string, bool8 loaded) {
	if (loaded) {
		free(string);
	}
}

void json_value_free(JsonValue* value, bool8 loaded) {
	switch (value->type) {
		case JSON_DATA_TYPE_OBJECT:
			json_object_free(&value->data._object, loaded);
			break;
		case JSON_DATA_TYPE_LIST:
			json_list_free(&value->data._object, loaded);
			break;
		case JSON_DATA_TYPE_STRING:
			json_string_free(value->data._string, loaded);
			break;
	}
}

void json_pair_free(JsonPair* pair, bool8 loaded) {
	// Free key & value in JsonPair

	if (loaded) {
		free(pair->key);
	}

	json_value_free(&pair->value, loaded);
}
