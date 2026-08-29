#include "bookmark_manager.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>

BookmarkManager& BookmarkManager::GetInstance() {
    static BookmarkManager instance;
    return instance;
}

BookmarkManager::BookmarkManager() {
    LoadBookmarks();
}

std::string BookmarkManager::GetStoragePath() {
    const char* home = getenv("HOME");
    if (!home) {
        struct passwd* pw = getpwuid(getuid());
        if (pw) home = pw->pw_dir;
    }
    std::string dir = std::string(home ? home : ".") + "/.config/daemon_browser";
    mkdir(dir.c_str(), 0755);
    return dir + "/bookmarks.txt";
}

void BookmarkManager::LoadBookmarks() {
    std::lock_guard<std::mutex> lock(mutex_);
    bookmarks_.clear();
    std::string path = GetStoragePath();
    std::ifstream infile(path);
    if (!infile.is_open()) return;

    std::string line;
    while (std::getline(infile, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        std::string ts_str, url, title;
        if (std::getline(iss, ts_str, '|') &&
            std::getline(iss, url, '|') &&
            std::getline(iss, title)) {
            BookmarkEntry entry;
            entry.timestamp = std::stoll(ts_str);
            entry.url = url;
            entry.title = title;
            bookmarks_.push_back(entry);
        }
    }
}

void BookmarkManager::SaveBookmarks() {
    std::string path = GetStoragePath();
    std::ofstream outfile(path, std::ios::trunc);
    if (!outfile.is_open()) return;

    for (const auto& entry : bookmarks_) {
        outfile << entry.timestamp << "|" << entry.url << "|" << entry.title << "\n";
    }
}

void BookmarkManager::AddBookmark(const std::string& title, const std::string& url) {
    if (url.empty() || url.rfind("data:", 0) == 0) return;
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = std::find_if(bookmarks_.begin(), bookmarks_.end(), [&url](const BookmarkEntry& e) {
        return e.url == url;
    });

    if (it != bookmarks_.end()) {
        it->title = title.empty() ? url : title;
    } else {
        BookmarkEntry entry;
        entry.title = title.empty() ? url : title;
        entry.url = url;
        entry.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        bookmarks_.push_back(entry);
    }
    SaveBookmarks();
}

void BookmarkManager::RemoveBookmark(const std::string& url) {
    if (url.empty()) return;
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = std::remove_if(bookmarks_.begin(), bookmarks_.end(), [&url](const BookmarkEntry& e) {
        return e.url == url;
    });
    if (it != bookmarks_.end()) {
        bookmarks_.erase(it, bookmarks_.end());
        SaveBookmarks();
    }
}

bool BookmarkManager::IsBookmarked(const std::string& url) {
    if (url.empty()) return false;
    std::lock_guard<std::mutex> lock(mutex_);
    return std::any_of(bookmarks_.begin(), bookmarks_.end(), [&url](const BookmarkEntry& e) {
        return e.url == url;
    });
}

std::vector<BookmarkEntry> BookmarkManager::GetBookmarks() {
    std::lock_guard<std::mutex> lock(mutex_);
    return bookmarks_;
}
