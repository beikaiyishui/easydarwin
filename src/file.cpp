#include "file.h"

char *readfile(char *path)
{
	FILE *file = NULL;
	int lsize;
	char *buffer;
	file = fopen(path, "rb");
	if (file == NULL) 
	{
		fputs("File error", stderr); exit(1);
	}
	fseek(file , 0 , SEEK_END); 
    lsize = ftell(file);
    rewind(file);
	buffer = (char*) malloc(sizeof(char) * lsize);  
    if (buffer == NULL) 
	{
		fputs("Memory error", stderr); exit(2);
	}
    fread(buffer, 1, lsize, file);
	fclose(file);
	return buffer;
}

