#include <game/Game.h>
#include <game/Debug.h>
#include <platform/UserInterface.h>

#include <stdio.h>
#include <string.h>

#define N 64
#define M 40

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

Gamestate InitState = { Init, OnEnter, OnLeave, Update, Draw };
Game* TheGame = &(Game) {&InitState};


void init_simple_row(int A[][M]){
    for(int i = 0; i < N; i++)
        for(int j = 0; j < M; j++)
            A[i][j] = 0;

    int x,y;
    for(int i = 0; i < 10; i++){
        x=(N/2)-5+i;
        y=(M/2);
        A[x][y] = 1;
    }    
}

void init_empty_field(int A[][M]){
    for(int i = 0; i < N; i++)
        for(int j = 0; j < M; j++)
            A[i][j] = 0;
}

void init_glider(int A[][M])
{
    for(int i = 0; i < N; i++)
        for(int j = 0; j < M; j++)
            A[i][j] = 0;

    // Gosper Glider Gun after template found on http://www.bitstorm.org/gameoflife/

    // left block
    A[10][10]=1;
    A[10][11]=1;
    A[11][10]=1;
    A[11][11]=1;

    // left ship
    A[18][11]=1;
    A[18][12]=1;
    A[19][10]=1;
    A[19][12]=1;
    A[20][10]=1;
    A[20][11]=1;

    // left glider
    A[26][12]=1;
    A[26][13]=1;
    A[26][14]=1;
    A[27][12]=1;
    A[28][13]=1;

    // right ship
    A[32][10]=1;
    A[32][9]=1;
    A[33][10]=1;
    A[33][8]=1;
    A[34][8]=1;
    A[34][9]=1;

    // middle glider
    A[34][20]=1;
    A[34][21]=1;
    A[35][20]=1;
    A[35][22]=1;
    A[36][20]=1;

    // right block
    A[44][8]=1;
    A[44][9]=1;
    A[45][8]=1;
    A[45][9]=1;

    // right glider
    A[45][15]=1;
    A[45][16]=1;
    A[45][17]=1;
    A[46][15]=1;
    A[47][16]=1;
}

int buttonTimes[2];

int buttonPressedCountStuff(int controller, int count){
    snes_button_state_t controller_state;
    // get controller state
    if(controller==0){
        controller_state = GetControllerState1();
    }else{
        controller_state = GetControllerState2();
    }
    // if no button is pressed, reset counter
    if(controller_state.raw == 0){
        buttonTimes[controller]=0;
    // otherwise update counter
    }else{
        buttonTimes[controller]++;
    }

    // if counter has reached the given number, reset counter and return true value
    if(buttonTimes[controller]==count){
        buttonTimes[controller]=0;
        return 1;
    // otherwise just return false value
    }else{
        return 0;
    }
}

void update_Array(int A[][M])
{
    // init temp array (containing data from last round)
    int temp[N][M];
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            temp[i][j] = A[i][j];
        }
    }
   
    // init mask array (for counting neighbours)
    int mask[N][M];
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            mask[i][j] = 0;
        }
    }

    // run cell checks
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            if(temp[i-1][j-1] == 1 && i     > 0 && j     > 0) ++mask[i][j]; // neighbour top-left
            if(temp[i  ][j-1] == 1              && j     > 0) ++mask[i][j]; // neighbour top
            if(temp[i-1][j  ] == 1 && i     > 0             ) ++mask[i][j]; // neighbour left
            if(temp[i-1][j+1] == 1 && i     > 0 && j + 1 < M) ++mask[i][j]; // neighbour bottom-left
            if(temp[i  ][j+1] == 1              && j + 1 < M) ++mask[i][j]; // neighbour bottom
            if(temp[i+1][j  ] == 1 && i + 1 < N             ) ++mask[i][j]; // neighbour right
            if(temp[i+1][j-1] == 1 && i + 1 < N && j     > 0) ++mask[i][j]; // neighbour top-right
            if(temp[i+1][j+1] == 1 && i + 1 < N && j + 1 < M) ++mask[i][j]; // neighbour bottom-right

            if(mask[i][j] < 2 || mask[i][j] > 3) A[i][j] = 0; // terminate cell
            else if(mask[i][j] == 3) A[i][j] = 1; // create new cell
        }
    }
} 

void Init(struct Gamestate* state)
{
    init_glider(A);
}

void OnEnter(struct Gamestate* state)
{
}

void OnLeave(struct Gamestate* state)
{
}

void Update(uint32_t a)
{
    snes_button_state_t controller_state = GetControllerState1();

    if(controller_state.buttons.Select){
        init_glider(A);
        run=0;
    }
    if(controller_state.buttons.Y){
        init_simple_row(A);
        run=0;
    }
    if(controller_state.buttons.X){
        init_empty_field(A);
        run=0;
    }

    if(run==1){
    	if(roundcount % speed == 0) update_Array(A);
        roundcount++;
        if(buttonPressedCountStuff(0,10)){
            if(controller_state.buttons.L) if(speed<20) speed++;
            if(controller_state.buttons.R) if(speed>1) speed--;
            if(controller_state.buttons.Start){
                run=0;
                roundcount=0;
            }
        }
    }else{
        if(controller_state.buttons.A) A[curx][cury]=1;
        if(controller_state.buttons.B) A[curx][cury]=0;

        if(buttonPressedCountStuff(0,10)){
            if(controller_state.buttons.Start){
                run=1;
            }

            if(controller_state.buttons.Left) curx--;
            if(controller_state.buttons.Right) curx++;
            if(controller_state.buttons.Up) cury--;
            if(controller_state.buttons.Down) cury++;
        }
    }
}

void Draw(Bitmap *b)
{
    ClearBitmap(b);

    if(!run){
         DrawFilledRectangle(b, curx*5,  cury*5, 5, 5, RGB(100,100,100));
    }

    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            if(A[i][j]==1){
                DrawFilledRectangle(b, i*5,  j*5, 5, 5, RGB(255,255,255));
            }
        }
    }
}
