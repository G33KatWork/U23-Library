#include <math.h>
#include <string.h> //memset

#include <Drawing.h>
#include <Bitmap.h>
#include <TiledMap.h>
#include <ChunkedMap.h>
#include <Clipping.h>
#include <Collision.h>


static inline int max(int a, int b) { return a >= b ? a : b; }
static inline int min(int a, int b) { return a <= b ? a : b; }

static inline float vectorLength(float x, float y)
{
  return sqrt((x * x) + (y * y));
  //return abs(x)+abs(y);
}



TiledMap* TiledMap_init(int sizeX, int sizeY, uint8_t tileSize, TileInfo *tileInfo)
{
  TiledMap *map = (TiledMap*) malloc(sizeof(TiledMap) + (sizeX * sizeY * sizeof(Tile)));
  map->sizeX = sizeX;
  map->sizeY = sizeY;
  map->tileSize = tileSize;
  map->tileInfo = tileInfo;
  map->objects = (list) { };
  memset(&map->tiles[0], '\0', sizeX * sizeY * sizeof(Tile));
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
  if (map->tileSize != 0)
  {
    // Indices of top-left most tile to draw
    int tx = xo / SCREEN_X;
    int ty = yo / SCREEN_Y;
    // Number of tiles per screen
    int txs = SCREEN_X / map->tileSize + 1;
    int tys = SCREEN_Y / map->tileSize + 1;

    ClipRectangle(&tx, &ty, &txs, &tys, map->sizeX, map->sizeY);

    for (int y = ty; y < tys + ty; y++)
      for (int x = tx; x < txs + tx; x++)
        DrawRLEBitmap(surface,
            map->tileInfo[ TiledMap_getTile(map, x, y) ].bitmap,
            xo + (x * map->tileSize),
            yo + (y * map->tileSize));
  }

  // Draw objects
  list_el *i = map->objects.head;
  while (i)
  {
    MObj_draw(surface, (MapObject*) i->val, xo, yo);
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

bool MObj_collisionMap(TiledMap *map, MapObject *obj)
{
  if (obj->collision == COLLISION_NONE)
    return false;

  if (map->tileSize == 0)
    return false;

  int tileSize = map->tileSize * PIXEL_RESOLUTION;
  int tx = obj->x / tileSize;
  int ty = obj->y / tileSize;
  int w, h;

  if (obj->collision == COLLISION_BB)
  {
    w = ((obj->x + obj->sizeX - 1) / tileSize) - tx + 1;
    h = ((obj->y + obj->sizeY - 1) / tileSize) - ty + 1;
    //w = obj->sizeX / tileSize + 1;
    //h = obj->sizeY / tileSize + 1;
  }
  else if (obj->collision == COLLISION_SPRITE)
  {
    w = ((obj->x + (obj->bitmap->width  * PIXEL_RESOLUTION)) / tileSize) - tx + 1;
    h = ((obj->y + (obj->bitmap->height * PIXEL_RESOLUTION)) / tileSize) - ty + 1;
    //w = obj->bitmap->width  / map->tileSize + 1;
    //h = obj->bitmap->height / map->tileSize + 1;
  }

  ClipRectangle(&tx, &ty, &w, &h, map->sizeX, map->sizeY);

  for (int x = tx; x < w + tx; x++)
    for (int y = ty; y < h + ty; y++)
      if (map->tileInfo[ TiledMap_getTile(map, x, y) ].collision &&
          (
            obj->collision == COLLISION_BB ||
            (obj->collision == COLLISION_SPRITE &&
                Collision_BB_Sprite(
                  x * map->tileSize,
                  y * map->tileSize,
                  map->tileSize / PIXEL_RESOLUTION,
                  map->tileSize / PIXEL_RESOLUTION,
                  obj->x / PIXEL_RESOLUTION,
                  obj->y / PIXEL_RESOLUTION,
                  obj->bitmap)
              )
          )
         )
        return true;
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

void MObj_draw(Bitmap *surface, MapObject *obj, int xo, int yo)
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
      if (obj->moving->onObjCollision)
      {
        if (!obj->moving->onObjCollision(obj, (MapObject*) i->val))
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

  if (MObj_collisionMap(map, obj))
  {
    // If the callback is defined
    if (obj->moving->onMapCollision)
    {
      if (!obj->moving->onMapCollision(obj))
        // Do not ignore the collision
        return true;
    }
    else
    {
      MObj_cancelMovement(obj);
      return true;
    }
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
  if (obj->moving && targetReached)
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
  obj->x += deltaX;
  obj->y += deltaY;

  bool targetReached = abs(deltaX) >= abs(dirX) && abs(deltaY) >= abs(dirY);
  if (targetReached)
  {
    // Move exactly to desired position
    obj->x = obj->moving->tx;
    obj->y = obj->moving->ty;
  }

  return targetReached;
}

static inline bool MObj_update_movement_forced(MapObject *obj, int delta)
{
  obj->moving->speed += obj->moving->tx * delta;
  obj->moving->speedY += obj->moving->ty * delta;
  obj->x += obj->moving->speed;
  obj->y += obj->moving->speedY;
  return false;
}





/*********************
 *                   *
 *    CHUNKED MAP    *
 *                   *
 *********************/


ChunkedMap* ChunkedMap_init(uint8_t tileSize, TileInfo *tileInfo, ChunkLoadHandler loadChunk, ChunkSaveHandler saveChunk)
{
  // Use TiledMap instead...
  if (tileSize == 0)
    return NULL;

  ChunkedMap *map = (ChunkedMap*) malloc(sizeof(ChunkedMap));
  *map = (ChunkedMap) {
    .tileSize = tileSize,
    .tileInfo = tileInfo,
    .objects = (list) {},
    .loadChunk = loadChunk,
    .saveChunk = saveChunk,
  };
  return map;
}

void ChunkedMap_update(ChunkedMap *map, uint32_t delta)
{
  list_el *i = map->objects.head, *t;
  while (i)
  {
    t = i->next;
    MObj_update_chunkedMap(map, (MapObject*) i->val, delta);
    i = t;
  }
}

void ChunkedMap_draw(Bitmap *surface, ChunkedMap *map, int xo, int yo)
{
  if (map->tileSize != 0)
  {
    // Indices of top-left most tile to draw
    int tx = xo / SCREEN_X;
    int ty = yo / SCREEN_Y;
    // Number of tiles per screen
    int txs = SCREEN_X / map->tileSize + 1;
    int tys = SCREEN_Y / map->tileSize + 1;

    for (int y = ty; y < tys + ty; y++)
      for (int x = tx; x < txs + tx; x++)
        DrawRLEBitmap(surface,
            map->tileInfo[ ChunkedMap_getTile(map, x, y) ].bitmap,
            xo + (x * map->tileSize),
            yo + (y * map->tileSize));
  }

  // Draw objects
  list_el *i = map->objects.head;
  while (i)
  {
    MObj_draw(surface, (MapObject*) i->val, xo, yo);
    i = i->next;
  }
}





bool MObj_collisionChunkedMap(ChunkedMap *map, MapObject *obj)
{
  if (obj->collision == COLLISION_NONE)
    return false;

  int tileSize = map->tileSize * PIXEL_RESOLUTION;
  int tx = obj->x / tileSize;
  int ty = obj->y / tileSize;
  int w, h;

  if (obj->collision == COLLISION_BB)
  {
    w = ((obj->x + obj->sizeX - 1) / tileSize) - tx + 1;
    h = ((obj->y + obj->sizeY - 1) / tileSize) - ty + 1;
  }
  else if (obj->collision == COLLISION_SPRITE)
  {
    w = ((obj->x + (obj->bitmap->width  * PIXEL_RESOLUTION)) / tileSize) - tx + 1;
    h = ((obj->y + (obj->bitmap->height * PIXEL_RESOLUTION)) / tileSize) - ty + 1;
  }

  for (int x = tx; x < w + tx; x++)
    for (int y = ty; y < h + ty; y++)
      if (map->tileInfo[ ChunkedMap_getTile(map, x, y) ].collision &&
          (
            obj->collision == COLLISION_BB ||
            (obj->collision == COLLISION_SPRITE &&
                Collision_BB_Sprite(
                  x * map->tileSize,
                  y * map->tileSize,
                  map->tileSize / PIXEL_RESOLUTION,
                  map->tileSize / PIXEL_RESOLUTION,
                  obj->x / PIXEL_RESOLUTION,
                  obj->y / PIXEL_RESOLUTION,
                  obj->bitmap)
              )
          )
         )
        return true;
  return false;
}



static inline bool MObj_update_collision_chunkedMap(ChunkedMap *map, MapObject *obj);
static inline void MObj_update_movement_chunkedMap(ChunkedMap *map, MapObject *obj, uint32_t delta);


void MObj_update_chunkedMap(ChunkedMap *map, MapObject *obj, uint32_t delta)
{
  // Update animation
  if (obj->animation)
    MObj_update_animation(obj, delta);

  if (obj->moving)
    MObj_update_movement_chunkedMap(map, obj, delta);
}



static inline bool MObj_update_collision_chunkedMap(ChunkedMap *map, MapObject *obj)
{
  list_el *i = map->objects.head, *t;
  while (i)
  {
    t = i->next;
    // For each known object:

    if (MObj_collisionMObj(obj, (MapObject*) i->val))
    {
      // If the callback is defined
      if (obj->moving->onObjCollision)
      {
        if (!obj->moving->onObjCollision(obj, (MapObject*) i->val))
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

  if (MObj_collisionChunkedMap(map, obj))
  {
    // If the callback is defined
    if (obj->moving->onMapCollision)
    {
      if (!obj->moving->onMapCollision(obj))
        // Do not ignore the collision
        return true;
    }
    else
    {
      MObj_cancelMovement(obj);
      return true;
    }
  }

  return false;
}


static inline void MObj_update_movement_chunkedMap(ChunkedMap *map, MapObject *obj, uint32_t delta)
{
  int oldPos[] = { obj->x, obj->y };
  bool targetReached = false;

  if (obj->moving->type == MOVEMENT_TARGETED)
    targetReached = MObj_update_movement_targeted(obj, delta);
  else
    MObj_update_movement_forced(obj, delta);

  // Do collision detection
  if (obj->moving->collision && obj->collision)
    if (MObj_update_collision_chunkedMap(map, obj))
    {
      // Reset position on collision
      obj->x = oldPos[0];
      obj->y = oldPos[1];
    }

  // Check for target reached
  if (obj->moving && targetReached)
  {
    if (obj->moving->onTargetReached)
      obj->moving->onTargetReached(obj);
    else
      MObj_cancelMovement(obj);
  }
}
