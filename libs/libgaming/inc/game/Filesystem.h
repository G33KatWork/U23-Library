#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

#include <stdint.h>

void InitializeFilesystem(void);
void DeinitializeFilesystem(void);
uint32_t IsFilesystemInitialized(void);

#endif
