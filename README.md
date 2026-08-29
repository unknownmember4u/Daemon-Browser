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

## Linux Sandbox Requirements & Deployment

Chromium's Linux sandbox architecture requires a helper binary (`chrome-sandbox`) to isolate renderer processes using namespaces and chroots.

### Sandbox Deployment
1. During build (`cmake .. && make`), `chrome-sandbox` is automatically copied from the CEF distribution into the `build/` directory alongside `daemon_browser`.
2. For Chromium to enforce the sandbox, `chrome-sandbox` must be owned by `root` and have SUID permissions (`4755` / `u+s`).

### Post-Build Setup (Requires Root Privileges)
To enable and enforce full sandbox isolation:
```bash
sudo chown root:root build/chrome-sandbox
sudo chmod 4755 build/chrome-sandbox
```

### Verifying Sandbox Status
We distinguish between **"Sandbox binary present"** (file exists in runtime folder) and **"Sandbox actually verified"** (binary is root-owned with `4755` SUID bit set).

Run the automated verification script:
```bash
./tools/check_sandbox.sh
```
- Outputs `SANDBOX_STATUS: VERIFIED` (exit code 0) if root ownership and SUID permissions are correctly set.
- Outputs `SANDBOX_STATUS: NOT_VERIFIED` (exit code 1) if permission conditions are not met.

## Rendering Backend & Wayland Support

Daemon Browser supports both default rendering (X11/XWayland) and native Wayland rendering via Chromium's Ozone abstraction layer.

### Default Backend
By default, running `./daemon_browser` uses Chromium's default Linux rendering windowing system (X11 / XWayland).

### Testing Native Wayland Backend
To launch Daemon Browser in native Wayland mode using Ozone auto-hinting:
```bash
cd build
./daemon_browser --wayland-test
# or
./daemon_browser --use-wayland
```
When enabled, CEF receives `--ozone-platform-hint=auto`, automatically configuring the GPU and renderer subprocesses for Wayland rendering (`--ozone-platform=wayland`).
