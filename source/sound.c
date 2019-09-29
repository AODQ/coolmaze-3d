#include <dlfcn.h>
#include <stdio.h>

#include <pthread.h>

#include <alsa/asoundlib.h>

/*
#include <pulse/simple.h>
#include <pulse/error.h>
*/
#define BUFSZ 128

#include  "sound.h"

#include "platform.h"

typedef struct {
  int len, cur;
  short * buf;
} sound_t;

sound_t sounds[16];
sound_t loop;

//function pointer
void (*_snd_play)(int, short*);

void play_work(void *arg)
{
  short buf[2*BUFSZ] = {0};
  while(1){
    for(int i = 0; i < BUFSZ; i++){
      short val;
      if(loop.buf){
        val = loop.buf[loop.cur];
        loop.cur++;
        if(loop.cur >= loop.len){
          loop.cur = 0;
        }
      }
      else {
        val = 0;
      }
      for(int j = 0; j < MAX_SOUNDS; j++){
        if(sounds[j].cur < sounds[j].len){
          val += sounds[j].buf[sounds[j].cur];
          sounds[j].cur++;
        }
      }
      buf[2*i] = val;
      buf[2*i+1] = val;
    }
    pl_send_stream(BUFSZ, buf);
  }
}

//TODO convert to float remove clipping vectorize etc.
void snd_play(int nsamples, short * samples)
{
  for(int i = 0; i < MAX_SOUNDS; i++) {
    if(sounds[i].cur >= sounds[i].len){
      sounds[i].buf = samples;
      sounds[i].len = nsamples;
      sounds[i].cur = 0;
      break;
    }
  }
}

void snd_loop(int nsamples, short * samples)
{
  loop.buf = samples;
  loop.len = nsamples;
  loop.cur = 0;
}

