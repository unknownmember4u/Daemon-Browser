#pragma once
#include "include/cef_browser.h"
#include "include/cef_ssl_status.h"
#include <string>

enum class SecurityLevel {
    UNKNOWN,
    INSECURE,
    SECURE
};

struct SecurityInfo {
    SecurityLevel level = SecurityLevel::UNKNOWN;

    std::string origin;
    std::string scheme;

    bool is_secure_connection = false;
    bool certificate_available = false;
    bool certificate_error = false;

    bool tls_enabled = true;
    bool web_security_enabled = true;
    bool site_isolation_enabled = true;
    bool sandbox_expected = true;

    std::string certificate_status = "Not available";
    cef_cert_status_t cert_status_code = CERT_STATUS_NONE;

    // Backward compatibility helper
    bool has_certificate() const { return certificate_available; }

    bool operator==(const SecurityInfo& other) const {
        return level == other.level &&
               origin == other.origin &&
               scheme == other.scheme &&
               is_secure_connection == other.is_secure_connection &&
               certificate_available == other.certificate_available &&
               certificate_error == other.certificate_error &&
               certificate_status == other.certificate_status &&
               cert_status_code == other.cert_status_code;
    }

    bool operator!=(const SecurityInfo& other) const {
        return !(*this == other);
    }
};

class SecurityState {
public:
    static SecurityInfo ComputeFromBrowser(CefRefPtr<CefBrowser> browser, const CefString& current_url);
};
