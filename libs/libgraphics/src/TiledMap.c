#include <math.h>

#include <Drawing.h>
#include <Bitmap.h>
#include <TiledMap.h>
#include <Clipping.h>
#include <Collision.h>

static inline int max(int a, int b) { return a >= b ? a : b; }
static inline int min(int a, int b) { return a <= b ? a : b; }

float vectorLength(int x, int y)
{
  return sqrt((x * x) + (y * y));
  //return abs(x)+abs(y);
}


TiledMap* TiledMap_init(int sizeX, int sizeY, uint8_t tileSize, const RLEBitmap **tileBitmaps)
{
  TiledMap *map = (TiledMap*) malloc(sizeof(TiledMap) + (sizeX * sizeY * sizeof(Tile)));
  map->sizeX = sizeX;
  map->sizeY = sizeY;
  map->tileSize = tileSize;
  map->tileBitmaps = tileBitmaps;
  map->objects = (list) { };
  for (int i = 0; i < sizeX * sizeY * sizeof(Tile); i++)
    map->tiles[i] = 0;
  return map;
}

void TiledMap_update(TiledMap *map, uint32_t delta)
{
  list_el *i = map->objects.head, *t;
  while (i)
  {
    t = i->next;
    MObj_update(map, (MapObject*) i->val, delta);
    i = t;
  }
}

void TiledMap_draw(Bitmap *surface, TiledMap *map, int xo, int yo)
{
  // Indices of top-left most tile to draw
  int tx = floor(xo / SCREEN_X);
  int ty = floor(yo / SCREEN_Y);
  // Number of tiles per screen
  int txs = ceil(SCREEN_X / map->tileSize);
  int tys = ceil(SCREEN_Y / map->tileSize);

  for (int y = max(0, ty); y < min(map->sizeY, ty + tys); y++)
    for (int x = max(0, tx); x < min(map->sizeX, tx + txs); x++)
      DrawRLEBitmap(surface,
          map->tileBitmaps[
              map->tiles[y * map->sizeX + x]
            ],
          xo + (x * map->tileSize),
          yo + (y * map->tileSize));

  // Draw objects
  list_el *i = map->objects.head;
  while (i)
  {
    MObj_draw(surface, map, (MapObject*) i->val, xo, yo);
    i = i->next;
  }
}





void MObj_moveTo(MapObject *obj, int tx, int ty, int speed, bool collision)
{
  if (!obj->moving)
    obj->moving = (MovementInfo*) malloc(sizeof(MovementInfo));
  *obj->moving = (MovementInfo) { MOVEMENT_TARGETED, tx, ty, speed, .collision = collision };
}

void MObj_moveForced(MapObject *obj, int velX, int velY, int forceX, int forceY, bool collision)
{
  if (!obj->moving)
    obj->moving = (MovementInfo*) malloc(sizeof(MovementInfo));
  *obj->moving = (MovementInfo) { MOVEMENT_FORCED, forceX, forceY, velX, velY, collision };
}

void MObj_cancelMovement(MapObject *obj)
{
  if (obj->moving)
  {
    free(obj->moving);
    obj->moving = NULL;
  }
}

bool MObj_collisionMObj(MapObject *obj, MapObject *target)
{
  if (obj == target)
    return false;

  if (obj->collision == COLLISION_BB && target->collision == COLLISION_BB)
    return Collision_BB_BB(
        obj->x,
        obj->y,
        obj->sizeX,
        obj->sizeY,
        target->x,
        target->y,
        target->sizeX,
        target->sizeY);
  else if (obj->collision == COLLISION_SPRITE && target->collision == COLLISION_SPRITE)
    return Collision_Sprite_Sprite(
        obj->x / PIXEL_RESOLUTION,
        obj->y / PIXEL_RESOLUTION,
        obj->bitmap,
        target->x / PIXEL_RESOLUTION,
        target->y / PIXEL_RESOLUTION,
        target->bitmap);
  else if (obj->collision == COLLISION_SPRITE && target->collision == COLLISION_BB)
    return Collision_BB_Sprite(
        target->x / PIXEL_RESOLUTION,
        target->y / PIXEL_RESOLUTION,
        target->sizeX / PIXEL_RESOLUTION,
        target->sizeY / PIXEL_RESOLUTION,
        obj->x / PIXEL_RESOLUTION,
        obj->y / PIXEL_RESOLUTION,
        obj->bitmap);
  else if (obj->collision == COLLISION_BB && target->collision == COLLISION_SPRITE)
    return Collision_BB_Sprite(
        obj->x / PIXEL_RESOLUTION,
        obj->y / PIXEL_RESOLUTION,
        obj->sizeX / PIXEL_RESOLUTION,
        obj->sizeY / PIXEL_RESOLUTION,
        target->x / PIXEL_RESOLUTION,
        target->y / PIXEL_RESOLUTION,
        target->bitmap);
  else
    return false;
}



static inline void MObj_update_animation(MapObject *obj, uint32_t delta);
static inline bool MObj_update_collision(TiledMap *map, MapObject *obj);
static inline void MObj_update_movement(TiledMap *map, MapObject *obj, uint32_t delta);
static inline bool MObj_update_movement_targeted(MapObject *obj, int delta);
static inline bool MObj_update_movement_forced(MapObject *obj, int delta);

void MObj_update(TiledMap *map, MapObject *obj, uint32_t delta)
{
  // Update animation
  if (obj->animation)
    MObj_update_animation(obj, delta);

  if (obj->moving)
    MObj_update_movement(map, obj, delta);
}

void MObj_draw(Bitmap *surface, TiledMap *map, MapObject *obj, int xo, int yo)
{
  DrawRLEBitmap(surface, obj->bitmap,
      obj->x / PIXEL_RESOLUTION + xo,
      obj->y / PIXEL_RESOLUTION + yo);
}





static inline void MObj_update_animation(MapObject *obj, uint32_t delta)
{
  // In ticks
  int animationLength = obj->animation->frametime * obj->animation->framecount;
  int animationHalfLength = animationLength;
  if (obj->animation->flags & ANIM_REPEAT_REVERSED)
    animationLength *= 2;

  obj->animationTime += delta;
  if (obj->animationTime >= animationLength)
  {
    // Callback
    if (obj->animation->onEnd)
      obj->animation->onEnd(obj);

    // Reset animation time / animation
    if (obj->animation->flags & ANIM_REPEAT)
      obj->animationTime %= animationLength;
    else
    {
      obj->animation = NULL;
      return;
    }
  }

  // Save current image
  int frameIndex;

  if (obj->animation->flags & ANIM_REPEAT_REVERSED
      && obj->animationTime >= animationHalfLength)
    frameIndex = (animationLength - obj->animationTime) / obj->animation->frametime;
  else
    frameIndex = obj->animationTime / obj->animation->frametime;

  if (obj->animation->flags & ANIM_REVERSED)
    frameIndex = obj->animation->framecount - frameIndex - 1;

  obj->bitmap = obj->animation->frames[frameIndex];
}

static inline bool MObj_update_collision(TiledMap *map, MapObject *obj)
{
  list_el *i = map->objects.head, *t;
  while (i)
  {
    t = i->next;
    // For each known object:

    if (MObj_collisionMObj(obj, (MapObject*) i->val))
    {
      // If the callback is defined
      if (obj->moving->onCollision)
      {
        if (!obj->moving->onCollision(obj, (MapObject*) i->val))
          // Do not ignore the collision
          return true;
      }
      else
      {
        MObj_cancelMovement(obj);
        return true;
      }
    }

    i = t;
  }

  return false;
}

static inline void MObj_update_movement(TiledMap *map, MapObject *obj, uint32_t delta)
{
  int oldPos[] = { obj->x, obj->y };
  bool targetReached = false;

  if (obj->moving->type == MOVEMENT_TARGETED)
    targetReached = MObj_update_movement_targeted(obj, delta);
  else
    MObj_update_movement_forced(obj, delta);

  // Do collision detection
  if (obj->moving->collision && obj->collision)
    if (MObj_update_collision(map, obj))
    {
      // Reset position on collision
      obj->x = oldPos[0];
      obj->y = oldPos[1];
    }

  // Check for target reached
  if (targetReached)
  {
    if (obj->moving->onTargetReached)
      obj->moving->onTargetReached(obj);
    else
      MObj_cancelMovement(obj);
  }
}

static inline bool MObj_update_movement_targeted(MapObject *obj, int delta)
{
  int dirX = (obj->moving->tx - obj->x);
  int dirY = (obj->moving->ty - obj->y);
  float length = vectorLength(dirX, dirY);
  float mult = delta * obj->moving->speed / length;
  int deltaX = (int) (dirX * mult);
  int deltaY = (int) (dirY * mult);
  obj->x = obj->x + deltaX;
  obj->y = obj->y + deltaY;
  return abs(deltaX) >= abs(dirX);
}

static inline bool MObj_update_movement_forced(MapObject *obj, int delta)
{
  obj->moving->speed += obj->moving->tx * delta;
  obj->moving->speedY += obj->moving->ty * delta;
  obj->x += obj->moving->speed;
  obj->y += obj->moving->speedY;
  return false;
}
