#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <stdint.h>

void InitializeRandom(void);
uint32_t GetRandomInteger(void);
void DeInitializeRandom(void);

#endif
