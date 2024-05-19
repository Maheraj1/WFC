#pragma once

#include "Visual.hpp"
#include <string>
#include <sys/types.h>
#include <vector>

namespace WFC {

	struct WFC_Object {
		uint16_t rot;
		uint16_t mask;
	};

	class WFC {
		public:
			WFC(std::string path = "./wfc_data.yaml", bool visual = true);
			~WFC();
		private:
			Visual* visual;
			std::vector<WFC_Object> m_objs;
	};
}