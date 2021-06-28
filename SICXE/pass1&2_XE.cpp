// **pass1&2 SIC/XE assembler**
// Author: 4108056005 Andy Chiang
// Time: 2021/06/28
// Line: 790
// Description: This is a program for Assembly-Language and System-Software final project.
// This program can complete pass1 and pass2, then convert source code (assembly language) into object code.
// Input(pass1): source code, opcode table.
// Output(pass1): intermediate file, symbol table.
// Input(pass2): intermediate file, symbol table, opcode table.
// Output(pass2): new source code, object code.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SOURCE_FILE "source_XE.txt"
#define OPCODE_FILE "opcode_XE.txt"
#define INTER_FILE "intermediate_XE.txt"
#define SYMTAB_FILE "symbol_table_XE.txt"
#define NEWSRC_FILE "new_source_XE.txt"
#define OBJ_FILE "object_XE.txt"
#define WORD_SIZE 10
#define LINE_SIZE 100
#define SYM_SIZE 100
#define TRECD_SIZE 10
#define MRECD_SIZE 1000
#define REGI_LEN 9

FILE * fsrc;
FILE * fop;
FILE * finter;
FILE * fsym;
FILE * fnew;
FILE * fobj;

int line = 0;	// current line in source or intermediate text file
int objLength = 0;	// object length from pass1
char REGI[REGI_LEN][3] = {"A", "X", "L", "B", "S", "T", "F", "PC", "SW"};
short REGI_NUM[REGI_LEN] = {0, 1, 2, 3, 4, 5, 6, 8, 9};

int pass1();	// pass1 assembler
int pass2();	// pass2 assembler
int hexToDec(char*);	// convert from hexadecimal string to decimail int
unsigned int objectCodeFormat2(int, short, short);	// create object code for format2
unsigned int objectCodeFormat3(int, short, short, short, short, short, short, int);	// create object code for format3
unsigned int objectCodeFormat4(int, short, short, short, short, short, short, int);	// create object code for format4

int main(void) {
	
	printf("=====SIC/XE assembler=====\n\n");
	
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
    char LABEL[WORD_SIZE], OPCODE[WORD_SIZE], FIRST_OPERAND[WORD_SIZE], SECOND_OPERAND[WORD_SIZE], ONE_LINE[LINE_SIZE];
    int i, j, startAddr = 0, LOCCTR = 0, symtabTop = 0, optabTop = 0;
    line = 1;
    
    // read first line
    fscanf(fsrc, "%s\t%s\t%s", LABEL, OPCODE, FIRST_OPERAND);
    
    // start pass1
    if(strcmp(OPCODE, "START") == 0) {
    	startAddr = hexToDec(FIRST_OPERAND);
    	LOCCTR = startAddr;
//    	printf("%04X\t%s\t%s\t%s\n", LOCCTR, LABEL, OPCODE, FIRST_OPERAND);
    	fprintf(finter, "%04X\t%s\t%s\t%s\n", LOCCTR, LABEL, OPCODE, FIRST_OPERAND);
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
		short state = 0, e = 0;
		bool isSpace = false;
		strcpy(LABEL, "");
		strcpy(OPCODE, "");
		strcpy(FIRST_OPERAND, "");
		strcpy(SECOND_OPERAND, "");
		strcpy(ONE_LINE, "");
		while(true) {
			char c = getc(fsrc);
			if(c == '\n' || c == EOF) {
				break;
			}
			strncat(ONE_LINE, &c, 1);
			if(isSpace) {	// span column by space or tab 
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
					if(c == '+') {
						e = 1;
					}
					else {
						strncat(OPCODE, &c, 1);
					}
				}
				else if(state == 2) {
					if(c == '\'') {
						state++;
					}
					else if(c == ',') {
						state++;
					}
					else {
						strncat(FIRST_OPERAND, &c, 1);
					}
				}
				else if(state == 3) {
					if(c == '\'') {
						continue;
					}
					strncat(SECOND_OPERAND, &c, 1);
				}	
			}
		}
		// skip comment
		if(LABEL[0] == '#') {	
			continue;
		}
		// base opcode
		if(strcmp(OPCODE, "BASE") == 0) {
//			printf("\t%s\n", ONE_LINE);
			fprintf(finter, "%04X\t%s\n", LOCCTR, ONE_LINE);
			continue;
		}
		
		// last line
		if(strcmp(OPCODE, "END") == 0) {
//			printf("\t%s\n", ONE_LINE);
			fprintf(finter, "%04X\t%s\n", LOCCTR, ONE_LINE);
			objLength = LOCCTR-startAddr;
			break;
		}
		
		// write intermediate file
//		printf("%04X\t%s\n", LOCCTR, ONE_LINE);
		fprintf(finter, "%04X\t%s\n", LOCCTR, ONE_LINE);
		
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
			bool isFoundREGI = false;
			for(j = 0; j < REGI_LEN; j++) {
				if(strcmp(FIRST_OPERAND, REGI[j]) == 0) {
					isFoundREGI = true;
					break;
				}
			}
			if(isFoundREGI) {
				LOCCTR += 2;
			}
			else if(e == 0) {
				LOCCTR += 3;
			}
			else {
				LOCCTR += 4;
			}
		}
		else if(strcmp(OPCODE, "BYTE") == 0) {	// a byte
			// length of operand
			int length = 0;
			for(i = 0; i < WORD_SIZE; i++) {
				if(SECOND_OPERAND[i] == '\0') {
					break;
				}
				length++;
			}
			if(strcmp(FIRST_OPERAND, "C") == 0) {	// one character one byte
				LOCCTR += length;
			}
			else if(strcmp(FIRST_OPERAND, "X") == 0) {	// two hexadecimal one byte
				LOCCTR += (length+1)/2;
			}
			else {
				printf("Error: invalid byte type '%c', in line %d\n", FIRST_OPERAND[0], line);
				return -1;
			}
		}
		else if(strcmp(OPCODE, "WORD") == 0) {	// a word
			LOCCTR += 3;
		}
		else if(strcmp(OPCODE, "RESB") == 0) {	// reserve byte
			LOCCTR += atoi(FIRST_OPERAND);
		}
		else if(strcmp(OPCODE, "RESW") == 0) {	// reserve word
			LOCCTR += 3*atoi(FIRST_OPERAND);
		}
		else {
			printf("Error: invalid opcode '%s', in line %d\n", OPCODE, line);
			return -1;
		}
		
		line++;
	}
	printf("%s complete!\n", INTER_FILE);	
	
	// show symbol table & write symbol table file
	for(i = 0; i < symtabTop; i++) {
//		printf("%s\t%04X\n", SYMTAB_LABEL[i], SYMTAB_ADDR[i]);
		fprintf(fsym, "%s\t%04X\n", SYMTAB_LABEL[i], SYMTAB_ADDR[i]);
	}
	for(i = 0; i < REGI_LEN; i++) {
//		printf("%s\t%04X\n", SYMTAB_LABEL[i], SYMTAB_ADDR[i]);
		fprintf(fsym, "%s\t%d\n", REGI[i], REGI_NUM[i]);
	}
	printf("%s complete!\n", SYMTAB_FILE);
	
    return 0;
}

int pass2() {
	
    char LABEL[WORD_SIZE], OPCODE[WORD_SIZE], FIRST_OPERAND[WORD_SIZE], SECOND_OPERAND[WORD_SIZE], ONE_OPERAND[WORD_SIZE*2], ONE_LINE[LINE_SIZE];
    int i, j, startAddr = 0, LOCCTR = 0, symtabTop = 0, optabTop = 0, BASE = 0, PC = 0;
    line = 1;
    
    // read first line
    fscanf(finter, "%X%s%s%X", &LOCCTR, LABEL, OPCODE, &startAddr);
    
    // start pass2
    if(strcmp(OPCODE, "START") == 0) {
    	PC = startAddr;
//    	printf("%04X\t%s\t%s\t%d\n", PC, LABEL, OPCODE, startAddr);
    	fprintf(fnew, "%04X\t%s\t%s\t%X\n", PC, LABEL, OPCODE, startAddr);
//    	printf("H%-6s%06s%06X\n", LABEL, OPERAND, objLength);
		fprintf(fobj, "H%-6s%06X%06X\n", LABEL, startAddr, objLength);	
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
	char textRecord[TRECD_SIZE*15];
	char modiRecord[MRECD_SIZE];
	strcpy(textRecord, "");
	strcpy(modiRecord, "");
	line++;
	while(true) {
		char c = getc(finter);
		if(c == '\t') {
			break;
		}
	}
	while(true) {
		// read location counter, label, opcode and operand in one line
		int state = 0;
		short N = 1, I = 1, X = 0, B = 0, P = 0, E = 0, format = 0;
		strcpy(LABEL, "");
		strcpy(OPCODE, "");
		strcpy(FIRST_OPERAND, "");
		strcpy(SECOND_OPERAND, "");
		strcpy(ONE_OPERAND, "");
		strcpy(ONE_LINE, "");
		LOCCTR = PC;
		while(true) {
			char c = getc(finter);
			if(c == '\n' || c == EOF) {
				break;
			}
			if(c == '\t') {
				state++;
			}
			else if(state == 0) {
				strncat(LABEL, &c, 1);
			}
			else if(state == 1) {
				if(c == '+') {		// format 4
					E = 1;
				}
				else {
					strncat(OPCODE, &c, 1);
				}
			}
			else if(state == 2) {
				if(c == '#') {	// immediate addressing
					N = 0;
					I = 1;
				}
				else if(c == '@') {		// indirect addressing
					N = 1;
					I = 0;
				}
				else if(c == '\'') {	// BYTE two operand
					state++;
				}
				else if(c == ',') {		// two operand
					state++;
				}
				else {
					strncat(FIRST_OPERAND, &c, 1);
				}
			}
			else if(state == 3) {
				if(c != '\'') {
					strncat(SECOND_OPERAND, &c, 1);
				}
			}
			
			if(state < 2) {
				strncat(ONE_LINE, &c, 1);
			}
			else if(c != '\t') {
				strncat(ONE_OPERAND, &c, 1);
			}
		}
		
		// read next PC first
		char nextPC[WORD_SIZE] = "";
		while(true) {
			char c = getc(finter);
			if(c == '\t' || c == EOF) {
				break;
			}
			strncat(nextPC, &c, 1);
		}
		PC = hexToDec(nextPC);
		
		// base opcode
		if(strcmp(OPCODE, "BASE") == 0) {
			bool isFoundBase = false;
			for(i = 0; i < symtabTop; i++) {
				if(strcmp(FIRST_OPERAND, SYMTAB_LABEL[i]) == 0) {
					isFoundBase = true;
					break;
				}
			}
			if(isFoundBase) {
				BASE = SYMTAB_ADDR[i];
//				printf("%s\t%s\n", ONE_LINE, ONE_OPERAND);
				fprintf(fnew, "%s\t%s\n", ONE_LINE, ONE_OPERAND);
				
				line++;
				continue;
			}
			else {
				printf("Error: undefined base operand \'%s\', in line %d\n", FIRST_OPERAND, line);
				return -1;
			}
		}
		
		// end of intermediate file
		if(strcmp(OPCODE, "END") == 0) {
//			printf("%s\t%s\n", ONE_LINE, ONE_OPERAND);
			fprintf(fnew, "%s\t%s\n", ONE_LINE, ONE_OPERAND);
//			printf("%02X%s\n", objLen, textRecord);
			fprintf(fobj, "%02X%s\n", objLen, textRecord);
//			printf("%s\n", modiRecord);
			fprintf(fobj, "%s", modiRecord);
			fprintf(fobj, "E%06X\n", startAddr);
			break;
		}
		
		// search OPCODE in OPTAB
		short regi1 = 0, regi2 = 0;
		int operAddr = 0, disp = 0;
		unsigned int objCode = 0;
		bool isFoundOP = false;
		for(i = 0; i < optabTop; i++) {
			if(strcmp(OPCODE, OPTAB_OP[i]) == 0) {
				isFoundOP = true;
				break;
			}
		}
		if(isFoundOP) {	// instruction
			if(strcmp(FIRST_OPERAND, "") != 0) {	// if there is an operand
				bool isFoundREGI = false;
				for(j = 0; j < REGI_LEN; j++) {
					if(strcmp(FIRST_OPERAND, REGI[j]) == 0) {
						isFoundREGI = true;
						regi1 = REGI_NUM[j];
						break;
					}
				}
				if(isFoundREGI) {
					isFoundREGI = false;
					if(strcmp(SECOND_OPERAND, "") != 0) {
						isFoundREGI = false;
						for(j = 0; j < REGI_LEN; j++) {
							if(strcmp(SECOND_OPERAND, REGI[j]) == 0) {
								isFoundREGI = true;
								regi2 = REGI_NUM[j];
								break;
							}
						}
						if(!isFoundREGI) {
							printf("Error: unvaild second register name \'%s\', in line %d\n", SECOND_OPERAND, line);
							return -1;
						}
					}
					format = 2;
				}
				else {
					// search symbol table for operand
					bool isFoundSYM = false;
					for(j = 0; j < symtabTop; j++) {
						if(strcmp(FIRST_OPERAND, SYMTAB_LABEL[j]) == 0) {
							isFoundSYM = true;
							break;
						}
					}
					if(isFoundSYM) {	// find symbol in symbol table
						if(strcmp(SECOND_OPERAND, "X") == 0) {
							X = 1;
						}
						operAddr = SYMTAB_ADDR[j];
						disp = operAddr - PC;
						if(disp >= -2048 && disp <= 2047) {	// PC-relative range
							P = 1;
							format = 3;
						}
						else {
							disp = operAddr - BASE;
							if(disp >= 0 && disp <= 4095) {	// Base-relative range
								B = 1;
								format = 3;
							}
							else {
								if(E == 1) {	// direct address: format 4
									disp = operAddr;
									format = 4;
									// append modification record
									char temp[10];
									sprintf(temp, "M%06X05\n", LOCCTR+1);
									strncat(modiRecord, temp, 10);
								}
								else {
									// for SIC
									N = 0, I = 0;
									format = 3;
								}
							}
						}
					}
					else {	// not find
						if(N == 0 && I == 1) {	// immediate address (#)
							disp = atoi(FIRST_OPERAND);
							if(disp >= 0x1000) {
								format = 4;
							}
							else {
								format = 3;
							}
						}
						else {
							printf("Error: undefined symbol \'%s\', in line %d\n", FIRST_OPERAND, line);
							return -1;
						}
					}	
				}
			}
			else {	// no operand
				operAddr = 0;
				format = 3;
			}
			
			if(format == 2) {
				// create object code
				objCode = objectCodeFormat2(OPTAB_CODE[i], regi1, regi2);
				// concatenate opcode behind text record
			}
			else if(format == 3) {
				// create object code
				objCode = objectCodeFormat3(OPTAB_CODE[i], N, I, X, B, P, E, disp);
			}
			else {
				objCode = objectCodeFormat4(OPTAB_CODE[i], N, I, X, B, P, E, disp);
			}
		}
		else if(strcmp(OPCODE, "BYTE") == 0) {	// BYTE
			int operLen = 0, tempLen = 0;
			if(strcmp(FIRST_OPERAND, "C") == 0) {	// character
				// get the value inside ''
				for(i = 0; i < WORD_SIZE; i++) {
					if(SECOND_OPERAND[i] == '\0') {
						break;
					}
					operLen++;
					objCode = objCode*0x100 + SECOND_OPERAND[i];
				}
				format = operLen;	// one char one byte
			}
			else if(strcmp(FIRST_OPERAND, "X") == 0) {	// hexadecimal
				char hexOPER[WORD_SIZE] = "";
				// get the value inside ''
				for(i = 0; i < WORD_SIZE; i++) {
					if(SECOND_OPERAND[i] == '\0') {
						break;
					}
					operLen++;
				}
				objCode = hexToDec(SECOND_OPERAND);
				format = (operLen+1)/2;		// two hex one byte
			}
			else {
				printf("Error: invalid byte type '%s', in line %d\n", FIRST_OPERAND, line);
				return -1;
			}
		}
		else if(strcmp(OPCODE, "WORD") == 0) {	// WORD
			objCode = atoi(FIRST_OPERAND);
			format = 3;
		}
		else if(strcmp(OPCODE, "RESW") == 0 || strcmp(OPCODE, "RESB") == 0) {
			// nothing to do
		}
		else {
			printf("Error: undefined opcode \'%s\', in line %d\n", OPCODE, line);
			return -1;
		}
		
		// write new source file
		objLen += format;
		char temp[format*2];
		if(format == 0) {
//			printf("%04X\t%s\t%-15s\n", LOCCTR, ONE_LINE, ONE_OPERAND, objCode);
			fprintf(fnew, "%04X\t%s\t%-15s\n", LOCCTR, ONE_LINE, ONE_OPERAND, objCode);
		}
		else if(format == 1) {
//			printf("%04X\t%s\t%-15s%02X\n", LOCCTR, ONE_LINE, ONE_OPERAND, objCode);
			fprintf(fnew, "%04X\t%s\t%-15s%02X\n", LOCCTR, ONE_LINE, ONE_OPERAND, objCode);
			sprintf(temp, "%02X", objCode);
		}
		else if(format == 2) {
//			printf("%04X\t%s\t%-15s%04X\n", LOCCTR, ONE_LINE, ONE_OPERAND, objCode);
			fprintf(fnew, "%04X\t%s\t%-15s%04X\n", LOCCTR, ONE_LINE, ONE_OPERAND, objCode);
			sprintf(temp, "%04X", objCode);
		}
		else if(format == 3) {
//			printf("%04X\t%s\t%-15s%06X\n", LOCCTR, ONE_LINE, ONE_OPERAND, objCode);
			fprintf(fnew, "%04X\t%s\t%-15s%06X\n", LOCCTR, ONE_LINE, ONE_OPERAND, objCode);
			sprintf(temp, "%06X", objCode);
		}
		else if(format == 4) {
//			printf("%04X\t%s\t%-15s%08X\n", LOCCTR, ONE_LINE, ONE_OPERAND, objCode);
			fprintf(fnew, "%04X\t%s\t%-15s%08X\n", LOCCTR, ONE_LINE, ONE_OPERAND, objCode);
			sprintf(temp, "%08X", objCode);	
		}
		
		// concatenate opcode behind text record
		strncat(textRecord, temp, format*2);

		// count number of object code
		if(objNum == 0) {
			// write T and starting address
//			printf("T%06X", LOCCTR);
			fprintf(fobj, "T%06X", LOCCTR);
		}
		objNum++;
		// max object number is 10 in this case, if over 10, then change to next line
		if(objNum == TRECD_SIZE) {
			// write length of object code and object code
//			printf("%02X", objLen);
//			printf("%s\n", textRecord);
			fprintf(fobj, "%02X", objLen);
			fprintf(fobj, "%s\n", textRecord);
			objNum = 0;
			objLen = 0;
			strcpy(textRecord, "");
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

unsigned int objectCodeFormat2(int opcode, short regi1, short regi2) {
//	printf("opcode=%02X, regi1 = %d, regi2 = %d\t", opcode, regi1, regi2);
	return opcode*0x100 + regi1*0x10 + regi2;
}

unsigned int objectCodeFormat3(int opcode, short N, short I, short X, short B, short P, short E, int disp) {
//	printf("opcode=%02X, N=%d, I=%d, X=%d, B=%d, P=%d, E=%d, disp=%03X\t", opcode, N, I, X, B, P, E, disp);
	return opcode*0x10000 + N*0x20000 + I*0x10000 + X*0x8000 + B*0x4000 + P*0x2000 + E*0x1000 + (disp&0x00FFF);
}

unsigned int objectCodeFormat4(int opcode, short N, short I, short X, short B, short P, short E, int addr) {
//	printf("opcode=%02X, N=%d, I=%d, X=%d, B=%d, P=%d, E=%d, addr=%05X\t", opcode, N, I, X, B, P, E, addr);
	return opcode*0x1000000 + N*0x2000000 + I*0x1000000 + X*0x800000 + B*0x400000 + P*0x200000 + E*0x100000 + addr;
}
