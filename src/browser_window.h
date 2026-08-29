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
#include <vector>
#include <memory>
#include <string>

struct TabInfo {
    int id;
    std::string title;
    std::string url;
    std::string favicon_url;
    CefRefPtr<CefBrowserView> browser_view;
    CefRefPtr<CefLabelButton> tab_button;
    CefRefPtr<CefLabelButton> close_button;
    SecurityInfo security_info;
    bool is_loading = false;
    bool can_go_back = false;
    bool can_go_forward = false;
};

class BrowserWindow : public CefWindowDelegate,
                      public CefTextfieldDelegate,
                      public CefMenuButtonDelegate,
                      public CefMenuModelDelegate {
public:
    BrowserWindow();

    void Show();

    // Tab Management
    void CreateNewTab(const std::string& url = "https://example.com/");
    void CloseTab(int tab_id);
    void CloseActiveTab();
    void SwitchToTab(int tab_id);
    void ReopenClosedTab();
    void StopLoading();

    // Updates the UI state from CEF callbacks
    void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title);
    void OnAddressChange(CefRefPtr<CefBrowser> browser, const CefString& url);
    void OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward);
    void OnFaviconURLChange(CefRefPtr<CefBrowser> browser, const CefString& icon_url);
    
    // Updates the Security Indicator
    void UpdateSecurityIndicator(CefRefPtr<CefBrowser> browser, const SecurityInfo& info);

    // Find In Page
    void ToggleFindBar();
    void HideFindBar();
    bool IsFindBarVisible() const { return find_bar_visible_; }
    void DoFind(bool forward);

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
    void ExecuteCommand(CefRefPtr<CefMenuModel> menu_model, int command_id, cef_event_flags_t event_flags) override;

private:
    std::shared_ptr<TabInfo> GetTabForBrowser(CefRefPtr<CefBrowser> browser);
    std::shared_ptr<TabInfo> GetActiveTab();
    void UpdateTabBarUI();
    void UpdateControlsForActiveTab();
    void SaveCurrentSession();

    CefRefPtr<CefWindow> window_;
    CefRefPtr<CefPanel> tab_bar_panel_;
    CefRefPtr<CefPanel> content_panel_;
    CefRefPtr<CefLabelButton> new_tab_button_;

    CefRefPtr<CefLabelButton> back_button_;
    CefRefPtr<CefLabelButton> forward_button_;
    CefRefPtr<CefLabelButton> reload_button_;
    CefRefPtr<CefTextfield> address_bar_;
    CefRefPtr<CefLabelButton> bookmark_button_;
    CefRefPtr<CefMenuButton> security_indicator_;
    CefRefPtr<CefMenuButton> app_menu_button_;

    CefRefPtr<CefPanel> sidebar_panel_;
    CefRefPtr<CefLabelButton> home_button_;
    CefRefPtr<CefLabelButton> bookmarks_button_;
    CefRefPtr<CefLabelButton> history_button_;
    CefRefPtr<CefLabelButton> downloads_button_;
    CefRefPtr<CefLabelButton> security_button_;

    CefRefPtr<CefPanel> find_panel_;
    CefRefPtr<CefTextfield> find_textfield_;
    CefRefPtr<CefLabelButton> find_next_button_;
    CefRefPtr<CefLabelButton> find_prev_button_;
    CefRefPtr<CefLabelButton> find_close_button_;
    bool find_bar_visible_ = false;
    
    std::vector<std::shared_ptr<TabInfo>> tabs_;
    int active_tab_id_ = -1;
    int next_tab_id_ = 1;
    std::vector<std::string> closed_tab_urls_;

    SecurityInfo current_security_info_;

    IMPLEMENT_REFCOUNTING(BrowserWindow);
};
