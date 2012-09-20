#include <game/Filesystem.h>
#include <fatfs/ff.h>
#include <stdio.h>
#include <sdcard/sdcard.h>

static FATFS fatfs;

uint32_t filesystem_initialized = 0;

void InitializeFilesystem()
{
	if(filesystem_initialized)
		return;

	//printf("Mounting drive 0...\r\n");
	FRESULT res = f_mount(0, &fatfs);
	
	if(res != FR_OK)
		return;

	filesystem_initialized = 1;
}

void DeinitializeFilesystem()
{
	printf("Unmounting drive 0...\r\n");

	if(f_mount(0, NULL) == FR_OK)
	{
		filesystem_initialized = 0;
		SD_DeInit();
	}
}

uint32_t IsFilesystemInitialized()
{
	return filesystem_initialized;
}
