#pragma once
#include "include/cef_app.h"

class DaemonApp : public CefApp, public CefBrowserProcessHandler {
public:
    DaemonApp();

    // CefApp methods:
    virtual void OnBeforeCommandLineProcessing(
        const CefString& process_type,
        CefRefPtr<CefCommandLine> command_line) override;
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override { return this; }

    // CefBrowserProcessHandler methods:
    virtual void OnContextInitialized() override;

private:
    IMPLEMENT_REFCOUNTING(DaemonApp);
};
