#include "session_manager.h"
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

SessionManager& SessionManager::GetInstance() {
    static SessionManager instance;
    return instance;
}

std::string SessionManager::GetStoragePath() {
    const char* home = std::getenv("HOME");
    if (home && std::strlen(home) > 0) {
        fs::path dir = fs::path(home) / ".config" / "daemon_browser";
        std::error_code ec;
        fs::create_directories(dir, ec);
        return (dir / "session.txt").string();
    }
    return "session.txt";
}

void SessionManager::SaveSession(const std::vector<std::string>& urls) {
    std::string path = GetStoragePath();
    std::ofstream file(path, std::ios::trunc);
    if (!file.is_open()) return;

    for (const auto& url : urls) {
        if (!url.empty() && url != "about:blank" && url.rfind("data:", 0) != 0) {
            file << url << "\n";
        }
    }
}

std::vector<std::string> SessionManager::LoadSession() {
    std::vector<std::string> urls;
    std::string path = GetStoragePath();
    std::ifstream file(path);
    if (!file.is_open()) return urls;

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && line.rfind("data:", 0) != 0) {
            urls.push_back(line);
        }
    }
    return urls;
}
