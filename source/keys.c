#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//Move this to platform specific files?
#ifdef __unix__
  #include "X11/keysym.h"
#elif defined _WIN32
  #include "Winuser.h"
#endif

#include "keys.h"


const char * key_names [NUM_KEYS] = {
  #define X(k, ...)  "KEY_" #k,
    KEYS_XMACRO
  #undef X
};

int key_syms[NUM_KEYS] = {
  #ifdef __unix__
    #define X(k, pos, win) pos,
      KEYS_XMACRO
    #undef X
  #elif defined _WIN32
    #define X(k, pos, win) win,
      KEYS_XMACRO
    #undef X
  #endif
};

int _strcmp(const void * key, const void * b)
{
  return strcmp(key, *((void **)b));
}

keycode_t keycode_from_string(const char * str)
{
  keycode_t * kp = bsearch(str, key_names, NUM_KEYS, sizeof(const char *), _strcmp);
  return kp ? *kp : KEY_INVALID;
}

int keysym_from_keycode(keycode_t k)
{
  return key_syms[k];
}

#define TABLE_SZ 256
keycode_t key_codes[TABLE_SZ];

int _hash(int n){
  return n % 256;
}

void build_keysym_table(void)
{
  for(int i = 0; i < TABLE_SZ; i++){
    key_codes[i] = KEY_INVALID;
  }
  #ifdef __unix__
    #define X(k, pos, win)\
    if(key_codes[_hash(pos)] == KEY_INVALID) \
      key_codes[_hash(pos)] = KEY_##k; \
    else \
      printf("collision: %s\n", key_names[KEY_##k]);
  #elif defined _WIN32
    #define X(k, pos, win) \
    if(key_codes[_hash(win)] == KEY_INVALID) \
      key_codes[_hash(win)] = KEY_##k; \
    else \
      printf("collision: %s\n", key_names[KEY_##k]);
  #endif
    KEYS_XMACRO
  #undef X
}

keycode_t keycode_from_keysym(int k)
{
  return key_codes[_hash(k)];
}
