#pragma once

#include "include/views/cef_window.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_panel.h"
#include "include/views/cef_label_button.h"
#include "include/views/cef_textfield.h"
#include "include/views/cef_window_delegate.h"
#include "include/views/cef_button_delegate.h"
#include "include/views/cef_textfield_delegate.h"

class BrowserWindow : public CefWindowDelegate,
                      public CefButtonDelegate,
                      public CefTextfieldDelegate {
public:
    BrowserWindow();

    void Show();

    // Updates the UI state from CEF callbacks
    void OnTitleChange(const CefString& title);
    void OnAddressChange(const CefString& url);
    void OnLoadingStateChange(bool isLoading, bool canGoBack, bool canGoForward);

    // CefWindowDelegate methods:
    void OnWindowCreated(CefRefPtr<CefWindow> window) override;
    void OnWindowDestroyed(CefRefPtr<CefWindow> window) override;
    bool CanClose(CefRefPtr<CefWindow> window) override;

    void FocusAddressBar();

    // CefButtonDelegate methods:
    void OnButtonPressed(CefRefPtr<CefButton> button) override;

    // CefTextfieldDelegate methods:
    bool OnKeyEvent(CefRefPtr<CefTextfield> textfield, const CefKeyEvent& event) override;

private:
    CefRefPtr<CefWindow> window_;
    CefRefPtr<CefBrowserView> browser_view_;
    CefRefPtr<CefLabelButton> back_button_;
    CefRefPtr<CefLabelButton> forward_button_;
    CefRefPtr<CefLabelButton> reload_button_;
    CefRefPtr<CefTextfield> address_bar_;

    IMPLEMENT_REFCOUNTING(BrowserWindow);
};
