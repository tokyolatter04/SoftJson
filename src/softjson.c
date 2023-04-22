
#include <stdlib.h>

#include "../include/softjson.h"
#include "../include/internal/lexer.h"
#include "../include/internal/parser.h"
#include "../include/internal/io.h"
#include "../include/internal/serialisation.h"

JsonHandler soft_create_handler()
{
	return (JsonHandler){
		.error = error_init()};
}

JsonValue soft_load_string(JsonHandler *handler, char *source)
{
	// Lexer

	Lexer lexer = lexer_init(source);
	TokenList tokens = lexer_get_result(&lexer);

	if (lexer.error.exists)
	{
		// Save Lexer error in handler error and return an empty value

		handler->error = lexer.error;

		return (JsonValue){.data = 0, .type = 0};
	}

	// Parser

	Parser parser = parser_init(tokens);
	JsonValue value = parser_get_result(&parser);

	if (parser.error.exists)
	{
		// Save Parser error in handler error and return an empty value

		handler->error = parser.error;
	}

	// Cleanup

	free(tokens.buffer);

	return value;
}

JsonValue soft_load_file(JsonHandler *handler, char *filepath)
{
	char *source;

	// Retrieve the source string from the filepath

	if (!file_read_all_text(filepath, &source))
	{
		// Error accessing filepath

		RAISE_FATAL_ERROR(
			handler->error,
			ERROR_TYPE_FILE_ACCESS,
			filepath);

		free(source);

		return (JsonValue){.data = 0, .type = 0};
	}

	// Create and return JsonValue

	JsonValue res = soft_load_string(handler, source);
	free(source);

	return res;
}

char *soft_dump_string(JsonHandler *handler, JsonValue value)
{
	// Serialise JsonValue

	return serialise_json_value(value);
}

void soft_dump_file(JsonHandler *handler, JsonValue value, char *filepath)
{
	// Serialise JsonValue

	char *serial = serialise_json_value(value);

	// Write serial to a filepath

	if (!file_write_all_text(filepath, serial))
	{
		// Error accessing path

		RAISE_FATAL_ERROR(
			handler->error,
			ERROR_TYPE_FILE_ACCESS,
			filepath);
	}
}
