#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	char input[16];

	printf("Please enter your name: \n");
	gets(input);

	if (strlen(input) > 16) {
		printf("Input too big :c");
		exit(1);
	}

	printf("Hi %s\n", input);
	return 0;
}
