// pass2 assembler
// author: Andy Chiang

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INTER_FILE "intermediate.txt"
#define OPCODE_FILE "opcode.txt"
#define SYMTAB_FILE "symbol_table.txt"
#define NEWSRC_FILE "new_source.txt"
#define OBJ_FILE "object.txt"
#define WORD_SIZE 10
#define SYM_SIZE 100

int hexToDec(char* hex);	// convert from hexadecimal string to decimail int
int objectCode(int opcode, int x, int addr);	// create object code

int main() {
	FILE * finter;
	FILE * fop;
	FILE * fsym;
	FILE * fnew;
	FILE * fobj;
	
	// start read file 
	finter = fopen(INTER_FILE, "r");
    if(finter == NULL) {
        printf("finter not found...");
        return 0;
    }
    else {
        printf("find finter!\n");
    }
    
    fop = fopen(OPCODE_FILE, "r");
    if(fop == NULL) {
        printf("fop not found...");
        return 0;
    }
    else {
        printf("find fop!\n");
    }
    
    fsym = fopen(SYMTAB_FILE, "r");
    if(fsym == NULL) {
        printf("fsym not found...");
        return 0;
    }
    else {
        printf("find fsym!\n");
    }
    
    fnew = fopen(NEWSRC_FILE, "w");
    if(fnew == NULL) {
        printf("fnew not found...");
        return 0;
    }
    else {
        printf("fnew fsym!\n");
    }
    
    fobj = fopen(OBJ_FILE, "w");
    if(fobj == NULL) {
        printf("fobj not found...");
        return 0;
    }
    else {
        printf("fobj fsym!\n");
    }
    
    char LABEL[WORD_SIZE], OPCODE[WORD_SIZE], OPERAND[WORD_SIZE], ALL_OPERAND[WORD_SIZE];
    int i, j, k, startAddr = 0, LOCCTR = 0, symtabTop = 0, optabTop = 0;
    
    // read first line
    fscanf(finter, "%X%s%s%s", &LOCCTR, LABEL, OPCODE, OPERAND);
    
    // start pass2
    printf("-----pass 2 START-----\n\n");
    printf("-----new source file START-----\n");
    if(strcmp(OPCODE, "START") == 0) {
    	startAddr = hexToDec(OPERAND);
    	LOCCTR = startAddr;
    	printf("%X\t%s\t%s\t%s\n", LOCCTR, LABEL, OPCODE, OPERAND);
    	fprintf(fnew, "%X\t%s\t%s\t%s\n", LOCCTR, LABEL, OPCODE, OPERAND);
	}
	else {
		return 0;
	}
	
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
	
	char OPTAB_OP[optabTop][WORD_SIZE];
	int OPTAB_CODE[optabTop];
	rewind(fop);
	for(i = 0; i < optabTop; i++) {
		fscanf(fop, "%s%X", OPTAB_OP[i], &OPTAB_CODE[i]);
	}
	
	// load symbol table
	while(true) {
		char c = getc(fsym);
		if(c == '\n') {
			symtabTop++;
		}
		if(c == EOF) {
			break;
		}
	}
	
	char SYMTAB_LABEL[symtabTop][WORD_SIZE];
	int SYMTAB_ADDR[symtabTop];
	rewind(fsym);
	for(i = 0; i < symtabTop; i++) {
		fscanf(fsym, "%s\t%X", SYMTAB_LABEL[i], &SYMTAB_ADDR[i]);
	}
	
	// read next line
	getc(finter);
	while(true) {
		// read location counter, label, opcode and operand in one line
		int state = 0;
		strcpy(LABEL, "");
		strcpy(OPCODE, "");
		strcpy(OPERAND, "");
		strcpy(ALL_OPERAND, "");
		LOCCTR = 0;
		char tempLOC[WORD_SIZE] = "";
		int X = 0;
		while(true) {
			char c = getc(finter);
			if(c == '\n' || c == EOF) {
				break;
			}
			else if(c == '\t') {
				state++;
			}
			else if(state == 0) {
				strncat(tempLOC, &c, 1);
			}
			else if(state == 1) {
				strncat(LABEL, &c, 1);
			}
			else if(state == 2) {
				strncat(OPCODE, &c, 1);
			}
			else if(state == 3) {
				if(c == ',') {
					X = 1;
				}
				if(X == 0) {
					strncat(OPERAND, &c, 1);
				}
				strncat(ALL_OPERAND, &c, 1);
			}
		}
		LOCCTR = hexToDec(tempLOC);
		// skip comment
		if(LABEL[0] == '#') {	
			continue;
		}
		
		if(strcmp(OPCODE, "END") == 0) {
			printf("\t%s\t%s\t%s\n", LABEL, OPCODE, ALL_OPERAND);
			break;
		}
		
		// search OPCODE in OPTAB
		int operAddr = 0, objCode = 0;
		bool isFoundOP = false;
		for(i = 0; i < optabTop; i++) {
			if(strcmp(OPCODE, OPTAB_OP[i]) == 0) {
				isFoundOP = true;
				break;
			}
		}
		if(isFoundOP) {	// instruction
			if(strcmp(OPERAND, "") != 0) {	// if there is an operand
				// search symbol table for operand
				bool isFoundSYM = false;
				for(j = 0; j < symtabTop; j++) {
					if(strcmp(OPERAND, SYMTAB_LABEL[j]) == 0) {
						isFoundSYM = true;
						break;
					}
				}
				if(isFoundSYM) {	// find symbol in symbol table
//					printf("find defined symbol \'%s\'\n", OPERAND);
					operAddr = SYMTAB_ADDR[j];
				}
				else {	// not find
					printf("Error: undefined symbol \'%s\'\n", OPERAND);
					operAddr = 0;
					return 0;
				}
			}
			else {
				operAddr = 0;
			}
			objCode = objectCode(OPTAB_CODE[i], X, operAddr);
		}
		else if(strcmp(OPCODE, "BYTE") == 0) {
			// length of operand
			char byteOPER[WORD_SIZE] = "";
			for(i = 2; i < WORD_SIZE; i++) {
				if(OPERAND[i] == '\'') {
					break;
				}
				strncat(byteOPER, &OPERAND[i], 1);
			}
			if(OPERAND[0] == 'C') {
				for(j = 0; byteOPER[j] != '\0'; j++) {
					objCode = objCode*0x100 + byteOPER[j];
				}
			}
			else if(OPERAND[0] == 'X') {
				objCode = hexToDec(byteOPER);
			}
			else {
				printf("Error: invalid byte type '%c'", OPERAND[0]);
				return 0;
			}
		}
		else if(strcmp(OPCODE, "WORD") == 0) {
			objCode = objectCode(0, 0, atoi(OPERAND));
		}
		
//		printf("Error: undefined opcode \'%s\'\n", OPCODE);
		
		printf("%X\t%s\t%s\t%-15s%06X\n", LOCCTR, LABEL, OPCODE, ALL_OPERAND, objCode);
	}
	printf("-----new source file END-----\n\n");	
	
	printf("-----pass 2 END-----\n");
    
    fclose(finter);
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

int objectCode(int opcode, int x, int addr) {
//	printf("opcode = %X\n", opcode);
//	printf("x = %X\n", x);
//	printf("addr = %X\n", addr);
	
	int objcode = opcode*0x10000 + x*0x8000 + addr;
	
//	printf("objcode = %X\n", objcode);
	
	return objcode;
}
