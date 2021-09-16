#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <sys/stat.h>
#include <vector>
#include <fstream>
#include <iterator>
#include <filesystem>
#include <stdlib.h>

namespace SteamFinder {
    using std::string;
    using std::vector;
    using std::filesystem::directory_iterator;

    LONG get_registry_string(HKEY hKey, const wchar_t* key_name, std::wstring& strValue) {
        wchar_t szBuffer[512] = { 0 };
        DWORD dwBufferSize = sizeof(szBuffer);
        ULONG nError = RegQueryValueExW(hKey, key_name, 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
        if(nError == ERROR_SUCCESS) strValue = szBuffer;
        return nError;
    }

    std::wstring get_game_path() {
        HKEY hKey;
        LONG lRes = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Valve\\Steam", 0, KEY_READ, &hKey);
        std::wstring install_path;
        get_registry_string(hKey, L"InstallPath", install_path);
        RegCloseKey(hKey);
        return install_path;
    }

    vector<std::wstring> get_steam_libraries(std::wstring install_path, std::wstring vdf_file) {
        vector<std::wstring> vec;
        vec.reserve(10);

        vec.push_back(install_path.append(L"\\steamapps\\common"));
        if(vdf_file.empty()) {
            return vec;
        }

        std::ifstream infile(vdf_file.c_str(), std::ios_base::binary);
        std::string buffer(std::istreambuf_iterator<char>{infile}, {});

        if(!buffer.empty()) {
            wchar_t temp[2048] = { 0 };
            int idx = 0;
            int len = 0;

            bool next_is_path = false;
            bool quote = false;
            for(int i = 16; i < buffer.size(); i++) {
                char c = buffer[i];
                if(c == '\\') c = buffer[++i];

                if(c == '"') {
                    temp[idx] = 0;
                    if(quote) {
                        len++;
                        
                        if(next_is_path) {
                            vec.push_back(std::wstring(temp).append(L"\\steamapps\\common"));
                            next_is_path = false;
                        } else {
                            next_is_path = (wcscmp(temp, L"path") == 0);
                        }

                        temp[0] = 0;
                        idx = 0;
                    }
                    
                    quote = !quote;
                } else {
                    if(quote) {
                        temp[idx++] = c;
                    }
                }
            }
        }

        return vec;
    }

    std::wstring find_game_from_libs(vector<std::wstring> list, const wchar_t* game) {
        std::wstring name(game);

        for(int i = 0; i < list.size(); i++) {
            for(const auto& file : directory_iterator(list[i])) {
                if(file.path().filename().compare(name) == 0) {
                    return std::wstring(file.path());
                }
            }
        }

        return L"";
    }

    std::wstring FindGame(const wchar_t* game) {
        std::wstring install_path = get_game_path();
        if(install_path.empty()) return L"";

        std::wstring vdf_test = std::wstring(install_path).append(L"\\steamapps\\libraryfolders.vdf");
        vector<std::wstring> libs = get_steam_libraries(install_path, vdf_test);
        return find_game_from_libs(libs, game);
    }
}
