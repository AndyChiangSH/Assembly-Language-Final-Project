#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int hexToDec(char* hex);

int main() {
	
	char textRecord[10][6];
	int i;
	
	for(i = 0; i < 10; i++) {
		strcpy(textRecord[i], "");
	}
	
	for(i = 0; i < 10; i++) {
		printf("'%s'\n", textRecord[i]);
	}
		
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
