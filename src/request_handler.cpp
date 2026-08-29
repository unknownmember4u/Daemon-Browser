#include "request_handler.h"
#include "browser_window.h"
#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"
#include "security_state.h"
#include "history_manager.h"
#include <iostream>

DaemonRequestHandler::DaemonRequestHandler(BrowserWindow* browser_window)
    : browser_window_(browser_window),
      download_manager_(new DaemonDownloadManager()) {}

cef_return_value_t DaemonRequestHandler::OnBeforeResourceLoad(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    CefRefPtr<CefCallback> callback) {

    int resource_type = request->GetResourceType(); // RT_MAIN_FRAME = 0, etc.

    if (resource_type == RT_MAIN_FRAME) {
        std::string method = request->GetMethod();
        std::string url = request->GetURL();
        std::string display_url = url;
        if (display_url.rfind("data:", 0) == 0 && display_url.length() > 60) {
            display_url = display_url.substr(0, 60) + "... [data URL]";
        }

        std::cout << "REQUEST" << std::endl;
        std::cout << "METHOD: " << method << std::endl;
        std::cout << "URL: " << display_url << std::endl;
        std::cout << "RESOURCE_TYPE: " << resource_type << std::endl;
        std::cout << "---------------------------------" << std::endl;
    }

    return RV_CONTINUE;
}

#include "include/wrapper/cef_stream_resource_handler.h"
#include "new_tab_page.h"
#include "bookmark_manager_page.h"
#include "history_manager_page.h"
#include "download_manager_page.h"
#include "settings_page.h"

CefRefPtr<CefResourceHandler> DaemonRequestHandler::GetResourceHandler(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request) {
    std::string url = request->GetURL().ToString();
    if (url.find("https://daemon.internal/") == 0) {
        std::string html;
        if (url == "https://daemon.internal/newtab" || url == "https://daemon.internal/newtab/") html = GetNewTabPageHTML();
        else if (url == "https://daemon.internal/bookmarks" || url == "https://daemon.internal/bookmarks/") html = GetBookmarkManagerPageHTML();
        else if (url == "https://daemon.internal/history" || url == "https://daemon.internal/history/") html = GetHistoryManagerPageHTML();
        else if (url == "https://daemon.internal/downloads" || url == "https://daemon.internal/downloads/") html = GetDownloadManagerPageHTML();
        else if (url == "https://daemon.internal/settings" || url == "https://daemon.internal/settings/") html = GetSettingsPageHTML();
        
        if (!html.empty()) {
            CefRefPtr<CefStreamReader> stream = CefStreamReader::CreateForData(
                static_cast<void*>(const_cast<char*>(html.c_str())), html.size());
            return new CefStreamResourceHandler("text/html", stream);
        }
    }
    return nullptr;
}

void DaemonRequestHandler::OnAddressChange(CefRefPtr<CefBrowser> browser,
                                           CefRefPtr<CefFrame> frame,
                                           const CefString& url) {
    if (browser_window_) {
        browser_window_->OnAddressChange(browser, url);
        
        SecurityInfo info = SecurityState::ComputeFromBrowser(browser, url);
        browser_window_->UpdateSecurityIndicator(browser, info);
    }
}

void DaemonRequestHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
                                         const CefString& title) {
    if (browser_window_) {
        browser_window_->OnTitleChange(browser, title);
    }
}

void DaemonRequestHandler::OnFaviconURLChange(CefRefPtr<CefBrowser> browser,
                                               const std::vector<CefString>& icon_urls) {
    if (browser_window_ && !icon_urls.empty()) {
        browser_window_->OnFaviconURLChange(browser, icon_urls[0]);
    }
}

void DaemonRequestHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                                bool isLoading,
                                                bool canGoBack,
                                                bool canGoForward) {
    if (browser_window_) {
        browser_window_->OnLoadingStateChange(browser, isLoading, canGoBack, canGoForward);
        
        if (!isLoading) {
            std::cout << "BENCHMARK_SIGNAL: LOAD_END" << std::endl;
            // Recompute security when load completes to capture TLS details
            CefString url = browser->GetMainFrame()->GetURL();
            SecurityInfo info = SecurityState::ComputeFromBrowser(browser, url);
            browser_window_->UpdateSecurityIndicator(browser, info);

            // Record History Entry
            CefRefPtr<CefNavigationEntry> entry = browser->GetHost()->GetVisibleNavigationEntry();
            std::string page_title = entry ? entry->GetTitle().ToString() : "";
            HistoryManager::GetInstance().AddEntry(url.ToString(), page_title);
        }
    }
}

void DaemonRequestHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
                                       CefRefPtr<CefFrame> frame,
                                       cef_errorcode_t errorCode,
                                       const CefString& errorText,
                                       const CefString& failedUrl) {
    if (errorCode == ERR_ABORTED) {
        return; // Don't show error page on user cancellation
    }

    if (frame->IsMain()) {
        std::string html = "<html><head><title>Navigation Error</title></head>"
                           "<body style=\"font-family: sans-serif; background: #1a1a1a; color: #fff; padding: 40px;\">"
                           "<h2 style=\"color: #ff5555;\">Navigation Failed</h2>"
                           "<p><strong>URL:</strong> " + failedUrl.ToString() + "</p>"
                           "<p><strong>Error:</strong> " + errorText.ToString() + " (" + std::to_string(errorCode) + ")</p>"
                           "<hr style=\"border-color: #333;\">"
                           "<p style=\"color: #aaa;\">Daemon Browser - Connection Error</p>"
                           "</body></html>";
        frame->LoadURL("data:text/html;charset=utf-8," + html);
    }
}

bool DaemonRequestHandler::OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                                         const CefKeyEvent& event,
                                         CefEventHandle os_event,
                                         bool* is_keyboard_shortcut) {
    // Shortcuts: Ctrl+T, Ctrl+W, Ctrl+Shift+T, Ctrl+L, Ctrl+R, Ctrl+F, Ctrl+=, Ctrl+-, Ctrl+0, Alt+Left, Alt+Right, Escape
    if (event.type == KEYEVENT_RAWKEYDOWN) {
        if (event.modifiers & EVENTFLAG_CONTROL_DOWN) {
            if ((event.modifiers & EVENTFLAG_SHIFT_DOWN) && event.windows_key_code == 'T') {
                if (browser_window_) browser_window_->ReopenClosedTab();
                return true;
            } else if (event.windows_key_code == 'T') {
                if (browser_window_) browser_window_->CreateNewTab("");
                return true;
            } else if (event.windows_key_code == 9) { // Tab key
                if (browser_window_) {
                    bool forward = !(event.modifiers & EVENTFLAG_SHIFT_DOWN);
                    browser_window_->CycleTab(forward);
                }
                return true;
            } else if (event.windows_key_code == 'W') {
                if (browser_window_) browser_window_->CloseActiveTab();
                return true;
            } else if (event.windows_key_code == 'L') {
                if (browser_window_) browser_window_->FocusAddressBar();
                return true;
            } else if (event.windows_key_code == 'R') {
                browser->Reload();
                return true;
            } else if (event.windows_key_code == 'F') {
                if (browser_window_) browser_window_->ToggleFindBar();
                return true;
            } else if (event.windows_key_code == 187 || event.windows_key_code == 61) { // '=' or '+'
                double zoom = browser->GetHost()->GetZoomLevel();
                browser->GetHost()->SetZoomLevel(zoom + 0.5);
                return true;
            } else if (event.windows_key_code == 189 || event.windows_key_code == 173) { // '-'
                double zoom = browser->GetHost()->GetZoomLevel();
                browser->GetHost()->SetZoomLevel(zoom - 0.5);
                return true;
            } else if (event.windows_key_code == '0') {
                browser->GetHost()->SetZoomLevel(0.0);
                return true;
            }
        } else if (event.modifiers & EVENTFLAG_ALT_DOWN) {
            if (event.windows_key_code == 37) { // Left arrow
                if (browser->CanGoBack()) browser->GoBack();
                return true;
            } else if (event.windows_key_code == 39) { // Right arrow
                if (browser->CanGoForward()) browser->GoForward();
                return true;
            }
        } else if (event.windows_key_code == 27) { // Escape
            if (browser_window_ && browser_window_->IsFindBarVisible()) {
                browser_window_->HideFindBar();
                return true;
            }
            if (browser->IsLoading()) {
                browser->StopLoad();
                return true;
            }
        }
    }
    return false;
}

void DaemonRequestHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                               CefRefPtr<CefFrame> frame,
                                               CefRefPtr<CefContextMenuParams> params,
                                               CefRefPtr<CefMenuModel> model) {
    model->Clear();

    if ((params->GetTypeFlags() & CM_TYPEFLAG_LINK) != 0 || !params->GetUnfilteredLinkUrl().empty()) {
        model->AddItem(101, "Open Link in New Tab");
        model->AddItem(102, "Copy Link Address");
        model->AddSeparator();
    }

    model->AddItem(103, "Back");
    model->SetEnabled(103, browser->CanGoBack());
    model->AddItem(104, "Forward");
    model->SetEnabled(104, browser->CanGoForward());
    model->AddItem(105, "Reload");
    model->AddSeparator();

    int edit_flags = params->GetEditStateFlags();
    model->AddItem(106, "Copy");
    model->SetEnabled(106, (edit_flags & CM_EDITFLAG_CAN_COPY) != 0);
    model->AddItem(107, "Paste");
    model->SetEnabled(107, (edit_flags & CM_EDITFLAG_CAN_PASTE) != 0);
    model->AddItem(108, "Select All");
    model->SetEnabled(108, (edit_flags & CM_EDITFLAG_CAN_SELECT_ALL) != 0);
}

bool DaemonRequestHandler::OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
                                                CefRefPtr<CefFrame> frame,
                                                CefRefPtr<CefContextMenuParams> params,
                                                int command_id,
                                                EventFlags event_flags) {
    switch (command_id) {
        case 101:
            if (browser_window_ && !params->GetUnfilteredLinkUrl().empty()) {
                browser_window_->CreateNewTab(params->GetUnfilteredLinkUrl().ToString());
            }
            return true;
        case 103:
            if (browser->CanGoBack()) browser->GoBack();
            return true;
        case 104:
            if (browser->CanGoForward()) browser->GoForward();
            return true;
        case 105:
            browser->Reload();
            return true;
        case 106:
            frame->Copy();
            return true;
        case 107:
            frame->Paste();
            return true;
        case 108:
            frame->SelectAll();
            return true;
        default:
            return false;
    }
}

bool DaemonRequestHandler::OnBeforePopup(CefRefPtr<CefBrowser> browser,
                                         CefRefPtr<CefFrame> frame,
                                         int popup_id,
                                         const CefString& target_url,
                                         const CefString& target_frame_name,
                                         CefLifeSpanHandler::WindowOpenDisposition target_disposition,
                                         bool user_gesture,
                                         const CefPopupFeatures& popupFeatures,
                                         CefWindowInfo& windowInfo,
                                         CefRefPtr<CefClient>& client,
                                         CefBrowserSettings& settings,
                                         CefRefPtr<CefDictionaryValue>& extra_info,
                                         bool* no_javascript_access) {
    std::string url_str = target_url.ToString();
    if (!url_str.empty() && browser) {
        browser->GetMainFrame()->LoadURL(url_str);
    }
    return true; // Cancel default standalone window creation
}


