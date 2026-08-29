#pragma once
#include "include/cef_browser.h"
#include "include/cef_ssl_status.h"

enum class SecurityLevel {
    UNKNOWN,
    INSECURE,
    SECURE
};

struct SecurityInfo {
    SecurityLevel level = SecurityLevel::UNKNOWN;
    CefString scheme;
    CefString origin;
    bool has_certificate = false;
    cef_cert_status_t cert_status = CERT_STATUS_NONE;
};

class SecurityState {
public:
    static SecurityInfo ComputeFromBrowser(CefRefPtr<CefBrowser> browser, const CefString& current_url);
};
