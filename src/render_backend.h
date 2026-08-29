#pragma once
#include "include/cef_command_line.h"

enum class RenderBackend {
    Default,
    NativeWayland
};

RenderBackend DetectRenderBackend(CefRefPtr<CefCommandLine> command_line);
const char* RenderBackendName(RenderBackend backend);
