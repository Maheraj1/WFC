#include "Visual.hpp"

#include "ImLog.hpp"

#include "SDL2/SDL.h"
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_keyboard.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_video.h"
#include "WFC.hpp"
#include <cstddef>
#include <sys/types.h>
#include <utility>

namespace WFC {
	Visual* Visual::s_instance = nullptr;
	static int ScreenSize = 0;

	void gridToSdl(const std::pair<uint, uint> gridPoint, int gridSize,
		 int screenSize, std::pair<uint, uint>& sdl2Point) {

		// Return the SDL coordinate
		sdl2Point.first  = gridPoint.first  * ((screenSize+GRID_SIZE) / GRID_SIZE);
		sdl2Point.second = gridPoint.second * ((screenSize+GRID_SIZE) / GRID_SIZE);
	}

	Visual::Visual(int Size, const char* title) {
		s_instance = this;
		if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
			ImLog::Error("Couldn't start SDL2", "SDL_INIT");
		}

		window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Size, Size, 0);
		renderer = SDL_CreateRenderer(window, -1, 
			SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_ACCELERATED);

		ScreenSize = Size;
	}

	SDL_Renderer* Visual::GetRenderer() {
		return renderer;
	}

	void Visual::Render() {
		bool quit = false;

		while (!quit) {
			SDL_RenderClear(renderer);

			auto wfc = ::WFC::WFC::wfc;

			for (int i = 0; i < wfc->grid.size(); i++) {
				std::size_t j = wfc->grid[i].wfc_obj ? wfc->grid[i].wfc_obj->index : wfc->m_objs.size()-1;
				
				SDL_Rect SrcRect {0, 0, r_images[j].size[0], r_images[j].size[1]};

				std::pair<uint, uint> sdlPoint = std::make_pair(0, 0);
				gridToSdl(std::make_pair(i % GRID_SIZE, i / GRID_SIZE), GRID_SIZE, ScreenSize, sdlPoint);

				SDL_Rect DstRect {
					static_cast<int>(sdlPoint.first), static_cast<int>(sdlPoint.second),
					ScreenSize/(GRID_SIZE), ScreenSize/(GRID_SIZE)
				};

				SDL_RenderCopy(renderer, r_images[j].tex, &SrcRect, &DstRect);
			}

			SDL_Event e;
			while (SDL_PollEvent(&e)) {
				if (e.type == SDL_QUIT) {
					quit = true;
				}

				if (e.key.keysym.scancode == SDL_SCANCODE_Q) {
					quit = true;
				}
			}

			SDL_RenderPresent(renderer);
		}
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
