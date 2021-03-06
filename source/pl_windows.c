#ifndef PLATFORM_H
#define PLATFORM_H

#include "types.h"
#include "keys.h"
#include "platform.h"

#include "assert.h"

#include <windows.h>

//Globals
int win_w, win_h, grabbed;

//Windowing
HWND win;
HDC dc;
HGLRC context;
HCURSOR cursor, no_cursor;
//Input
int grabbed;

LRESULT APIENTRY myProc (
  HWND hWnd,
  UINT message,
  WPARAM wParam,
  LPARAM lParam)
{
  PAINTSTRUCT ps;
  switch(message) {
    default: break;
    case WM_PAINT:
      BeginPaint(win, &ps);
      EndPaint(win, &ps);
  }
  return 1;
}

//Windowing
void pl_create_window(int w, int h, int fullscr)
{
  win_w = w;
  win_h = h;

  HINSTANCE hInst = GetModuleHandle(NULL);
  WNDCLASS wc = {0};
    wc.lpfnWndProc = myProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "Coolmaze3D";
    wc.style = CS_OWNDC;
  assert(RegisterClass(&wc) != 0);

  win = CreateWindow (
      "Coolmaze3D",
      "Coolmaze3D",
      WS_VISIBLE | WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT,
      w, h,
      NULL,
      NULL,
      hInst,
      NULL
  );
  
  assert(win != 0);

  dc = GetDC(win);
  //OpenGL
  PIXELFORMATDESCRIPTOR pfd = {
    .nSize = sizeof(PIXELFORMATDESCRIPTOR),
    .nVersion = 1,
    .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
    .iPixelType = PFD_TYPE_RGBA,
    .cColorBits = 32,
    .cDepthBits = 24,
    .cStencilBits = 8,
    .iLayerType = PFD_MAIN_PLANE
  };
  int pf = ChoosePixelFormat(dc, &pfd);
  SetPixelFormat(dc, pf, &pfd);
  context = wglCreateContext(dc);
  wglMakeCurrent(dc, context);
  load_opengl_functions();

  ShowWindow(win, SW_SHOW);
}

void pl_destroy_window(void)
{
  DestroyWindow(win);
}

void pl_swap_window(void)
{
  //wglSwapLayerBuffers(dc, WGL_SWAP_MAIN_PLANE);
  SwapBuffers(dc);
}

//Input
void pl_grab_input(void)
{
  grabbed = 1;
  SetCapture(win);
  SetFocus(win);
  ShowCursor(FALSE);
}

void pl_release_input(void)
{
  grabbed = 0;
  SetCursor(cursor);
  ShowCursor(TRUE);
}


//Events
//TODO factor this out
struct {
  key_callback_t fn;
  void * arg;
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

void pl_unbind_mouse(mouse_callback_t mc, void * arg)
{
  mouse.fn = NULL;
  mouse.arg = NULL;
}

void pl_poll_events(void)
{
  MSG msg;
  while(PeekMessage(&msg, win, WM_KEYFIRST, WM_MOUSELAST, PM_REMOVE) != 0) {
    int down = 0;
    int key;
    switch(msg.message){
      case WM_KEYDOWN:
      case WM_MBUTTONDOWN:
        down = 1;
      case WM_KEYUP:
      case WM_MBUTTONUP:
        key = keycode_from_keysym(msg.wParam);
        if(key != KEY_INVALID && keymap[key].fn){
          keymap[key].fn(down, keymap[key].arg);
        }
        break;
    }
  }

  //Mouse
  if(mouse.fn) {
    POINT p;
    GetCursorPos(&p);
    ScreenToClient(win, &p);
    mouse.fn(p.x - win_w/2, p.y - win_h/2, mouse.arg);
    if(grabbed) {
      p.x = win_w/2;
      p.y = win_h/2;
      ClientToScreen(win, &p);
      SetCursorPos(p.x, p.y);
    }
  }
}

//Timing
float pl_mstime(void)
{
}


float pl_deltat(void)
{
  static __int64 last, freq;
  __int64 next;
  float dt;
  QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
  QueryPerformanceCounter((LARGE_INTEGER*)&next);
  dt = 1000.0 * (next - last) / (float) (freq);
  last = next;
  return dt;
}

void pl_sleep(void)
{
  Sleep(0);
}

//Audio
typedef void(*audio_work_t)(void *arg);

void pl_create_stream(audio_work_t work)
{
}

void pl_send_stream(int nsamples, short *samples)
{
}

#endif
