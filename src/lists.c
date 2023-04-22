
#include <stdlib.h>
#include <string.h>

#include "../include/internal/lists.h"
#include "../include/internal/token.h"
#include "../include/internal/objects.h"
#include "../include/internal/globals.h"

//////////////////////////////////
/////////// CHAR LIST ////////////
//////////////////////////////////

static void char_list_increase(CharList *list)
{
	if (json_mode == JSON_MODE_PRIORITY_MEMORY)
	{
		// Increase capacity by 1 (significantly slower but saves memory)
		// Start at allocation size 1

		list->capacity = list->capacity > 0
							 ? list->capacity + 1
							 : 1;
	}
	else
	{
		// Default to processor priority
		// Increase capacity * 2 (faster but may waste memory)
		// Start at allocation size of 16

		list->capacity = list->capacity > 0
							 ? list->capacity * 2
							 : 16;
	}

	// Extend buffer

	list->buffer = realloc(list->buffer, sizeof(char) * list->capacity);
}

CharList char_list_init(uint64 start_capacity)
{
	return (CharList){
		.buffer = malloc(sizeof(char) * start_capacity),
		.length = 0,
		.capacity = start_capacity};
}

void char_list_add(CharList *list, char element)
{
	if (list->length >= list->capacity)
	{
		char_list_increase(list);
	}

	list->buffer[list->length] = element;
	list->length++;
}

void char_list_remove(CharList *list, uint64 index)
{
	char *buffer = malloc(sizeof(char) * list->capacity);

	for (uint64 i = 0, j = 0; i < list->length; i++)
	{
		if (i != index)
		{
			buffer[j++] = list->buffer[i];
		}
	}

	free(list->buffer);

	list->buffer = buffer;
	list->length--;
}

///////////////////////////////////
/////////// TOKEN LIST ////////////
///////////////////////////////////

static void token_list_increase(TokenList *list)
{
	if (json_mode == JSON_MODE_PRIORITY_MEMORY)
	{
		// Increase capacity by 1 (significantly slower but saves memory)
		// Start at allocation size 1

		list->capacity = list->capacity > 0
							 ? list->capacity + 1
							 : 1;
	}
	else
	{
		// Default to processor priority
		// Increase capacity * 2 (faster but may waste memory)
		// Start at allocation size of 16

		list->capacity = list->capacity > 0
							 ? list->capacity * 2
							 : 16;
	}

	// Extend buffer

	list->buffer = realloc(list->buffer, sizeof(Token) * list->capacity);
}

TokenList token_list_init(uint64 start_capacity)
{
	return (TokenList){
		.buffer = malloc(sizeof(Token) * start_capacity),
		.length = 0,
		.capacity = start_capacity};
}

void token_list_add(TokenList *list, struct _Token element)
{
	if (list->length >= list->capacity)
	{
		token_list_increase(list);
	}

	list->buffer[list->length] = element;
	list->length++;
}

void token_list_remove(TokenList *list, uint64 index)
{
	Token *buffer = malloc(sizeof(Token) * list->capacity);

	for (uint64 i = 0, j = 0; i < list->length; i++)
	{
		if (i != index)
		{
			buffer[j++] = list->buffer[i];
		}
	}

	free(list->buffer);

	list->buffer = buffer;
	list->length--;
}

////////////////////////////////////////
/////////// JSON VALUE LIST ////////////
////////////////////////////////////////

static void json_value_list_increase(JsonValueList *list)
{
	if (json_mode == JSON_MODE_PRIORITY_MEMORY)
	{
		// Increase capacity by 1 (significantly slower but saves memory)
		// Start at allocation size 1

		list->capacity = list->capacity > 0
							 ? list->capacity + 1
							 : 1;
	}
	else
	{
		// Default to processor priority
		// Increase capacity * 2 (faster but may waste memory)
		// Start at allocation size of 32

		list->capacity = list->capacity > 0
							 ? list->capacity * 2
							 : 32;
	}

	// Extend buffer

	list->buffer = realloc(list->buffer, sizeof(JsonValue) * list->capacity);
}

JsonValueList json_value_list_init(uint64 start_capacity)
{
	return (JsonValueList){
		.buffer = malloc(sizeof(JsonValue) * start_capacity),
		.length = 0,
		.capacity = start_capacity};
}

void json_value_list_add(JsonValueList *list, struct _JsonValue element)
{
	if (list->length >= list->capacity)
	{
		json_value_list_increase(list);
	}

	list->buffer[list->length] = element;
	list->length++;
}

void json_value_list_remove(JsonValueList *list, uint64 index)
{
	JsonValue *buffer = malloc(sizeof(JsonValue) * list->capacity);

	for (uint64 i = 0, j = 0; i < list->length; i++)
	{
		if (i != index)
		{
			buffer[j++] = list->buffer[i];
		}
	}

	free(list->buffer);

	list->buffer = buffer;
	list->length--;
}

///////////////////////////////////////
/////////// JSON PAIR LIST ////////////
///////////////////////////////////////

static void json_pair_list_increase(JsonPairList *list)
{
	uint64 before = list->capacity;

	if (json_mode == JSON_MODE_PRIORITY_MEMORY)
	{
		// Increase capacity by 1 (significantly slower but saves memory)
		// Start at allocation size 1

		list->capacity = list->capacity > 0
							 ? list->capacity + 1
							 : 1;
	}
	else
	{
		// Default to processor priority
		// Increase capacity * 2 (faster but may waste memory)
		// Start at allocation size of 16

		list->capacity = list->capacity > 0
							 ? list->capacity * 2
							 : 16;
	}

	// Extend buffer

	list->buffer = realloc(list->buffer, sizeof(JsonPair) * list->capacity);
}

JsonPairList json_pair_list_init(uint64 start_capacity)
{
	return (JsonPairList){
		.buffer = malloc(sizeof(JsonPair) * start_capacity),
		.length = 0,
		.capacity = start_capacity};
}

void json_pair_list_add(JsonPairList *list, struct _JsonPair element)
{
	if (list->length >= list->capacity)
	{
		json_pair_list_increase(list);
	}

	list->buffer[list->length] = element;
	list->length++;
}

void json_pair_list_remove(JsonPairList *list, uint64 index)
{
	JsonPair *buffer = malloc(sizeof(JsonPair) * list->capacity);

	for (uint64 i = 0, j = 0; i < list->length; i++)
	{
		if (i != index)
		{
			buffer[j++] = list->buffer[i];
		}
	}

	free(list->buffer);

	list->buffer = buffer;
	list->length--;
}
