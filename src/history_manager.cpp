#include "history_manager.h"
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <chrono>

namespace fs = std::filesystem;

HistoryManager& HistoryManager::GetInstance() {
    static HistoryManager instance;
    return instance;
}

HistoryManager::HistoryManager() {
    LoadHistory();
}

std::string HistoryManager::GetStoragePath() {
    const char* home = std::getenv("HOME");
    if (home && std::strlen(home) > 0) {
        fs::path dir = fs::path(home) / ".config" / "daemon_browser";
        std::error_code ec;
        fs::create_directories(dir, ec);
        return (dir / "history.txt").string();
    }
    return "history.txt";
}

void HistoryManager::LoadHistory() {
    std::string path = GetStoragePath();
    std::ifstream file(path);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        size_t first = line.find('|');
        size_t second = line.find('|', first + 1);
        if (first != std::string::npos && second != std::string::npos) {
            try {
                HistoryEntry entry;
                entry.timestamp = std::stoll(line.substr(0, first));
                entry.url = line.substr(first + 1, second - first - 1);
                entry.title = line.substr(second + 1);
                entries_.push_back(entry);
            } catch (...) {}
        }
    }
}

void HistoryManager::SaveHistory() {
    std::string path = GetStoragePath();
    std::ofstream file(path, std::ios::trunc);
    if (!file.is_open()) return;

    for (const auto& entry : entries_) {
        file << entry.timestamp << "|" << entry.url << "|" << entry.title << "\n";
    }
}

void HistoryManager::AddEntry(const std::string& url, const std::string& title) {
    if (url.empty() || url == "about:blank" || url.rfind("data:", 0) == 0) return;

    if (url == last_url_) return;
    last_url_ = url;

    HistoryEntry entry;
    entry.url = url;
    entry.title = title.empty() ? "Untitled Page" : title;
    entry.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    entries_.push_back(entry);
    SaveHistory();
    std::cout << "HISTORY_ENTRY: " << entry.url << " (" << entry.title << ")" << std::endl;
}

std::vector<HistoryEntry> HistoryManager::GetEntries() const {
    return entries_;
}
