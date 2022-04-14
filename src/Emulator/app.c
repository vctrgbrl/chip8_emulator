#include "app.h"

void App_Init(SDL_Window** window, SDL_Surface** surface) {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		printf("%s\n", SDL_GetError());
		exit(-1);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	*window = SDL_CreateWindow("CHIP8 EMU",
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 
		640, 
		320, 
		SDL_WINDOW_OPENGL);

	*surface = SDL_GetWindowSurface(*window);
	
	if (!*window) {
		printf("Couldn't create window\n");
		printf("%s\n", SDL_GetError());
		exit(-1);
	}

	 if(!*surface) {
		printf("Failed to get window's surface\n");
		printf("%s\n", SDL_GetError());
		exit(-1);
	}
}

void App_Draw_Screen(CHIP8* chip8, SDL_Surface* surface, SDL_Rect* rect) {
	SDL_FillRect(surface, NULL, 0x000000);
	for (int x = 0; x < 64; x++) {
		for (int y = 0; y < 32; y++) {
			if (chip8->f_buffer[x][y] != 0) {
				rect->x = x * 10;
				rect->y = y * 10;
				SDL_FillRect(surface, rect, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));
			}
		}
	}
}

void App_Handle_Input(SDL_Event* event, CHIP8* chip8, int* shouldQuit) {
	switch (event->type) {
			case SDL_QUIT:
				(*shouldQuit) = 1;
				break;
			case SDL_KEYDOWN:
				switch (event->key.keysym.sym) {
					case SDLK_1: chip8->keys[0x1] = 1; break;
					case SDLK_2: chip8->keys[0x2] = 1; break;
					case SDLK_3: chip8->keys[0x3] = 1; break;
					case SDLK_4: chip8->keys[0xC] = 1; break;
					case SDLK_q: chip8->keys[0x4] = 1; break;
					case SDLK_w: chip8->keys[0x5] = 1; break;
					case SDLK_e: chip8->keys[0x6] = 1; break;
					case SDLK_r: chip8->keys[0xD] = 1; break;
					case SDLK_a: chip8->keys[0x7] = 1; break;
					case SDLK_s: chip8->keys[0x8] = 1; break;
					case SDLK_d: chip8->keys[0x9] = 1; break;
					case SDLK_f: chip8->keys[0xE] = 1; break;
					case SDLK_z: chip8->keys[0xA] = 1; break;
					case SDLK_x: chip8->keys[0x0] = 1; break;
					case SDLK_c: chip8->keys[0xB] = 1; break;
					case SDLK_v: chip8->keys[0xF] = 1; break;
				default:
					break;
				}
				break;
			case SDL_KEYUP:
				switch (event->key.keysym.sym) {
					case SDLK_1: chip8->keys[0x1] = 0; break;
					case SDLK_2: chip8->keys[0x2] = 0; break;
					case SDLK_3: chip8->keys[0x3] = 0; break;
					case SDLK_4: chip8->keys[0xC] = 0; break;
					case SDLK_q: chip8->keys[0x4] = 0; break;
					case SDLK_w: chip8->keys[0x5] = 0; break;
					case SDLK_e: chip8->keys[0x6] = 0; break;
					case SDLK_r: chip8->keys[0xD] = 0; break;
					case SDLK_a: chip8->keys[0x7] = 0; break;
					case SDLK_s: chip8->keys[0x8] = 0; break;
					case SDLK_d: chip8->keys[0x9] = 0; break;
					case SDLK_f: chip8->keys[0xE] = 0; break;
					case SDLK_z: chip8->keys[0xA] = 0; break;
					case SDLK_x: chip8->keys[0x0] = 0; break;
					case SDLK_c: chip8->keys[0xB] = 0; break;
					case SDLK_v: chip8->keys[0xF] = 0; break;
				default:
					break;
				}
				break;
			default:
				break;
			}
}

void App_Quit(SDL_Window* window) {
	SDL_DestroyWindow(window);
	SDL_Quit();
}
