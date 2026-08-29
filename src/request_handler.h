#pragma once
#include "include/cef_client.h"

// Forward declaration
class BrowserWindow;

class DaemonRequestHandler : public CefClient,
                             public CefRequestHandler,
                             public CefResourceRequestHandler,
                             public CefDisplayHandler,
                             public CefLoadHandler,
                             public CefKeyboardHandler {
public:
    DaemonRequestHandler(BrowserWindow* browser_window);

    // CefClient methods:
    virtual CefRefPtr<CefRequestHandler> GetRequestHandler() override { return this; }
    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override { return this; }
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override { return this; }
    virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() override { return this; }

    // CefRequestHandler methods:
    virtual CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        bool is_navigation,
        bool is_download,
        const CefString& request_initiator,
        bool& disable_default_handling) override {
        return this;
    }

    // CefResourceRequestHandler methods:
    virtual cef_return_value_t OnBeforeResourceLoad(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        CefRefPtr<CefCallback> callback) override;

    // CefDisplayHandler methods:
    virtual void OnAddressChange(CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 const CefString& url) override;
    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
                               const CefString& title) override;

    // CefLoadHandler methods:
    virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                      bool isLoading,
                                      bool canGoBack,
                                      bool canGoForward) override;

    // CefKeyboardHandler methods:
    virtual bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                               const CefKeyEvent& event,
                               CefEventHandle os_event,
                               bool* is_keyboard_shortcut) override;

private:
    BrowserWindow* browser_window_; // Weak pointer, owned by CefWindow

    IMPLEMENT_REFCOUNTING(DaemonRequestHandler);
};
