#include "WFC.hpp"

#include "ImLog.hpp"
#include "Visual.hpp"
#include "yaml-cpp/yaml.h"
#include <__random/uniform_random_bit_generator.h>
#include <filesystem>
#include <random>

const std::string currentVersion = "0.1a";

namespace WFC {

	void LoadWFCObjects(std::vector<WFC_Object>& objs, YAML::Node& file, bool visual = false, std::string path = "", Visual* visualObj = nullptr) {
		YAML::Node N_Objs = file["OBJECTS"];

		for (int i = 0; YAML::Node obj : N_Objs) {
			objs[i].rot = obj["ROTATION"].as<uint16_t>();

			for (int j = 0; j < 4; j++)
				objs[i].mask[j] = obj["MASK"][j].as<uint64_t>();

			if (visual)
				visualObj->r_images[i].Generate(path);

			i++;
		}
	}

	#pragma region Main WFC class
	
	WFC::WFC(std::string path, bool _visual) {

		#pragma region Setup

		if (_visual) {
			visual = new Visual(1024, 576, "WFC Visual");
		}

		YAML::Node mainFile;

		try {
			mainFile = YAML::LoadFile(path);
		} catch (YAML::BadFile) {
			ImLog::Error("Couldn't Load File kindly check the filepath", "WFC::BadFilePath");
		} catch (YAML::ParserException) {
			ImLog::Error("Couldn't Load File kindly check the file", "WFC::BadDataFile");
		}

		std::string version = mainFile["Version"].as<std::string>();

		if (version != currentVersion) {
			ImLog::Warning("Version of the file doesn't match of the current version of parser");
		}

		std::filesystem::path i_fp = mainFile["ImageFolderBasePath"].as<std::string>();

		if (!std::filesystem::exists(i_fp) && _visual) {
			ImLog::Error("File path to the images doesn't exist", "WFC::BadFolderPath");
		}
		#pragma endregion

		LoadWFCObjects(m_objs, mainFile, visual);
	}

	void WFC::Start() {
		
	}

	void WFC::Collapse() {
		std::vector<WFC_Grid_Object*> objs;
		objs.reserve(grid.size());

		// copy into vector
		for (int i = 0; i < grid.size(); i++) {
			objs[i] = &grid[i];
		}

		std::sort(objs.begin(), objs.end(), [](WFC_Grid_Object* a, WFC_Grid_Object* b) 
			{return a->calculateEntropy()-b->calculateEntropy();}
		);

		uint entropy = objs[0]->calculateEntropy();
		std::size_t remove_i = grid.size();
		
		for (int i = 1; i < grid.size(); i++) {
			if (objs[i]->calculateEntropy() != entropy) {
				remove_i = i;
				break;
			}
		}
		
		objs.resize(remove_i);

		// Select one object tile from list of similar entropy

		std::uniform_int_distribution<uint64_t> uid(0, objs.size());
		std::mt19937_64 engine;

		WFC_Grid_Object* selection = objs[uid(engine)];

		// Select tiles form the available options
		std::uniform_int_distribution<uint64_t> _uid(0, selection->calculateEntropy());
		uint64_t selection_tile = _uid(engine);
		
		// tile collapsed
		selection->wfc_obj = &m_objs[selection->possibleTiles()[selection_tile]];

		/// Make all four Neighbours recalculate entropy
		
		// Right Side
		if ((selection->i+1) % GRID_SIZE != 0)
			(selection + sizeof(WFC_Grid_Object*))->calculateEntropy(true);

		// UP
		if ((selection->i) < GRID_SIZE)
			(selection - sizeof(WFC_Grid_Object*) * GRID_SIZE)->calculateEntropy(true);

		// Left Side
		if ((selection->i) % GRID_SIZE != 0)
			(selection - sizeof(WFC_Grid_Object*))->calculateEntropy(true);

		// DOWN
		if ((selection->i) >= GRID_SIZE * (GRID_SIZE - 1))
			(selection + sizeof(WFC_Grid_Object*) * GRID_SIZE)->calculateEntropy(true);
	}
	
	#pragma endregion
}