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
        info.scheme = CefString(&parts.scheme);
        info.origin = CefString(&parts.host);
    }
    
    if (info.scheme == "http") {
        info.level = SecurityLevel::INSECURE;
        return info;
    } else if (info.scheme == "https") {
        info.level = SecurityLevel::SECURE; // Base assumption for https
    } else {
        info.level = SecurityLevel::UNKNOWN;
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
        info.has_certificate = (ssl->GetX509Certificate() != nullptr);
        info.cert_status = ssl->GetCertStatus();
        
        if (!ssl->IsSecureConnection() || CefIsCertStatusError(info.cert_status)) {
            info.level = SecurityLevel::INSECURE;
        }
    }
    
    return info;
}
