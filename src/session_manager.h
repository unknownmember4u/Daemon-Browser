#pragma once
#include <string>
#include <vector>

class SessionManager {
public:
    static SessionManager& GetInstance();

    void SaveSession(const std::vector<std::string>& urls);
    std::vector<std::string> LoadSession();

private:
    SessionManager() = default;
    std::string GetStoragePath();
};
