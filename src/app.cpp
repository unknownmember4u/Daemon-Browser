#include "app.h"
#include "browser_window.h"
#include "include/wrapper/cef_helpers.h"

#include "render_backend.h"
#include <iostream>

DaemonApp::DaemonApp() {}

void DaemonApp::OnBeforeCommandLineProcessing(
    const CefString& process_type,
    CefRefPtr<CefCommandLine> command_line) {
    if (process_type.empty()) {
        // Main browser process
        RenderBackend backend = DetectRenderBackend(command_line);
        if (backend == RenderBackend::NativeWayland && !command_line->HasSwitch("ozone-platform-hint")) {
            command_line->AppendSwitchWithValue("ozone-platform-hint", "auto");
        }
        std::cout << "[RENDER_BACKEND] Mode: " << RenderBackendName(backend) << std::endl;
    }
}

void DaemonApp::OnContextInitialized() {
    CEF_REQUIRE_UI_THREAD();

    CefRefPtr<BrowserWindow> window(new BrowserWindow());
    window->Show();
}
