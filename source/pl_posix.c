#include "platform.h"

#include "glfns.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <time.h>

#include <X11/X.h>
#include <X11/Xlib.h>

#include <GL/glx.h>

#include <pthread.h>
#include <alsa/asoundlib.h>

//GLOBALS
int win_w, win_h, grabbed;

// Windows
Display * x_dp;
Window  x_win;
GLXContext x_glc;
GLint x_glatt[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};

XSetWindowAttributes swa; //global scope to initialize
void pl_create_window(int w, int h, int fullscr)
{
  win_w = w;
  win_h = h;
  Window root;
  Colormap cmap;
  XVisualInfo * vi;
  XSizeHints hints = {
    .flags = PMinSize | PMaxSize,
    .min_width = w, .min_height =h,
    .max_width = w, .max_height =h
  };

  x_dp = XOpenDisplay(NULL);
  if(!x_dp){
    puts("Cannot connect to X server");
    exit(-1);
  }
  root = DefaultRootWindow(x_dp);
  vi = glXChooseVisual(x_dp, 0, x_glatt);
  if(!vi){
    puts("No suitable visual found");
    exit(-1);
  }
  cmap = XCreateColormap(x_dp, root, vi->visual, AllocNone);
  swa.colormap = cmap;
  swa.event_mask = ButtonPressMask | ExposureMask | KeyPressMask
                 | ButtonReleaseMask | KeyReleaseMask;
  x_win = XCreateWindow(x_dp, root, 0, 0, w, h, 0, vi->depth, InputOutput, 
                        vi->visual, CWColormap | CWEventMask, &swa);
  XSetWMNormalHints(x_dp, x_win, &hints); // no resizing
  XMapWindow(x_dp, x_win);
  x_glc = glXCreateContext(x_dp, vi, NULL, GL_TRUE);
  if(!(glXMakeCurrent(x_dp, x_win, x_glc))){
    puts("Could not make context Current");
  }
  load_opengl_functions();

  glXSwapIntervalEXT(x_dp, x_win, 0);
}

void pl_destroy_window(void)
{
  glXMakeCurrent(x_dp, None, NULL);
  XDestroyWindow(x_dp, x_win);
  XCloseDisplay(x_dp);
}

void pl_swap_window(void)
{
  glXSwapBuffers(x_dp, x_win);
}

//only begins when window receives an input
void pl_grab_input(void)
{
  char noData[] = {0,0,0,0,0,0,0,0};
  XColor black = {.red = 0, .green = 0, .blue = 0};
  Pixmap blank = XCreateBitmapFromData(x_dp, x_win, noData, 8, 8);
  Cursor blank_cursor = XCreatePixmapCursor(x_dp, blank, blank,
                                            &black, &black, 0, 0);
  //TODO move cursor generation to init
  XGrabPointer(x_dp, x_win, False,
               ButtonPressMask | ButtonReleaseMask | PointerMotionMask
               | FocusChangeMask | EnterWindowMask | LeaveWindowMask,
               GrabModeAsync, GrabModeAsync, x_win, blank_cursor, CurrentTime);
  grabbed = 1;
}

void pl_release_input(void)
{
  XUngrabPointer(x_dp, CurrentTime);
  XUngrabKeyboard(x_dp, CurrentTime);
  grabbed = 0;
}

//Keymap and Events

//TODO factor this out for other platforms
struct{
  key_callback_t fn;
  void  * arg;
} keymap[NUM_KEYS];

void pl_bind_key(keycode_t k, key_callback_t kc, void * arg)
{
  keymap[k].fn = kc;
  keymap[k].arg = arg;
}

void pl_unbind_key(keycode_t k)
{
  keymap[k].fn = NULL;
  keymap[k].arg = NULL;
}

struct {
  mouse_callback_t fn;
  void * arg;
} mouse;

void pl_bind_mouse(mouse_callback_t mc, void * arg)
{
  mouse.fn = mc;
  mouse.arg = arg;
}

void pl_unbind_mouse()
{
  mouse.fn = NULL;
  mouse.arg = NULL;
}

void pl_poll_events(void)
{
  //for(int i = XEventsQueued(x_dp, QueuedAfterReading); i > 0; i--) { 
  for(int i = XPending(x_dp); i > 0; --i){ 
    XEvent ev;
    XNextEvent(x_dp, &ev);
    if(grabbed){
      int down = 0;// key pressed
      switch (ev.type) {
      default:
        break;
      case KeyPress:
      case ButtonPress:// TODO are button events same as keyPress/
        down = 1;
      case KeyRelease:
      case ButtonRelease:
        ;KeySym keysym = XLookupKeysym((XKeyEvent*)&ev, 0);
        int key = keycode_from_keysym(keysym);
        if(key != KEY_INVALID && keymap[key].fn){
          keymap[key].fn(down, keymap[key].arg);
        }
        break;
      }
    }
    else{
    }
  }
  int x, y;
  int ip;
  unsigned int up;
  Window wp;
  XQueryPointer(x_dp, x_win, &wp, &wp, &ip, &ip, &x, &y, &up);
  x = x - win_w/2;
  y = win_h/2 - y;
  if(!(x == 0 && y == 0) && mouse.fn){
    //printf("%d, %d\n", x, y);
    mouse.fn(x, y, mouse.arg);
    XWarpPointer(x_dp, None, x_win, 0, 0, 0, 0, win_w/2, win_h/2);
    //XFlush(x_dp);
  }
}

float pl_mstime(void)
{
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  return t.tv_sec * 1000.0 + t.tv_nsec / 1000000.0;
}

//Not threadsafe don't even think about it
float pl_deltat(void)
{
  static struct timespec last;
  struct timespec next;
  clock_gettime(CLOCK_MONOTONIC, &next);
  float dt = (next.tv_sec - last.tv_sec) * 1000.0 + (next.tv_nsec - last.tv_nsec) /1000000.0;
  last = next;
  return dt;
}

void pl_sleep(void)
{
  sleep(0);
}

// AUDIO
pthread_t snd_thread;
snd_pcm_t * handle;

#define BUFSZ 128

audio_work_t _work;
void * _alsa_thread(void * arg)
{
  while(1){
    _work(arg);
  }
  return NULL;
}

void pl_create_stream(audio_work_t work)
{
  int err;
  _work = work;
  snd_pcm_hw_params_t *params;
  #define TEST(call) \
  if( (err = call) < 0 ) \
    printf( #call "\n%s\n", snd_strerror(err));
  TEST(snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0));
  //TEST(snd_pcm_hw_params_alloca(&params));// this is stack alloc dummy
  TEST(snd_pcm_hw_params_malloc(&params));
  TEST(snd_pcm_hw_params_any(handle, params));//default required???
  //Set params
  TEST(snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED));
  TEST(snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE));
  TEST(snd_pcm_hw_params_set_channels(handle, params, 2));
  unsigned int rate = 44100;
  int dir;
  TEST(snd_pcm_hw_params_set_rate_near(handle, params, &rate, &dir));
  TEST(snd_pcm_hw_params_set_periods(handle, params, 4, 0));
  TEST(snd_pcm_hw_params_set_period_size(handle, params, 1024, 0));
  //write params
  TEST(snd_pcm_hw_params(handle, params));
  snd_pcm_uframes_t buffer_size;
  snd_pcm_uframes_t period_size;
  snd_pcm_hw_params_get_buffer_size(params, &buffer_size);
  snd_pcm_hw_params_get_period_size(params, &period_size, &err);
  unsigned int periods;
  snd_pcm_hw_params_get_periods(params, &periods, &err);
  printf("period %lu, %u\n", period_size, periods);
  snd_pcm_hw_params_free(params);
  //err = snd_pcm_nonblock(handle, 1);
  //TEST(snd_pcm_prepare(handle));

  snd_pcm_sw_params_t *sw_params;
  snd_pcm_sw_params_malloc(&sw_params);
  TEST(snd_pcm_sw_params_current(handle, sw_params));
  TEST(snd_pcm_sw_params_set_start_threshold(handle, sw_params, (buffer_size / period_size) * period_size));
  TEST(snd_pcm_sw_params_set_avail_min(handle, sw_params, BUFSZ));
  TEST(snd_pcm_sw_params(handle, sw_params));

  pthread_create(&snd_thread, NULL, _alsa_thread, NULL);
}

void pl_send_stream(int nsamples, short *samples)
{
  int count = snd_pcm_writei(
    handle,
    samples,
    nsamples
  );
  if(count < BUFSZ){
    if(count < 0)
      printf("%s\n", snd_strerror(count));
    else
      printf("xrun %d\n", count);
    snd_pcm_prepare(handle);
  }
}

