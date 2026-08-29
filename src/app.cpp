#include "app.h"
#include "browser_window.h"
#include "include/wrapper/cef_helpers.h"

DaemonApp::DaemonApp() {}

void DaemonApp::OnContextInitialized() {
    CEF_REQUIRE_UI_THREAD();

    CefRefPtr<BrowserWindow> window(new BrowserWindow());
    window->Show();
}
