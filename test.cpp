#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	char str[10] = "123";
	printf("'%s'\n", str);
	
	strcpy(str, "");
	printf("'%s'\n", str);
	
	return 0;
}
