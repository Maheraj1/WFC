#include "WFC.hpp"

#include "ImLog.hpp"
#include "InstrumentalTimmer.hpp"
#include "Visual.hpp"
#include "yaml-cpp/yaml.h"
#include <__random/uniform_random_bit_generator.h>
#include <_types/_uint16_t.h>
#include <cassert>
#include <cmath>
#include <filesystem>
#include <limits>
#include <random>
#include <thread>

const std::string currentVersion = "0.1a";
using namespace std::chrono_literals;

#define ADD_TO_MASK_AND_SIDE(_side) sides |= 1 << _side;mask |= wfc->m_objs[i].mask[(uint)_side]

// Get tile of the corresponding side
#define TILE_RIGHT(obj) (obj + 1		)
#define TILE_LEFT(obj)  (obj - 1		)
#define TILE_UP(obj)    (obj - GRID_SIZE)
#define TILE_DOWN(obj)  (obj + GRID_SIZE)

// Checking that the corresponding sides are valid (to avoid over access memory)
#define CHECK_TILE_RIGHT(obj) ((obj->i) % GRID_SIZE) != (GRID_SIZE - 1)
#define CHECK_TILE_LEFT(obj)  ((obj->i) % GRID_SIZE) != 0
#define CHECK_TILE_UP(obj)    (obj->i) >= GRID_SIZE
#define CHECK_TILE_DOWN(obj)  (obj->i) <= GRID_SIZE * (GRID_SIZE - 1)

// Checking that object has not collapsed
#define WFC_CHECK(I)\
if (obj->wfc_obj) {\
	sides |= obj->wfc_obj->mask[I] & wfc->m_objs[i].mask[I] ? 1 << I : 0;\
} else {\
ADD_TO_MASK_AND_SIDE(I);\
}

namespace WFC {

	WFC* WFC::wfc = nullptr;

	void LoadWFCObjects(std::vector<WFC_Object>& objs, YAML::Node& file, bool visual = false,
	 std::string path = "", Visual* visualObj = nullptr, std::string ext = ".png") {
		YAML::Node N_Objs = file["OBJECTS"];

		for (int i = 0; YAML::Node obj : N_Objs) {
			objs.push_back({});
			objs[i].rot = obj["ROTATION"].as<uint16_t>();

			for (int j = 0; j < 4; j++) {
				objs[i].mask[j] = obj["MASK"][j].as<uint64_t>();
			}

			if (visual && visualObj) {
				Image::Generate(
					path + std::filesystem::path::preferred_separator + obj["ID"].as<std::string>() + ext
				);
				objs[i].index = i;
			}

			i++;
		}
	}

	#pragma region Main WFC class
	
	WFC::WFC(std::string path, bool _visual) {

		#pragma region Setup

		wfc = this;

		if (_visual) {
			visual = new Visual(720, "WFC Visual");
		}

		YAML::Node mainFile;

		try {
			mainFile = YAML::LoadFile(path);
		} catch (YAML::BadFile e) {
			ImLog::Error("Couldn't Load File kindly check the filepath; " + e.msg, "WFC::BadFilePath", true);
		} catch (YAML::ParserException e) {
			ImLog::Error("Couldn't Load File kindly check the file; " + e.msg + ";" + e.what(), "WFC::BadDataFile", true);
		}

		std::string version = mainFile["Version"].as<std::string>();

		if (version != currentVersion) {
			ImLog::Warning("Version of the file doesn't match of the current version of parser");
		}

		std::filesystem::path i_fp = mainFile["ImageFolderBasePath"].as<std::string>();

		if (!std::filesystem::exists(i_fp) && _visual) {
			ImLog::Error("File path to the images doesn't exist", "WFC::BadFolderPath", true);
		}
		#pragma endregion

		LoadWFCObjects(m_objs, mainFile, _visual, i_fp, visual);

		if (_visual) {
			Image::Generate(
				i_fp.string() + std::filesystem::path::preferred_separator + "Empty.png"
			);
		}
	}

	WFC::~WFC() {}

	void WFC::Start() {
		{
			// PROFILE_SCOPE("Setting Base Entropy");
			for (int i = 0; i < grid.size(); i++) {
				grid[i].entropy = m_objs.size();
			}
		}
		START_SESSION("WFC Main");
		{
			// PROFILE_SCOPE("Collapse Algorithm");
			
			Collapse(true);

			for (int i = 1; i < grid.size(); i++) {
				Collapse();
			}
		}

		END_SESSION;
	}

	void WFC::Collapse(bool skipSort) {
		PROFILE_FUNC();
		std::vector<WFC_Grid_Object*> objs;
		{
			// PROFILE_SCOPE("Collapse Begin");
			objs.resize(grid.size());

			int offset = 0;

			// copy into vector
			for (int i = 0; i < grid.size(); i++) {
				if (grid[i].entropy == 0) {
					offset++;
					continue;
				}
				objs[i - offset] = &grid[i];
				if (skipSort)
					objs[i]->entropy = m_objs.size();
			}
			objs.resize(objs.size() - offset);

			if (!skipSort) {
				std::sort(objs.begin(), objs.end(), [](WFC_Grid_Object* a, WFC_Grid_Object* b) 
					{return a->getEntropy() < b->getEntropy();}
				);
			}
			
			uint entropy = objs[0]->getEntropy();
			
			if (entropy == 0) // we have no other tiles to collapse
				return;
			
			std::size_t remove_i = grid.size();
			
			for (int i = 1; i < grid.size(); i++) {
				if (objs[i]->getEntropy() != entropy) {
					remove_i = i;
					break;
				}
			}
			
			objs.resize(remove_i);
		}
		
		WFC_Grid_Object* selection = nullptr;
		
		{
			PROFILE_SCOPE("Tile Selection");
			// Select one object tile from list of similar entropy

			std::uniform_int_distribution<uint64_t> tileToCollapse_uid(0, objs.size());
			std::mt19937_64 engine;
			selection = objs[tileToCollapse_uid(engine)];

			// Select tiles form the available options
			
			std::uniform_int_distribution<uint64_t> optionsOfTiles_uid(0, selection->getEntropy()-1);
			uint64_t selection_tile = optionsOfTiles_uid(engine);

			auto tiles = selection->computePossibleTiles();
			
			// tile collapsed
			selection->wfc_obj = &m_objs[selection_tile];
		}
		
		{
			PROFILE_SCOPE("Recalculate entropy");
			/// Make all four Neighbours recalculate entropy
			
			// Right Side
			if (CHECK_TILE_RIGHT(selection))
				(TILE_RIGHT(selection))->computePossibleTiles();

			// UP
			if (CHECK_TILE_UP(selection))
				(TILE_UP(selection))->computePossibleTiles();

			// Left Side
			if (CHECK_TILE_LEFT(selection))
				(TILE_LEFT(selection))->computePossibleTiles();

			// DOWN
			if (CHECK_TILE_DOWN(selection))
				(TILE_DOWN(selection))->computePossibleTiles();
		}
	}

	std::vector<uint64_t> WFC_Grid_Object::computePossibleTiles() {
		/*
			[?] -> tile in question
			[&] -> other tiles
			[*] -> checking tiles
			[#] -> neighbour tiles

			[&][&][*][&][&]
			[&][*][#][*][&]
			[*][#][?][#][*]
			[&][*][#][*][&]
			[&][&][*][&][&]
		*/
		PROFILE_FUNC();
		std::this_thread::sleep_for(1ns);
		std::array<std::vector<uint64_t>, 4> tiles;
		uint16_t side_mask = 0;
		// Right Side
		if (CHECK_TILE_RIGHT(this)) {
			auto obj = TILE_RIGHT(this);
			tiles[0] = ComputeNeighbour(obj, Side::LEFT);
			side_mask |= 1 << 0;
		}

		// UP
		if (CHECK_TILE_UP(this)) {
			auto obj = TILE_UP(this);
			tiles[1] = ComputeNeighbour(obj, Side::DOWN);
			side_mask |= 1 << 1;
		}

		// Left Side
		if (CHECK_TILE_LEFT(this)) {
			auto obj = TILE_LEFT(this);
			tiles[2] = ComputeNeighbour(obj, Side::RIGHT);
			side_mask |= 1 << 2;
		}

		// DOWN
		if (CHECK_TILE_DOWN(this)) {
			auto obj = TILE_DOWN(this);
			tiles[3] = ComputeNeighbour(obj, Side::UP);
			side_mask |= 1 << 3;
		}

		std::vector<uint64_t> possible_tiles;
		
		// I don't think this is good for performance like it can be O(n^5) or O(n)
		for (int ii = 0; ii < WFC::wfc->m_objs.size(); ii++) {
			for (int i = 0; i < tiles[0].size(); i++) {
				for (int j = 0; j < tiles[1].size(); j++) {
					for (int k = 0; k < tiles[2].size(); k++) {
						for (int l = 0; l < tiles[3].size(); l++) {
							if (!((WFC::wfc->m_objs[ii].mask[0] & tiles[0][i]) &&
								  (WFC::wfc->m_objs[ii].mask[1] & tiles[1][j]) && 
								  (WFC::wfc->m_objs[ii].mask[2] & tiles[2][k]) &&
								  (WFC::wfc->m_objs[ii].mask[3] & tiles[3][l])))
								continue;
							if (!possible_tiles.empty())
								if (WFC::wfc->m_objs[ii].index == possible_tiles.back())
									continue;
							
							possible_tiles.push_back(WFC::wfc->m_objs[ii].index);
						}
					}
				}
			}
		}

		entropy = possible_tiles.size();

		return possible_tiles;
	}

	std::vector<uint64_t> WFC_Grid_Object::ComputeNeighbour(WFC_Grid_Object* Neighbour, Side main_side) {
		PROFILE_FUNC();
		std::vector<uint64_t> tiles;
		auto wfc = WFC::wfc;
		
		// The tile has already collapsed
		if (Neighbour->wfc_obj) {
			tiles.push_back(Neighbour->wfc_obj->mask[(uint)main_side]);
			return tiles;
		}

		// TODO: Fix this

		uint16_t toCheckSide = 0;
		// Add 1 (1 << 0) OR (true) to the side mask
		toCheckSide |= (main_side != Side::RIGHT && CHECK_TILE_RIGHT(Neighbour)) ? 1 << 0: 0;
		toCheckSide |= (main_side != Side::UP	 && CHECK_TILE_UP(Neighbour))	 ? 1 << 1: 0;
		toCheckSide |= (main_side != Side::LEFT  && CHECK_TILE_LEFT(Neighbour))	 ? 1 << 2: 0;
		toCheckSide |= (main_side != Side::DOWN  && CHECK_TILE_DOWN(Neighbour))	 ? 1 << 3: 0;

		for (int i = 0; i < wfc->m_objs.size(); i++) {
			uint16_t sides = 0;
			// Available mask for side
			uint64_t mask = 0;
			// Right Side
			if (toCheckSide & 1) {
				auto obj = TILE_RIGHT(Neighbour);
				WFC_CHECK(0)
			} else {
				ADD_TO_MASK_AND_SIDE(0);
			}
			// UP
			if (toCheckSide & 2) {
				auto obj = TILE_UP(Neighbour);
				WFC_CHECK(1)
			} else {
				ADD_TO_MASK_AND_SIDE(1);
			}

			// Left Side
			if (toCheckSide & 4) {
				auto obj = TILE_LEFT(Neighbour);
				WFC_CHECK(2)
			} else {
				ADD_TO_MASK_AND_SIDE(2);
			}

			// DOWN
			if (toCheckSide & 8) {
				auto obj = TILE_DOWN(Neighbour);
				WFC_CHECK(3)
			} else {
				ADD_TO_MASK_AND_SIDE(3);
			}
			
			if (sides == 15)
				tiles.push_back(mask);
		}

		return tiles;
	}

	uint WFC_Grid_Object::getEntropy() {
		return entropy;
	}

	WFC_Grid_Object::WFC_Grid_Object() {
		static int _i = -1;
		_i++;
		i = _i;
	}
	
	#pragma endregion
}