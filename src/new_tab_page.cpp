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
            background-color: #121318;
            color: #e2e8f0;
            font-family: system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            min-height: 100vh;
            padding: 20px;
        }
        .container {
            width: 100%;
            max-width: 720px;
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
        .brand-icon {
            font-size: 48px;
            background: linear-gradient(135deg, #6366f1, #a855f7);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
        }
        .brand-title {
            font-size: 32px;
            font-weight: 700;
            letter-spacing: -0.5px;
            color: #f8fafc;
        }
        .brand-subtitle {
            font-size: 14px;
            color: #94a3b8;
        }
        .search-box {
            width: 100%;
            display: flex;
            position: relative;
        }
        .search-input {
            width: 100%;
            padding: 16px 24px;
            font-size: 16px;
            border-radius: 28px;
            border: 1px solid #334155;
            background-color: #1e293b;
            color: #f8fafc;
            outline: none;
            box-shadow: 0 4px 20px rgba(0, 0, 0, 0.2);
            transition: all 0.2s ease;
        }
        .search-input:focus {
            border-color: #8b5cf6;
            box-shadow: 0 4px 24px rgba(139, 92, 246, 0.25);
        }
        .shortcuts {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 16px;
            width: 100%;
        }
        .card {
            background: #1e293b;
            border: 1px solid #334155;
            border-radius: 12px;
            padding: 16px;
            display: flex;
            align-items: center;
            gap: 12px;
            text-decoration: none;
            color: #e2e8f0;
            transition: all 0.2s ease;
        }
        .card:hover {
            background: #334155;
            border-color: #64748b;
            transform: translateY(-2px);
        }
        .card-icon {
            font-size: 20px;
            width: 36px;
            height: 36px;
            border-radius: 8px;
            background: #0f172a;
            display: flex;
            align-items: center;
            justify-content: center;
        }
        .card-title {
            font-size: 14px;
            font-weight: 500;
        }
        .badge-container {
            display: flex;
            gap: 12px;
            margin-top: 16px;
        }
        .badge {
            background: #0f172a;
            border: 1px solid #334155;
            padding: 6px 14px;
            border-radius: 20px;
            font-size: 12px;
            color: #38bdf8;
            font-weight: 500;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="brand">
            <div class="brand-icon">&#9889;</div>
            <div class="brand-title">Daemon Browser</div>
            <div class="brand-subtitle">Secure &bull; Fast &bull; Native Desktop</div>
        </div>

        <form class="search-box" onsubmit="event.preventDefault(); var v = document.getElementById('q').value; window.location.href = v.includes('.') && !v.includes(' ') ? (v.includes('://') ? v : 'https://' + v) : 'https://www.google.com/search?q=' + encodeURIComponent(v);">
            <input type="text" id="q" class="search-input" placeholder="Search the web or type a URL..." autofocus>
        </form>

        <div class="shortcuts">
            <a href="https://example.com/" class="card">
                <div class="card-icon">&#127760;</div>
                <div class="card-title">Example Domain</div>
            </a>
            <a href="http://http.badssl.com/" class="card">
                <div class="card-icon">&#9888;</div>
                <div class="card-title">HTTP Insecure Test</div>
            </a>
            <a href="https://www.google.com/" class="card">
                <div class="card-icon">&#128077;</div>
                <div class="card-title">Google Search</div>
            </a>
            <a href="https://github.com/" class="card">
                <div class="card-icon">&#128187;</div>
                <div class="card-title">GitHub</div>
            </a>
            <a href="https://archlinux.org/" class="card">
                <div class="card-icon">&#128039;</div>
                <div class="card-title">Arch Linux</div>
            </a>
            <a href="https://www.wikipedia.org/" class="card">
                <div class="card-icon">&#128218;</div>
                <div class="card-title">Wikipedia</div>
            </a>
        </div>

        <div class="badge-container">
            <div class="badge">&#128737; Sandbox: Active (SUID Root)</div>
            <div class="badge">&#9889; Ozone Wayland Native</div>
            <div class="badge">&#9881; Multi-Process (9)</div>
        </div>
    </div>
</body>
</html>)raw";
}
