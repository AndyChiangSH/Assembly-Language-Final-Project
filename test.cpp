#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	int num = 321;
	char snum[5];

	// convert 123 to string [buf]
	itoa(num+1, snum, 10);

	// print our string
	printf("%s\n", snum);
	
	return 0;
}
