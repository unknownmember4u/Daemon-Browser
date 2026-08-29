#include "bookmark_manager_page.h"
#include "bookmark_manager.h"
#include <sstream>
#include <ctime>

std::string GetBookmarkManagerPageHTML() {
    auto bookmarks = BookmarkManager::GetInstance().GetBookmarks();

    std::ostringstream items;
    if (bookmarks.empty()) {
        items << "<div class='empty'>No bookmarks saved yet. Click the &#11088; star in the address bar to bookmark pages!</div>";
    } else {
        for (const auto& b : bookmarks) {
            std::time_t t = static_cast<std::time_t>(b.timestamp);
            char time_buf[64] = {0};
            std::strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M", std::localtime(&t));

            items << "<div class='card'>"
                  << "  <div class='card-info'>"
                  << "    <a class='card-title' href='" << b.url << "'>" << b.title << "</a>"
                  << "    <div class='card-url'>" << b.url << " &bull; " << time_buf << "</div>"
                  << "  </div>"
                  << "</div>";
        }
    }

    std::ostringstream html;
    html << R"raw(<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>Bookmarks - Daemon Browser</title>
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
            gap: 12px;
            border-bottom: 1px solid #334155;
            padding-bottom: 16px;
        }
        .header-icon {
            font-size: 28px;
            color: #f59e0b;
        }
        .header-title {
            font-size: 24px;
            font-weight: 700;
        }
        .list {
            display: flex;
            flex-direction: column;
            gap: 12px;
        }
        .card {
            background: #1e293b;
            border: 1px solid #334155;
            border-radius: 10px;
            padding: 16px;
            display: flex;
            align-items: center;
            justify-content: space-between;
        }
        .card-info {
            display: flex;
            flex-direction: column;
            gap: 4px;
        }
        .card-title {
            font-size: 16px;
            font-weight: 600;
            color: #38bdf8;
            text-decoration: none;
        }
        .card-title:hover {
            text-decoration: underline;
        }
        .card-url {
            font-size: 12px;
            color: #94a3b8;
        }
        .empty {
            color: #94a3b8;
            font-size: 15px;
            padding: 32px;
            text-align: center;
            background: #1e293b;
            border-radius: 10px;
            border: 1px dashed #334155;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <div class="header-icon">&#11088;</div>
            <div class="header-title">Daemon Bookmarks</div>
        </div>
        <div class="list">
)raw" << items.str() << R"raw(
        </div>
    </div>
</body>
</html>)raw";

    return html.str();
}
