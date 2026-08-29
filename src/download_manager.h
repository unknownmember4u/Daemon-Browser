#pragma once

#include "include/cef_download_handler.h"
#include <string>

class DaemonDownloadManager : public CefDownloadHandler {
public:
    DaemonDownloadManager();

    // CefDownloadHandler methods:
    virtual bool OnBeforeDownload(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefDownloadItem> download_item,
                                  const CefString& suggested_name,
                                  CefRefPtr<CefBeforeDownloadCallback> callback) override;

    virtual void OnDownloadUpdated(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefDownloadItem> download_item,
                                   CefRefPtr<CefDownloadItemCallback> callback) override;

private:
    std::string GetDownloadsDirectory();
    std::string GetUniqueFilePath(const std::string& dir, const std::string& filename);

    IMPLEMENT_REFCOUNTING(DaemonDownloadManager);
};
