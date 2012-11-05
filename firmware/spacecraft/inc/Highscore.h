#ifndef _HIGHSCORE_H_
#define _HIGHSCORE_H_

#include "stdint.h"

void WriteHighscore(int32_t newScore);
void GetHighscoreList(char **formattedString);

#endif
