// pass1 assembler
// author: Andy Chiang

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SOURCE_FILE "source.txt"
#define INTER_FILE "intermediate.txt"

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
    
    char LABEL[10], OPCODE[10], OPERAND[10];
    
    // read first line
    fscanf(fsrc, "%s\t%s\t%s", LABEL, OPCODE, OPERAND);
    printf("%s\t%s\t%s\n", LABEL, OPCODE, OPERAND);
    
    int startAddr, LOCCTR;
    
    if(strcmp(OPCODE, "START") == 0) {
    	startAddr = atoi(OPERAND);
    	LOCCTR = startAddr;
    	printf("%d\t%s\t%s\t%s\n", LOCCTR, LABEL, OPCODE, OPERAND);
    	fprintf(finter, "%d\t%s\t%s\t%s\n", LOCCTR, LABEL, OPCODE, OPERAND);
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
		if(LABEL[0] == '#') {	// skip comment
//			printf("skip comment\n");
			continue;
		}
		printf("%s\t%s\t%s\n", LABEL, OPCODE, OPERAND);
		
		
		
		// last line
		if(strcmp(OPCODE, "END") == 0) {
			break;
		}
	}
    
    fclose(fsrc);
    fclose(finter);
    return 0;
}
