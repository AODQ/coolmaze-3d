#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "map.h"

int map_collision(map_t * map, float x, float y)
{
  const float offset[5][2] = {
    {0.f, 0.f},
    {0.2, 0.f},
    {-0.2, 0.f},
    {0.0, 0.2},
    {0.0, -0.2}
  };
  for(int i = 0; i < 5; ++i){
    int xi = (x + offset[i][0]);
    int yi = (y + offset[i][1]);
    if(xi < 0 || xi >= map->w
    || yi < 0 || yi >= map->h
    || map->points[yi*map->w + xi])
      return 1;
  }
  return 0;
}

int map_check_goal(map_t * map, float x, float y)
{
    unsigned char xi = x;
    unsigned char yi = y;
    return (xi == map->end[0]) && (yi == map->end[1]);
}

map_t * map_ld(const char* path)
{
  FILE * fp = fopen(path, "rb");
  map_t * m = malloc(sizeof(map_t));
  char buf[5] = {0,0,0,0,0};
  fread(&buf, 1, 4, fp);
  if(strcmp(buf, "BAD1") != 0){
    return NULL;
  }
  m->w = fgetc(fp);
  m->h = fgetc(fp);
  m->start[0] = fgetc(fp);
  m->start[1] = fgetc(fp);
  m->end[0] = fgetc(fp);
  m->end[1] = fgetc(fp);

  m->points = malloc(m->w * m->h);
  for(int i = 0; i < m->h; i++){
    for(int j = 0; j < m->w; j+=8){
      unsigned char c = fgetc(fp);
      for(int k = 0; k < (j+8 > m->w ? m->w - j : 8); k++){
        m->points[i*m->w + j + k] = (c & 0x80 ? 1 : 0);
        c <<= 1;
      }
    }
  }
  return m;
}

void map_delete(map_t * map)
{
  free(map->points);
  free(map);
}

void map_debug(map_t * m)
{
  if(m){
    printf("%d, %d\n", m->end[0], m->end[1]);
    if(m->points){
      for(int i = 0; i < m->h; i++) {
        for(int j = 0; j < m->w; j++) {
          putchar(m->points[i*m->w + j] ? '1' : '0');
        }
        putchar('\n');
    }
  }
    else
      puts("Invalid points");
  }
  else
    puts("Invalid map");
}

g_model_t * map_model(map_t * m)
{
  g_model_t * model;
  const float TC[8] = {
    0.f, 0.f, 
    0.f, 1.f,
    1.f, 1.f,
    1.f, 0.f
  };
  float VX[12] = {
    0.f, 1.f, 0.f,
    0.f, 0.f, 0.f,
    0.f, 0.f, 0.f,
    0.f, 1.f, 0.f
  };
  //TODO: make this more DRY
  size_t faces = 0;
  float * vx = NULL;
  float * tc = NULL;
  for(int i = 0; i < m->h; i++){
    for(int j = 0; j < m->w; j++){
      if(m->points[i*m->w + j] == 1){
        if(i > 0 && m->points[(i-1)*m->w + j] == 0){
          faces++;
          vx = realloc(vx, faces * sizeof(float)*12);
          tc = realloc(tc, faces * sizeof(float)*8);
          VX[2] = VX[5] = VX[8] = VX[11] = i;
          VX[0] = VX[3] = j+1;
          VX[6] = VX[9] = j;
          memcpy(vx + 12*(faces-1), VX, sizeof(float)*12);
          memcpy(tc + 8*(faces-1), TC, sizeof(float)*8);
        }
        if(i < (m->h - 1) && m->points[(i+1)*m->w + j] == 0){
          faces++;
          vx = realloc(vx, faces * sizeof(float)*12);
          tc = realloc(tc, faces * sizeof(float)*8);
          VX[2] = VX[5] = VX[8] = VX[11] = i+1;
          VX[0] = VX[3] = j;
          VX[6] = VX[9] = j+1;
          memcpy(vx + 12*(faces-1), VX, sizeof(float)*12);
          memcpy(tc + 8*(faces-1), TC, sizeof(float)*8);
        }
        if(j > 0 && m->points[i*m->w + j-1] == 0){
          faces++;
          vx = realloc(vx, faces * sizeof(float)*12);
          tc = realloc(tc, faces * sizeof(float)*8);
          VX[0] = VX[3] = VX[6] = VX[9] = j;
          VX[2] = VX[5] = i;
          VX[8] = VX[11] = i+1;
          memcpy(vx + 12*(faces-1), VX, sizeof(float)*12);
          memcpy(tc + 8*(faces-1), TC, sizeof(float)*8);
        }
        if(j < m->w - 1 && m->points[i*m->w + j+1] == 0){
          faces++;
          vx = realloc(vx, faces * sizeof(float)*12);
          tc = realloc(tc, faces * sizeof(float)*8);
          VX[0] = VX[3] = VX[6] = VX[9] = j+1;
          VX[2] = VX[5] = i+1;
          VX[8] = VX[11] = i;
          memcpy(vx + 12*(faces-1), VX, sizeof(float)*12);
          memcpy(tc + 8*(faces-1), TC, sizeof(float)*8);
        }
      }
    }
  }

  //Generate Indices
  unsigned int IN[6] = {
    0, 1, 2,
    2, 3, 0
  };
  unsigned int * en = malloc(faces*sizeof(IN));
  for (size_t i = 0; i < faces; i++){
    memcpy(en + i*6, IN, sizeof(IN));
    for(int i = 0; i < 6; i++){
      IN[i] += 4;
    }
  }

  //Create model
  model = g_ld_model(
    faces * 4, vx, tc,
    faces * 6, en,
    0
  );
  free(vx);
  free(tc);
  free(en);
  return model;
}

