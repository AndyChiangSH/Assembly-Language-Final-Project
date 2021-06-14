// pass1 assembler
// author: Andy Chiang

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SOURCE_FILE "source.txt"
#define OPCODE_FILE "opcode.txt"
#define INTER_FILE "intermediate.txt"
#define SYMTAB_FILE "symbol_table.txt"
#define WORD_SIZE 10
#define SYM_SIZE 100

int hexToDec(char* hex);	// convert from hexadecimal string to decimail int

int main() {
	FILE * fsrc;
	FILE * fop;
	FILE * finter;
	FILE * fsym;
	
	// start read file 
	fsrc = fopen(SOURCE_FILE, "r");
    if(fsrc == NULL) {
        printf("fsrc not found...");
        return 0;
    }
    else {
        printf("find fsrc!\n");
    }
    
    fop = fopen(OPCODE_FILE, "r");
    if(fop == NULL) {
        printf("fop not found...");
        return 0;
    }
    else {
        printf("find fop!\n");
    }
    
    finter = fopen(INTER_FILE, "w");
    if(finter == NULL) {
        printf("finter not found...");
        return 0;
    }
    else {
        printf("find finter!\n");
    }
    
    fsym = fopen(SYMTAB_FILE, "w");
    if(fsym == NULL) {
        printf("fsym not found...");
        return 0;
    }
    else {
        printf("find fsym!\n");
    }
    
    char SYMTAB_LABEL[SYM_SIZE][WORD_SIZE];
    int SYMTAB_ADDR[SYM_SIZE];
    char LABEL[WORD_SIZE], OPCODE[WORD_SIZE], OPERAND[WORD_SIZE];
    int i, j, k, startAddr = 0, LOCCTR = 0, symtabTop = 0, optabTop = 0;
    
    // read first line
    fscanf(fsrc, "%s\t%s\t%s", LABEL, OPCODE, OPERAND);
    
    // start pass1
    printf("-----pass 1 START-----\n\n");
    printf("-----intermediate file START-----\n");
    if(strcmp(OPCODE, "START") == 0) {
    	startAddr = hexToDec(OPERAND);
    	LOCCTR = startAddr;
    	printf("%X\t%s\t%s\t%s\n", LOCCTR, LABEL, OPCODE, OPERAND);
    	fprintf(finter, "%X\t%s\t%s\t%s\n", LOCCTR, LABEL, OPCODE, OPERAND);
	}
	else {
		return 0;
	}
    
    // initial symbol table
//    for(i = 0; i < SYM_SIZE; i++) {
//    	strcpy(SYMTAB_LABEL[i], "");
//    	SYMTAB_ADDR[i] = 0;
//	}
	
	// load opcode
	while(true) {
		char c = getc(fop);
		if(c == '\n') {
			optabTop++;
		}
		if(c == EOF) {
			break;
		}
	}
	optabTop++;
	rewind(fop);
	char OPTAB[optabTop][2][WORD_SIZE];
	for(i = 0; i < optabTop; i++) {
		char op[WORD_SIZE], code[WORD_SIZE];
    	fscanf(fop, "%s%s", OPTAB[i][0], OPTAB[i][1]);
	}
	
	// read next line
	getc(fsrc);
	while(true) {
		// read label, opcode and operand in one line
		int state = 0;
		strcpy(LABEL, "");
		strcpy(OPCODE, "");
		strcpy(OPERAND, ""); 
		while(true) {
			char c = getc(fsrc);
			if(c == '\n' || c == EOF) {
				break;
			}
			else if(c == '\t') {
				state++;
			}
			else if(state == 0) {
				strncat(LABEL, &c, 1);
			}
			else if(state == 1) {
				strncat(OPCODE, &c, 1);
			}
			else if(state == 2) {
				strncat(OPERAND, &c, 1);
			}
		}
		// skip comment
		if(LABEL[0] == '#') {	
			continue;
		}
		
		// add label and address into symbol table
		if(strcmp(LABEL, "") != 0) {
			// check duplicate label
			for(i = 0; i < symtabTop; i++) {
				if(strcmp(LABEL, SYMTAB_LABEL[i]) == 0) {
					printf("Error: duplicate label '%s'\n", LABEL);
					return 0;
				}
			}
			strcpy(SYMTAB_LABEL[symtabTop], LABEL);
			// itoa: C standard function to convert int to string
			SYMTAB_ADDR[symtabTop] = LOCCTR;
			symtabTop++;
		}
		
		// last line
		if(strcmp(OPCODE, "END") == 0) {
			printf("\t%s\t%s\t%s\n", LABEL, OPCODE, OPERAND);
			fprintf(finter, "\t%s\t%s\t%s\n", LABEL, OPCODE, OPERAND);
			break;
		}
		
		// write intermediate file
		printf("%X\t%s\t%s\t%s\n", LOCCTR, LABEL, OPCODE, OPERAND);
		fprintf(finter, "%X\t%s\t%s\t%s\n", LOCCTR, LABEL, OPCODE, OPERAND);
		
		// calculate next location counter
		bool isFoundOP = false;
		for(i = 0; i < optabTop; i++) {
			// search opcode in optable
			if(strcmp(OPCODE, OPTAB[i][0]) == 0) {
				isFoundOP = true;
				break;
			}
		}
		if(isFoundOP) {	// instruction
			LOCCTR += 3;
		}
		else if(strcmp(OPCODE, "BYTE") == 0) {	// a byte
			// length of operand
			int length = 0;
			for(i = 2; i < WORD_SIZE; i++) {
				if(OPERAND[i] == '\'') {
					break;
				}
				length++;
			}
			if(OPERAND[0] == 'C') {	// one character one byte
				LOCCTR += length;
			}
			else if(OPERAND[0] == 'X') {	// two hexadecimal one byte
				LOCCTR += (length+1)/2;
			}
			else {
				printf("Error: invalid byte type '%c'", OPERAND[0]);
				return 0;
			}
		}
		else if(strcmp(OPCODE, "WORD") == 0) {	// a word
			LOCCTR += 3;
		}
		else if(strcmp(OPCODE, "RESB") == 0) {	// reserve byte
			LOCCTR += atoi(OPERAND);
		}
		else if(strcmp(OPCODE, "RESW") == 0) {	// reserve word
			LOCCTR += 3*atoi(OPERAND);
		}
		else {
			printf("Error: invalid opcode '%s'", OPCODE);
			return 0;
		}
	}
	printf("-----intermediate file END-----\n\n");	
	
	// show symbol table & write symbol table file
	printf("-----symbol table START-----\n");
	printf("label\taddress\n");
	for(i = 0; i < symtabTop; i++) {
		printf("%s\t%X\n", SYMTAB_LABEL[i], SYMTAB_ADDR[i]);
		fprintf(fsym, "%s\t%X\n", SYMTAB_LABEL[i], SYMTAB_ADDR[i]);
	}
	printf("-----symbol table END-----\n\n");
	printf("-----pass 1 END-----\n");
    
    fclose(fsrc);
    fclose(fop);
    fclose(finter);
    fclose(fsym);
    return 0;
}

int hexToDec(char hex[]) {
	int i, dec = 0;
	for(i = 0; hex[i] != '\0'; i++) {
		int val = 0;
		if(hex[i] >= '0' && hex[i] <= '9')
        {
            val = hex[i] - 48;
        }
        else if(hex[i] >= 'A' && hex[i] <= 'F')
        {
            val = hex[i] - 65 + 10;
        }
        else if(hex[i] >= 'a' && hex[i] <= 'f')
        {
            val = hex[i] - 97 + 10;
        }
        else {
        	printf("Error: Wrong hexadecimal format \'%c\'\n", hex[i]);
        	return -1;
		}
		dec = dec*16 + val;
	}
	
	return dec;
}
