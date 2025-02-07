#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VARIABLE_ADDRESS 16
#define LINE_BUFFER 1024
#define MAX_SYMBOLS 2048
#define MAX_FILE_SIZE 2048 * 2048

typedef struct {
	char symbol[MAX_SYMBOLS];
	int value;
} Symbol;

enum State {
	NONE,
	COMMENT,
	WHITESPACE,
	A_REGISTER,
	SYMBOL,
	A_INSTRUCTION,
	C_INSTRUCTION,
	DEST,
	COMP,
	JUMP
};

char* parser(char* line, int size, FILE* file);
char* parse_line(char* line, int* current_line);
void intt16b(int num, char *binaryStr, int bits);
void convert_cmp(int* instruction, char* cmp);
void convert_dest(int* instruction, char* cmp);
void convert_jmp(int* instruction, char* cmp);
int lookupSymbolValue(char* symbol);
int isSymbolValue(char* symbol);
void setSymbolValue(char* symbol, int value);

Symbol symbol_table[MAX_SYMBOLS];
int symbol_c = 0;
int variable_c = VARIABLE_ADDRESS;
int is_second_pass = 0;

char* parser(char* line, int size, FILE* file) {
	int current_line = 0;
	variable_c = VARIABLE_ADDRESS;
	char* output = malloc(MAX_FILE_SIZE);

	while(fgets(line, size, file)) {
		switch(line[0]) {
			case '\n':
			case '/':
				continue;
			default:
				char* line_output = parse_line(line, &current_line);
				if (strlen(line_output) > 0) {
					strcat(output, line_output);
					strcat(output, "\n");
				}
		}
	}

	if (is_second_pass)
		printf("%s", output);

	is_second_pass = 1;
	return output;
}

char* parse_line(char* line, int* current_line) {
	// Assume that the instruction is a c instruction
	static enum State current_state = C_INSTRUCTION;
	char* pos = &line[0]; // get the memory address of the array so we can iterate over it
			      
	int shouldRun = 1; // determines if the line should print/write an instruction

	char buffer[LINE_BUFFER];
	int buffer_c = 0;

	int string_end = 0;

	int instruction = 0b1110000000000000;
	while(!string_end && shouldRun)
	{
		switch(*pos) {
			case ' ':
			break;
			case '/':
				// Ignore this line if two //
				if (current_state == COMMENT)
					shouldRun = 0;

				current_state = COMMENT;
			break;
			case '\0':
			case '\n':
				// Check if the buffer is 
				buffer[buffer_c++] = '\0';
				buffer_c = 0;

				switch(current_state) {
					case COMP:
						convert_cmp(&instruction, buffer);
					break;
					case JUMP:
						convert_jmp(&instruction, buffer);

						char dbg[16];
						intt16b(instruction, dbg, 16);
					break;
					case SYMBOL:
						// A symbol has been defined here
						if (!isSymbolValue(buffer)) {
							strcpy(symbol_table[symbol_c].symbol, buffer);
							symbol_table[symbol_c++].value = *current_line;
						}
						else {
							// Update the current table value to have this line value
							setSymbolValue(buffer, *current_line);
						}

						shouldRun = 0;
					break;
					case A_REGISTER:
						char* end;

						// Attempt to convert buffer to int
						int constant = (int)strtol(buffer, &end, 10);
						if ((end == buffer) || (*end != '\0')) {
							// process symbol
							if (isSymbolValue(buffer)) {
								int location = lookupSymbolValue(buffer);
								instruction = instruction + location;
							}
							else {
								if (!isSymbolValue(buffer)) {
									strcpy(symbol_table[symbol_c].symbol, buffer);
									if (is_second_pass) {
										symbol_table[symbol_c++].value = variable_c++;

										int location = lookupSymbolValue(buffer);
										instruction = instruction + location;
									}
								}
								else {
									int location = lookupSymbolValue(buffer);
									instruction = instruction + location;
								}
							}
						}
						else {
							// process constant
							instruction = instruction + constant;
						}
				}
				
				string_end = 1;
			break;
			case '@':
				// instruction is an A instruction
				instruction = instruction & 0b0000000000000000;
				current_state = A_REGISTER;
			break;
			case '=':
				// process last buffer as dest
				current_state = COMP;
				buffer[buffer_c++] = '\0';
				buffer_c = 0;
				convert_dest(&instruction, buffer);
			break;
			case ';':
				// process last buffer as comp
				current_state = JUMP;
				buffer[buffer_c++] = '\0';
				buffer_c = 0;
				convert_cmp(&instruction, buffer);
			break;
			case '(':
			break;
			case ')':
				current_state = SYMBOL;
			break;
			default:
				switch (current_state) {
					case COMMENT:
					break;
					default:
					buffer[buffer_c++] = *pos;
				}
		}
		pos++;
	}

	char* buf = malloc(16);
	intt16b(instruction, buf, 16);

	if (shouldRun)
	{
		(*current_line)++;

		if (is_second_pass)
			return buf;
	}
	return malloc(1);
}

void convert_dest(int* instruction, char* cmp) {
	if (strcmp(cmp, "M") == 0)
		*instruction = *instruction | (0b001 << 3);
	if (strcmp(cmp, "D") == 0)
		*instruction = *instruction | (0b010 << 3);
	if (strcmp(cmp, "MD") == 0)
		*instruction = *instruction | (0b011 << 3);
	if (strcmp(cmp, "A") == 0)
		*instruction = *instruction | (0b100 << 3);
	if (strcmp(cmp, "AM") == 0)
		*instruction = *instruction | (0b101 << 3);
	if (strcmp(cmp, "AD") == 0)
		*instruction = *instruction | (0b110 << 3);
	if (strcmp(cmp, "AMD") == 0)
		*instruction = *instruction | (0b111 << 3);
}

void convert_jmp(int* instruction, char* cmp) {
	if (strcmp(cmp, "JGT") == 0)
		*instruction = *instruction | 0b001;
	else if (strcmp(cmp, "JEQ") == 0)
		*instruction = *instruction | 0b010;
	else if (strcmp(cmp, "JGE") == 0)
		*instruction = *instruction | 0b011;
	else if (strcmp(cmp, "JLT") == 0)
		*instruction = *instruction | 0b100;
	else if (strcmp(cmp, "JNE") == 0)
		*instruction = *instruction | 0b101;
	else if (strcmp(cmp, "JLE") == 0)
		*instruction = *instruction | 0b110;
	else if (strcmp(cmp, "JMP") == 0)
		*instruction = *instruction | 0b111;
}

void convert_cmp(int* instruction, char* cmp) {
	// unfortunately c can't compare strings in a switch statement
	// so have to resort to an if else ladder :(
	if (strcmp(cmp, "0") == 0)
		*instruction = *instruction | (0b0101010 << 6);
	else if (strcmp(cmp, "1") == 0)
		*instruction = *instruction | (0b0111111 << 6);
	else if (strcmp(cmp, "-1") == 0)
		*instruction = *instruction | (0b0111010 << 6);
	else if (strcmp(cmp, "D") == 0)
		*instruction = *instruction | (0b0001100 << 6);
	else if (strcmp(cmp, "A") == 0)
		*instruction = *instruction | (0b0110000 << 6);
	else if (strcmp(cmp, "!D") == 0)
		*instruction = *instruction | (0b0001101 << 6);
	else if (strcmp(cmp, "!A") == 0)
		*instruction = *instruction | (0b0110001 << 6);
	else if (strcmp(cmp, "-D") == 0)
		*instruction = *instruction | (0b0001111 << 6);
	else if (strcmp(cmp, "-A") == 0)
		*instruction = *instruction | (0b0110011 << 6);
	else if (strcmp(cmp, "D+1") == 0)
		*instruction = *instruction | (0b0011111 << 6);
	else if (strcmp(cmp, "A+1") == 0)
		*instruction = *instruction | (0b0110111 << 6);
	else if (strcmp(cmp, "D-1") == 0)
		*instruction = *instruction | (0b0001110 << 6);
	else if (strcmp(cmp, "A-1") == 0)
		*instruction = *instruction | (0b0110010 << 6);
	else if (strcmp(cmp, "D+A") == 0)
		*instruction = *instruction | (0b0000010 << 6);
	else if (strcmp(cmp, "D-A") == 0)
		*instruction = *instruction | (0b0010011 << 6);
	else if (strcmp(cmp, "A-D") == 0)
		*instruction = *instruction | (0b0000111 << 6);
	else if (strcmp(cmp, "D&A") == 0)
		*instruction = *instruction | (0b0000000 << 6);
	else if (strcmp(cmp, "D|A") == 0)
		*instruction = *instruction | (0b0010101 << 6);
	else if (strcmp(cmp, "M") == 0)
		*instruction = *instruction | (0b1110000 << 6);
	else if (strcmp(cmp, "!M") == 0)
		*instruction = *instruction | (0b1110001 << 6);
	else if (strcmp(cmp, "-M") == 0)
		*instruction = *instruction | (0b1110011 << 6);
	else if (strcmp(cmp, "M+1") == 0)
		*instruction = *instruction | (0b1110111 << 6);
	else if (strcmp(cmp, "M-1") == 0)
		*instruction = *instruction | (0b1110010 << 6);
	else if (strcmp(cmp, "D+M") == 0)
		*instruction = *instruction | (0b1000010 << 6);
	else if (strcmp(cmp, "D-M") == 0)
		*instruction = *instruction | (0b1010011 << 6);
	else if (strcmp(cmp, "M-D") == 0)
		*instruction = *instruction | (0b1000111 << 6);
	else if (strcmp(cmp, "D&M") == 0)
		*instruction = *instruction | (0b1000000 << 6);
	else if (strcmp(cmp, "D|M") == 0)
		*instruction = *instruction | (0b1010101 << 6);
}

void intt16b(int num, char *binaryStr, int bits) {
	for (int i = bits - 1; i >= 0; i--) {
		binaryStr[bits - 1 - i] = (num & (1<< i)) ? '1' : '0';
	}
	binaryStr[bits] = '\0';
}

int lookupSymbolValue(char* symbol) {
	for (int i = symbol_c - 1; i >= 0; i--) {
		if (strcmp(symbol_table[i].symbol, symbol) == 0) {
			return symbol_table[i].value;
		}
	}
	return -1;
}

void setSymbolValue(char* symbol, int value) {
	for (int i = symbol_c - 1; i >= 0; i--) {
		if (strcmp(symbol_table[i].symbol, symbol) == 0) {
			symbol_table[i].value = value;
		}
	}
}

int isSymbolValue(char* symbol) {
	for (int i = symbol_c - 1; i >= 0; i--) {
		if (strcmp(symbol_table[i].symbol, symbol) == 0) {
			return 1;
		}
	}

	return 0;
}

int main(int argc, char *argv[]) {
	FILE* asmFile;
	char fileContents[1024];

	// Assume that the first argument is the filename
	asmFile = fopen(argv[1], "r");

	// define default symbols
	for(int i = symbol_c; i <= 16; i++) {
		snprintf(symbol_table[i].symbol, MAX_SYMBOLS,"R%d", i);
		symbol_table[i].value = i;
		symbol_c = i;
	}

	strcpy(symbol_table[symbol_c].symbol, "SCREEN");
	symbol_table[symbol_c++].value = 16384;

	strcpy(symbol_table[symbol_c].symbol, "KBD");
	symbol_table[symbol_c++].value = 24576;

	strcpy(symbol_table[symbol_c].symbol, "SP");
	symbol_table[symbol_c++].value = 0;

	strcpy(symbol_table[symbol_c].symbol, "LCL");
	symbol_table[symbol_c++].value = 1;

	strcpy(symbol_table[symbol_c].symbol, "ARG");
	symbol_table[symbol_c++].value = 2;

	strcpy(symbol_table[symbol_c].symbol, "THIS");
	symbol_table[symbol_c++].value = 3;

	strcpy(symbol_table[symbol_c].symbol, "THAT");
	symbol_table[symbol_c++].value = 4;

	// Check if the file opened successfully
	if (asmFile == NULL) {
		printf("File at location '%s' could not be opened.\n", argv[1]);

		return -1;
	}
	else {
		// Pass twice
		parser(fileContents, 1024, asmFile);
		rewind(asmFile);
		char* hackContents = parser(fileContents, 1024, asmFile);

		FILE* hackFile;

		hackFile = fopen(argv[2], "w");

		if (hackFile == NULL) {
			printf("File %s could not be written.\n", argv[2]);

			return -1;
		}
		else {
			fprintf(hackFile, "%s", hackContents);

			fclose(hackFile);
		}

	}

	fclose(asmFile);
	return 0;
}
