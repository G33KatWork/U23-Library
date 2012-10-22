#include "BinaryLoader.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

void *AllocAndLoadFile(const char *filename,size_t *sizeptr)
{
	// Find file size.
	struct stat st;
	if(stat(filename,&st)!=0)
	{
		fprintf(stderr,"Could not find file.\n");
		return NULL;
	}

	// Open the file, then alloc and read the contents.
	FILE *fh=fopen(filename,"rb");
	if(!fh)
	{
		fprintf(stderr,"Failed to open file.\n");
		return NULL;
	}

	void *bytes=malloc(st.st_size);
	if(!bytes)
	{
		fprintf(stderr,"Out of memory.\n");
		exit(1);
	}

	if(fread(bytes,1,st.st_size,fh)!=st.st_size)
	{
		fprintf(stderr,"Failed to read from file.\n");
		fclose(fh);
		free(bytes);
		exit(1);
	}

	fclose(fh);

	if(sizeptr) *sizeptr=st.st_size;

	return bytes;
}
