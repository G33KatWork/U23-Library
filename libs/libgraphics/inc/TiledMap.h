#ifndef TILEDMAP_H
#define TILEDMAP_H

#include <RLEBitmap.h>
#include <list.h>


#ifndef GRAVITATION
  #define GRAVITATION 10
#endif

#ifndef PIXEL_RESOLUTION
  #define PIXEL_RESOLUTION 256
#endif

// Simply an index into an array
//  TODO: turn into an info-struct array
typedef int8_t Tile;


// Size may be (0,0) to only use MapObjects
typedef struct TiledMap {
  int sizeX, sizeY;    // Size in tiles
  uint8_t tileSize;    // Size of one tile in pixels
  const RLEBitmap **tileBitmaps;
  list objects;        // List of MapObjects
  Tile tiles[];
} TiledMap;


// Allocate a new TiledMap, return pointer
TiledMap* TiledMap_init(int sizeX, int sizeY, uint8_t tileSize, const RLEBitmap **tileBitmaps);
// Update and draw loop calls
void TiledMap_update(TiledMap *map, uint32_t delta);
void TiledMap_draw(Bitmap *surface, TiledMap *map, int xo, int yo);




typedef struct MapObject MapObject;
typedef struct MovementInfo MovementInfo;
typedef enum MovementType MovementType;
typedef enum CollisionFlags CollisionFlags;
typedef enum AnimationFlags AnimationFlags;
typedef struct Animation Animation;


// Collision behaviour for MapObjects
enum CollisionFlags
{
  COLLISION_NONE,
  COLLISION_BB,     // Bounding box, uses sizeX, sizeY attributes
  COLLISION_SPRITE, // Sprite collision (pixelwise)
};


struct MapObject
{
  // Object position, a pixel is PIXEL_RESOLUTION wide
  int x, y;
  const RLEBitmap *bitmap;    // Holds the current bitmap, changes during animation
  int sizeX, sizeY;           // Size of the bounding box
  CollisionFlags collision;
  MovementInfo *moving;       // Info-struct while moving, do not set this directly
  const Animation* animation; // Info-struct while animating
  int animationTime;          // Animation time in ticks
};

// Available types of object movement
enum MovementType
{
  MOVEMENT_TARGETED, // Moves to designated target position
  MOVEMENT_FORCED,   // Moves into given direction, applies force
};

struct MovementInfo
{
  MovementType type;
  int tx, ty;
  int speed, speedY;
  bool collision;     // Whether to check for collisions

  // Callbacks
  //  You may remove 'obj' from the list within the callbacks
  //  Removing target may lead to null pointer dereferencing
  bool (*onCollision) (MapObject *obj, MapObject *target); // Return true to ignore collision
  void (*onTargetReached) (MapObject *obj);
};


// Animation types
enum AnimationFlags
{
  ANIM_NORMAL=0,
  ANIM_REVERSED=1<<0,        // Animate backwards
  ANIM_REPEAT=1<<1,          // Repeat the sequence indefinitely
  ANIM_REPEAT_REVERSED=1<<2, // Additionaly goes through the sequence backwards
                             //  in each cycle (to be used with ANIM_REPEAT)
};

// Information for an animation, can be reused for different MapObjects
struct Animation
{
  int frametime, framecount;
  AnimationFlags flags;
  const RLEBitmap **frames;
  void (*onEnd) (MapObject *obj);
};


// Initiate a movement
static inline void MObj_moveDirection(MapObject *obj, int velX, int velY, bool collision);
static inline void MObj_moveGravity(MapObject *obj, int velX, int velY, bool collision);
void MObj_moveTo(MapObject *obj, int tx, int ty, int speed, bool collision);
void MObj_moveForced(MapObject *obj, int velX, int velY, int forceX, int forceY, bool collision);

void MObj_cancelMovement(MapObject *obj);

// Check whether two objects collide
bool MObj_collisionMObj(MapObject *obj, MapObject *target);

// Update and draw loop calls for individual MapObjects
void MObj_update(TiledMap *map, MapObject *obj, uint32_t delta);
void MObj_draw(Bitmap *surface, TiledMap *map, MapObject *obj, int xo, int yo);


static inline void MObj_moveDirection(MapObject *obj, int velX, int velY, bool collision)
{
  MObj_moveForced(obj, velX, velY, 0, 0, collision);
}
static inline void MObj_moveGravity(MapObject *obj, int velX, int velY, bool collision)
{
  MObj_moveForced(obj, velX, velY, 0, GRAVITATION, collision);
}

#endif
