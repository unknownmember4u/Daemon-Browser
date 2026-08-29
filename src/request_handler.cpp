#include "request_handler.h"
#include "browser_window.h"
#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"
#include "security_state.h"
#include <iostream>

DaemonRequestHandler::DaemonRequestHandler(BrowserWindow* browser_window)
    : browser_window_(browser_window) {}

cef_return_value_t DaemonRequestHandler::OnBeforeResourceLoad(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    CefRefPtr<CefCallback> callback) {

    // REQUEST OBSERVATION (Maintained exactly as requested)
    std::string method = request->GetMethod();
    std::string url = request->GetURL();
    int resource_type = request->GetResourceType(); // RT_MAIN_FRAME = 0, etc.

    std::cout << "REQUEST" << std::endl;
    std::cout << "METHOD: " << method << std::endl;
    std::cout << "URL: " << url << std::endl;
    std::cout << "RESOURCE_TYPE: " << resource_type << std::endl;
    std::cout << "---------------------------------" << std::endl;

    return RV_CONTINUE;
}

void DaemonRequestHandler::OnAddressChange(CefRefPtr<CefBrowser> browser,
                                           CefRefPtr<CefFrame> frame,
                                           const CefString& url) {
    if (browser_window_) {
        browser_window_->OnAddressChange(url);
        
        SecurityInfo info = SecurityState::ComputeFromBrowser(browser, url);
        browser_window_->UpdateSecurityIndicator(info);
    }
}

void DaemonRequestHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
                                         const CefString& title) {
    if (browser_window_) {
        browser_window_->OnTitleChange(title);
    }
}

void DaemonRequestHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                                bool isLoading,
                                                bool canGoBack,
                                                bool canGoForward) {
    if (browser_window_) {
        browser_window_->OnLoadingStateChange(isLoading, canGoBack, canGoForward);
        
        if (!isLoading) {
            std::cout << "BENCHMARK_SIGNAL: LOAD_END" << std::endl;
            // Recompute security when load completes to capture TLS details
            CefString url = browser->GetMainFrame()->GetURL();
            SecurityInfo info = SecurityState::ComputeFromBrowser(browser, url);
            browser_window_->UpdateSecurityIndicator(info);
        }
    }
}

bool DaemonRequestHandler::OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                                         const CefKeyEvent& event,
                                         CefEventHandle os_event,
                                         bool* is_keyboard_shortcut) {
    // Handle Ctrl+L (focus address bar), Ctrl+R (reload), Alt+Left (back), Alt+Right (forward)
    if (event.type == KEYEVENT_RAWKEYDOWN) {
        if (event.modifiers & EVENTFLAG_CONTROL_DOWN) {
            if (event.windows_key_code == 'L') {
                if (browser_window_) browser_window_->FocusAddressBar();
                return true;
            } else if (event.windows_key_code == 'R') {
                browser->Reload();
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
        }
    }
    return false;
}


