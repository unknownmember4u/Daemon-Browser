#include "new_tab_page.h"

std::string GetNewTabPageHTML() {
    return R"raw(<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>New Tab - Daemon Browser</title>
    <style>
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body {
            background-color: #12131a;
            color: #e2e8f0;
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
            display: flex;
            flex-direction: column;
            align-items: center;
            min-height: 100vh;
            padding: 40px 24px;
        }
        .container {
            width: 100%;
            max-width: 920px;
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 32px;
        }
        .brand {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 8px;
        }
        .brand-logo {
            width: 72px;
            height: 72px;
            background: radial-gradient(circle at 35% 35%, #ff4d4d 0%, #b31217 60%, #500003 100%);
            border-radius: 50%;
            display: flex;
            align-items: center;
            justify-content: center;
            box-shadow: 0 0 30px rgba(220, 38, 38, 0.4);
            position: relative;
        }
        .brand-logo::after {
            content: '';
            width: 44px;
            height: 44px;
            background: #12131a;
            border-radius: 50%;
            position: absolute;
            top: 14px;
            left: 8px;
        }
        .brand-title {
            font-size: 28px;
            font-weight: 800;
            letter-spacing: 2px;
            color: #ffffff;
            margin-top: 4px;
        }
        .brand-subtitle {
            font-size: 14px;
            color: #94a3b8;
            letter-spacing: 0.2px;
        }
        .search-container {
            width: 100%;
            max-width: 680px;
            position: relative;
        }
        .search-box {
            width: 100%;
            display: flex;
            align-items: center;
            background-color: #1a1b26;
            border: 1px solid #2a2c3d;
            border-radius: 30px;
            padding: 6px 8px 6px 20px;
            box-shadow: 0 8px 24px rgba(0, 0, 0, 0.4);
            transition: all 0.25s ease;
        }
        .search-box:focus-within {
            border-color: #dc2626;
            box-shadow: 0 8px 30px rgba(220, 38, 38, 0.25);
        }
        .search-icon {
            color: #64748b;
            font-size: 18px;
            margin-right: 14px;
        }
        .search-input {
            width: 100%;
            border: none;
            background: transparent;
            color: #ffffff;
            font-size: 15px;
            outline: none;
        }
        .search-input::placeholder {
            color: #64748b;
        }
        .search-btn {
            width: 38px;
            height: 38px;
            border-radius: 50%;
            background: #dc2626;
            border: none;
            color: #ffffff;
            display: flex;
            align-items: center;
            justify-content: center;
            cursor: pointer;
            font-weight: bold;
            font-size: 16px;
            flex-shrink: 0;
            transition: transform 0.15s ease;
        }
        .search-btn:hover {
            transform: scale(1.05);
            background: #ef4444;
        }
        .badges {
            display: flex;
            gap: 16px;
        }
        .badge {
            background: #181924;
            border: 1px solid #262838;
            padding: 8px 18px;
            border-radius: 20px;
            font-size: 13px;
            color: #94a3b8;
            display: flex;
            align-items: center;
            gap: 8px;
        }
        .section {
            width: 100%;
            display: flex;
            flex-direction: column;
            gap: 16px;
        }
        .section-header {
            display: flex;
            align-items: center;
            justify-content: space-between;
        }
        .section-title {
            font-size: 18px;
            font-weight: 700;
            color: #ffffff;
        }
        .shortcuts-grid {
            display: grid;
            grid-template-columns: repeat(4, 1fr);
            gap: 14px;
            width: 100%;
        }
        .card {
            background: #181924;
            border: 1px solid #262838;
            border-radius: 14px;
            padding: 14px 16px;
            display: flex;
            align-items: center;
            justify-content: space-between;
            text-decoration: none;
            color: #ffffff;
            transition: all 0.2s ease;
        }
        .card:hover {
            background: #1f2130;
            border-color: #dc2626;
            transform: translateY(-2px);
        }
        .card-left {
            display: flex;
            align-items: center;
            gap: 12px;
            overflow: hidden;
        }
        .card-icon {
            width: 38px;
            height: 38px;
            border-radius: 10px;
            background: #12131a;
            display: flex;
            align-items: center;
            justify-content: center;
            font-size: 20px;
            flex-shrink: 0;
        }
        .card-info {
            display: flex;
            flex-direction: column;
            overflow: hidden;
        }
        .card-name {
            font-size: 14px;
            font-weight: 600;
            color: #f1f5f9;
            white-space: nowrap;
            overflow: hidden;
            text-overflow: ellipsis;
        }
        .card-sub {
            font-size: 12px;
            color: #64748b;
            white-space: nowrap;
            overflow: hidden;
            text-overflow: ellipsis;
        }
        .card-opt {
            color: #64748b;
            font-size: 16px;
            padding: 4px;
        }
        .categories {
            display: flex;
            gap: 10px;
            overflow-x: auto;
        }
        .cat-pill {
            background: #181924;
            border: 1px solid #262838;
            color: #94a3b8;
            padding: 6px 16px;
            border-radius: 20px;
            font-size: 13px;
            font-weight: 500;
            cursor: pointer;
            transition: all 0.2s ease;
        }
        .cat-pill.active {
            background: #dc2626;
            color: #ffffff;
            border-color: #dc2626;
        }
        .stories-grid {
            display: grid;
            grid-template-columns: repeat(4, 1fr);
            gap: 16px;
            width: 100%;
        }
        .story-card {
            background: #181924;
            border: 1px solid #262838;
            border-radius: 14px;
            overflow: hidden;
            display: flex;
            flex-direction: column;
            transition: all 0.2s ease;
        }
        .story-card:hover {
            border-color: #dc2626;
            transform: translateY(-3px);
        }
        .story-thumb {
            width: 100%;
            height: 120px;
            background-size: cover;
            background-position: center;
            display: flex;
            align-items: center;
            justify-content: center;
            position: relative;
        }
        .thumb-1 {
            background: linear-gradient(135deg, #1e1b4b 0%, #065f46 50%, #0f172a 100%);
        }
        .thumb-2 {
            background: linear-gradient(135deg, #0f172a 0%, #1e3a8a 50%, #0284c7 100%);
        }
        .thumb-3 {
            background: linear-gradient(135deg, #311b92 0%, #880e4f 50%, #12131a 100%);
        }
        .thumb-4 {
            background: linear-gradient(135deg, #004d40 0%, #311b92 50%, #4a148c 100%);
        }
        .story-body {
            padding: 14px;
            display: flex;
            flex-direction: column;
            gap: 8px;
            flex-grow: 1;
            justify-content: space-between;
        }
        .story-tag {
            font-size: 11px;
            font-weight: 700;
            letter-spacing: 0.5px;
            text-transform: uppercase;
        }
        .tag-tech { color: #ef4444; }
        .tag-sec { color: #10b981; }
        .tag-ai { color: #6366f1; }
        .story-title {
            font-size: 13px;
            font-weight: 600;
            line-height: 1.4;
            color: #f1f5f9;
        }
        .story-time {
            font-size: 11px;
            color: #64748b;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="brand">
            <div class="brand-logo"></div>
            <div class="brand-title">DAEMON</div>
            <div class="brand-subtitle">Confidential browsing, without unnecessary browser clutter.</div>
        </div>

        <div class="search-container">
            <form class="search-box" onsubmit="event.preventDefault(); var v = document.getElementById('q').value; if(v){ window.location.href = v.includes('.') && !v.includes(' ') ? (v.includes('://') ? v : 'https://' + v) : 'https://www.google.com/search?q=' + encodeURIComponent(v); }">
                <span class="search-icon">&#128099;</span>
                <input type="text" id="q" class="search-input" placeholder="What are you looking for today?" autofocus>
                <button type="submit" class="search-btn">&#10140;</button>
            </form>
        </div>

        <div class="badges">
            <div class="badge"><span>&#128737;</span> Secure by default</div>
            <div class="badge"><span>&#128274;</span> Privacy focused</div>
            <div class="badge"><span>&#128640;</span> Performance optimized</div>
        </div>

        <div class="section">
            <div class="section-title">Quick Access</div>
            <div class="shortcuts-grid">
                <a href="https://github.com/" class="card">
                    <div class="card-left">
                        <div class="card-icon">&#128187;</div>
                        <div class="card-info">
                            <div class="card-name">GitHub</div>
                            <div class="card-sub">github.com</div>
                        </div>
                    </div>
                    <div class="card-opt">&#8942;</div>
                </a>
                <a href="https://mail.google.com/" class="card">
                    <div class="card-left">
                        <div class="card-icon">&#9993;</div>
                        <div class="card-info">
                            <div class="card-name">Gmail</div>
                            <div class="card-sub">mail.google.com</div>
                        </div>
                    </div>
                    <div class="card-opt">&#8942;</div>
                </a>
                <a href="https://youtube.com/" class="card">
                    <div class="card-left">
                        <div class="card-icon">&#9654;</div>
                        <div class="card-info">
                            <div class="card-name">YouTube</div>
                            <div class="card-sub">youtube.com</div>
                        </div>
                    </div>
                    <div class="card-opt">&#8942;</div>
                </a>
                <a href="https://discord.com/" class="card">
                    <div class="card-left">
                        <div class="card-icon">&#128483;</div>
                        <div class="card-info">
                            <div class="card-name">Discord</div>
                            <div class="card-sub">discord.com</div>
                        </div>
                    </div>
                    <div class="card-opt">&#8942;</div>
                </a>
                <a href="https://reddit.com/" class="card">
                    <div class="card-left">
                        <div class="card-icon">&#128172;</div>
                        <div class="card-info">
                            <div class="card-name">Reddit</div>
                            <div class="card-sub">reddit.com</div>
                        </div>
                    </div>
                    <div class="card-opt">&#8942;</div>
                </a>
                <a href="https://linkedin.com/" class="card">
                    <div class="card-left">
                        <div class="card-icon">&#128188;</div>
                        <div class="card-info">
                            <div class="card-name">LinkedIn</div>
                            <div class="card-sub">linkedin.com</div>
                        </div>
                    </div>
                    <div class="card-opt">&#8942;</div>
                </a>
                <a href="https://x.com/" class="card">
                    <div class="card-left">
                        <div class="card-icon">&#128038;</div>
                        <div class="card-info">
                            <div class="card-name">X</div>
                            <div class="card-sub">x.com</div>
                        </div>
                    </div>
                    <div class="card-opt">&#8942;</div>
                </a>
                <div class="card" style="cursor: pointer;">
                    <div class="card-left">
                        <div class="card-icon" style="background: #1f2130;">+</div>
                        <div class="card-info">
                            <div class="card-name">Add shortcut</div>
                        </div>
                    </div>
                </div>
            </div>
        </div>

        <div class="section">
            <div class="section-header">
                <div class="section-title">Top Stories</div>
                <div class="categories">
                    <div class="cat-pill active">All</div>
                    <div class="cat-pill">Technology</div>
                    <div class="cat-pill">Security</div>
                    <div class="cat-pill">AI</div>
                    <div class="cat-pill">News</div>
                    <div class="cat-pill">Entertainment</div>
                    <div class="cat-pill">Travel</div>
                    <div class="cat-pill">Sports</div>
                </div>
            </div>

            <div class="stories-grid">
                <div class="story-card">
                    <div class="story-thumb thumb-1">
                        <svg width="60" height="60" viewBox="0 0 24 24" fill="none" stroke="#ef4444" stroke-width="1.5"><rect x="4" y="4" width="16" height="16" rx="2"/><rect x="9" y="9" width="6" height="6"/><path d="M9 2v2M15 2v2M9 20v2M15 20v2M2 9h2M2 15h2M20 9h2M20 15h2"/></svg>
                    </div>
                    <div class="story-body">
                        <div class="story-tag tag-tech">TECHNOLOGY</div>
                        <div class="story-title">New AI Breakthrough Transforms Browser Security</div>
                        <div class="story-time">2 hours ago</div>
                    </div>
                </div>

                <div class="story-card">
                    <div class="story-thumb thumb-2">
                        <svg width="60" height="60" viewBox="0 0 24 24" fill="none" stroke="#10b981" stroke-width="1.5"><rect x="3" y="11" width="18" height="11" rx="2" ry="2"/><path d="M7 11V7a5 5 0 0 1 10 0v4"/></svg>
                    </div>
                    <div class="story-body">
                        <div class="story-tag tag-sec">SECURITY</div>
                        <div class="story-title">How End-to-End Encryption Protects Your Data</div>
                        <div class="story-time">5 hours ago</div>
                    </div>
                </div>

                <div class="story-card">
                    <div class="story-thumb thumb-3">
                        <svg width="60" height="60" viewBox="0 0 24 24" fill="none" stroke="#6366f1" stroke-width="1.5"><polyline points="16 18 22 12 16 6"/><polyline points="8 6 2 12 8 18"/></svg>
                    </div>
                    <div class="story-body">
                        <div class="story-tag tag-tech">TECHNOLOGY</div>
                        <div class="story-title">WebAssembly 2.0 Brings Major Performance Boost</div>
                        <div class="story-time">1 day ago</div>
                    </div>
                </div>

                <div class="story-card">
                    <div class="story-thumb thumb-4">
                        <svg width="60" height="60" viewBox="0 0 24 24" fill="none" stroke="#a855f7" stroke-width="1.5"><path d="M12 2a10 10 0 1 0 10 10A10 10 0 0 0 12 2zm0 18a8 8 0 1 1 8-8 8 8 0 0 1-8 8z"/><circle cx="12" cy="12" r="3"/></svg>
                    </div>
                    <div class="story-body">
                        <div class="story-tag tag-ai">AI</div>
                        <div class="story-title">The Future of AI in Everyday Browser Experiences</div>
                        <div class="story-time">2 days ago</div>
                    </div>
                </div>
            </div>
        </div>
    </div>
</body>
</html>)raw";
}
