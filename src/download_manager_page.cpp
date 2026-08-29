#include "download_manager_page.h"
#include <sstream>

std::string GetDownloadManagerPageHTML() {
    std::ostringstream html;
    html << R"raw(<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>Downloads - Daemon Browser</title>
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
            justify-content: space-between;
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
        .info-card {
            background: #1e293b;
            border: 1px solid #334155;
            border-radius: 10px;
            padding: 24px;
            display: flex;
            flex-direction: column;
            gap: 12px;
        }
        .info-title {
            font-size: 18px;
            font-weight: 600;
            color: #38bdf8;
        }
        .info-desc {
            font-size: 14px;
            color: #94a3b8;
            line-height: 1.5;
        }
        .badge {
            background: #0f172a;
            border: 1px solid #334155;
            padding: 8px 16px;
            border-radius: 20px;
            font-size: 13px;
            color: #34d399;
            align-self: flex-start;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <div class="header-title"><span>&#128229;</span> Downloads Manager</div>
        </div>
        <div class="info-card">
            <div class="info-title">Download Destination</div>
            <div class="info-desc">All downloaded files are automatically routed to your system downloads folder (<code>~/Downloads/</code>) with non-blocking async transfer and filename collision prevention.</div>
            <div class="badge">&#10004; Download Handler Active</div>
        </div>
    </div>
</body>
</html>)raw";

    return html.str();
}
