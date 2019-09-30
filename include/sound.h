#ifndef SOUND_H
#define SOUND_H

#define MAX_SOUNDS 16 

//CD sample rate audio

void play_work(void *arg);
void snd_init(void);
void snd_play(int nsamples, short * samples);
void snd_loop(int nsamples, short * samples);

#endif

