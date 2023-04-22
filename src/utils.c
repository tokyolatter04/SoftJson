
#include <stdlib.h>
#include <string.h>

#include "../include/internal/utils.h"

// Char is a letter

bool8 char_is_alphabetical(char chr)
{
	return (chr >= 'A' && chr <= 'Z') || (chr >= 'a' && chr <= 'z');
}

// Char is a number

bool8 char_is_number(char chr)
{
	return chr >= '0' && chr <= '9';
}

// Char is numerical

bool8 char_is_numerical(char chr)
{
	return char_is_number(chr) || chr == '+' || chr == '-' || chr == '.' || chr == 'e' || chr == 'E';
}

bool8 char_is_whitespace(char chr)
{
	return chr == ' ' || chr == '\t' || chr == '\n' || chr == '\v' || chr == '\f' || chr == '\r';
}

bool8 string_compare(char *str1, char *str2)
{
	return strcmp(str1, str2) == 0;
}
