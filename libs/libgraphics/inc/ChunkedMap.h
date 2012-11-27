#ifndef CHUNKEDMAP_H
#define CHUNKEDMAP_H

#include <TiledMap.h>



#define CHUNKSIZE 9

typedef struct Chunk Chunk;

struct Chunk
{
  int x, y, height;
  Tile tiles[CHUNKSIZE * CHUNKSIZE];
};



typedef void (*ChunkLoadHandler) (Chunk *c, int x, int y);
typedef void (*ChunkSaveHandler) (Chunk *c);

// Size may be (0,0) to only use MapObjects
typedef struct ChunkedMap
{
  uint8_t tileSize;    // Size of one tile in pixels
  list objects;        // List of MapObjects
  TileInfo *tileInfo;  // Properties of the tile types
  ChunkLoadHandler loadChunk;
  ChunkSaveHandler saveChunk;
  Chunk *chunkHashMap[256];
} ChunkedMap;


// Allocate a new ChunkedMap, return pointer
ChunkedMap* ChunkedMap_init(uint8_t tileSize, TileInfo *tileInfo, ChunkLoadHandler loadChunk, ChunkSaveHandler saveChunk);
// Update and draw loop calls
void ChunkedMap_update(ChunkedMap *map, uint32_t delta);
void ChunkedMap_draw(Bitmap *surface, ChunkedMap *map, int xo, int yo);

static inline int ChunkHash(int x, int y);
static inline Chunk *chunk_load(ChunkedMap *map, int x, int y);
static inline Chunk *ChunkedMap_getChunk(ChunkedMap *map, int x, int y);
static inline Tile ChunkedMap_getTile(ChunkedMap *map, int x, int y);
static inline void ChunkedMap_setTile(ChunkedMap *map, int x, int y, Tile t);


// Check whether an object collides with the map's tiles
bool MObj_collisionChunkedMap(ChunkedMap *map, MapObject *obj);

// Update loop call for individual MapObjects
void MObj_update_chunkedMap(ChunkedMap *map, MapObject *obj, uint32_t delta);




static inline int ChunkHash(int x, int y)
{
  return ((x & 0xf) << 4) | (y & 0xf);
  //return (x^y) & 0xff;
}

static inline Chunk *chunk_load(ChunkedMap *map, int x, int y)
{
  int i = ChunkHash(x, y);
  Chunk *c = map->chunkHashMap[i];
  if (c)
    map->saveChunk(c);
  else
  {
    c = malloc(sizeof(Chunk));
    map->chunkHashMap[i] = c;
  }
  map->loadChunk(c, x, y);
  return c;
}

static inline Chunk *ChunkedMap_getChunk(ChunkedMap *map, int x, int y)
{
  Chunk *c = map->chunkHashMap[ChunkHash(x, y)];
  if (!c || c->x != x || c-> y != y)
    return chunk_load(map, x, y);
  return c;
}

static inline Tile ChunkedMap_getTile(ChunkedMap *map, int x, int y)
{
  Chunk *c = ChunkedMap_getChunk(map, x / CHUNKSIZE, y / CHUNKSIZE);
  return c->tiles[ (normMod(y, CHUNKSIZE) * CHUNKSIZE) + normMod(x, CHUNKSIZE) ];
}
static inline void ChunkedMap_setTile(ChunkedMap *map, int x, int y, Tile t)
{
  Chunk *c = ChunkedMap_getChunk(map, x / CHUNKSIZE, y / CHUNKSIZE);
  c->tiles[ (normMod(y, CHUNKSIZE) * CHUNKSIZE) + normMod(x, CHUNKSIZE) ] = t;
}


#endif
