#pragma once
#pragma warning(push)
#pragma warning(disable : 26819 28020)
#define JSON_DIAGNOSTICS 1
#include <nlohmann/json.hpp>
#pragma warning(pop)
#include <any>
#include <vector>
#include <set>
#include <algorithm>

using json = nlohmann::json;

namespace LuaHook {

	struct selector;
	struct hookItem;

	typedef std::function<bool(std::map<std::string, std::any> fields, hookItem hookItem, selector selector)> selectorFunction;

	struct selector {
		selectorFunction* func;

		json j_selector;
	};
	
	struct hookItem {
		float priority = 1;
		std::vector<selector> selector;
		std::vector<json> execute;

		json j_hook;

		bool operator<(const hookItem& other) const {
			return this->priority < other.priority;
		}
	};

	namespace SelectorHelper {

		std::any getInput(std::map<std::string, std::any>& fields, hookItem& hookItem, selector& selector) {
			if (selector.j_selector.contains("field")) {
				std::string fieldName = selector.j_selector.at("field");

				if (fields.find(fieldName) == fields.end()) {
					throw new std::invalid_argument("Field not found");
				}

				return fields[fieldName];
			}
			else {
				if (fields.size() == 1) {
					return fields.begin()->second;
				}
				else {
					// Throw an exception
					selector.j_selector.at("field");
					return NULL;
				}
			}
		}

		std::string getInputConstCharPtr(std::map<std::string, std::any>& fields, hookItem& hookItem, selector& selector) {
			std::any input = SelectorHelper::getInput(fields, hookItem, selector);

			if (const char*** str = std::any_cast<const char**>(&input)) {
				return std::string(**str);
			}
			else {
				throw new std::invalid_argument(std::string("Field type not castable to const char**: ") + input.type().name());
			}
		}

	}
	
	std::map<std::string, selectorFunction> selectorFunctions = {
		{
			"CONTAINS", [](std::map<std::string, std::any> fields, hookItem hookItem, selector selector) {
				std::string input = SelectorHelper::getInputConstCharPtr(fields, hookItem, selector);
				
				std::string value = selector.j_selector.at("value");

				return input.find(value) != std::string::npos;
			}
		},
		{
			"EQUALS", [](std::map<std::string, std::any> fields, hookItem hookItem, selector selector) {
				std::string input = SelectorHelper::getInputConstCharPtr(fields, hookItem, selector);

				std::string value = selector.j_selector.at("value");

				return input.compare(value) == 0;
			}
		}
	};



	void from_json(const json& j, selector& s) {
		s.func = &selectorFunctions.at(j.at("operator").get<std::string>());

		j.get_to(s.j_selector);
	}

	void from_json(const json& j, hookItem& hi) {
		j.at("priority").get_to(hi.priority);
		j.at("selector").get_to(hi.selector);
		j.at("execute").get_to(hi.execute);

		j.get_to(hi.j_hook);
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

			std::sort(items.begin(), items.end());

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