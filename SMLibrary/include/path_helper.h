#pragma once
#include <string>
#include <filesystem>

#include "plugin_config.h"

namespace fs = std::filesystem;

namespace PathHelper {

	fs::path getInstallationPath() {
		TCHAR dir[MAX_PATH] = { 0 };
		DWORD length = GetModuleFileName(NULL, dir, _countof(dir));
		return fs::path(dir).parent_path();
	}

	fs::path resolvePath(std::string path) {

#ifdef _LIB_PLUGIN_NAME_STR
		if (path.rfind("$PLUGIN_CONFIG", 0) == 0) {
			return PluginConfig::getConfigDirectory() / fs::path(_LIB_PLUGIN_NAME_STR) / path.substr(path.find_first_of("/") + 1);
		}
#endif

		if (path.rfind("$GAME_DATA", 0) == 0) {
			return PathHelper::getInstallationPath() / fs::path("Data") / path.substr(path.find_first_of("/") + 1);
		}

		if (path.rfind("$SURVIVAL_DATA", 0) == 0) {
			return PathHelper::getInstallationPath() / fs::path("Survival") / path.substr(path.find_first_of("/") + 1);
		}

		if (path.rfind("$CHALLENGE_DATA", 0) == 0) {
			return PathHelper::getInstallationPath() / fs::path("ChallengeData") / path.substr(path.find_first_of("/") + 1);
		}

		// TODO: Call the funtion in the game to do this

		return path;
	}

}
