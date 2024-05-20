#pragma once

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_surface.h"
#include <string>
#include <vector>

namespace WFC {
	struct Image {
		SDL_Texture* tex;
		int size[2];
		void Generate(std::string path);
		~Image();
	};

	class Visual {
		public:
			 Visual(int Width, int Height, const char* title);
			~Visual();

			void Render();
			SDL_Renderer* GetRenderer();

			std::vector<Image> r_images;
			static Visual* s_instance;
		private:
			SDL_Window* window;
			SDL_Renderer* renderer;
	};
}
