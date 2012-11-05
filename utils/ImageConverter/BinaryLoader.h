#ifndef __BINARY_LOADER_H__
#define __BINARY_LOADER_H__

#include <stdlib.h>

void *AllocAndLoadFile(const char *filename,size_t *sizeptr);

#endif
