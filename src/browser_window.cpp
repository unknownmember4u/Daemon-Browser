#include "browser_window.h"
#include "request_handler.h"
#include "session_manager.h"
#include "new_tab_page.h"
#include "bookmark_manager.h"
#include "bookmark_manager_page.h"
#include "include/views/cef_box_layout.h"
#include "include/views/cef_fill_layout.h"
#include "include/wrapper/cef_helpers.h"
#include "include/cef_command_line.h"
#include "include/cef_parser.h"
#include <iostream>
#include <algorithm>

BrowserWindow::BrowserWindow() {
    CEF_REQUIRE_UI_THREAD();
}

void BrowserWindow::Show() {
    CEF_REQUIRE_UI_THREAD();
    CefWindow::CreateTopLevelWindow(this);
    std::cout << "BENCHMARK_SIGNAL: STARTUP_READY" << std::endl;
}

void BrowserWindow::OnWindowCreated(CefRefPtr<CefWindow> window) {
    CEF_REQUIRE_UI_THREAD();
    window_ = window;

    // Outer layout: vertical box layout
    CefBoxLayoutSettings window_layout_settings;
    window_layout_settings.horizontal = false;
    auto window_layout = window->SetToBoxLayout(window_layout_settings);

    // 1. Create Tab Bar Panel
    tab_bar_panel_ = CefPanel::CreatePanel(nullptr);
    CefBoxLayoutSettings tab_bar_layout_settings;
    tab_bar_layout_settings.horizontal = true;
    auto tab_bar_layout = tab_bar_panel_->SetToBoxLayout(tab_bar_layout_settings);

    new_tab_button_ = CefLabelButton::CreateLabelButton(this, " + ");
    app_menu_button_ = CefMenuButton::CreateMenuButton(this, " [ ⚡ Daemon ] ");

    // 2. Create Toolbar Panel
    CefRefPtr<CefPanel> toolbar = CefPanel::CreatePanel(nullptr);
    CefBoxLayoutSettings toolbar_layout_settings;
    toolbar_layout_settings.horizontal = true;
    auto toolbar_layout = toolbar->SetToBoxLayout(toolbar_layout_settings);

    back_button_ = CefLabelButton::CreateLabelButton(this, " < ");
    back_button_->SetEnabled(false);
    toolbar->AddChildView(back_button_);

    forward_button_ = CefLabelButton::CreateLabelButton(this, " > ");
    forward_button_->SetEnabled(false);
    toolbar->AddChildView(forward_button_);

    reload_button_ = CefLabelButton::CreateLabelButton(this, " R ");
    toolbar->AddChildView(reload_button_);

    security_indicator_ = CefMenuButton::CreateMenuButton(this, " [ ❓ Unknown ] ");
    toolbar->AddChildView(security_indicator_);

    address_bar_ = CefTextfield::CreateTextfield(this);
    toolbar->AddChildView(address_bar_);
    toolbar_layout->SetFlexForView(address_bar_, 1);

    bookmark_button_ = CefLabelButton::CreateLabelButton(this, " ☆ ");
    toolbar->AddChildView(bookmark_button_);

    // 3. Create Find Bar Panel (hidden by default)
    find_panel_ = CefPanel::CreatePanel(nullptr);
    CefBoxLayoutSettings find_layout_settings;
    find_layout_settings.horizontal = true;
    auto find_layout = find_panel_->SetToBoxLayout(find_layout_settings);

    find_textfield_ = CefTextfield::CreateTextfield(this);
    find_prev_button_ = CefLabelButton::CreateLabelButton(this, " ^ ");
    find_next_button_ = CefLabelButton::CreateLabelButton(this, " v ");
    find_close_button_ = CefLabelButton::CreateLabelButton(this, " x ");

    find_panel_->AddChildView(find_textfield_);
    find_layout->SetFlexForView(find_textfield_, 1);
    find_panel_->AddChildView(find_prev_button_);
    find_panel_->AddChildView(find_next_button_);
    find_panel_->AddChildView(find_close_button_);
    find_panel_->SetVisible(false);

    // 4. Create Main Workspace Panel (Horizontal Box Layout)
    CefRefPtr<CefPanel> workspace_panel = CefPanel::CreatePanel(nullptr);
    CefBoxLayoutSettings workspace_layout_settings;
    workspace_layout_settings.horizontal = true;
    auto workspace_layout = workspace_panel->SetToBoxLayout(workspace_layout_settings);

    // Left Sidebar Panel
    sidebar_panel_ = CefPanel::CreatePanel(nullptr);
    CefBoxLayoutSettings sidebar_layout_settings;
    sidebar_layout_settings.horizontal = false;
    sidebar_panel_->SetToBoxLayout(sidebar_layout_settings);

    home_button_ = CefLabelButton::CreateLabelButton(this, " Home ");
    bookmarks_button_ = CefLabelButton::CreateLabelButton(this, " Bookmarks ");
    history_button_ = CefLabelButton::CreateLabelButton(this, " History ");
    downloads_button_ = CefLabelButton::CreateLabelButton(this, " Downloads ");
    security_button_ = CefLabelButton::CreateLabelButton(this, " Security ");

    sidebar_panel_->AddChildView(home_button_);
    sidebar_panel_->AddChildView(bookmarks_button_);
    sidebar_panel_->AddChildView(history_button_);
    sidebar_panel_->AddChildView(downloads_button_);
    sidebar_panel_->AddChildView(security_button_);

    // Center Content Panel for active browser view
    content_panel_ = CefPanel::CreatePanel(nullptr);
    content_panel_->SetToFillLayout();

    workspace_panel->AddChildView(sidebar_panel_);
    workspace_panel->AddChildView(content_panel_);
    workspace_layout->SetFlexForView(content_panel_, 1);

    window->AddChildView(tab_bar_panel_);
    window->AddChildView(toolbar);
    window->AddChildView(find_panel_);
    window->AddChildView(workspace_panel);
    window_layout->SetFlexForView(workspace_panel, 1);

    window->SetTitle("Daemon Browser");
    window->SetBounds(CefRect(100, 100, 1024, 768));
    window->Show();

    // Determine initial URL or restore session
    CefRefPtr<CefCommandLine> command_line = CefCommandLine::GetGlobalCommandLine();
    if (command_line->HasSwitch("url")) {
        std::string initial_url = command_line->GetSwitchValue("url").ToString();
        CreateNewTab(initial_url);
    } else {
        std::vector<std::string> session_urls = SessionManager::GetInstance().LoadSession();
        if (!session_urls.empty()) {
            for (const auto& url : session_urls) {
                CreateNewTab(url);
            }
        } else {
            CreateNewTab("");
        }
    }
}

void BrowserWindow::CreateNewTab(const std::string& url) {
    CEF_REQUIRE_UI_THREAD();

    std::string target_url = url;
    if (target_url.empty() || target_url == "daemon://newtab") {
        target_url = "data:text/html;charset=utf-8," + GetNewTabPageHTML();
    } else if (target_url == "daemon://bookmarks") {
        target_url = "data:text/html;charset=utf-8," + GetBookmarkManagerPageHTML();
    }

    CefRefPtr<DaemonRequestHandler> client = new DaemonRequestHandler(this);
    CefBrowserSettings browser_settings;

    CefRefPtr<CefBrowserView> browser_view =
        CefBrowserView::CreateBrowserView(client, target_url, browser_settings, nullptr, nullptr, nullptr);

    auto tab = std::make_shared<TabInfo>();
    tab->id = next_tab_id_++;
    tab->title = "New Tab";
    tab->url = target_url;
    tab->browser_view = browser_view;

    std::string tab_label = " Tab " + std::to_string(tab->id) + " ";
    tab->tab_button = CefLabelButton::CreateLabelButton(this, tab_label);
    tab->close_button = CefLabelButton::CreateLabelButton(this, "x");

    tabs_.push_back(tab);

    UpdateTabBarUI();
    SwitchToTab(tab->id);
    SaveCurrentSession();
}

void BrowserWindow::CloseTab(int tab_id) {
    CEF_REQUIRE_UI_THREAD();

    auto it = std::find_if(tabs_.begin(), tabs_.end(), [tab_id](const std::shared_ptr<TabInfo>& t) {
        return t->id == tab_id;
    });

    if (it == tabs_.end()) {
        return;
    }

    std::shared_ptr<TabInfo> tab_to_close = *it;
    if (!tab_to_close->url.empty()) {
        closed_tab_urls_.push_back(tab_to_close->url);
    }

    if (tab_to_close->browser_view && tab_to_close->browser_view->GetBrowser()) {
        tab_to_close->browser_view->GetBrowser()->GetHost()->TryCloseBrowser();
    }

    tabs_.erase(it);
    SaveCurrentSession();

    if (tabs_.empty()) {
        if (window_) {
            window_->Close();
        }
        return;
    }

    if (active_tab_id_ == tab_id) {
        SwitchToTab(tabs_.back()->id);
    } else {
        UpdateTabBarUI();
    }
}

void BrowserWindow::CloseActiveTab() {
    CEF_REQUIRE_UI_THREAD();
    if (active_tab_id_ != -1) {
        CloseTab(active_tab_id_);
    }
}

void BrowserWindow::SwitchToTab(int tab_id) {
    CEF_REQUIRE_UI_THREAD();

    auto tab = std::find_if(tabs_.begin(), tabs_.end(), [tab_id](const std::shared_ptr<TabInfo>& t) {
        return t->id == tab_id;
    });

    if (tab == tabs_.end()) {
        return;
    }

    active_tab_id_ = tab_id;

    if (content_panel_) {
        content_panel_->RemoveAllChildViews();
        content_panel_->AddChildView((*tab)->browser_view);
        content_panel_->InvalidateLayout();
    }

    UpdateTabBarUI();
    UpdateControlsForActiveTab();

    if ((*tab)->browser_view) {
        (*tab)->browser_view->RequestFocus();
    }
}

void BrowserWindow::ReopenClosedTab() {
    CEF_REQUIRE_UI_THREAD();
    if (!closed_tab_urls_.empty()) {
        std::string last_url = closed_tab_urls_.back();
        closed_tab_urls_.pop_back();
        CreateNewTab(last_url);
    }
}

void BrowserWindow::StopLoading() {
    CEF_REQUIRE_UI_THREAD();
    auto tab = GetActiveTab();
    if (tab && tab->browser_view && tab->browser_view->GetBrowser()) {
        if (tab->browser_view->GetBrowser()->IsLoading()) {
            tab->browser_view->GetBrowser()->StopLoad();
        }
    }
}

void BrowserWindow::ToggleFindBar() {
    CEF_REQUIRE_UI_THREAD();
    if (!find_panel_) return;
    if (find_bar_visible_) {
        HideFindBar();
    } else {
        find_bar_visible_ = true;
        find_panel_->SetVisible(true);
        if (find_textfield_) {
            find_textfield_->RequestFocus();
            find_textfield_->SelectAll(true);
        }
        if (window_) window_->InvalidateLayout();
    }
}

void BrowserWindow::HideFindBar() {
    CEF_REQUIRE_UI_THREAD();
    if (!find_panel_) return;
    find_bar_visible_ = false;
    find_panel_->SetVisible(false);
    if (window_) window_->InvalidateLayout();

    auto tab = GetActiveTab();
    if (tab && tab->browser_view && tab->browser_view->GetBrowser()) {
        tab->browser_view->GetBrowser()->GetHost()->StopFinding(true);
    }
}

void BrowserWindow::DoFind(bool forward) {
    CEF_REQUIRE_UI_THREAD();
    if (!find_textfield_) return;
    std::string text = find_textfield_->GetText().ToString();
    if (text.empty()) return;

    auto tab = GetActiveTab();
    if (tab && tab->browser_view && tab->browser_view->GetBrowser()) {
        tab->browser_view->GetBrowser()->GetHost()->Find(text, forward, false, true);
    }
}

void BrowserWindow::UpdateTabBarUI() {
    CEF_REQUIRE_UI_THREAD();
    if (!tab_bar_panel_) return;

    tab_bar_panel_->RemoveAllChildViews();

    for (const auto& tab : tabs_) {
        std::string title_display = tab->title.empty() ? "Tab " + std::to_string(tab->id) : tab->title;
        if (title_display.length() > 18) {
            title_display = title_display.substr(0, 15) + "...";
        }
        std::string btn_label = (tab->id == active_tab_id_ ? "[ " : "  ") + title_display + (tab->id == active_tab_id_ ? " ]" : "  ");
        tab->tab_button->SetText(btn_label);

        tab_bar_panel_->AddChildView(tab->tab_button);
        tab_bar_panel_->AddChildView(tab->close_button);
    }

    if (new_tab_button_) {
        tab_bar_panel_->AddChildView(new_tab_button_);
    }
    if (app_menu_button_) {
        tab_bar_panel_->AddChildView(app_menu_button_);
    }

    tab_bar_panel_->InvalidateLayout();
}

std::shared_ptr<TabInfo> BrowserWindow::GetTabForBrowser(CefRefPtr<CefBrowser> browser) {
    if (!browser) return nullptr;
    int browser_id = browser->GetIdentifier();

    for (const auto& tab : tabs_) {
        if (tab->browser_view && tab->browser_view->GetBrowser()) {
            if (tab->browser_view->GetBrowser()->GetIdentifier() == browser_id) {
                return tab;
            }
        }
    }
    return nullptr;
}

std::shared_ptr<TabInfo> BrowserWindow::GetActiveTab() {
    for (const auto& tab : tabs_) {
        if (tab->id == active_tab_id_) {
            return tab;
        }
    }
    return nullptr;
}

void BrowserWindow::UpdateControlsForActiveTab() {
    CEF_REQUIRE_UI_THREAD();
    auto tab = GetActiveTab();
    if (!tab) return;

    if (window_) {
        window_->SetTitle(tab->title.empty() ? "Daemon Browser" : tab->title + " - Daemon Browser");
    }

    if (address_bar_ && !address_bar_->HasFocus()) {
        address_bar_->SetText(tab->url);
    }

    if (back_button_) back_button_->SetEnabled(tab->can_go_back);
    if (forward_button_) forward_button_->SetEnabled(tab->can_go_forward);
    if (reload_button_) reload_button_->SetText(tab->is_loading ? " X " : " R ");

    if (bookmark_button_) {
        bool is_bm = BookmarkManager::GetInstance().IsBookmarked(tab->url);
        bookmark_button_->SetText(is_bm ? " ★ " : " ☆ ");
    }

    UpdateSecurityIndicator(nullptr, tab->security_info);
}

void BrowserWindow::SaveCurrentSession() {
    std::vector<std::string> urls;
    for (const auto& tab : tabs_) {
        if (!tab->url.empty()) {
            urls.push_back(tab->url);
        }
    }
    SessionManager::GetInstance().SaveSession(urls);
}

void BrowserWindow::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) {
    CEF_REQUIRE_UI_THREAD();
    auto tab = GetTabForBrowser(browser);
    if (tab) {
        std::string t_str = title.ToString();
        tab->title = t_str.empty() ? "Untitled Page" : t_str;
        if (tab->id == active_tab_id_) {
            UpdateControlsForActiveTab();
        }
        UpdateTabBarUI();
    }
}

void BrowserWindow::OnAddressChange(CefRefPtr<CefBrowser> browser, const CefString& url) {
    CEF_REQUIRE_UI_THREAD();
    auto tab = GetTabForBrowser(browser);
    if (tab) {
        tab->url = url.ToString();
        if (tab->id == active_tab_id_) {
            UpdateControlsForActiveTab();
        }
        SaveCurrentSession();
    }
}

void BrowserWindow::OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward) {
    CEF_REQUIRE_UI_THREAD();
    auto tab = GetTabForBrowser(browser);
    if (tab) {
        tab->is_loading = isLoading;
        tab->can_go_back = canGoBack;
        tab->can_go_forward = canGoForward;
        if (tab->id == active_tab_id_) {
            UpdateControlsForActiveTab();
        }
    }
}

void BrowserWindow::OnFaviconURLChange(CefRefPtr<CefBrowser> browser, const CefString& icon_url) {
    CEF_REQUIRE_UI_THREAD();
    auto tab = GetTabForBrowser(browser);
    if (tab) {
        tab->favicon_url = icon_url.ToString();
    }
}

void BrowserWindow::UpdateSecurityIndicator(CefRefPtr<CefBrowser> browser, const SecurityInfo& info) {
    CEF_REQUIRE_UI_THREAD();
    if (browser) {
        auto tab = GetTabForBrowser(browser);
        if (tab) {
            tab->security_info = info;
        }
    }

    auto active_tab = GetActiveTab();
    if (active_tab && (browser == nullptr || (active_tab->browser_view && active_tab->browser_view->GetBrowser() && active_tab->browser_view->GetBrowser()->IsSame(browser)))) {
        current_security_info_ = active_tab->security_info;
        if (security_indicator_) {
            std::string state_str;
            switch (active_tab->security_info.level) {
                case SecurityLevel::SECURE: state_str = " [ 🛡️ Secure ] "; break;
                case SecurityLevel::INSECURE: state_str = " [ ⚠️ Insecure ] "; break;
                case SecurityLevel::UNKNOWN: default: state_str = " [ ❓ Unknown ] "; break;
            }
            security_indicator_->SetText(state_str);
            std::cout << "SECURITY_STATE: " << state_str << " | URL: " << (active_tab->security_info.scheme.empty() ? "" : active_tab->security_info.scheme + "://") << (active_tab->security_info.origin.empty() ? "" : active_tab->security_info.origin) << std::endl;
        }
    }
}

void BrowserWindow::OnMenuButtonPressed(
      CefRefPtr<CefMenuButton> menu_button,
      const CefPoint& screen_point,
      CefRefPtr<CefMenuButtonPressedLock> button_pressed_lock) {
    CEF_REQUIRE_UI_THREAD();

    if (app_menu_button_ && menu_button->GetID() == app_menu_button_->GetID()) {
        CefRefPtr<CefMenuModel> model = CefMenuModel::CreateMenuModel(this);
        model->AddItem(201, "New Tab (Ctrl+T)");
        model->AddItem(202, "Reopen Closed Tab (Ctrl+Shift+T)");
        model->AddItem(203, "Close Tab (Ctrl+W)");
        model->AddSeparator();
        model->AddItem(204, "Find in Page (Ctrl+F)");
        model->AddItem(205, "Zoom In (Ctrl++)");
        model->AddItem(206, "Zoom Out (Ctrl+-)");
        model->AddItem(207, "Reset Zoom (Ctrl+0)");
        model->AddSeparator();
        model->AddItem(208, "Security Center Details");
        model->AddItem(212, "Bookmarks Manager");
        model->AddItem(209, "Exit Daemon Browser");
        menu_button->ShowMenu(model, screen_point, CEF_MENU_ANCHOR_BOTTOMCENTER);
        return;
    }

    CefRefPtr<CefMenuModel> model = CefMenuModel::CreateMenuModel(this);

    std::string state_text = "Security State: ";
    switch (current_security_info_.level) {
        case SecurityLevel::SECURE: state_text += "Secure"; break;
        case SecurityLevel::INSECURE: state_text += "Insecure"; break;
        default: state_text += "Unknown"; break;
    }

    std::string origin_text = "Origin: " + (current_security_info_.origin.empty() ? "N/A" : current_security_info_.origin);
    std::string conn_text = "Connection: " + (current_security_info_.scheme.empty() ? "Unknown" : current_security_info_.scheme);

    std::string cert_text = "Certificate: " + current_security_info_.certificate_status;
    std::string cert_err_text = "Cert Errors: ";
    cert_err_text += current_security_info_.certificate_error ? "Detected" : "None";

    std::string web_sec_text = "Web Security: " + std::string(current_security_info_.web_security_enabled ? "Enabled" : "Disabled");
    std::string site_iso_text = "Site Isolation: " + std::string(current_security_info_.site_isolation_enabled ? "Enabled" : "Disabled");
    std::string sandbox_text = "Sandbox: Configured";

    model->AddItem(1, state_text);
    model->SetEnabled(1, false);
    model->AddSeparator();
    model->AddItem(2, origin_text);
    model->SetEnabled(2, false);
    model->AddItem(3, conn_text);
    model->SetEnabled(3, false);
    model->AddSeparator();
    model->AddItem(4, cert_text);
    model->SetEnabled(4, false);
    model->AddItem(5, cert_err_text);
    model->SetEnabled(5, false);
    model->AddSeparator();
    model->AddItem(6, web_sec_text);
    model->SetEnabled(6, false);
    model->AddItem(7, site_iso_text);
    model->SetEnabled(7, false);
    model->AddItem(8, sandbox_text);
    model->SetEnabled(8, false);

    menu_button->ShowMenu(model, screen_point, CEF_MENU_ANCHOR_BOTTOMCENTER);
}

void BrowserWindow::ExecuteCommand(CefRefPtr<CefMenuModel> menu_model, int command_id, cef_event_flags_t event_flags) {
    CEF_REQUIRE_UI_THREAD();
    switch (command_id) {
        case 201: CreateNewTab(""); break;
        case 202: ReopenClosedTab(); break;
        case 203: CloseActiveTab(); break;
        case 204: ToggleFindBar(); break;
        case 205: {
            auto tab = GetActiveTab();
            if (tab && tab->browser_view && tab->browser_view->GetBrowser()) {
                double zoom = tab->browser_view->GetBrowser()->GetHost()->GetZoomLevel();
                tab->browser_view->GetBrowser()->GetHost()->SetZoomLevel(zoom + 0.5);
            }
            break;
        }
        case 206: {
            auto tab = GetActiveTab();
            if (tab && tab->browser_view && tab->browser_view->GetBrowser()) {
                double zoom = tab->browser_view->GetBrowser()->GetHost()->GetZoomLevel();
                tab->browser_view->GetBrowser()->GetHost()->SetZoomLevel(zoom - 0.5);
            }
            break;
        }
        case 207: {
            auto tab = GetActiveTab();
            if (tab && tab->browser_view && tab->browser_view->GetBrowser()) {
                tab->browser_view->GetBrowser()->GetHost()->SetZoomLevel(0.0);
            }
            break;
        }
        case 208:
            if (security_indicator_) {
                CefPoint pt;
                OnMenuButtonPressed(security_indicator_, pt, nullptr);
            }
            break;
        case 212:
            CreateNewTab("daemon://bookmarks");
            break;
        case 209:
            if (window_) window_->Close();
            break;
    }
}

void BrowserWindow::OnWindowDestroyed(CefRefPtr<CefWindow> window) {
    CEF_REQUIRE_UI_THREAD();
    for (auto& tab : tabs_) {
        if (tab->browser_view && tab->browser_view->GetBrowser()) {
            tab->browser_view->GetBrowser()->GetHost()->CloseBrowser(true);
        }
    }
    tabs_.clear();
    content_panel_ = nullptr;
    tab_bar_panel_ = nullptr;
    sidebar_panel_ = nullptr;
    window_ = nullptr;
    address_bar_ = nullptr;
    back_button_ = nullptr;
    forward_button_ = nullptr;
    reload_button_ = nullptr;
    security_indicator_ = nullptr;
    app_menu_button_ = nullptr;
    home_button_ = nullptr;
    history_button_ = nullptr;
    downloads_button_ = nullptr;
    security_button_ = nullptr;
}

bool BrowserWindow::CanClose(CefRefPtr<CefWindow> window) {
    CEF_REQUIRE_UI_THREAD();
    for (auto& tab : tabs_) {
        if (tab->browser_view && tab->browser_view->GetBrowser()) {
            tab->browser_view->GetBrowser()->GetHost()->TryCloseBrowser();
        }
    }
    return true;
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
    int btn_id = button->GetID();

    if (new_tab_button_ && btn_id == new_tab_button_->GetID()) {
        CreateNewTab("");
        return;
    }

    if (home_button_ && btn_id == home_button_->GetID()) {
        CreateNewTab("");
        return;
    }
    if (bookmarks_button_ && btn_id == bookmarks_button_->GetID()) {
        CreateNewTab("daemon://bookmarks");
        return;
    }
    if (bookmark_button_ && btn_id == bookmark_button_->GetID()) {
        auto active_tab = GetActiveTab();
        if (active_tab && !active_tab->url.empty()) {
            if (BookmarkManager::GetInstance().IsBookmarked(active_tab->url)) {
                BookmarkManager::GetInstance().RemoveBookmark(active_tab->url);
            } else {
                BookmarkManager::GetInstance().AddBookmark(active_tab->title, active_tab->url);
            }
            UpdateControlsForActiveTab();
        }
        return;
    }
    if (security_button_ && btn_id == security_button_->GetID()) {
        if (security_indicator_) {
            CefPoint pt;
            OnMenuButtonPressed(security_indicator_, pt, nullptr);
        }
        return;
    }
    if (history_button_ && btn_id == history_button_->GetID()) {
        CreateNewTab("");
        return;
    }
    if (downloads_button_ && btn_id == downloads_button_->GetID()) {
        CreateNewTab("");
        return;
    }

    if (find_close_button_ && btn_id == find_close_button_->GetID()) {
        HideFindBar();
        return;
    }
    if (find_next_button_ && btn_id == find_next_button_->GetID()) {
        DoFind(true);
        return;
    }
    if (find_prev_button_ && btn_id == find_prev_button_->GetID()) {
        DoFind(false);
        return;
    }

    for (const auto& tab : tabs_) {
        if (tab->tab_button && btn_id == tab->tab_button->GetID()) {
            SwitchToTab(tab->id);
            return;
        }
        if (tab->close_button && btn_id == tab->close_button->GetID()) {
            CloseTab(tab->id);
            return;
        }
    }

    auto active_tab = GetActiveTab();
    if (!active_tab || !active_tab->browser_view || !active_tab->browser_view->GetBrowser()) return;

    CefRefPtr<CefBrowser> browser = active_tab->browser_view->GetBrowser();

    if (back_button_ && btn_id == back_button_->GetID()) {
        if (browser->CanGoBack()) browser->GoBack();
    } else if (forward_button_ && btn_id == forward_button_->GetID()) {
        if (browser->CanGoForward()) browser->GoForward();
    } else if (reload_button_ && btn_id == reload_button_->GetID()) {
        if (browser->IsLoading()) {
            browser->StopLoad();
        } else {
            browser->Reload();
        }
    }
}

bool BrowserWindow::OnKeyEvent(CefRefPtr<CefTextfield> textfield, const CefKeyEvent& event) {
    CEF_REQUIRE_UI_THREAD();
    if (address_bar_ && textfield->GetID() == address_bar_->GetID()) {
        if (event.type == KEYEVENT_RAWKEYDOWN && event.windows_key_code == 13) {
            auto active_tab = GetActiveTab();
            if (active_tab && active_tab->browser_view && active_tab->browser_view->GetBrowser()) {
                CefRefPtr<CefBrowser> browser = active_tab->browser_view->GetBrowser();
                std::string input = address_bar_->GetText().ToString();
                std::string target_url;

                if (input.find("://") != std::string::npos) {
                    target_url = input;
                } else if (input.find(".") != std::string::npos && input.find(" ") == std::string::npos) {
                    target_url = "https://" + input;
                } else {
                    target_url = "https://www.google.com/search?q=" + CefURIEncode(input, true).ToString();
                }

                browser->GetMainFrame()->LoadURL(target_url);
                active_tab->browser_view->RequestFocus();
            }
            return true;
        }
    } else if (find_textfield_ && textfield->GetID() == find_textfield_->GetID()) {
        if (event.type == KEYEVENT_RAWKEYDOWN) {
            if (event.windows_key_code == 13) {
                bool forward = !(event.modifiers & EVENTFLAG_SHIFT_DOWN);
                DoFind(forward);
                return true;
            } else if (event.windows_key_code == 27) {
                HideFindBar();
                return true;
            }
        }
    }
    return false;
}
