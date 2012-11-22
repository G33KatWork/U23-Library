#include <TiledMap.h>
#include <game/Game.h>
#include <Debug.h>
#include <game/Filesystem.h>
#include <game/Accelerometer.h>
#include <game/Font.h>

#include <stdio.h>
#include <string.h>
#include <stm32f4xx/stm32f4xx_rng.h>
#include <math.h>

#include "../inc/Highscore.h"
#include "../inc/Sprites.h"

//#define ACCELEROMETER

TiledMap *map;
MapObject *ship;
MapObject *stars;

static int32_t highscore = 0;
static int32_t lastMenuUpdate;
static int32_t gameOverEntryTime;
static bool pause_mode = false;
static bool gameover = false;
static bool doneOnce = false;

//#define WRITE_HIGHSCORE
#define NumberOfStars 9


void Init(struct Gamestate*);
void OnEnter(struct Gamestate*);
void Update(uint32_t );
void Draw(Bitmap* surface);

Gamestate game = { Init, OnEnter, NULL, Update, Draw, };


void gameOver(Bitmap *currframe)
{
  if (!gameover)
    return;

  if (!doneOnce)
  {
    if (currframe)
      DrawFilledRectangle(currframe,0,0,320,200,RGB(255,0,0));

#ifdef WRITE_HIGHSCORE
    if (highscore)
      WriteHighscore(highscore);
#endif

    doneOnce = true;
  }

  DrawText(currframe, "GAME OVER", 50, 50);
}

void OnEnter(struct Gamestate* state)
{
  highscore = 0;
  pause_mode = false;
  gameOver(NULL);
  gameover = false;
}

// Set gameover at a star-ship collision
bool starCollision(MapObject *obj, MapObject *target)
{
  if (target != ship)
    return true;

  doneOnce = false;
  gameover = true;
  gameOverEntryTime = SysTickCounter;
  // Do not ignore collision
  return false;
}

// Reset a star and let it fall down again
void resetStar(MapObject *obj)
{
  // New sprite
  obj->bitmap = sprites[GetRandomInteger()%42];
  // Random position
  obj->x = GetRandomInteger() % (360 * PIXEL_RESOLUTION) - (32/2);
  obj->y = -32 * PIXEL_RESOLUTION;
  // Initiate movement
  MObj_moveTo(obj, obj->x, 200 * PIXEL_RESOLUTION, PIXEL_RESOLUTION, true);
  // Set callbacks
  obj->moving->onTargetReached = resetStar;
  obj->moving->onObjCollision = starCollision;
}

// Reset all objects for new game
void resetObjects()
{
  for (int i=0; i<NumberOfStars; i++)
  {
    resetStar(&stars[i]);
    stars[i].y = GetRandomInteger() % (200 * PIXEL_RESOLUTION) - (200 * PIXEL_RESOLUTION);
  }

  ship->x = (320-42)*PIXEL_RESOLUTION/2;
  ship->y = (200-42)*PIXEL_RESOLUTION/2;
}

void Init(struct Gamestate* state)
{
  EnableDebugOutput(DEBUG_USART);
  printf("Init\r\n");

  InitializeLEDs();

  SetLEDs(0x01);
  SetLEDs(0x07);

#ifdef ACCELEROMETER
  InitializeAccelerometer();
  printf("Init Accelerometer: %s\r\n", PingAccelerometer() > 0 ? "OKAY" : "FAILED");
  CalibrateAccelerometer();
#endif


  // Create the map
  map = TiledMap_init(0, 0, 0, NULL);

  // Create the stars
  stars = malloc(sizeof(MapObject) * NumberOfStars);
  for(int i=0;i<NumberOfStars;i++)
  {
    // Initialize
    stars[i] = (MapObject) { .collision = COLLISION_SPRITE };
    // Add to map
    listInsert(&map->objects, &stars[i]);
  }

  ship = malloc(sizeof(MapObject));
  *ship = (MapObject) {
    .bitmap = spacecraft[0],
    .collision = COLLISION_SPRITE
  };
  listInsert(&map->objects, ship);

  resetObjects();
}

void Draw(Bitmap *surface) {
  ClearBitmap(surface);

  if (pause_mode)
  {
    setFont(fontwhite16);
    DrawText(surface, "PAUSE", 50, 180);
  }
  else if (gameover)
  {
    // Draw the objects
    TiledMap_draw(surface, map, 0, 0);
    gameOver(surface);

    if (SysTickCounter - gameOverEntryTime > 200)
    {
      // Start new game
      gameover = false;
      highscore = 0;
      resetObjects();
    }
  }
  else
  {
    // Draw the objects
    TiledMap_draw(surface, map, 0, 0);
  }

  setFont(fontwhite16);
  char *highscoreString;
  asprintf(&highscoreString, "%d", highscore);
  DrawText(surface, highscoreString, 0, 8);
  free(highscoreString);
}

void Update(uint32_t delta) {
  int32_t old_pos_x = ship->x;
  int32_t old_pos_y = ship->y;

  // Update movement
  if (!gameover && !pause_mode)
    TiledMap_update(map, delta);

  if ((SysTickCounter - lastMenuUpdate) > 40 )
  {
    if (GetControllerState1().buttons.Start) {
      lastMenuUpdate = SysTickCounter;
      pause_mode = pause_mode ? 0 : 1;
    } else if (GetControllerState1().buttons.Select) {
      ExitState();
    }
  }

  if (pause_mode || gameover)
    return;
  else
    highscore += delta;

  #ifdef ACCELEROMETER
    /*if (UserButtonState()) {
      ReadRawAccelerometerData(zero);
    }*/

    int8_t components[3];
    ReadCalibratedAccelerometerData(components);
    int32_t dx = components[1];
    int32_t dy = components[0];

    dx = (dx / 10) * 10;
    dy = (dy / 10) * 10;

    ship->x -= (dx / 10) * delta * PIXEL_RESOLUTION;
    ship->y -= (dy / 10) * delta * PIXEL_RESOLUTION;
  #else
    ship->x += GetControllerState1().buttons.Right * delta * PIXEL_RESOLUTION;
    ship->x -= GetControllerState1().buttons.Left * delta * PIXEL_RESOLUTION;
    ship->y -= GetControllerState1().buttons.Up * delta * PIXEL_RESOLUTION;
    ship->y += GetControllerState1().buttons.Down * delta * PIXEL_RESOLUTION;
  #endif

  int dir = 0;

  if (old_pos_x < ship->x) {
    dir = 1;
  } else if (old_pos_x > ship->x) {
    dir = -1;
  }

  if (ship->x < 0 * PIXEL_RESOLUTION) {
    ship->x = 0 * PIXEL_RESOLUTION;
  }

  if (ship->x >= (320 - 42) * PIXEL_RESOLUTION) {
    ship->x = (320 - 42) * PIXEL_RESOLUTION;
  }

  if (ship->y >= (200 - 42) * PIXEL_RESOLUTION) {
    ship->y = (200 - 42) * PIXEL_RESOLUTION;
  }

  if (ship->y < 0 * PIXEL_RESOLUTION) {
    ship->y = 0 * PIXEL_RESOLUTION;
  }

  ship->bitmap = spacecraft[dir + 1];
}
