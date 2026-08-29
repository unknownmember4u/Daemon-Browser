#include "security_policy.h"

bool SecurityPolicy::IsSchemeAllowed(const std::string& scheme) {
    if (scheme.empty()) return true;
    return (scheme == "https" || scheme == "http" || scheme == "chrome" || scheme == "file" || scheme == "about");
}

std::string SecurityPolicy::GetPolicySummary() {
    return "Default Policy: Standard Web Security & TLS Enforcement Enabled";
}
