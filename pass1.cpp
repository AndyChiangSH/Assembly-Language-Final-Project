// pass1 assembler
// author: Andy Chiang

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SOURCE_FILE "source.txt"
#define INTER_FILE "intermediate.txt"
#define SYM_SIZE 100 

int main() {
	FILE * fsrc;
	FILE * finter;
	
	// start read file 
	fsrc = fopen (SOURCE_FILE, "r");
    if (fsrc == NULL) {
        printf("fsrc not found...");
        return 0;
    }
    else {
        printf("find fsrc!\n");
    }
    
    finter = fopen (INTER_FILE, "w");
    if (finter == NULL) {
        printf("finter not found...");
        return 0;
    }
    else {
        printf("fsrcd finter!\n");
    }
    
    char SYMTAB[SYM_SIZE][2][10];
    char LABEL[10], OPCODE[10], OPERAND[10];
    int i, j, k, startAddr = 0, LOCCTR = 0, symtabTop = 0;
    
    // read first line
    fscanf(fsrc, "%s\t%s\t%s", LABEL, OPCODE, OPERAND);
    printf("%s\t%s\t%s\n", LABEL, OPCODE, OPERAND);
    
    for(i = 0; i < SYM_SIZE; i++) {
    	strcpy(SYMTAB[i][0], "");
    	strcpy(SYMTAB[i][1], "");
	}
//    printf("label\taddress\n");
//    for(i = 0; i < SYM_SIZE; i++) {
//    	printf("'%s'\t'%s'\n", SYMTAB[i][0], SYMTAB[i][1]);
//	}
    
    if(strcmp(OPCODE, "START") == 0) {
    	startAddr = atoi(OPERAND);
    	LOCCTR = startAddr;
//    	printf("%d\t%s\t%s\t%s\n", LOCCTR, LABEL, OPCODE, OPERAND);
//    	fprintf(finter, "%d\t%s\t%s\t%s\n", LOCCTR, LABEL, OPCODE, OPERAND);
	}
	else {
		return 0;
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
//			printf("skip comment\n");
			continue;
		}
//		printf("%s\t%s\t%s\n", LABEL, OPCODE, OPERAND);
		
		// add label and address into symbol table
		if(strcmp(LABEL, "") != 0) {
			// check duplicate label
			for(i = 0; i < symtabTop; i++) {
				if(strcmp(LABEL, SYMTAB[i][0]) == 0) {
					printf("Error: duplicate label\n");
					return 0;
				}
			}
			strcpy(SYMTAB[symtabTop][0], LABEL);
			// itoa: C standard function to convert int to string
			itoa(LOCCTR, SYMTAB[symtabTop][1], 10);
			symtabTop++;
		}
		
		// last line
		if(strcmp(OPCODE, "END") == 0) {
			break;
		}
	}
	
	printf("label\taddress\n");
    for(i = 0; i < SYM_SIZE; i++) {
    	printf("%s\t%s\n", SYMTAB[i][0], SYMTAB[i][1]);
	}
    
    fclose(fsrc);
    fclose(finter);
    return 0;
}
