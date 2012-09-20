#include <fatfs/ff.h>

#if _USE_LFN != 0

#if   _CODE_PAGE == 932
#include <fatfs/cc932.c>
#elif _CODE_PAGE == 936
#include <fatfs/cc936.c>
#elif _CODE_PAGE == 949
#include <fatfs/cc949.c>
#elif _CODE_PAGE == 950
#include <fatfs/cc950.c>
#else
#include <fatfs/ccsbcs.c>
#endif

#endif
