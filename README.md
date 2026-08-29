# Daemon Browser

CEF PROOF OF CONCEPT

## Purpose
Daemon Browser is a lightweight, extremely fast, secure, privacy-focused, cross-platform browser designed primarily for security-conscious developers, security researchers, penetration testers, bug bounty researchers, CTF users, and technical users.

This is a **CEF PROOF OF CONCEPT** to verify that the Chromium Embedded Framework (CEF) works in this environment.

## Dependencies
- CMake 3.10+
- Modern C++ compiler supporting C++20
- CEF Minimal Distribution (Linux x86_64)

## How to Configure CEF Path
Extract the CEF minimal distribution to `third_party/cef` in the project root.
Alternatively, pass `-DCEF_ROOT=/path/to/cef` when configuring CMake.

## How to Build
```bash
mkdir build
cd build
cmake ..
make -j4
```

## How to Run
```bash
cd build
./daemon_browser
```

## Known Linux/Wayland Limitations
- The application currently defaults to XWayland because native Ozone Wayland flags (`--ozone-platform-hint=auto`) are not explicitly forced.
- Full sandbox verification requires the `chrome-sandbox` binary to have SUID root permissions.
