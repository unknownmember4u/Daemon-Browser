#!/usr/bin/env python3
import subprocess
import os
import sys

def run_test_case(name, env_override, extra_args, expected_mode):
    env = os.environ.copy()
    # Apply environment overrides
    for k, v in env_override.items():
        if v is None:
            env.pop(k, None)
        else:
            env[k] = v

    cmd = ['./daemon_browser', '--url=https://example.com/'] + extra_args
    proc = subprocess.Popen(cmd, cwd='build', env=env, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

    logged_mode = None
    try:
        start_time = proc.stdout.readline # dummy
        for _ in range(50):
            line = proc.stdout.readline()
            if not line:
                break
            if '[RENDER_BACKEND]' in line:
                logged_mode = line.strip().split()[-1]
                break
    finally:
        proc.terminate()
        try:
            proc.wait(timeout=3)
        except subprocess.TimeoutExpired:
            proc.kill()

    passed = (logged_mode == expected_mode)
    status = "PASS" if passed else "FAIL"
    print(f"Test Case [{name}]: Expected={expected_mode}, Got={logged_mode} -> {status}")
    return passed

def main():
    print("=== BACKEND SELECTION AUTOMATED TEST SUITE ===")
    results = []

    # Case A: Normal Wayland environment
    results.append(run_test_case("A: Wayland Session", 
                                 {"WAYLAND_DISPLAY": "wayland-0", "XDG_SESSION_TYPE": "wayland"}, 
                                 [], "NATIVE_WAYLAND"))

    # Case B: X11 environment
    results.append(run_test_case("B: X11 Session", 
                                 {"WAYLAND_DISPLAY": None, "XDG_SESSION_TYPE": "x11", "DISPLAY": ":0"}, 
                                 [], "DEFAULT"))

    # Case C: Wayland display available, session type missing
    results.append(run_test_case("C: Wayland Display Only", 
                                 {"WAYLAND_DISPLAY": "wayland-0", "XDG_SESSION_TYPE": None}, 
                                 [], "NATIVE_WAYLAND"))

    # Case D: No Wayland variables
    results.append(run_test_case("D: No Wayland Env", 
                                 {"WAYLAND_DISPLAY": None, "XDG_SESSION_TYPE": None}, 
                                 [], "DEFAULT"))

    # Case E: Explicit --wayland-test
    results.append(run_test_case("E: Explicit --wayland-test", 
                                 {"WAYLAND_DISPLAY": None, "XDG_SESSION_TYPE": "x11"}, 
                                 ["--wayland-test"], "NATIVE_WAYLAND"))

    all_passed = all(results)
    print("-----------------------------------------------")
    if all_passed:
        print("ALL BACKEND SELECTION TESTS PASSED!")
        sys.exit(0)
    else:
        print("SOME TESTS FAILED!")
        sys.exit(1)

if __name__ == '__main__':
    main()
