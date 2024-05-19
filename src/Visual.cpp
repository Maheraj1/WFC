#include "Visual.hpp"

#include "ImLog.hpp"

#include "SDL2/SDL.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_video.h"

namespace WFC {
	Visual::Visual(int Width, int Height, const char* title) {
		if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
			ImLog::Error("Couldn't start SDL2", "SDL_INIT");
		}

		window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Width, Height, 0);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_ACCELERATED);
	}
}
