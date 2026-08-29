#pragma once
#include <string>

enum class SecurityPolicyMode {
    Default,
    RequireHTTPS
};

class SecurityPolicy {
public:
    static bool IsSchemeAllowed(const std::string& scheme);
    static std::string GetPolicySummary();
};
