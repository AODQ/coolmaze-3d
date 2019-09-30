#include "game.h"

int main( int argc, char ** argv )
{
  if(argc < 2) return -1;
  game_init(argc, argv);
  while( game_running() ){
    game_menuloop();
    game_mainloop();
  }
}

