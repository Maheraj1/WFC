#include "Visual.hpp"

#include "ImLog.hpp"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_video.h"
#include <cstddef>

namespace WFC {
	Visual* Visual::s_instance = nullptr;

	Visual::Visual(int Width, int Height, const char* title) {
		s_instance = this;
		if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
			ImLog::Error("Couldn't start SDL2", "SDL_INIT");
		}

		window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Width, Height, 0);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_ACCELERATED);
	}

	SDL_Renderer* Visual::GetRenderer() {
		return renderer;
	}

	Image::~Image() {
		
	}

	void Image::Generate(std::string path) {
		SDL_Texture* texture = IMG_LoadTexture(Visual::s_instance->GetRenderer(), path.c_str());

		int w, h;
		SDL_QueryTexture(texture, NULL, NULL, &w, &h);

		Visual::s_instance->r_images.push_back({texture, {w, h}});
	}
}
