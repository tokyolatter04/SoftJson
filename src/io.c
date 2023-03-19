
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/internal/io.h"

uint8 file_read_all_text(char* filepath, char** text) {
	FILE* fp = fopen(filepath, "rb");

	if (!fp) {
		// Error accessing filepath

		return FAILURE;
	}

	// Get allocation length

	fseek(fp, 0, SEEK_END);
	uint64 fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// Read bytes into a buffer & close file

	char* data = malloc(sizeof(char) * (fsize + 1));
	fread(data, sizeof(char), fsize, fp);
	fclose(fp);

	data[fsize] = '\0';
	*text = data;

	return SUCCESS;
}

uint8 file_write_all_text(char* filepath, char* text) {
	FILE* fp = fopen(filepath, "wb");

	if (!fp) {
		// Error accessing filepath

		return FAILURE;
	}

	// Write text & close file

	fwrite(text, sizeof(char), strlen(text), fp);
	fclose(fp);

	return SUCCESS;
}
