#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct HistoryEntry {
    std::string url;
    std::string title;
    int64_t timestamp;
};

class HistoryManager {
public:
    static HistoryManager& GetInstance();

    void AddEntry(const std::string& url, const std::string& title);
    std::vector<HistoryEntry> GetEntries() const;

private:
    HistoryManager();
    std::string GetStoragePath();
    void LoadHistory();
    void SaveHistory();

    std::vector<HistoryEntry> entries_;
    std::string last_url_;
};
