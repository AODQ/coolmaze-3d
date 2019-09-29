#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <netpbm.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "map.h"
#include "platform.h"
#include "wavefront.h"
#include "gfx.h"
#include "sound.h"
#include "vmath.h"

void quit(int d, void * arg)
{
  exit(1);
}

void say(int down, void * arg)
{
  if(down) puts((const char *)arg);
}

short tone[20000];
void gen_tone(float frequency)
{
  for(int i = 0; i < 20000; i++){
    tone[i] = 4096*sin(6.28*(i/44100.0)*frequency);
  }
}

void play_tone(int down, void * arg)
{
  if(down) snd_play(20000, tone);
}

//camera
float deg = 0.f;

void ptr(int x, int y, void *arg)
{
  deg += x / 200.f;
}

//Movement
int dir[4];
#define PI 3.14159f
const float off[4] = {
  //0.f, PI/2.f, PI, 3.f*PI/2.f
  PI/2.f, 0.f, 3.f*PI/2.f, PI
};

vec_t pos = {.data=
  {0.f, 0.6, 0.f, 0.f}
};

void move_cb(int down, void * arg)
{
  *((int *) arg) = down;
}

char * msg = "test of arg and second binding";

int main(int argc, char ** argv)
{
  if(argc < 2) return -1;
  build_keysym_table();
  pl_create_window(640, 480, 0);
  g_init();
  //snd_init();
  pl_create_stream(play_work);
  
  //Initialize the font
  unsigned char white[4] = {255, 255, 255, 255};
  unsigned char clear[4] = {0, 0, 0, 0};
  image_t * font_img = pbm_read("tom-thumb-new.pbm", white, clear);
  img_flip(font_img);
  g_texture_t font = g_ld_texture(font_img->w, font_img->h, font_img->rgba);
  img_delete(font_img);
  g_set_font(font, 4, 6, 4, 32);

  //Initialize the model
  /*
  model_t * obj = obj_read(argv[1]);
  g_model_t * robj = g_ld_model(
    obj->n_verts, obj->verts->data, NULL,
    obj->n_tris*3, obj->tris->verts,
    0
  );
  obj_delete(obj);
  */
  map_t * obj = map_ld(argv[1]);
  g_model_t * robj = map_model(obj);
  pos.data[0] = obj->start[0] + 0.5f;
  pos.data[2] = obj->start[1] + 0.5f;
  map_debug(obj);
  vec_t end_vec = {.data ={
    ((float)obj->end[0])+0.5, 0.5, ((float)obj->end[1])+0.5, 1.f
  }};

  //Billboard
  g_billboard_t * bb = g_ld_billboard(0.5f, 0);

  //Initialize sound
  gen_tone(440.0);

  //Bind the input
  pl_bind_key(KEY_Q, quit, NULL);
  pl_bind_key(KEY_E, say, msg);
  pl_bind_key(KEY_M, play_tone, NULL);

  pl_bind_key(KEY_W, move_cb, dir+0);
  pl_bind_key(KEY_A, move_cb, dir+1);
  pl_bind_key(KEY_S, move_cb, dir+2);
  pl_bind_key(KEY_D, move_cb, dir+3);

  pl_bind_mouse(ptr, NULL);
  pl_grab_input();

  //Set up rendering
  mat_t proj = perspective(60.f*3.14/180.f, 4.f/3.f, 0.1f, 40.f);
  vec_t up = {.data =
    {0, 1, 0, 1}
  };
  int won = 0;
  while(1) {
    g_clear();

    //render map
    mat_t mvp = m4xm4(proj, m4xm4(rotate(up, deg), translate(v3neg(pos))));
    g_mvp_mat(mvp);
    g_render_model(robj);

    //render exit
    mvp = m4xm4(mvp, m4xm4(translate(end_vec), rotate(up, -deg)));
    g_mvp_mat(mvp);
    g_render_billboard(bb);

    //TODO print position!

    //Time
    float delta = pl_deltat();
    char printstr[16];
    snprintf(printstr, 16, "%.3f ms", delta);
    g_puts(-0.75, -0.75, 0.25, printstr);

    //Debug pos
    snprintf(printstr, 16, "%.1f, %.1f", pos.data[0], pos.data[2]);
    g_puts(-0.5, +0.5, 0.25, printstr);

    //game logic
    pl_poll_events();
    float npos[2] = { pos.data[0], pos.data[2] };
    for(int i=0; i < 4; ++i){
      if(dir[i]){
        npos[0] += -delta * cos(deg + off[i]) / 200.f;
        npos[1] += -delta * sin(deg + off[i]) / 200.f;
      }
    }
    if(!map_collision(obj, npos[0], npos[1])){
      pos.data[0] = npos[0];
      pos.data[2] = npos[1];
    }
    if(map_check_goal(obj, pos.data[0], pos.data[2])){
        won = 1;
    }
    if (won){
      g_puts(-0.5, 0, 0.5, "GOOD!");
    }
    pl_swap_window();
    pl_sleep();
  }
  return 0;
}

