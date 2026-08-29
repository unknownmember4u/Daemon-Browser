#include "render_backend.h"
#include <cstdlib>
#include <cstring>
#include <string>

RenderBackend DetectRenderBackend(CefRefPtr<CefCommandLine> command_line) {
    if (command_line && command_line->HasSwitch("wayland-test")) {
        return RenderBackend::NativeWayland;
    }

    const char* wayland_display = std::getenv("WAYLAND_DISPLAY");
    const char* session_type = std::getenv("XDG_SESSION_TYPE");

    // Explicit X11 session takes precedence unless --wayland-test is specified
    if (session_type && std::strcmp(session_type, "x11") == 0) {
        return RenderBackend::Default;
    }

    bool has_wayland_session = (session_type && std::strcmp(session_type, "wayland") == 0);
    bool has_wayland_display = (wayland_display && std::strlen(wayland_display) > 0);

    if (has_wayland_session || has_wayland_display) {
        return RenderBackend::NativeWayland;
    }

    return RenderBackend::Default;
}

const char* RenderBackendName(RenderBackend backend) {
    switch (backend) {
        case RenderBackend::NativeWayland:
            return "NATIVE_WAYLAND";
        case RenderBackend::Default:
        default:
            return "DEFAULT";
    }
}
