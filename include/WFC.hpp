#pragma once

#include "Visual.hpp"
#include <array>
#include <string>
#include <sys/types.h>
#include <vector>

#define GRID_SIZE 4

namespace WFC {

	struct WFC_Object {
		uint16_t rot;
		uint64_t mask[4];
	};

	struct WFC_Grid_Object {
		WFC_Object* wfc_obj;
		uint16_t c_rot = 0;
		uint64_t i;

		uint calculateEntropy(bool recalculate = false);
		std::vector<uint64_t> possibleTiles();
	};

	class WFC {
		public:
			WFC(std::string path = "./wfc_data.yaml", bool visual = true);
			~WFC();
			
			void Start();

			Visual* visual;
		private:
			void Collapse();

			std::vector<WFC_Object> m_objs;
			std::array<WFC_Grid_Object, GRID_SIZE * GRID_SIZE> grid;
	};
}