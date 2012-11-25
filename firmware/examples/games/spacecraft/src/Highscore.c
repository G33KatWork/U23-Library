#include <game/Game.h>
#include <game/Filesystem.h>

#include <stdio.h>
#include <string.h>

void ReadHighscore(FILE *file);
void ErrorHandler(FILE *file);
void quick_sort (int *a, int n);
void reverse(int *array, int n);

static int score[10];

void WriteHighscore(int newScore) {
	InitializeFilesystem();

	//Open a file
	FILE* file = fopen("0:h_space", "r+");
	if(!file) {
		fclose(file);
		file = fopen("0:h_space", "w+");
		if (!file)
		{
			perror("fopen()");
			ErrorHandler(file);
			return;
		}
	}

	ReadHighscore(file);

	// if (newScore > score[0])
	// {
	// 	score[9] = newScore;
	// 	// quick_sort(score, 10);
	// 	// reverse(score, 10);
	// } else {
	// 	ErrorHandler(file);
	// 	return;
	// }

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

	printf("WRITE: %s\n", highscoreBuf);

	res = fwrite(highscoreBuf, 1, strlen(highscoreBuf), file);
	if(!res) {
		perror("fwrite()");
		ErrorHandler(file);
		return;
	}

	free(highscoreBuf);

	// //Close file
	// res = fclose(file);
	// if(res != 0) {
	// 	perror("fclose()");
	// 	ErrorHandler(file);
	// 	return;
	// }
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
	reverse(score, 10);

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
	readcontent[res] = '\0';

	printf("Read raw: %s\n", readcontent);

	/*score[0] = atoi(strtok(readcontent, ","));
	for (int i = 1; i < 10; ++i)
	{
		score[i] = atoi(strtok(NULL, ","));
		if (score[i] < 0)
		{
			score[i] = 0;
		}
	}

	quick_sort(score, 10);*/
}

void ErrorHandler(FILE *file) {
	int res = fclose(file);
	if(res != 0) {
		perror("fclose()");
	}
	DeinitializeFilesystem();
}

void reverse(int *array, int n) {
    int i = 0;
    while (i < n)
    {
        int32_t c = array[i];
        array[i++] = array[n];
        array[n--] = c;
    }
}

void quick_sort (int *a, int n) {
    if (n < 2)
        return;
    int p = a[n / 2];
    int *l = a;
    int *r = a + n - 1;
    while (l <= r) {
        while (*l < p)
            l++;
        while (*r > p)
            r--;
        if (l <= r) {
            int t = *l;
            *l++ = *r;
            *r-- = t;
        }
    }
    quick_sort(a, r - a + 1);
    quick_sort(l, a + n - l);
}
