
#include <stdlib.h>
#include <string.h>

#include "../include/internal/lexer.h"
#include "../include/internal/token.h"
#include "../include/internal/utils.h"

////////////////////////////////////////////
/////////// INTERNAL LEXER CODE ////////////
////////////////////////////////////////////

#define EMPTY_TOKEN (Token){.type = 0, .value = 0};

/*
	Advance to the next character in the Lexer's source string
*/

static void advance(Lexer *lexer)
{
	lexer->index++;

	if (lexer->length > lexer->index)
	{
		lexer->character = lexer->source[lexer->index];
	}
	else
	{
		lexer->character = CHAR_EOF;
	}
}

/*
	Advance with a symbol token (no value required)
*/

static Token advance_with_symbol(Lexer *lexer, TokenType type)
{
	// Advance with a syntax token

	advance(lexer);
	return token_init(type, NULL);
}

/*
	Skip all whitespace characters until a non-whitespace character is found
*/

static void skip_whitespace(Lexer *lexer)
{
	// Advance past the current character while it is a whitespace character

	while (char_is_whitespace(lexer->character))
	{
		advance(lexer);
	}
}

/*
	Build a token with a number
*/

static Token build_number_token(Lexer *lexer)
{
	CharList string = char_list_init(0);

	// Append the char while it is a numerical character

	while (char_is_numerical(lexer->character))
	{
		char_list_add(&string, lexer->character);
		advance(lexer);
	}

	// End string

	char_list_add(&string, '\0');

	// Create number token with string data

	return token_init(TOKEN_TYPE_NUMBER, string.buffer);
}

/*
	Build a token with a string
*/

static Token build_string_token(Lexer *lexer)
{
	CharList string = char_list_init(0);
	uint8 string_escaped = FALSE;

	// Advance past starting '"'

	advance(lexer);

	// String has ended if it encounters a '"' or an EOF (null termination)

	while (lexer->character != CHAR_QUOTE && lexer->character != CHAR_EOF)
	{

		if (lexer->character == '\\')
		{
			// Escape character

			char_list_add(&string, lexer->character);
			advance(lexer);

			switch (lexer->character)
			{
			case '\\':
			case '"':
			case '/':
			case 'b':
			case 'f':
			case 'n':
			case 'r':
			case 't':
				break;
			default:
				RAISE_FATAL_ERROR(
					lexer->error,
					ERROR_INVALID_ESCAPE_CHARACTER,
					lexer->character);

				return EMPTY_TOKEN;
			}
		}

		char_list_add(&string, lexer->character);
		advance(lexer);
	}

	// Advance past ending '"'

	advance(lexer);

	// End string

	char_list_add(&string, '\0');

	// Create string token with string data

	return token_init(TOKEN_TYPE_STRING, string.buffer);
}

/*
	Build a token with a keyword
*/

static Token build_keyword_token(Lexer *lexer)
{
	CharList string = char_list_init(0);

	// Only alphabetical characters are allowed (A-Z/a-z)

	while (char_is_alphabetical(lexer->character))
	{
		char_list_add(&string, lexer->character);
		advance(lexer);
	}

	// End string

	char_list_add(&string, '\0');

	// Get keyword type

	TokenType type;

	if (string_compare(string.buffer, KEYWORD_TRUE))
	{
		type = TOKEN_TYPE_TRUE;
	}
	else if (string_compare(string.buffer, KEYWORD_FALSE))
	{
		type = TOKEN_TYPE_FALSE;
	}
	else if (string_compare(string.buffer, KEYWORD_NULL))
	{
		type = TOKEN_TYPE_NULL;
	}
	else
	{
		// Unrecognised keyword type

		RAISE_FATAL_ERROR(
			lexer->error,
			ERROR_TYPE_UNRECOGNISED_KEYWORD,
			string.buffer);

		return EMPTY_TOKEN;
	}

	// String is no longer required

	free(string.buffer);

	// Create keyword token with no data

	return token_init(type, NULL);
}

/*
	Build next token using the Lexer's current character
*/

static Token build_next_token(Lexer *lexer)
{
	skip_whitespace(lexer);

	// Find syntax (single-character) tokens

	switch (lexer->character)
	{
	case CHAR_COMMA:
		return advance_with_symbol(lexer, TOKEN_TYPE_COMMA);
	case CHAR_COLON:
		return advance_with_symbol(lexer, TOKEN_TYPE_COLON);
	case CHAR_LCURLY:
		return advance_with_symbol(lexer, TOKEN_TYPE_LCURLY);
	case CHAR_RCURLY:
		return advance_with_symbol(lexer, TOKEN_TYPE_RCURLY);
	case CHAR_LBRACKET:
		return advance_with_symbol(lexer, TOKEN_TYPE_LBRACKET);
	case CHAR_RBRACKET:
		return advance_with_symbol(lexer, TOKEN_TYPE_RBRACKET);
	case CHAR_EOF:
		return token_init(TOKEN_TYPE_EOF, NULL);
	}

	// Find multi-character tokens

	if (char_is_numerical(lexer->character))
	{
		return build_number_token(lexer);
	}
	else if (char_is_alphabetical(lexer->character))
	{
		return build_keyword_token(lexer);
	}
	else if (lexer->character == CHAR_QUOTE)
	{
		return build_string_token(lexer);
	}

	// Unrecognised character

	RAISE_FATAL_ERROR(
		lexer->error,
		ERROR_TYPE_UNRECOGNISED_TOKEN,
		lexer->character);

	return EMPTY_TOKEN;
}

///////////////////////////////////////////////////
/////////// HEADER FILE IMPLEMENTATION ////////////
///////////////////////////////////////////////////

/*
	Create Lexer struct from source string
*/

Lexer lexer_init(char *source)
{
	return (Lexer){
		.source = source,
		.length = strlen(source),
		.character = source[0],
		.index = 0,
		.error = error_init()};
}

/*
	Get result from a Lexer struct
*/

TokenList lexer_get_result(Lexer *lexer)
{
	TokenList tokens = token_list_init(0);
	Token token;

	// Continue until an EOF token is found, signaling the end of the source string

	do
	{
		// Generate next token to use

		token = build_next_token(lexer);

		// Do not continue if an error was found

		if (lexer->error.exists)
		{
			break;
		}

		token_list_add(&tokens, token);
	} while (token.type != TOKEN_TYPE_EOF);

	return tokens;
}
