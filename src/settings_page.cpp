#include "settings_page.h"
#include <sstream>

std::string GetSettingsPageHTML() {
    std::ostringstream html;
    html << R"raw(<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>Settings - Daemon Browser</title>
    <style>
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body {
            background-color: #121318;
            color: #e2e8f0;
            font-family: system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
            padding: 40px;
            display: flex;
            justify-content: center;
        }
        .container {
            width: 100%;
            max-width: 800px;
            display: flex;
            flex-direction: column;
            gap: 24px;
        }
        .header {
            display: flex;
            align-items: center;
            border-bottom: 1px solid #334155;
            padding-bottom: 16px;
        }
        .header-title {
            font-size: 24px;
            font-weight: 700;
            display: flex;
            align-items: center;
            gap: 12px;
        }
        .setting-group {
            background: #1e293b;
            border: 1px solid #334155;
            border-radius: 10px;
            padding: 20px;
            display: flex;
            flex-direction: column;
            gap: 16px;
        }
        .group-title {
            font-size: 16px;
            font-weight: 600;
            color: #38bdf8;
        }
        .row {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding-top: 8px;
            border-top: 1px solid #334155;
        }
        .row-info {
            display: flex;
            flex-direction: column;
            gap: 4px;
        }
        .row-label {
            font-size: 14px;
            font-weight: 500;
            color: #f1f5f9;
        }
        .row-sub {
            font-size: 12px;
            color: #94a3b8;
        }
        .val-badge {
            background: #0f172a;
            border: 1px solid #334155;
            padding: 6px 14px;
            border-radius: 6px;
            font-size: 12px;
            color: #e2e8f0;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <div class="header-title"><span>&#9881;</span> Daemon Preferences & Settings</div>
        </div>
        <div class="setting-group">
            <div class="group-title">General Configuration</div>
            <div class="row" style="border-top: none;">
                <div class="row-info">
                    <div class="row-label">Default Search Engine</div>
                    <div class="row-sub">Freeform queries map to Google HTTPS search</div>
                </div>
                <div class="val-badge">Google Search (HTTPS)</div>
            </div>
            <div class="row">
                <div class="row-info">
                    <div class="row-label">Startup Behavior</div>
                    <div class="row-sub">Restores active tab session on startup</div>
                </div>
                <div class="val-badge">Session Restoration Active</div>
            </div>
            <div class="row">
                <div class="row-info">
                    <div class="row-label">Visual Theme</div>
                    <div class="row-sub">Dark charcoal desktop theme shell</div>
                </div>
                <div class="val-badge">Daemon Dark (#171820)</div>
            </div>
            <div class="row">
                <div class="row-info">
                    <div class="row-label">Storage Location</div>
                    <div class="row-sub">Local user configuration & session data path</div>
                </div>
                <div class="val-badge">~/.config/daemon_browser/</div>
            </div>
        </div>
    </div>
</body>
</html>)raw";

    return html.str();
}
