#include <SDL2/SDL.h>
#include <stdbool.h>

int main(int argc, char** argv) {
  bool quit = false;
  SDL_Event event;
  
  SDL_Init(SDL_INIT_VIDEO);
 
  SDL_Window * screen = SDL_CreateWindow("Conquest of Ichabod",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);

  while (!quit){
    SDL_WaitEvent(&event);
 
    switch (event.type){
      case SDL_QUIT:
	quit = true;
	break;
    }
  }
  
  SDL_Quit();
 
  return 0;
}
