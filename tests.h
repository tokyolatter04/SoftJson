
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <Windows.h>

#include "include/softjson.h"

void run_sample_test(char *filepath)
{
	JsonHandler handler = soft_create_handler();

	// Read File

	char *source;

	if (!file_read_all_text(filepath, &source))
	{
		return;
	}

	// Parse

	JsonValue value = soft_load_string(&handler, source);

	/*
		File starts with:
			'y' - Parser must accept input
			'n' - Parser must reject input
			'i' - Parser can either accept or reject input
	*/

	if (!handler.error.exists)
	{
		if (filepath[14] == 'y' || filepath[14] == 'i')
		{
			// Parser passed a test it was supposed to pass (result: SUCCESS)

			printf("<passed: YES, result: PARSE_SUCCESS, file: %s>\n", filepath);
		}
		else
		{
			// Parser passed a test it was supposed to fail (result: FAILURE)

			printf("<passed: NO, result: PARSE_FAILURE, file: %s>\n", filepath);
		}

		json_value_free(&value, TRUE);
	}
	else
	{
		if (filepath[14] == 'n' || filepath[14] == 'i')
		{
			// Parser failed a test it was supposed to fail (result: SUCCESS)

			printf("<passed: NO, result: PARSE_SUCCESS, file: %s, log: %s>\n", filepath, handler.error.log);
		}
		else
		{
			// Parser failed a test it was supposed to pass (result: FAILURE)

			printf("<passed: YES, result: PARSE_FAILURE, file: %s, log: %s>\n", filepath, handler.error.log);
		}
	}
}

void run_all_sample_tests()
{
	WIN32_FIND_DATA fdata;
	HANDLE hFind;

	hFind = FindFirstFile(L"tests/samples/*", &fdata);

	while (FindNextFile(hFind, &fdata) != 0)
	{
		static char filepath[256];
		snprintf(filepath, 256, "tests/samples/%ws", fdata.cFileName);

		run_sample_test(filepath);
	}

	FindClose(hFind);
}

void run_load_string_test()
{
	char *string = "{				\
		\"name\": \"John\",			\
		\"age\": 20,				\
		\"gender\": \"male\",		\
		\"occupation\": null,		\
		\"hobbies\": [				\
			\"Cycling\",			\
			\"Swimming\"			\
		]							\
	}";

	JsonHandler handler = soft_create_handler();

	clock_t begin = clock();
	JsonValue value = soft_load_string(&handler, string);
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	if (handler.error.exists)
	{
		printf("[TEST_ERROR] Time: %g ms, At: run_load_string_test, Log: %s\n", time_spent, handler.error.log);
	}
	else
	{
		printf("[TEST_SUCCESS] Time: %g ms, At: run_load_string_test\n", time_spent);

		json_value_free(&value, TRUE);
	}
}

void run_load_file_test()
{
	JsonHandler handler = soft_create_handler();

	clock_t begin = clock();
	JsonValue value = soft_load_file(&handler, "tests/run_load_file_test.json");
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	if (handler.error.exists)
	{
		printf("[TEST_ERROR] Time: %g ms, At: run_load_string_test, Log: %s\n", time_spent, handler.error.log);
	}
	else
	{
		printf("[TEST_SUCCESS] Time: %g ms, At: run_load_string_test\n", time_spent);

		json_value_free(&value, TRUE);
	}
}

void run_dump_string_test()
{
	// Build Data

	JsonList hobbies = json_create_list();

	json_list_add(&hobbies, json_create_string_value("Cycling"));
	json_list_add(&hobbies, json_create_string_value("Swimming"));

	JsonObject person = json_create_object();

	json_object_add(&person, "name", json_create_string_value("John"));
	json_object_add(&person, "age", json_create_int_value(20));
	json_object_add(&person, "gender", json_create_string_value("male"));
	json_object_add(&person, "occupation", json_create_null_value());
	json_object_add(&person, "hobbies", json_create_list_value(hobbies));

	// Serialise

	JsonHandler handler = soft_create_handler();

	clock_t begin = clock();
	soft_dump_string(&handler, json_create_object_value(person));
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	if (handler.error.exists)
	{
		printf("[TEST_ERROR] Time: %g ms, At: run_dump_string_test, Log: %s\n", time_spent, handler.error.log);
	}
	else
	{
		printf("[TEST_SUCCESS] Time: %g ms, At: run_dump_string_test\n", time_spent);

		json_value_free(&person, FALSE);
	}
}

void run_dump_file_test()
{
	// Build Data

	JsonList hobbies = json_create_list();

	json_list_add(&hobbies, json_create_string_value("Cycling"));
	json_list_add(&hobbies, json_create_string_value("Swimming"));

	JsonObject person = json_create_object();

	json_object_add(&person, "name", json_create_string_value("John"));
	json_object_add(&person, "age", json_create_int_value(20));
	json_object_add(&person, "gender", json_create_string_value("male"));
	json_object_add(&person, "occupation", json_create_null_value());
	json_object_add(&person, "hobbies", json_create_list_value(hobbies));

	// Write to File

	JsonHandler handler = soft_create_handler();
	soft_dump_file(&handler, json_create_object_value(person), "tests/run_dump_file_test.json");

	if (handler.error.exists)
	{
		printf("[TEST_ERROR] At: run_dump_file_test, Log: %s\n", handler.error.log);
	}
	else
	{
		printf("[TEST_SUCCESS] At: run_dump_file_test\n");

		json_object_free(&person, FALSE);
	}
}

void run_all_tests()
{
	run_load_string_test();
	run_load_file_test();
	run_dump_string_test();
	run_dump_file_test();
	run_all_sample_tests();
}
