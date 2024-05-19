#pragma once

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_surface.h"
#include <string>
#include <vector>

namespace WFC {
	struct Image {
		SDL_Texture* tex;
		uint id;
		void Generate(std::string path);
	};

	class Visual {
		public:
			 Visual(int Width, int Height, const char* title);
			~Visual();

			void Render();

			std::vector<Image> r_images;
		private:
			SDL_Window* window;
			SDL_Renderer* renderer;
	};
}
