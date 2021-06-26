// This is in testArea !!!
// 
// **pass1&2 assembler**
// Author: 4108056005 Andy Chiang
// Time: 2021/06/15
// Line: 555 
// Description: This is a program for Assembly-Language and System-Software final project.
// This program can complete pass1 and pass2, then convert source code (assembly language) into object code.
// Input(pass1): source code, opcode table.
// Output(pass1): intermediate file, symbol table.
// Input(pass2): intermediate file, symbol table, opcode table.
// Output(pass2): new source code, object code.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SOURCE_FILE "source.txt"
#define OPCODE_FILE "opcode.txt"
#define INTER_FILE "intermediate.txt"
#define SYMTAB_FILE "symbol_table.txt"
#define NEWSRC_FILE "new_source.txt"
#define OBJ_FILE "object.txt"
#define WORD_SIZE 10
#define SYM_SIZE 100
#define TEXT_RECD 10

FILE * fsrc;
FILE * fop;
FILE * finter;
FILE * fsym;
FILE * fnew;
FILE * fobj;

int line = 0;	// current line in source or intermediate text file
int objLength = 0;	// object length from pass1

int pass1();	// pass1 assembler
int pass2();	// pass2 assembler
int hexToDec(char* hex);	// convert from hexadecimal string to decimail int
int objectCode(int opcode, int x, int addr);	// create object code

int main(void) {
	
	printf("-----pass 1 START-----\n\n");
	
	// start read file for pass1
	fsrc = fopen(SOURCE_FILE, "r");
    if(fsrc == NULL) {
        printf("Error: %s not found...\n", SOURCE_FILE);
		exit(-1);
    }
    fop = fopen(OPCODE_FILE, "r");
    if(fop == NULL) {
        printf("Error: %s not found...\n", OPCODE_FILE);
        exit(-1);
    }
    finter = fopen(INTER_FILE, "w");
    if(finter == NULL) {
        printf("Error: %s not found...\n", INTER_FILE);
        exit(-1);
    }
    fsym = fopen(SYMTAB_FILE, "w");
    if(fsym == NULL) {
        printf("Error: %s not found...\n", SYMTAB_FILE);
        exit(-1);
    }
    
	if(pass1() != 0) {
		printf("Error: error in pass1\n");
		exit(-1);
	}
	
	// close file for pass1
	fclose(fsrc);
    fclose(fop);
    fclose(finter);
    fclose(fsym);
    
    printf("\n-----pass 1 END-----\n\n");
    
    printf("-----pass 2 START-----\n\n");
	
	// start read file for pass2
	finter = fopen(INTER_FILE, "r");
    if(finter == NULL) {
        printf("Error: %s not found...\n", INTER_FILE);
        exit(-1);
    }
    fop = fopen(OPCODE_FILE, "r");
    if(fop == NULL) {
        printf("Error: %s not found...\n", OPCODE_FILE);
        exit(-1);
    }
    fsym = fopen(SYMTAB_FILE, "r");
    if(fsym == NULL) {
        printf("Error: %s not found...\n", SYMTAB_FILE);
        exit(-1);
    }
    fnew = fopen(NEWSRC_FILE, "w");
    if(fnew == NULL) {
        printf("Error: %s not found...\n", NEWSRC_FILE);
        exit(-1);
    }  
    fobj = fopen(OBJ_FILE, "w");
    if(fobj == NULL) {
        printf("Error: %s not found...\n", OBJ_FILE);
        exit(-1);
    }
	   
    if(pass2() != 0) {
		printf("Error: error in pass2\n");
		exit(-1);
	}
    
    // close file for pass2
    fclose(finter);
    fclose(fop);
    fclose(fsym);
    fclose(fnew);
    fclose(fobj);
    
    printf("\n-----pass 2 END-----\n\n");
    
    system("PAUSE");
    return 0;
}

int pass1() {
    
    char SYMTAB_LABEL[SYM_SIZE][WORD_SIZE];
    int SYMTAB_ADDR[SYM_SIZE];
    char LABEL[WORD_SIZE], OPCODE[WORD_SIZE], OPERAND[WORD_SIZE];
    int i, j, startAddr = 0, LOCCTR = 0, symtabTop = 0, optabTop = 0;
    line = 1;
    
    // read first line
    fscanf(fsrc, "%s\t%s\t%s", LABEL, OPCODE, OPERAND);
    
    // start pass1
    if(strcmp(OPCODE, "START") == 0) {
    	startAddr = hexToDec(OPERAND);
    	LOCCTR = startAddr;
//    	printf("%X\t%s\t%s\t%s\n", LOCCTR, LABEL, OPCODE, OPERAND);
    	fprintf(finter, "%X\t%s\t%s\t%s\n", LOCCTR, LABEL, OPCODE, OPERAND);
	}
	else {
		printf("Error: No START at first line, in line %d\n", line);
		return -1;
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
	
	// read next line
	line++;
	getc(fsrc);
	while(true) {
		// read label, opcode and operand in one line
		int state = 0;
		bool isSpace = false;
		strcpy(LABEL, "");
		strcpy(OPCODE, "");
		strcpy(OPERAND, "");
		while(true) {
			char c = getc(fsrc);
			if(c == '\n' || c == EOF) {
				break;
			}
			if(isSpace) {
				if(c != ' ' && c != '\t') {
					isSpace = false;
				}
			}
			else {
				if(state != 2 && (c == ' ' || c == '\t')) {
					isSpace = true;
					state++;
				}
			}
			if(!isSpace) {
				if(state == 0) {
					strncat(LABEL, &c, 1);
				}
				else if(state == 1) {
					strncat(OPCODE, &c, 1);
				}
				else if(state == 2) {
					strncat(OPERAND, &c, 1);
				}	
			}
		}
		// skip comment
		if(LABEL[0] == '#') {	
			continue;
		}
		
		// last line
		if(strcmp(OPCODE, "END") == 0) {
//			printf("\t%s\t%s\t%s\n", LABEL, OPCODE, OPERAND);
			fprintf(finter, "\t%s\t%s\t%s\n", LABEL, OPCODE, OPERAND);
			objLength = LOCCTR-startAddr;
			break;
		}
		
		// write intermediate file
//		printf("%04X\t%s\t%s\t%s\n", LOCCTR, LABEL, OPCODE, OPERAND);
		fprintf(finter, "%04X\t%s\t%s\t%s\n", LOCCTR, LABEL, OPCODE, OPERAND);
		
		// add label and address into symbol table
		if(strcmp(LABEL, "") != 0) {
			// check duplicate label
			for(i = 0; i < symtabTop; i++) {
				if(strcmp(LABEL, SYMTAB_LABEL[i]) == 0) {
					printf("Error: duplicate label '%s', in line %d\n", LABEL, line);
					return -1;
				}
			}
			strcpy(SYMTAB_LABEL[symtabTop], LABEL);
			SYMTAB_ADDR[symtabTop] = LOCCTR;
			symtabTop++;
		}
		
		// calculate next location counter
		bool isFoundOP = false;
		for(i = 0; i < optabTop; i++) {
			// search opcode in optable
			if(strcmp(OPCODE, OPTAB_OP[i]) == 0) {
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
				printf("Error: invalid byte type '%c', in line %d\n", OPERAND[0], line);
				return -1;
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
			printf("Error: invalid opcode '%s', in line %d\n", OPCODE, line);
			return -1;
		}
		line++;
	}
	printf("%s complete!\n", INTER_FILE);	
	
	// show symbol table & write symbol table file
//	printf("label\taddress\n");
	for(i = 0; i < symtabTop; i++) {
//		printf("%s\t%04X\n", SYMTAB_LABEL[i], SYMTAB_ADDR[i]);
		fprintf(fsym, "%s\t%04X\n", SYMTAB_LABEL[i], SYMTAB_ADDR[i]);
	}
	printf("%s complete!\n", SYMTAB_FILE);
	
    return 0;
}

int pass2() {
	
    char LABEL[WORD_SIZE], OPCODE[WORD_SIZE], OPERAND[WORD_SIZE], ALL_OPERAND[WORD_SIZE];
    int i, j, startAddr = 0, LOCCTR = 0, symtabTop = 0, optabTop = 0;
    line = 1;
    
    // read first line
    fscanf(finter, "%X%s%s%s", &LOCCTR, LABEL, OPCODE, OPERAND);
    startAddr = LOCCTR;
    
    // start pass2
    if(strcmp(OPCODE, "START") == 0) {
    	startAddr = hexToDec(OPERAND);
    	LOCCTR = startAddr;
//    	printf("%X\t%s\t%s\t%s\n", LOCCTR, LABEL, OPCODE, OPERAND);
    	fprintf(fnew, "%X\t%s\t%s\t%s\n", LOCCTR, LABEL, OPCODE, OPERAND);
//    	printf("H%-6s%06s%06X\n", LABEL, OPERAND, objLength);
		fprintf(fobj, "H%-6s%06s%06X\n", LABEL, OPERAND, objLength);	
	}
	else {
		printf("Error: No START at first line, in line %d\n", line);
		return -1;
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
	int objNum = 0, objLen = 0;
	char textRecord[TEXT_RECD*12];
	strcpy(textRecord, "");
	line++;
	getc(finter);
	while(true) {
		// read location counter, label, opcode and operand in one line
		int state = 0, X = 0;
		strcpy(LABEL, "");
		strcpy(OPCODE, "");
		strcpy(OPERAND, "");
		strcpy(ALL_OPERAND, "");
		LOCCTR = 0;
		char tempLOC[WORD_SIZE] = "";
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
		
		// end of intermediate file
		if(strcmp(OPCODE, "END") == 0) {
//			printf("\t%s\t%s\t%s\n", LABEL, OPCODE, ALL_OPERAND);
			fprintf(fnew, "\t%s\t%s\t%s\n", LABEL, OPCODE, ALL_OPERAND);
//			printf("%02X", objLen);
//			printf("%s\n", textRecord);
//			printf("E%06X\n", startAddr);
			fprintf(fobj, "%02X", objLen);
			fprintf(fobj, "%s\n", textRecord);
			fprintf(fobj, "E%06X\n", startAddr);
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
					operAddr = SYMTAB_ADDR[j];
				}
				else {	// not find
					printf("Error: undefined symbol \'%s\', in line %d\n", OPERAND, line);
					operAddr = 0;
					return -1;
				}
			}
			else {	// no operand
				operAddr = 0;
			}
			objCode = objectCode(OPTAB_CODE[i], X, operAddr);
			// concatenate opcode behind text record
			objLen += 3;
			char temp[6];
			sprintf(temp, "%06X", objCode);
			strncat(textRecord, temp, 6);
		}
		else if(strcmp(OPCODE, "BYTE") == 0) {	// BYTE
			int operLen = 0, tempLen = 0;
			if(OPERAND[0] == 'C') {	// character
				// get the value inside ''
				for(i = 2; i < WORD_SIZE; i++) {
					if(OPERAND[i] == '\'') {
						break;
					}
					operLen++;
					objCode = objCode*0x100 + OPERAND[i];
				}
				objLen += operLen;	// one character is one byte
				char temp[operLen*2];
				sprintf(temp, "%06X", objCode);
				strncat(textRecord, temp, operLen*2);	// concatenate opcode behind text record
			}
			else if(OPERAND[0] == 'X') {	// hexadecimal
				char hexOPER[WORD_SIZE] = "";
				// get the value inside ''
				for(i = 2; i < WORD_SIZE; i++) {
					if(OPERAND[i] == '\'') {
						break;
					}
					operLen++;
					strncat(hexOPER, &OPERAND[i], 1);
				}
				objCode = hexToDec(hexOPER);
				objLen += (operLen+1)/2;	// two hexadecimal is one byte
				strncat(textRecord, hexOPER, operLen);	// concatenate opcode behind text record
			}
			else {
				printf("Error: invalid byte type '%c', in line %d\n", OPERAND[0], line);
				return -1;
			}
		}
		else if(strcmp(OPCODE, "WORD") == 0) {	// WORD
			objCode = objectCode(0, 0, atoi(OPERAND));
			objLen += 3;
			char temp[6];
			sprintf(temp, "%06X", objCode);
			strncat(textRecord, temp, 6);	// concatenate opcode behind text record
		}
		else if(strcmp(OPCODE, "RESW") == 0 || strcmp(OPCODE, "RESB") == 0) {
			// nothing to do
		}
		else {
			printf("Error: undefined opcode \'%s\', in line %d\n", OPCODE, line);
		}
		
		// count number of object code
		if(objNum == 0) {
			// write T and starting address
//			printf("T%06X", LOCCTR);
			fprintf(fobj, "T%06X", LOCCTR);
		}
		objNum++;
		// max object number is 10 in this case
		// if over 10, then change to next line
		if(objNum == TEXT_RECD) {
			// write length of object code and object code
//			printf("%02X", objLen);
//			printf("%s\n", textRecord);
			fprintf(fobj, "%02X", objLen);
			fprintf(fobj, "%s\n", textRecord);
			objNum = 0;
			objLen = 0;
			strcpy(textRecord, "");
		}
		
		// write new source file
		if(strcmp(OPCODE, "RESW") == 0 || strcmp(OPCODE, "RESB") == 0) {
//			printf("%X\t%s\t%s\t%-15s\n", LOCCTR, LABEL, OPCODE, ALL_OPERAND);
			fprintf(fnew, "%X\t%s\t%s\t%-15s\n", LOCCTR, LABEL, OPCODE, ALL_OPERAND);		
		}
		else if(strcmp(OPCODE, "BYTE") == 0) {
//			printf("%X\t%s\t%s\t%-15s%02X\n", LOCCTR, LABEL, OPCODE, ALL_OPERAND, objCode);
			fprintf(fnew, "%X\t%s\t%s\t%-15s%X\n", LOCCTR, LABEL, OPCODE, ALL_OPERAND, objCode);				
		}
		else {
//			printf("%X\t%s\t%s\t%-15s%06X\n", LOCCTR, LABEL, OPCODE, ALL_OPERAND, objCode);
			fprintf(fnew, "%X\t%s\t%s\t%-15s%06X\n", LOCCTR, LABEL, OPCODE, ALL_OPERAND, objCode);	
		}

		line++;
	}
	printf("%s complete!\n", NEWSRC_FILE);
	printf("%s complete!\n", OBJ_FILE);
    
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
        	printf("Error: Wrong hexadecimal format \'%c\', in line %d\n", hex[i], line);
        	exit(-1);
		}
		dec = dec*16 + val;
	}
	
	return dec;
}

int objectCode(int opcode, int x, int addr) {
	return opcode*0x10000 + x*0x8000 + addr;
}
