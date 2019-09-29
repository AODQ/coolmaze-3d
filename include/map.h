#ifndef MAP_H
#define MAP_H
#include "gfx.h"

typedef struct{
  unsigned char w, h;
  unsigned char start[2];
  unsigned char end[2];
  char *points;
} map_t;

int map_collision(map_t * m, float x, float y);

int map_check_goal(map_t * m, float x, float y);

map_t * map_ld(const char* path);

void map_delete(map_t * map);

void map_debug(map_t * map);

g_model_t * map_model(map_t * map);

#endif

