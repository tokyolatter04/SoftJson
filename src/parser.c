
#include <stdlib.h>

#include "../include/internal/parser.h"
#include "../include/internal/conversions.h"
#include "../include/internal/utils.h"

/////////////////////////////////////////////
/////////// INTERNAL PARSER CODE ////////////
/////////////////////////////////////////////

static uint8 parse_object(Parser* parser, JsonValue* value);
static uint8 parse_list(Parser* parser, JsonValue* value);
static uint8 parse_number(Parser* parser, JsonValue* value);
static uint8 parse_string(Parser* parser, JsonValue* value);
static uint8 parse_value(Parser* parser, JsonValue* value);
static uint8 parse_pair(Parser* parser, JsonPair* pair);

#define PARSER_FUNC(name)						uint8 (*name)(Parser*, void*)

#define MAX_CALL_DEPTH							128

uint64 call_depth = 0;

// Wrap a parser function in an error check

#define CALL(call, parser, out_value)					\
	if (!execute_parse_func(call, parser, out_value))	\
		return FAILURE;

static uint8 execute_parse_func(PARSER_FUNC(func), Parser* parser, void* out_value) {
	// Entering function, call depth must be increased

	call_depth++;

	if (call_depth > MAX_CALL_DEPTH) {
		RAISE_FATAL_ERROR(
			parser->error,
			ERROR_TYPE_CALL_STACK_OVERFLOW,
			MAX_CALL_DEPTH
		);

		return FAILURE;
	}

	// Execute function and return result

	uint8 result = func(parser, out_value);

	// Function has been executed, call depth must be decreased

	call_depth--;

	return result;
}

static uint8 advance(Parser* parser, TokenType expected_type) {
	// Check if current token type is equal to the expected type

	if (parser->token->type != expected_type) {
		RAISE_FATAL_ERROR(
			parser->error,
			ERROR_TYPE_WRONG_TYPE,
			token_type_to_string(expected_type),
			token_type_to_string(parser->token->type)
		);

		return FAILURE;
	}

	// Advancing to the next token

	parser->index++;

	// Set token to the next token in the list, stop at the last token

	if (parser->tokens.length > parser->index) {
		parser->token = &parser->tokens.buffer[parser->index];
	}
	else {
		parser->token = &parser->tokens.buffer[parser->tokens.length - 1];
	}

	return SUCCESS;
}

/*
	Parse a JSON Object
*/

static uint8 parse_object(Parser* parser, JsonValue* out_value) {
	JsonObject object = json_create_object();
	JsonPair pair;

	// Advance past '{'

	ERROR_CHECK(advance(parser, TOKEN_TYPE_LCURLY));

	// Check if object is empty

	if (parser->token->type == TOKEN_TYPE_RCURLY) {
		ERROR_CHECK(advance(parser, TOKEN_TYPE_RCURLY));

		*out_value = json_create_value(JSON_DATA_TYPE_OBJECT, &object);
		return SUCCESS;
	}

	// Store the first key value pair in the object

	CALL(parse_pair, parser, &pair);
	json_object_add(&object, pair.key, pair.value);

	// Store all other key value pairs found (if ',' is used)

	while (parser->token->type == TOKEN_TYPE_COMMA) {
		ERROR_CHECK(advance(parser, TOKEN_TYPE_COMMA));
		CALL(parse_pair, parser, &pair);
		json_object_add(&object, pair.key, pair.value);
	}

	// Advance past '}'

	ERROR_CHECK(advance(parser, TOKEN_TYPE_RCURLY));

	// Create and return value

	*out_value = json_create_value(JSON_DATA_TYPE_OBJECT, &object);
	return SUCCESS;
}

/*
	Parse a JSON List
*/

static uint8 parse_list(Parser* parser, JsonValue* out_value) {
	JsonList list = json_create_list();
	JsonValue value;

	// Advance past '['

	ERROR_CHECK(advance(parser, TOKEN_TYPE_LBRACKET));

	if (parser->token->type == TOKEN_TYPE_RBRACKET) {
		ERROR_CHECK(advance(parser, TOKEN_TYPE_RBRACKET));

		*out_value = json_create_value(JSON_DATA_TYPE_LIST, &list);
		return SUCCESS;
	}

	// Store the first value in the list

	CALL(parse_value, parser, & value);
	json_list_add(&list, value);

	// Store all other values in the list (if ',' is found)

	while (parser->token->type == TOKEN_TYPE_COMMA) {
		ERROR_CHECK(advance(parser, TOKEN_TYPE_COMMA));
		CALL(parse_value, parser, &value);
		json_list_add(&list, value);
	}

	// Advance past ']'

	ERROR_CHECK(advance(parser, TOKEN_TYPE_RBRACKET));

	// Create and return value

	*out_value = json_create_value(JSON_DATA_TYPE_LIST, &list);
	return SUCCESS;
}

/*
	Parse a JSON Keyword
*/

static uint8 parse_keyword(Parser* parser, JsonValue* out_value) {
	// JSON Keywords: true, false, null

	switch (parser->token->type) {
		case TOKEN_TYPE_TRUE:
			*out_value = json_create_value(JSON_DATA_TYPE_TRUE, NULL);
			break;
		case TOKEN_TYPE_FALSE:
			*out_value = json_create_value(JSON_DATA_TYPE_FALSE, NULL);
			break;
		case TOKEN_TYPE_NULL:
			*out_value = json_create_value(JSON_DATA_TYPE_NULL, NULL);
			break;
		default:
			// Token type is not an accepted keyword

			RAISE_FATAL_ERROR(
				parser->error,
				ERROR_TYPE_UNRECOGNISED_KEYWORD,
				token_type_to_string(parser->token->type)
			);

			return FAILURE;
	}

	// Advance to next token

	ERROR_CHECK(advance(parser, parser->token->type));

	return SUCCESS;
}

/*
	Parse a JSON Number
*/

static uint8 parse_number(Parser* parser, JsonValue* out_value) {
	Number number;

	// Convert raw token string to a number (int or float)

	if (!string_to_number(parser->token->value, &number)) {
		RAISE_FATAL_ERROR(
			parser->error,
			ERROR_TYPE_INVALID_NUMBER,
			parser->token->value
		);

		return FAILURE;
	}

	// Token string will not longer be used

	free(parser->token->value);

	// Advance to next token

	ERROR_CHECK(advance(parser, TOKEN_TYPE_NUMBER));

	// Create and return value

	switch (number.type) {
		case NUMBER_TYPE_INT:
			*out_value = json_create_value(JSON_DATA_TYPE_INT, &number.data._int);
			break;
		case NUMBER_TYPE_FLOAT:
			*out_value = json_create_value(JSON_DATA_TYPE_FLOAT, &number.data._float);
			break;
	}

	return SUCCESS;
}

/*
	Parse a JSON String
*/

static uint8 parse_string(Parser* parser, JsonValue* out_value) {
	// Create and return value

	*out_value = json_create_value(JSON_DATA_TYPE_STRING, parser->token->value);

	// Advance to next token

	ERROR_CHECK(advance(parser, TOKEN_TYPE_STRING));

	return SUCCESS;
}

/*
	Parse a JSON Value
*/

static uint8 parse_value(Parser* parser, JsonValue* out_value) {
	switch (parser->token->type) {
		case TOKEN_TYPE_LCURLY:
			// '{' token found, opening to an object

			return execute_parse_func(parse_object, parser, out_value);
		case TOKEN_TYPE_LBRACKET:
			// '[' token found, opening to a list

			return execute_parse_func(parse_list, parser, out_value);
		case TOKEN_TYPE_TRUE:
		case TOKEN_TYPE_FALSE:
		case TOKEN_TYPE_NULL:
			// Keyword token (true, false, null) found

			return execute_parse_func(parse_keyword, parser, out_value);
		case TOKEN_TYPE_NUMBER:
			// Number token found (int/float)

			return execute_parse_func(parse_number, parser, out_value);
		case TOKEN_TYPE_STRING:
			// String token found

			return execute_parse_func(parse_string, parser, out_value);
	}

	// Unexpected token

	RAISE_FATAL_ERROR(
		parser->error,
		ERROR_TYPE_UNEXPECTED_TYPE,
		token_type_to_string(parser->token->type)
	);

	return FAILURE;
}

/*
	Parse a JSON Pair
*/

static uint8 parse_pair(Parser* parser, JsonPair* pair) {
	// Parse pair e.g. "number": 10

	// Key is the current token value

	char* key = parser->token->value;

	// Advance past key token

	ERROR_CHECK(advance(parser, TOKEN_TYPE_STRING));

	// Advance past ':'

	ERROR_CHECK(advance(parser, TOKEN_TYPE_COLON));

	// Parse value

	JsonValue value;
	CALL(parse_value, parser, &value);

	// Create and return pair

	*pair = json_create_pair(key, value);
	return SUCCESS;
}

///////////////////////////////////////////////////
/////////// HEADER FILE IMPLEMENTATION ////////////
///////////////////////////////////////////////////

/*
	Create a Parser struct from a source TokenList
*/

Parser parser_init(TokenList tokens) {
	return (Parser) {
		.tokens = tokens,
		.token = &tokens.buffer[0],
		.index = 0,
		.error = error_init()
	};
}

/*
	Get the result from a Parser struct
*/

JsonValue parser_get_result(Parser* parser) {
	JsonValue value;
	parse_value(parser, &value);

	return value;
}
