#include <game/Game.h>
#include <game/Filesystem.h>

#include <stdio.h>
#include <string.h>

void ReadHighscore(FILE *file);
void ErrorHandler(FILE *file);
void reverse(int32_t *array, int32_t n);
int intcmp(const void *a, const void *b);

static int32_t score[10];

void WriteHighscore(int32_t newScore) {
	InitializeFilesystem();

	//Open a file
	FILE* file = fopen("0:h_space", "w+");
	if(!file) {
		perror("fopen()");
		ErrorHandler(file);
		return;
	}

	ReadHighscore(file);

	if (newScore > score[0])
	{
		score[9] = newScore;
		qsort(score, 10, sizeof(score), intcmp);
		//reverse(score, 10);
	} else {
		ErrorHandler(file);
		return;
	}


	//Seek to start
	int res = fseek(file, 0, SEEK_SET);
	if(res == -1) {
		perror("fseek()");
		ErrorHandler(file);
		return;
	}

	char *highscoreBuf;

	//warum schleifen wenn man episch faul sein kann :)
	asprintf(&highscoreBuf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", score[0], score[1], score[2], score[3], score[4], score[5], score[6], score[7], score[8], score[9]);

	res = fwrite(highscoreBuf, 1, strlen(highscoreBuf), file);
	if(!res) {
		perror("fwrite()");
		ErrorHandler(file);
		return;
	}

	free(highscoreBuf);

	//Close file
	res = fclose(file);
	if(res != 0) {
		perror("fclose()");
		ErrorHandler(file);
		return;
	}
}

void GetHighscoreList(char **formattedString) {
	InitializeFilesystem();

	//Open a file
	FILE* file = fopen("0:h_space", "r");
	if(!file) {
		perror("fopen()");
		ErrorHandler(file);
		return;
	}

	ReadHighscore(file);

	// HAHAHHAHA
	asprintf(formattedString,"\
1.  %d\n\
2.  %d\n\
3.  %d\n\
4.  %d\n\
5.  %d\n\
6.  %d\n\
7.  %d\n\
8.  %d\n\
9.  %d\n\
10. %d\n\
", 	score[0], score[1], score[2], score[3] ,score[4], \
			score[5], score[6], score[7], score[8], score[9]);

	ErrorHandler(file);
}

int intcmp(const void *o, const void *t)
{
	const int *a = o, *b = t;

	if (*a < *b)
		return -1;
	else if (*a > *b)
		return 1;
	else
		return 0;
}

void ReadHighscore(FILE *file) {
	//Read file
	char readcontent[1024];
	int res = fread(readcontent, 1, 1023, file);
	if(!res) {
		if(ferror(file))
			printf("Error occured\r\n");
		if(feof(file))
			printf("EOF occured\r\n");
		perror("fread()");
	}

	reverse(score, 10);

	score[0] = atoi(strtok(readcontent, ","));
	for (int i = 1; i < 10; ++i)
	{
		score[i] = atoi(strtok(NULL, ","));
		if (score[i] < 0)
		{
			score[i] = 0;
		}
	}

	qsort(score, 10, sizeof(score), intcmp);
}

void ErrorHandler(FILE *file) {
	int res = fclose(file);
	if(res != 0) {
		perror("fclose()");
	}
	printf("closed file: %x\r\n", res);
	DeinitializeFilesystem();
}

void reverse(int32_t *array, int32_t n) {
    int i = 0;
    while (i < n)
    {
        int32_t c = array[i];
        array[i++] = array[n];
        array[n--] = c;
    }
}
