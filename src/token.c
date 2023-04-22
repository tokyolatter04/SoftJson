
#include "../include/internal/token.h"

Token token_init(TokenType type, char *value)
{
	return (Token){
		.type = type,
		.value = value};
}

char *token_type_to_string(TokenType type)
{
	switch (type)
	{
	case TOKEN_TYPE_COMMA:
		return "COMMA";
	case TOKEN_TYPE_COLON:
		return "COLON";
	case TOKEN_TYPE_QUOTE:
		return "QUOTE";
	case TOKEN_TYPE_LCURLY:
		return "LCURLY";
	case TOKEN_TYPE_RCURLY:
		return "RCURLY";
	case TOKEN_TYPE_LBRACKET:
		return "LBRACKET";
	case TOKEN_TYPE_RBRACKET:
		return "RBRACKET";
	case TOKEN_TYPE_TRUE:
		return "TRUE";
	case TOKEN_TYPE_FALSE:
		return "FALSE";
	case TOKEN_TYPE_NULL:
		return "NULL";
	case TOKEN_TYPE_NUMBER:
		return "NUMBER";
	case TOKEN_TYPE_STRING:
		return "STRING";
	case TOKEN_TYPE_EOF:
		return "EOF";
	default:
		return "UNRECOGNISED";
	}
}
