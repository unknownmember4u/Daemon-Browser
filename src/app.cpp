#include "app.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"
#include "request_handler.h"

namespace {
class SimpleWindowDelegate : public CefWindowDelegate {
public:
    explicit SimpleWindowDelegate(CefRefPtr<CefBrowserView> browser_view)
        : browser_view_(browser_view) {}

    void OnWindowCreated(CefRefPtr<CefWindow> window) override {
        CefRefPtr<CefView> view(browser_view_);
        window->AddChildView(view);
        window->Show();
        browser_view_->RequestFocus();
    }

    void OnWindowDestroyed(CefRefPtr<CefWindow> window) override {
        browser_view_ = nullptr;
    }

    bool CanClose(CefRefPtr<CefWindow> window) override {
        CefRefPtr<CefBrowser> browser = browser_view_->GetBrowser();
        if (browser) {
            bool result = browser->GetHost()->TryCloseBrowser();
            if (!result) {
                return false;
            }
        }
        return true;
    }

private:
    CefRefPtr<CefBrowserView> browser_view_;
    IMPLEMENT_REFCOUNTING(SimpleWindowDelegate);
};
} // namespace

DaemonApp::DaemonApp() {}

void DaemonApp::OnContextInitialized() {
    CEF_REQUIRE_UI_THREAD();

    CefRefPtr<CefClient> client(new DaemonRequestHandler());

    CefBrowserSettings browser_settings;
    std::string url = "https://example.com/";

    // Create the browser view.
    CefRefPtr<CefBrowserView> browser_view = CefBrowserView::CreateBrowserView(client, url, browser_settings, nullptr, nullptr, nullptr);
    CefRefPtr<CefWindow> window = CefWindow::CreateTopLevelWindow(new SimpleWindowDelegate(browser_view));
    window->SetTitle("Daemon Browser PoC");
    window->SetBounds(CefRect(100, 100, 800, 600));
}
