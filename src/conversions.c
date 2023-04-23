
#include <stdlib.h>
#include <string.h>
#include "../include/internal/conversions.h"
#include "../include/internal/utils.h"

#define SIGN_POSITIVE								1
#define SIGN_NEGATIVE								0

#define NUMBER_DO_MATHS(num, _operator, amount)		\
	if (num->type == NUMBER_TYPE_INT) {				\
		num->data._int _operator ##= amount;			\
	}												\
	else if (num->type == NUMBER_TYPE_FLOAT) {		\
		num->data._float _operator ##= amount;			\
	}

#define NUMBER_ADD(num, amount)						\
	NUMBER_DO_MATHS(num, +, amount)

#define NUMBER_SUBTRACT(num, amount)				\
	NUMBER_DO_MATHS(num, -, amount)

#define NUMBER_MULTIPLY(num, amount)				\
	NUMBER_DO_MATHS(num, *, amount)

#define NUMBER_DIVIDE(num, amount)					\
	NUMBER_DO_MATHS(num, /, amount)

static void number_switch_types(Number* number) {
	switch (number->type) {
		case NUMBER_TYPE_INT:
			number->data._float = number->data._int;
			number->type = NUMBER_TYPE_FLOAT;
			break;
		case NUMBER_TYPE_FLOAT:
			number->data._int = number->data._float;
			number->type = NUMBER_TYPE_INT;
			break;
	}
}

uint8 string_to_number(char* string, Number* num) {
	*num = (Number){
		.data._int = 0,
		.data._float = 0.0f,
		.type = NUMBER_TYPE_INT
	};

	float64 after_decimal = 0.0f;
	uint64 float_depth = 10;
	uint8 sign = SIGN_POSITIVE;

	if (string[0] == '\0') {
		// Invalid input data
		return FAILURE;
	}

	uint64 i;

	for (i = 0; char_is_numerical(string[i]); i++) {

		if (string[i] == '-') {
			if (i != 0) {
				// '-' symbol appears after the first character

				return FAILURE;
			}

			sign = SIGN_NEGATIVE;

			continue;
		}
		else if (string[i] == '+') {
			if (i != 0) {
				// '+' symbol appears after the first character

				return FAILURE;
			}

			sign = SIGN_POSITIVE;

			continue;
		}

		if (string[i] == 'e' || string[i] == 'E') {
			break;
		}

		if (string[i] == '.') {
			if (num->type == NUMBER_TYPE_FLOAT) {
				// Decimal point found after number is already float
				// (Most likely two decimals in one number)

				return FAILURE;
			}

			number_switch_types(num);

			continue;
		}

		switch (num->type) {
			case NUMBER_TYPE_INT: {
				/*
					New character is added to the number
				*/

				NUMBER_MULTIPLY(num, 10);
				NUMBER_ADD(num, string[i] - '0');

				break;
			}
			case NUMBER_TYPE_FLOAT: {
				/*
					Number is discovered to be a float
					This only occurs when a decimal point is found
					All characters in the string variable are past the decimal point
				*/

				// Floating point position has increase by 1 decimal place

				float_depth *= 10;

				// Number after the decimal point is added to after_decimal

				after_decimal += (string[i] - '0') / (float_depth / 10.0f);

				break;
			}
		}
	}

	// Add any data found after a decimal point
	// 0 will be added to the number if the number is an integer

	NUMBER_ADD(num, after_decimal);

	if (string[i] == 'e' || string[i] == 'E') {
		// Parse Euler's number

		int64 euler_int = 0;
		int64 euler_sign = SIGN_POSITIVE;
		i++;

		switch (string[i]) {
			case '-':
				euler_sign = SIGN_NEGATIVE;
				i++;
				break;
			case '+':
				euler_sign = SIGN_POSITIVE;
				i++;
				break;
			case '\0':
				// Expected Euler's number contains no data

				return FAILURE;
		}

		for (; char_is_number(string[i]); i++) {
			euler_int += (euler_int * 10) + string[i] - '0';
		}

		if (euler_sign == 0) {
			euler_int *= -1;
		}

		// Number must be converted to a float before it can be divided

		if (euler_int < 0 && num->type != NUMBER_TYPE_FLOAT) {
			number_switch_types(num);
		}

		for (int64 y = 0; y < abs(euler_int); y++) {
			if (euler_int > 0) {
				NUMBER_MULTIPLY(num, 10);
			}
			else {
				NUMBER_DIVIDE(num, 10);
			}
		}
	}

	if (string[i] != '\0') {
		// Trailing invalid characters

		return FAILURE;
	}

	if (sign == SIGN_NEGATIVE) {
		// Convert number to negative

		NUMBER_MULTIPLY(num, -1);
	}

	return num;
}
