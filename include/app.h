#include <SDL2/SDL.h>
#include "chip8.h"

void App_Init(SDL_Window** window, SDL_Surface** surface);
void App_Draw_Screen(CHIP8* chip8, SDL_Surface* surface, SDL_Rect* rect);
void App_Handle_Input(SDL_Event* event, CHIP8* chip8, int* shouldQuit);
void App_Quit(SDL_Window* window);