#include "security_state.h"
#include "include/cef_parser.h"
#include "include/cef_ssl_info.h"

SecurityInfo SecurityState::ComputeFromBrowser(CefRefPtr<CefBrowser> browser, const CefString& current_url) {
    SecurityInfo info;

    if (current_url.empty()) {
        return info;
    }

    CefURLParts parts;
    if (CefParseURL(current_url, parts)) {
        info.scheme = CefString(&parts.scheme).ToString();
        info.origin = CefString(&parts.host).ToString();
    }

    if (info.scheme == "http") {
        info.level = SecurityLevel::INSECURE;
        info.certificate_status = "Not available (HTTP)";
        return info;
    } else if (info.scheme == "https") {
        info.level = SecurityLevel::SECURE; // Base assumption for https
    } else {
        info.level = SecurityLevel::UNKNOWN;
        info.certificate_status = "Unknown scheme";
        return info;
    }

    if (!browser || !browser->GetHost()) {
        return info;
    }

    CefRefPtr<CefNavigationEntry> entry = browser->GetHost()->GetVisibleNavigationEntry();
    if (!entry) {
        return info;
    }

    CefRefPtr<CefSSLStatus> ssl = entry->GetSSLStatus();
    if (ssl) {
        info.is_secure_connection = ssl->IsSecureConnection();
        info.certificate_available = (ssl->GetX509Certificate() != nullptr);
        info.cert_status_code = ssl->GetCertStatus();
        info.certificate_error = CefIsCertStatusError(info.cert_status_code);

        if (info.certificate_error) {
            info.certificate_status = "Certificate Error";
            info.level = SecurityLevel::INSECURE;
        } else if (info.certificate_available) {
            info.certificate_status = "Valid / OK";
        } else {
            info.certificate_status = "No Certificate";
        }

        if (!info.is_secure_connection) {
            info.level = SecurityLevel::INSECURE;
        }
    }

    return info;
}
