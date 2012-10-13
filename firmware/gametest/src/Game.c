#include <game/Game.h>
#include <game/Debug.h>
#include <game/Filesystem.h>
#include <game/Font.h>

#include <stdio.h>
#include <string.h>

void Init(void);
void Update(uint32_t);
void Draw(Bitmap* surface);

Game MyGame = { Init, Update, Draw };
Game* TheGame = &MyGame;

void Init()
{
	EnableDebugOutput(DEBUG_USART);
	printf("Init\r\n");
	setFont(fontblack8);

	InitializeFilesystem();

	//Open a file
	FILE* file = fopen("0:TESTFILE", "w+");
	if(!file) {
		perror("fopen()");
		goto fs_err;
	}
	printf("opened file: %x\r\n", (uint32_t)file);

	//Write to file
	const char* writecontent = "This is a test";
	int res = fwrite(writecontent, 1, strlen(writecontent), file);
	if(!res) {
		if(ferror(file))
			printf("Error occured\r\n");
		perror("fwrite()");
		goto fs_err;
	}
	printf("wrote file: %x\r\n", res);

	//Seek to start
	res = fseek(file, 0, SEEK_SET);
	if(res == -1) {
		perror("fseek()");
		goto fs_err;
	}
	printf("seeked file: %x\r\n", res);

	//Read file
	char readcontent[256];
	res = fread(readcontent, 1, 255, file);
	if(!res) {
		if(ferror(file))
			printf("Error occured\r\n");
		if(feof(file))
			printf("EOF occured\r\n");
		perror("fread()");
		goto fs_err;
	}
	readcontent[res] = '\0';
	printf("read file: %x\r\n", res);
	printf("content: %s\r\n", readcontent);

fs_err:
	//Close file
	res = fclose(file);
	if(res != 0) {
		perror("fclose()");
		goto fs_err;
	}
	printf("closed file: %x\r\n", res);

	DeinitializeFilesystem();
}

int myframe = 0;

void Update(uint32_t delta)
{
	snes_button_state_t state = GetControllerState();
	myframe++;
}

void Draw(Bitmap* surface)
{
	ClearBitmap(surface);
	DrawFilledRectangle(surface, 30, 30, 50, 50, RGB(myframe%256,0,0));
	setFont(fontwhite16);
	DrawText(surface, "TEST", 4, 10, 10);
	setFont(fontblack8);
	DrawText(surface, "TEST", 4, 10, 30);
}
