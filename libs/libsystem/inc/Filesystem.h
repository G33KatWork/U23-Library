#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

#include <stdint.h>

#define MAX_OPEN_FILES	3

void InitializeFilesystem(void);
void DeinitializeFilesystem(void);
uint32_t IsFilesystemInitialized(void);

#endif
