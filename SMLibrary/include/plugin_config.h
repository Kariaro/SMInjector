#pragma once
#include "stdafx.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#pragma warning(push)
#pragma warning(disable : 26819 28020)
#define JSON_DIAGNOSTICS 1
#include <nlohmann/json.hpp>
#pragma warning(pop)
#include "console.h"

namespace fs = std::filesystem;
using json = nlohmann::json;

class PluginConfig {
private:
	inline static fs::path configDirectory;

	const char* pluginName = NULL;
	fs::path path;
	fs::path fullPath;

	const char* defaultContent = "// Empty config file\n{}\n";

public:
	json root;

	_LIB_FUNCTION static bool setConfigDirectory(const fs::path& configDirectory) {
		PluginConfig::configDirectory = fs::absolute(configDirectory);

		if (!fs::is_directory(configDirectory) || !fs::exists(configDirectory)) {
			if (!fs::create_directories(configDirectory)) {
				Console::wlog(Console::Color::LightRed, L"Failed creating main config directory \"%s\"", PluginConfig::configDirectory.c_str());
				return false;
			}
		}

		Console::wlog(Console::Color::Aqua, L"Set main config directory to \"%s\"", PluginConfig::configDirectory.c_str());

		return true;
	}

	_LIB_FUNCTION static fs::path getConfigDirectory() {
		return PluginConfig::configDirectory;
	}

	PluginConfig(const char *pluginName, const fs::path& path) {
		this->pluginName = pluginName;
		this->path = path;

		this->fullPath = PluginConfig::getConfigDirectory() / fs::path(pluginName) / this->path;
	}

#ifdef _LIB_PLUGIN_NAME_STR
	PluginConfig(const fs::path& path) : PluginConfig(_LIB_PLUGIN_NAME_STR, path) {}
#endif

	void setDefaultContent(const char* defaultContent) {
		this->defaultContent = defaultContent;
	}

	bool createIfNotExists() {
		if (fs::exists(this->fullPath)) {
			return false;
		}

		Console::wlog(Console::Color::LightYellow, L"Warning: Unable to find config file \"%s\", creating with default content...", this->fullPath.c_str());

		if (!fs::exists(this->fullPath.parent_path())) {
			fs::create_directories(this->fullPath.parent_path());
		}

		std::ofstream ofs(this->fullPath);

		if (!ofs.is_open()) {
#pragma warning(suppress : 4996)
			Console::wlog(Console::Color::LightRed, L"Failed creating config file \"%s\": %s", this->fullPath.c_str(), _wcserror(errno));
			return false;
		}

		ofs << this->defaultContent;
		ofs.close();

		return true;
	}

	bool load() {
		std::ifstream ifs(this->fullPath);

		if (!ifs.is_open()) {
#pragma warning(suppress : 4996)
			Console::wlog(Console::Color::LightRed, L"Failed reading config file \"%s\": %s", this->fullPath.c_str(), _wcserror(errno));
			return false;
		}

		Console::wlog(Console::Color::Gray, L"Loaded config file \"%s\"", this->fullPath.c_str());


		try {
			this->root = json::parse(ifs, nullptr, true, true);
			std::cout << this->root.dump(4) << '\n';

		} catch (json::exception& e) {
			Console::wlog(Console::Color::LightRed, L"Failed parsing config file \"%s\":", this->fullPath.c_str());
			Console::log(Console::Color::LightRed, "    %s", e.what());
		}

		ifs.close();

		return true;
	}

};