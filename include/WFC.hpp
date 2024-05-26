#pragma once

#include "Visual.hpp"
#include <array>
#include <string>
#include <sys/types.h>
#include <vector>

#define GRID_SIZE 4

namespace WFC {

	enum class Side {
		RIGHT, UP, LEFT, DOWN
	};

	struct WFC_Object {
		uint16_t rot;
		uint64_t mask[4];
		std::size_t index;
	};

	struct WFC_Grid_Object {
		WFC_Grid_Object();
	public:

		WFC_Object* wfc_obj = nullptr;
		uint16_t c_rot = 0;
		uint64_t i = 0;
		uint entropy = 0;

		uint getEntropy();
		std::vector<uint64_t> computePossibleTiles();
	private:
		/* `Neighbour` the object to check; `main_side` the side to ignore */
		std::vector<uint64_t> ComputeNeighbour(WFC_Grid_Object* Neighbour, Side main_side);
	};

	class WFC {
		public:
			WFC(std::string path = "./wfc_data.yaml", bool visual = true);
			~WFC();
			
			void Start();

			Visual* visual;
			static WFC* wfc;
		private:
			void Collapse(bool skipSort = false);

			std::vector<WFC_Object> m_objs;
			std::array<WFC_Grid_Object, GRID_SIZE * GRID_SIZE> grid;
		friend class Visual;
		friend class WFC_Grid_Object;
	};
}