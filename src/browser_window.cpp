#include "browser_window.h"
#include "request_handler.h"
#include "include/views/cef_box_layout.h"
#include "include/wrapper/cef_helpers.h"
#include <iostream>

BrowserWindow::BrowserWindow() {
    CEF_REQUIRE_UI_THREAD();

    CefRefPtr<DaemonRequestHandler> client = new DaemonRequestHandler(this);

    CefBrowserSettings browser_settings;
    std::string url = "https://example.com/";

    // Create the browser view.
    browser_view_ = CefBrowserView::CreateBrowserView(client, url, browser_settings, nullptr, nullptr, nullptr);
}

void BrowserWindow::Show() {
    CEF_REQUIRE_UI_THREAD();
    // Create the window. This calls OnWindowCreated.
    CefWindow::CreateTopLevelWindow(this);
}

void BrowserWindow::OnWindowCreated(CefRefPtr<CefWindow> window) {
    CEF_REQUIRE_UI_THREAD();
    window_ = window;

    // Create toolbar panel
    CefRefPtr<CefPanel> toolbar = CefPanel::CreatePanel(nullptr);
    CefBoxLayoutSettings toolbar_layout_settings;
    toolbar_layout_settings.horizontal = true;
    auto toolbar_layout = toolbar->SetToBoxLayout(toolbar_layout_settings);

    // Create buttons
    back_button_ = CefLabelButton::CreateLabelButton(this, "<");
    back_button_->SetEnabled(false);
    toolbar->AddChildView(back_button_);

    forward_button_ = CefLabelButton::CreateLabelButton(this, ">");
    forward_button_->SetEnabled(false);
    toolbar->AddChildView(forward_button_);

    reload_button_ = CefLabelButton::CreateLabelButton(this, "R");
    toolbar->AddChildView(reload_button_);

    // Create address bar
    address_bar_ = CefTextfield::CreateTextfield(this);
    toolbar->AddChildView(address_bar_);
    toolbar_layout->SetFlexForView(address_bar_, 1);

    // Add everything to window
    CefBoxLayoutSettings window_layout_settings;
    window_layout_settings.horizontal = false;
    auto window_layout = window->SetToBoxLayout(window_layout_settings);

    window->AddChildView(toolbar);
    window->AddChildView(browser_view_);
    window_layout->SetFlexForView(browser_view_, 1);

    window->SetTitle("Daemon Browser");
    window->SetBounds(CefRect(100, 100, 1024, 768));
    window->Show();
    browser_view_->RequestFocus();
}

void BrowserWindow::OnWindowDestroyed(CefRefPtr<CefWindow> window) {
    CEF_REQUIRE_UI_THREAD();
    browser_view_ = nullptr;
    window_ = nullptr;
    address_bar_ = nullptr;
    back_button_ = nullptr;
    forward_button_ = nullptr;
    reload_button_ = nullptr;
}

bool BrowserWindow::CanClose(CefRefPtr<CefWindow> window) {
    CEF_REQUIRE_UI_THREAD();
    CefRefPtr<CefBrowser> browser = browser_view_->GetBrowser();
    if (browser) {
        bool result = browser->GetHost()->TryCloseBrowser();
        if (!result) {
            return false;
        }
    }
    return true;
}

void BrowserWindow::OnTitleChange(const CefString& title) {
    CEF_REQUIRE_UI_THREAD();
    if (window_) {
        window_->SetTitle(title.empty() ? "Daemon Browser" : title);
    }
    std::cout << "UI_TEST: TITLE_CHANGED to " << title.ToString() << std::endl;
}

void BrowserWindow::OnAddressChange(const CefString& url) {
    CEF_REQUIRE_UI_THREAD();
    if (address_bar_ && !address_bar_->HasFocus()) {
        address_bar_->SetText(url);
    }
    std::cout << "UI_TEST: ADDRESS_CHANGED to " << url.ToString() << std::endl;
}

void BrowserWindow::OnLoadingStateChange(bool isLoading, bool canGoBack, bool canGoForward) {
    CEF_REQUIRE_UI_THREAD();
    if (back_button_) back_button_->SetEnabled(canGoBack);
    if (forward_button_) forward_button_->SetEnabled(canGoForward);
    if (reload_button_) {
        reload_button_->SetText(isLoading ? "X" : "R");
    }
    std::cout << "UI_TEST: LOADING_STATE_CHANGED isLoading=" << isLoading << std::endl;
}

void BrowserWindow::FocusAddressBar() {
    CEF_REQUIRE_UI_THREAD();
    if (address_bar_) {
        address_bar_->RequestFocus();
        address_bar_->SelectAll(true);
    }
}

void BrowserWindow::OnButtonPressed(CefRefPtr<CefButton> button) {
    CEF_REQUIRE_UI_THREAD();
    CefRefPtr<CefBrowser> browser = browser_view_->GetBrowser();
    if (!browser) return;

    if (button->GetID() == back_button_->GetID()) {
        if (browser->CanGoBack()) browser->GoBack();
    } else if (button->GetID() == forward_button_->GetID()) {
        if (browser->CanGoForward()) browser->GoForward();
    } else if (button->GetID() == reload_button_->GetID()) {
        if (browser->IsLoading()) {
            browser->StopLoad();
        } else {
            browser->Reload();
        }
    }
}

bool BrowserWindow::OnKeyEvent(CefRefPtr<CefTextfield> textfield, const CefKeyEvent& event) {
    CEF_REQUIRE_UI_THREAD();
    // Native key code 65293 is usually Enter on Linux/X11, or 13.
    // In CEF, event.windows_key_code is mapped uniformly. Enter is 13 (VK_RETURN).
    if (textfield->GetID() == address_bar_->GetID()) {
        if (event.type == KEYEVENT_RAWKEYDOWN && event.windows_key_code == 13) {
            CefRefPtr<CefBrowser> browser = browser_view_->GetBrowser();
            if (browser) {
                CefString url = address_bar_->GetText();
                std::string url_str = url.ToString();
                // Extremely basic URL validation: if it doesn't contain "://", prepend "https://"
                if (url_str.find("://") == std::string::npos) {
                    url_str = "https://" + url_str;
                }
                browser->GetMainFrame()->LoadURL(url_str);
            }
            // Defocus address bar
            browser_view_->RequestFocus();
            return true;
        }
    }
    return false;
}
