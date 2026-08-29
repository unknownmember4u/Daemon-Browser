#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

RUNTIME_DIR="${PROJECT_ROOT}/build"
SANDBOX_BIN="${RUNTIME_DIR}/chrome-sandbox"

echo "=== DAEMON BROWSER SANDBOX INSTALLER ==="

if [[ $EUID -ne 0 ]]; then
    echo "Error: This installation script must be run with root privileges."
    echo "Please execute: sudo $0"
    exit 1
fi

if [[ ! -f "$SANDBOX_BIN" ]]; then
    echo "Error: chrome-sandbox binary not found at $SANDBOX_BIN."
    echo "Please build the project first (e.g., cd build && cmake .. && make)."
    exit 1
fi

if [[ ! -x "$SANDBOX_BIN" ]]; then
    echo "Error: $SANDBOX_BIN is not an executable file."
    exit 1
fi

echo "Target Sandbox helper: $SANDBOX_BIN"
echo "Required owner: root:root"
echo "Required permissions: 4755 (u+s)"
echo "Configuring permissions..."

chown root:root "$SANDBOX_BIN"
chmod 4755 "$SANDBOX_BIN"

# Immediate verification
OWNER=$(stat -c "%U" "$SANDBOX_BIN")
GROUP=$(stat -c "%G" "$SANDBOX_BIN")
PERMS_OCTAL=$(stat -c "%a" "$SANDBOX_BIN")
PERMS_HUMAN=$(stat -c "%A" "$SANDBOX_BIN")

if [[ "$OWNER" == "root" ]] && [[ "$GROUP" == "root" ]] && [[ "$PERMS_OCTAL" == "4755" ]]; then
    echo "SUCCESS: $SANDBOX_BIN is now root-owned (root:root) with mode 4755 ($PERMS_HUMAN)."
    echo "SANDBOX_INSTALLATION: SUCCESSFUL"
    exit 0
else
    echo "Error: Permission verification failed after chown/chmod."
    echo "Current state: $OWNER:$GROUP mode $PERMS_OCTAL ($PERMS_HUMAN)"
    exit 1
fi
