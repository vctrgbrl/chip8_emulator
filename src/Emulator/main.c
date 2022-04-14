#include "app.h"

int main(int argc, char** argv) {
	SDL_Window* window;
	SDL_Surface* surface;
	SDL_Event event;
	SDL_Rect rect;
	CHIP8 chip8;

	rect.h = 10;
	rect.w = 10;

	App_Init(&window, &surface);
	CHIP8_Init(&chip8);

	if (argc != 2) {
		printf("No CHIP8 program path was passed as an argument");
		return -1;
	}
	CHIP8_LoadFile(&chip8, argv[1]);

	int shouldQuit = 0;
	while (!shouldQuit) {
		while (SDL_PollEvent(&event))	App_Handle_Input(&event, &chip8, &shouldQuit);
		
		chip8.delay -= chip8.delay == 0 ? 0 : 1;
		chip8.sound -= chip8.sound == 0 ? 0 : 1;
		if (!chip8.sound) {
			printf("\a");
		}

		CHIP8_Exec(&chip8);
		if (chip8.has_drawn) {
			App_Draw_Screen(&chip8, surface, &rect);
			SDL_UpdateWindowSurface(window);
		}
		SDL_Delay(1);
	}

	App_Quit(window);
	return 0;
}