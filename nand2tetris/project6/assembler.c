#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VARIABLE_ADDRESS 1024
#define LINE_BUFFER 1024

typedef struct {
	char* symbol;
	int instruction;
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

int parser(char* line, int size, FILE* file);
int parse_line(char* line);
void intt16b(int num, char *binaryStr, int bits);
void convert_cmp(int* instruction, char* cmp);
void convert_dest(int* instruction, char* cmp);
void convert_jmp(int* instruction, char* cmp);

int parser(char* line, int size, FILE* file) {
	while(fgets(line, size, file)) {
		switch(line[0]) {
			case '\n':
			case ' ':
			case '/':
				continue;
			default:
				parse_line(line);
		}
	}

	return 0;
}

int parse_line(char* line) {
	// Assume that the instruction is a c instruction
	static enum State currentState = C_INSTRUCTION;
	char* pos = &line[0]; // get the memory address of the array so we can iterate over it
			      
	char buffer[LINE_BUFFER];
	int buffer_c = 0;

	int instruction = 0b1110000000000000;
	while(*pos != '\0')
	{
		switch(*pos) {
			case '/':
			case ' ':
			break;
			case '\n':
				// Check if the buffer is 
				buffer[buffer_c++] = '\0';
				buffer_c = 0;

				switch(currentState) {
					case COMP:
						convert_cmp(&instruction, buffer);
					break;
					case JUMP:
						convert_jmp(&instruction, buffer);
					break;
					case A_REGISTER:
						char* end;
						int constant = (int)strtol(buffer, &end, 10);
						if ((end == buffer) || (*end != '\0')) {
							// process symbol
						}
						else {
							// process constant
							instruction = instruction + constant;
						}
				}
			break;
			case '@':
				// instruction is an A instruction
				instruction = instruction & 0b0000000000000000;
				currentState = A_REGISTER;
			break;
			case '=':
				// process last buffer as dest
				currentState = COMP;
				buffer[buffer_c++] = '\0';
				buffer_c = 0;
				convert_dest(&instruction, buffer);
			break;
			case ';':
				// process last buffer as comp
				currentState = JUMP;
				buffer[buffer_c++] = '\0';
				buffer_c = 0;
				convert_cmp(&instruction, buffer);
			break;
			default:

				buffer[buffer_c++] = *pos;
		}
		pos++;
	}

	char buf[16];
	intt16b(instruction, buf, 16);
	printf("%s\n", buf);
	return instruction;
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

int main(int argc, char *argv[]) {
	FILE* asmFile;
	char fileContents[1024];

	// Assume that the first argument is the filename
	asmFile = fopen(argv[1], "r");

	// Check if the file opened successfully
	if (asmFile == NULL) {
		printf("File at location '%s' could not be opened.\n", argv[1]);

		return -1;
	}
	else {
		parser(fileContents, 1024, asmFile);
	}

	fclose(asmFile);
	return 0;
}
