#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	FILE *inp;
	int16_t c;
	inp = fopen("audio.wav", "r");

	printf("const int16_t data[] = {\n");
	do {
		fread(&c, 2, 1, inp);
		printf("%i, ", c);
	} while(!feof(inp));
	printf("\n};\n");

	fclose(inp);

	return 0;
}
