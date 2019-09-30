#include "game.h"
#include "platform.h"
#include "gfx.h"
#include "sound.h"
#include "netpbm.h"
#include "map.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//input callbacks
void quit(int, void *);
void move_cb(int, void *);
void ptr(int, int, void*);

//global State
map_t * map;
g_model_t * obj;
g_billboard_t * bb;
vec_t pos, goal;
short tone[10000];
int dir[4];
const float PI = 3.14159f;
int won = 0;
float dt;
float deg;
mat_t proj;
mat_t mvp;
vec_t up = {.data =
  {0, 1, 0, 1}
};
const float off[4] = {
  PI/2.f, 0.f, 3.f*PI/2.f, PI
};
vec_t pos = {.data=
  {0.f, 0.6, 0.f, 0.f}
};

char * map_path;

//Initialization 
void gen_font(void);
void gen_tone(float freq);
void game_init(int argc, char **argv)
{
  build_keysym_table();
  pl_create_window(640, 480, 0);
  g_init();
  snd_init();

  gen_font();
  gen_tone(440);

  map_path = argv[1];
}

//game menu
void bind_keys_menu(void);
void gen_map(const char *path);
void game_menuloop(void)
{
  gen_map(map_path);
}

//Main Loop
void bind_keys_main(void);
void draw_map(void);
void draw_goal(void);
void draw_text(void);
void update_pos(void);
void check_goal(void);
void game_mainloop(void)
{
  bind_keys_main();
  proj = perspective(60.f*3.14/180.f, 4.f/3.f, 0.1f, 40.f);
  bb = g_ld_billboard(0.5f, 0);
  while(1) {
    pl_poll_events();
    dt = pl_deltat();

    draw_map();
    draw_goal();
    draw_text();
    update_pos();
    check_goal();

    pl_swap_window();
    pl_sleep();
  }
}

int game_running(void)
{
  return 1;
}

void gen_font(void)
{
  unsigned char white[4] = {255, 255, 255, 255};
  unsigned char clear[4] = {0, 0, 0, 0};
  image_t * font_img = pbm_read("tom-thumb-new.pbm", white, clear);
  img_flip(font_img);
  g_texture_t font = g_ld_texture(font_img->w, font_img->h, font_img->rgba);
  img_delete(font_img);
  g_set_font(font, 4, 6, 4, 32);
}

void gen_tone(float frequency)
{
  for(int i = 0; i < 10000; i++){
    tone[i] = 4096*sin(6.28*(i/44100.0)*frequency);
  }
}

// main Loop
void bind_keys_main(void)
{
  pl_bind_key(KEY_Q, quit, NULL);
  pl_bind_key(KEY_W, move_cb, dir+0);
  pl_bind_key(KEY_A, move_cb, dir+1);
  pl_bind_key(KEY_S, move_cb, dir+2);
  pl_bind_key(KEY_D, move_cb, dir+3);
  pl_bind_mouse(ptr, NULL);
  pl_grab_input();
}

void gen_map(const char * path)
{
  map = map_ld(path);
  obj = map_model(map);
  pos.data[0] = map->start[0] + 0.5f;
  pos.data[2] = map->start[1] + 0.5f;
  map_debug(map);
  goal.data[0] = ((float)map->end[0])+0.5;
  goal.data[1] = 0.5;
  goal.data[2] = ((float)map->end[1])+0.5;
  goal.data[3] = 1.0;
}

void draw_map(void)
{
  g_clear();
  mvp = m4xm4(proj, m4xm4(rotate(up, deg), translate(v3neg(pos))));
  g_mvp_mat(mvp);
  g_render_model(obj);
}

void draw_goal(void)
{
  mvp = m4xm4(mvp, m4xm4(translate(goal), rotate(up, -deg)));
  g_mvp_mat(mvp);
  g_render_billboard(bb);
}

void draw_text(void)
{
  char printstr[16];
  snprintf(printstr, 16, "%.3f ms", dt);
  g_puts(-0.75, -0.75, 0.25, printstr);
  snprintf(printstr, 16, "%.1f, %.1f", pos.data[0], pos.data[2]);
  g_puts(-0.5, +0.5, 0.25, printstr);
  if(won){
    g_puts(-0.5, 0, 0.5, "GOAL!");
  }
}

void update_pos(void)
{
  float npos[2] = { pos.data[0], pos.data[2] };
  for(int i=0; i < 4; ++i){
    if(dir[i]){
      npos[0] += -dt * cos(deg + off[i]) / 200.f;
      npos[1] += -dt * sin(deg + off[i]) / 200.f;
    }
  }
  if(map_collision(map, npos[0], npos[1]) == 0){
    pos.data[0] = npos[0];
    pos.data[2] = npos[1];
  }
}

void check_goal(void)
{
  if(map_check_goal(map, pos.data[0], pos.data[2])){
    won = 1;
  }
}

//input callbacks
void ptr(int x, int y, void *arg)
{
  deg += x / 200.f;
}

void quit(int d, void * arg)
{
  exit(1);
}

void move_cb(int down, void * arg)
{
  *((int *) arg) = down;
}

