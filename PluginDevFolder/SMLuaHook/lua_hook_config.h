#pragma once
#pragma warning(push)
#pragma warning(disable : 26819 28020)
#include <nlohmann/json.hpp>
#pragma warning(pop)
#include <any>
#include <vector>
#include <set>
#include <algorithm>

using json = nlohmann::json;

namespace LuaHook {

	typedef std::function<bool(std::map<std::string, std::any> fields, json j_hook, json j_selector)> selectorFunction;

	std::map<std::string, selectorFunction> selectorFunctions = {
		{
			"CONTAINS", [](std::map<std::string, std::any> fields, json j_hook, json j_selector) {
				//assert(j_selector["field"], "selector.field not found!");
				std::string fieldName = j_selector["field"];

				//assert(fields.find(fieldName) != fields.end(), "this field does not exist for this hook!");
				std::string input = std::any_cast<std::string>(fields[j_selector["field"]]);

				//assert(j_selector["value"], "selector.value not found!");
				std::string value = j_selector["value"];

				return input.find(value) != std::string::npos;
			}
		}
	};

	struct selector {
		selectorFunction* func;
	};

	void from_json(const json& j, selector& s) {
		s.func = &selectorFunctions.at(j.at("operator").get<std::string>());
	}



	struct hookItem {
		float priority = 1;
		std::vector<selector> selector;
		std::vector<json> execute;
	};

	void from_json(const json& j, hookItem& hi) {
		j.at("priority").get_to(hi.priority);
		j.at("selector").get_to(hi.selector);
		j.at("execute").get_to(hi.execute);
	}



	class HookConfig {
	public:
		inline static std::vector<HookConfig*> enabledConfigs;

	private:
		json root;
		std::map<std::string, std::vector<hookItem>> hooks;
		bool enabled = false;

	public:
		HookConfig(json root) {
			this->root = root;

			//assert(this->root["hooks"], "lua hook config has no \"hooks\" object");

			for (auto& [key, value] : this->root.at("hooks").items()) {
				this->hooks[key] = value.get<std::vector<hookItem>>();
			}
		}

		~HookConfig() {
			if (this->enabled) {
				Console::log(Color::LightYellow, "Warning: Deconstructing HookConfig while enabled!");
			}
		}

		void setEnabled(bool enabled) {
			if (enabled == this->enabled) {
				return;
			}

			if (enabled) {
				// Add this
				HookConfig::enabledConfigs.push_back(this);
			}
			else {
				// Remove this
				HookConfig::enabledConfigs.erase(std::remove(HookConfig::enabledConfigs.begin(), HookConfig::enabledConfigs.end(), this), HookConfig::enabledConfigs.end());
			}
		}

		static std::vector<hookItem> getHookItems(std::string name) {
			size_t totalSize = 0;

			// Calculate the total size
			for (auto& hookConfig : HookConfig::enabledConfigs) {
				if (hookConfig->hooks.find(name) == hookConfig->hooks.end())
					continue;

				totalSize += hookConfig->hooks[name].size();
			}

			std::vector<hookItem> items;
			items.reserve(totalSize);

			// Insert all hookItems into items
			for (auto& hookConfig : HookConfig::enabledConfigs) {
				if (hookConfig->hooks.find(name) == hookConfig->hooks.end())
					continue;

				items.insert(items.end(), hookConfig->hooks[name].begin(), hookConfig->hooks[name].end());
			}

			return items;
		}

		static std::set<std::string> getHookNames() {
			std::set<std::string> names;

			// Insert all hookItems into items
			for (auto& hookConfig : HookConfig::enabledConfigs) {
				for (auto& [hookName, hookItems] : hookConfig->hooks) {
					names.insert(hookName);
				}
			}

			return names;
		}

	};

}