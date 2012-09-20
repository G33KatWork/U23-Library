#include <game/Game.h>
#include <game/Debug.h>
#include <game/Filesystem.h>

#include <stdio.h>
#include <string.h>

#include <fatfs/ff.h>

void Init(void);
void Update(uint32_t);
void Draw(void);

FRESULT scan_files(char* path);

Game MyGame = { Init, Update, Draw };
Game* TheGame = &MyGame;

void Init()
{
	EnableDebugOutput(DEBUG_USART);
	printf("Init\r\n");

	InitializeFilesystem();

	FRESULT res;

	// printf("Formatting...\r\n");
	// res = f_mkfs(0, 1, 2*8192);
	// printf("mkfs result: %x\r\n", res);

	printf("Creating file\r\n");
	FIL f;
	res = f_open(&f, "0:TESTFILE", FA_CREATE_ALWAYS | FA_WRITE);
	printf("open(): %x\r\n", res);

	printf("Writing\r\n");
	char* string = "Test";
	UINT written;
	res = f_write(&f, string, 4, &written);
	printf("write(): %x written: %x\r\n", res, written);

	res = f_sync(&f);
	printf("sync(): %x\r\n", res);

	printf("Closing\r\n");
	res = f_close(&f);
	printf("close(): %x\r\n", res);

	printf("Dirlisting:\r\n");
	res = scan_files("0:");
	printf("Scan files: %x\r\n", res);

	DeinitializeFilesystem();
}

void Update(uint32_t delta)
{
	snes_button_state_t state = GetControllerState();
	//printf("Delta: %d\r\n", delta);
	//printf("Button A: %d\r\n", state.buttons.A);
	Delay(100);
}

void Draw()
{

}


FRESULT scan_files(char* path)
{
	FRESULT res;
	FILINFO fno;
	DIR dir;
	int i;
	char *fn;

	res = f_opendir(&dir, path);
	if (res == FR_OK) {
		i = strlen(path);
		for (;;) {
			res = f_readdir(&dir, &fno);
			if (res != FR_OK || fno.fname[0] == 0) break;
			if (fno.fname[0] == '.') continue;
			fn = fno.fname;

			if (fno.fattrib & AM_DIR) {
				printf("Directory: %s/%s\r\n", path, fn);
			} else {
				printf("File: %s/%s\r\n", path, fn);
			}
		}
	}

	return res;
}