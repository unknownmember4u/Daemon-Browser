#pragma once

#include "include/views/cef_window.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_panel.h"
#include "include/views/cef_label_button.h"
#include "include/views/cef_textfield.h"
#include "include/views/cef_window_delegate.h"
#include "include/views/cef_button_delegate.h"
#include "include/views/cef_textfield_delegate.h"
#include "include/views/cef_menu_button_delegate.h"
#include "include/views/cef_menu_button.h"
#include "include/cef_menu_model_delegate.h"
#include "security_state.h"

class BrowserWindow : public CefWindowDelegate,
                      public CefTextfieldDelegate,
                      public CefMenuButtonDelegate,
                      public CefMenuModelDelegate {
public:
    BrowserWindow();

    void Show();

    // Updates the UI state from CEF callbacks
    void OnTitleChange(const CefString& title);
    void OnAddressChange(const CefString& url);
    void OnLoadingStateChange(bool isLoading, bool canGoBack, bool canGoForward);
    
    // Updates the Security Indicator
    void UpdateSecurityIndicator(const SecurityInfo& info);

    // CefWindowDelegate methods:
    void OnWindowCreated(CefRefPtr<CefWindow> window) override;
    void OnWindowDestroyed(CefRefPtr<CefWindow> window) override;
    bool CanClose(CefRefPtr<CefWindow> window) override;

    void FocusAddressBar();

    // CefButtonDelegate methods:
    void OnButtonPressed(CefRefPtr<CefButton> button) override;

    // CefTextfieldDelegate methods:
    bool OnKeyEvent(CefRefPtr<CefTextfield> textfield, const CefKeyEvent& event) override;

    // CefMenuButtonDelegate methods:
    void OnMenuButtonPressed(CefRefPtr<CefMenuButton> menu_button,
                             const CefPoint& screen_point,
                             CefRefPtr<CefMenuButtonPressedLock> button_pressed_lock) override;
                             
    // CefMenuModelDelegate methods:
    void ExecuteCommand(CefRefPtr<CefMenuModel> menu_model, int command_id, cef_event_flags_t event_flags) override {}

private:
    CefRefPtr<CefWindow> window_;
    CefRefPtr<CefBrowserView> browser_view_;
    CefRefPtr<CefLabelButton> back_button_;
    CefRefPtr<CefLabelButton> forward_button_;
    CefRefPtr<CefLabelButton> reload_button_;
    CefRefPtr<CefTextfield> address_bar_;
    CefRefPtr<CefMenuButton> security_indicator_;
    
    SecurityInfo current_security_info_;

    IMPLEMENT_REFCOUNTING(BrowserWindow);
};

