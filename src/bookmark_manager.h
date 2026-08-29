#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <cstdint>

struct BookmarkEntry {
    std::string title;
    std::string url;
    int64_t timestamp;
};

class BookmarkManager {
public:
    static BookmarkManager& GetInstance();

    void AddBookmark(const std::string& title, const std::string& url);
    void RemoveBookmark(const std::string& url);
    bool IsBookmarked(const std::string& url);
    std::vector<BookmarkEntry> GetBookmarks();

private:
    BookmarkManager();
    void LoadBookmarks();
    void SaveBookmarks();
    std::string GetStoragePath();

    std::vector<BookmarkEntry> bookmarks_;
    std::mutex mutex_;
};
