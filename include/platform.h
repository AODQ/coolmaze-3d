#ifndef PLATFORM_H
#define PLATFORM_H

#include "types.h"
#include "glfns.h"
#include "keys.h"

extern int mouse_x, mouse_y;

//Windowing
void pl_create_window(int w, int h, int fullscr);
void pl_destroy_window(void);
void pl_swap_window(void);

//Input
void pl_grab_input(void);
void pl_release_input(void);
const char * pl_text_buffer(void);

//Events
void pl_poll_events(void);
void pl_bind_key(keycode_t k, key_callback_t kc, void * arg);
void pl_unbind_key(keycode_t k);
void pl_bind_mouse(mouse_callback_t mc, void * arg);
void pl_unbind_mouse(void);

//Timing
float pl_mstime(void);
float pl_deltat(void);
void pl_sleep(void);

//TODO Audio
typedef void(*audio_work_t)(void *arg);

void pl_create_stream(audio_work_t );
void pl_send_stream(int nsamples, short *samples);

#endif

