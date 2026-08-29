#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

RUNTIME_DIR="${PROJECT_ROOT}/build"
SANDBOX_BIN="${RUNTIME_DIR}/chrome-sandbox"

echo "=== DAEMON BROWSER SANDBOX CHECK ==="

# 1. Expected runtime path & directory
if [[ ! -d "$RUNTIME_DIR" ]]; then
    echo "Error: Runtime directory $RUNTIME_DIR does not exist. Build the project first."
    echo "SANDBOX_STATUS: NOT_VERIFIED"
    exit 1
fi

# 2. File exists & regular file (not symlink)
if [[ ! -f "$SANDBOX_BIN" ]] || [[ -L "$SANDBOX_BIN" ]]; then
    echo "Error: chrome-sandbox binary not found or is a symlink at $SANDBOX_BIN."
    echo "SANDBOX_STATUS: NOT_VERIFIED"
    exit 1
fi

# 3. Executable
if [[ ! -x "$SANDBOX_BIN" ]]; then
    echo "Error: $SANDBOX_BIN is not executable."
    echo "SANDBOX_STATUS: NOT_VERIFIED"
    exit 1
fi

OWNER=$(stat -c "%U" "$SANDBOX_BIN")
GROUP=$(stat -c "%G" "$SANDBOX_BIN")
PERMS_OCTAL=$(stat -c "%a" "$SANDBOX_BIN")
PERMS_HUMAN=$(stat -c "%A" "$SANDBOX_BIN")

# 6. SUID bit present
IS_SUID=false
if [[ "$PERMS_HUMAN" =~ ^...[sS] ]]; then
    IS_SUID=true
fi

echo "Path: $SANDBOX_BIN"
echo "Owner: $OWNER"
echo "Group: $GROUP"
echo "Permissions (Octal): $PERMS_OCTAL"
echo "Permissions (Human): $PERMS_HUMAN"
echo "SUID Bit Set: $IS_SUID"

echo "-----------------------------------"

# 4. Owner root, 5. Group root, 7. Mode 4755, SUID bit
if [[ "$OWNER" == "root" ]] && [[ "$GROUP" == "root" ]] && [[ "$PERMS_OCTAL" == "4755" ]] && [[ "$IS_SUID" == "true" ]]; then
    echo "SANDBOX_STATUS: VERIFIED"
    exit 0
else
    echo "SANDBOX_STATUS: NOT_VERIFIED"
    if [[ "$OWNER" != "root" ]]; then
        echo "Reason: Binary is owned by '$OWNER', expected 'root'."
    fi
    if [[ "$GROUP" != "root" ]]; then
        echo "Reason: Binary group is '$GROUP', expected 'root'."
    fi
    if [[ "$PERMS_OCTAL" != "4755" ]]; then
        echo "Reason: Octal permissions are '$PERMS_OCTAL', expected '4755'."
    fi
    if [[ "$IS_SUID" != "true" ]]; then
        echo "Reason: SUID bit is not set (expected mode 4755 or u+s)."
    fi
    exit 1
fi
