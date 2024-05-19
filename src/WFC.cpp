#include "WFC.hpp"

#include "ImLog.hpp"
#include "yaml-cpp/yaml.h"
#include <filesystem>

const std::string currentVersion = "0.1a";

namespace WFC {

	void LoadWFCObjects(std::vector<WFC_Object>& objs, YAML::Node& file) {
		YAML::Node N_Objs = file["OBJECTS"];

		// for (YAML::Node obj : N_Objs) {

		// }
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

		LoadWFCObjects(m_objs, mainFile);
	}
	
	#pragma endregion
}