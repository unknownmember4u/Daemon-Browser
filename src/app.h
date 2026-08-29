#pragma once
#include "include/cef_app.h"

class DaemonApp : public CefApp, public CefBrowserProcessHandler {
public:
    DaemonApp();

    // CefApp methods:
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override { return this; }

    // CefBrowserProcessHandler methods:
    virtual void OnContextInitialized() override;

private:
    IMPLEMENT_REFCOUNTING(DaemonApp);
};
