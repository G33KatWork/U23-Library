#include <game/Game.h>
#include <Debug.h>
#include <game/Filesystem.h>
#include <game/Font.h>

#include <stdio.h>
#include <string.h>

void Init(struct Gamestate*);
void Update(uint32_t);
void Draw(Bitmap* surface);

Gamestate InitState = { Init, NULL, NULL, Update, Draw };
Game* TheGame = &(Game) {&InitState};

void Init(struct Gamestate* state)
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
	snes_button_state_t state = GetControllerState1();
	pushbutton_button_state_t anotherState = GetPushbuttonState();
	myframe++;
}

void Draw(Bitmap* surface)
{
	ClearBitmap(surface);
	DrawFilledRectangle(surface, 30, 30, 50, 50, GetPushbuttonState().A ? RGB(128,128,128) : RGB(255, 255, 255));
	setFont(fontwhite16);

	char testString[8];
	testString[7] = '\0';

	testString[0] = GetPushbuttonState().A ? 'A' : ' ';
	testString[1] = GetPushbuttonState().B ? 'B' : ' ';
	testString[2] = GetPushbuttonState().Up ? 'U' : ' ';
	testString[3] = GetPushbuttonState().Down ? 'D' : ' ';
	testString[4] = GetPushbuttonState().Left ? 'L' : ' ';
	testString[5] = GetPushbuttonState().Right ? 'R' : ' ';
	testString[6] = GetPushbuttonState().User ? 'u' : ' ';

	setFont(fontwhite8);
	DrawText(surface, testString, 10, 100);
}
