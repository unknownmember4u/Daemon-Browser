#include "download_manager.h"
#include "include/wrapper/cef_helpers.h"
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

DaemonDownloadManager::DaemonDownloadManager() {}

std::string DaemonDownloadManager::GetDownloadsDirectory() {
    const char* home = std::getenv("HOME");
    if (home && std::strlen(home) > 0) {
        fs::path downloads_path = fs::path(home) / "Downloads";
        if (!fs::exists(downloads_path)) {
            std::error_code ec;
            fs::create_directories(downloads_path, ec);
        }
        return downloads_path.string();
    }
    return fs::current_path().string();
}

std::string DaemonDownloadManager::GetUniqueFilePath(const std::string& dir, const std::string& filename) {
    fs::path base_path = fs::path(dir) / filename;
    if (!fs::exists(base_path)) {
        return base_path.string();
    }

    std::string stem = base_path.stem().string();
    std::string ext = base_path.extension().string();

    int counter = 1;
    while (true) {
        fs::path candidate = fs::path(dir) / (stem + " (" + std::to_string(counter) + ")" + ext);
        if (!fs::exists(candidate)) {
            return candidate.string();
        }
        counter++;
    }
}

bool DaemonDownloadManager::OnBeforeDownload(CefRefPtr<CefBrowser> browser,
                                             CefRefPtr<CefDownloadItem> download_item,
                                             const CefString& suggested_name,
                                             CefRefPtr<CefBeforeDownloadCallback> callback) {
    CEF_REQUIRE_UI_THREAD();

    std::string name = suggested_name.empty() ? "download" : suggested_name.ToString();
    std::string target_dir = GetDownloadsDirectory();
    std::string full_path = GetUniqueFilePath(target_dir, name);

    std::cout << "DOWNLOAD_START: " << name << " -> " << full_path << std::endl;

    callback->Continue(full_path, false);
    return true;
}

void DaemonDownloadManager::OnDownloadUpdated(CefRefPtr<CefBrowser> browser,
                                              CefRefPtr<CefDownloadItem> download_item,
                                              CefRefPtr<CefDownloadItemCallback> callback) {
    CEF_REQUIRE_UI_THREAD();

    int percent = download_item->GetPercentComplete();
    std::string full_path = download_item->GetFullPath().ToString();

    if (download_item->IsInProgress()) {
        std::cout << "DOWNLOAD_STATE: DOWNLOADING (" << (percent >= 0 ? std::to_string(percent) + "%" : "in progress") << ") | " << full_path << std::endl;
    } else if (download_item->IsComplete()) {
        std::cout << "DOWNLOAD_STATE: COMPLETED | " << full_path << std::endl;
    } else if (download_item->IsCanceled()) {
        std::cout << "DOWNLOAD_STATE: CANCELLED | " << full_path << std::endl;
    }
}
