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

    LONG GetStringRegKey(HKEY hKey, const char* key_name, string& strValue) {
        CHAR szBuffer[512] = { 0 };
        DWORD dwBufferSize = sizeof(szBuffer);
        ULONG nError = RegQueryValueExA(hKey, key_name, 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
        if(nError == ERROR_SUCCESS) strValue = szBuffer;
        return nError;
    }

    string get_game_path() {
        HKEY hKey;
        LONG lRes = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Valve\\Steam", 0, KEY_READ, &hKey);
        string install_path;
        GetStringRegKey(hKey, "InstallPath", install_path);

        RegCloseKey(hKey);
        return install_path;
    }

    inline bool does_file_exist(const string& name) {
        struct stat buffer;
        return (stat(name.c_str(), &buffer) == 0);
    }

    vector<string> get_steam_libraries(string install_path, string vdf_file) {
        vector<string> vec;
        vec.reserve(10);

        vec.push_back(install_path.append("\\steamapps\\common"));
        if(vdf_file.empty()) {
            return vec;
        }

        std::ifstream infile(vdf_file.c_str(), std::ios_base::binary);
        std::string buffer(std::istreambuf_iterator<char>{infile}, {});

        if(!buffer.empty()) {
            char temp[2048] = { 0 };
            int idx = 0;
            int len = 0;

            bool quote = false;
            for(int i = 16; i < buffer.size(); i++) {
                char c = buffer[i];
                if(c == '\\') c = buffer[++i];

                if(c == '"') {
                    temp[idx] = 0;
                    if(i > 16 && quote) {
                        len++;
                        if(len > 4 && ((len & 1) == 0)) {
                            vec.push_back(string(temp).append("\\steamapps\\common"));
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

    string find_game_from_libs(vector<string> list, const char* game) {
        std::wstring name(game, game + strlen(game));

        for(int i = 0; i < list.size(); i++) {
            for(const auto& file : directory_iterator(list[i])) {
                if(file.path().filename().compare(name) == 0) {
                    char szBuffer[512] = { 0 };
                    size_t num;
                    wcstombs_s(&num, szBuffer, file.path().c_str(), 512);
                    return string(szBuffer);
                }
            }
        }

        return "";
    }

    string FindGame(const char* game) {
        string install_path = get_game_path();
        if(install_path.empty()) return "";

        string vdf_test = string(install_path).append("\\steamapps\\libraryfolders.vdf");
        vector<string> libs = get_steam_libraries(install_path, vdf_test);
        return find_game_from_libs(libs, game);
    }
}