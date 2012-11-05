#include <game/Game.h>
#include <game/Debug.h>

#include <stdio.h>
#include <string.h>

#include "../inc/GameOfLife.h"
#include "../inc/Sprites.h"

void Init(struct Gamestate*);
void OnEnter(struct Gamestate* state);
void OnLeave(struct Gamestate* state);
void Update(uint32_t);
void Draw(Bitmap *);

int roundcount=1;
int A[N][M];
int run=0;
int curx=0;
int cury=0;
int speed=10;

int countdown, countleft, countup, countright, speedpluscount, speedminuscount;

Gamestate InitState = { Init, OnEnter, OnLeave, Update, Draw };
Game* TheGame = &(Game) {&InitState};


void init_array(int A[][M])
{
    for(int i = 0; i < N; i++)
        for(int j = 0; j < M; j++)
            A[i][j] = 0;

    A[10][10]=1;
    A[10][11]=1;
    A[11][10]=1;
    A[11][11]=1;

    A[18][11]=1;
    A[18][12]=1;
    A[19][10]=1;
    A[19][12]=1;
    A[20][10]=1;
    A[20][11]=1;

    A[26][12]=1;
    A[26][13]=1;
    A[26][14]=1;
    A[27][12]=1;
    A[28][13]=1;

    A[32][10]=1;
    A[32][9]=1;
    A[33][10]=1;
    A[33][8]=1;
    A[34][8]=1;
    A[34][9]=1;

    A[34][20]=1;
    A[34][21]=1;
    A[35][20]=1;
    A[35][22]=1;
    A[36][20]=1;

    A[44][8]=1;
    A[44][9]=1;
    A[45][8]=1;
    A[45][9]=1;

    A[45][15]=1;
    A[45][16]=1;
    A[45][17]=1;
    A[46][15]=1;
    A[47][16]=1;
} 

void update_Array(int A[][M])
{
    int temp[N][M];
    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < M; j++)
        {
            temp[i][j] = A[i][j];
        }
    }
   
    int mask[N][M];
    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < M; j++)
        {
            mask[i][j] = 0;
        }
    }

    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < M; j++)
        {
            //Angrenzende lebende Zellen zählen (ES GIBT NUR 8 ANGRENZENDE ZELLEN ALSO AUCH NUR 8 BEDINGUNGEN!)
            if(temp[i-1][j-1] == 1 && i     > 0 && j     > 0) ++mask[i][j];
            if(temp[i  ][j-1] == 1              && j     > 0) ++mask[i][j];
            if(temp[i-1][j  ] == 1 && i     > 0             ) ++mask[i][j];
            if(temp[i-1][j+1] == 1 && i     > 0 && j + 1 < M) ++mask[i][j];
            if(temp[i  ][j+1] == 1              && j + 1 < M) ++mask[i][j];
            if(temp[i+1][j  ] == 1 && i + 1 < N             ) ++mask[i][j];
            if(temp[i+1][j-1] == 1 && i + 1 < N && j     > 0) ++mask[i][j];
            if(temp[i+1][j+1] == 1 && i + 1 < N && j + 1 < M) ++mask[i][j];

            if(mask[i][j] < 2 || mask[i][j] > 3)
                A[i][j] = 0; //Sterben durch Vereinsamung/Überbevölkerung
            else if(mask[i][j] == 3)
                A[i][j] = 1; //Neues Leben schaffen
        }
    }
} 

void Init(struct Gamestate* state)
{
    init_array(A);
    EnableDebugOutput(DEBUG_USART);
    printf("Init\r\n");

    InitializeLEDs();

    SetLEDs(0x01);
    SetLEDs(0x07);
}

void OnEnter(struct Gamestate* state)
{
}

void OnLeave(struct Gamestate* state)
{
}

void Update(uint32_t a)
{
    if(run==1){
    	if(roundcount % speed == 0) update_Array(A);
        roundcount++;
    }else{
        if(GetControllerState1().buttons.A){
            A[curx][cury]=1;
        }else if(GetControllerState1().buttons.B){
            A[curx][cury]=0;
        }
    }
    if(GetControllerState1().buttons.L){
        speed--;
    }
    if(GetControllerState1().buttons.R){
        speed++;
    }

    if(GetControllerState1().buttons.L){
        speedminuscount++;
        if(speedminuscount==10){
            speed++;
            speedminuscount=0;
        }
    }else{
        speedminuscount=0;
    }
    if(GetControllerState1().buttons.R){
        speedpluscount++;
        if(speedpluscount==10){
            speed--;
            speedpluscount=0;
        }
    }else{
        speedpluscount=0;
    }


    if(GetControllerState1().buttons.Start){
        run=1;
    }
    if(GetControllerState1().buttons.Left){
        countleft++;
        if(countleft==10){
            curx--;
            countleft=0;
        }
    }else{
        countleft=0;
    }
    if(GetControllerState1().buttons.Right){
        countright++;
        if(countright==10){
            curx++;
            countright=0;
        }
    }else{
        countright=0;
    }
    if(GetControllerState1().buttons.Up){
        countup++;
        if(countup==10){
            cury--;
            countup=0;
        }
    }else{
        countup=0;
    }
    if(GetControllerState1().buttons.Down){
        countdown++;
        if(countdown==10){
            cury++;
            countdown=0;
        }
    }else{
        countdown=0;
    }
}

void Draw(Bitmap *b)
{
    ClearBitmap(b);

    if(!run){
         (b, curx*5,  cury*5, 5, 5, RGB(100,100,100));
    }

    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < M; j++)
        {
            if(A[i][j]==1){
                DrawFilledRectangle(b, i*5,  j*5, 5, 5, RGB(255,255,255));
            }
        }
    }
}
