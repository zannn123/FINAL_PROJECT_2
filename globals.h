#ifndef FINAL_PROJECT_2_GLOBALS_H
#define FINAL_PROJECT_2_GLOBALS_H

#include <string>
#include <vector>
#include <unordered_map>

// REMOVED "using namespace std;" to fix errors.
// We use std::string, std::vector, etc.

struct User {
    std::string username;
    std::string password;
    std::string realName;
    std::string description;
    std::string securityAnswer;
    bool isLocked;
    std::vector<std::string> connections; // List of friends
};

struct Message {
    std::string sender;
    std::string recipient;
    std::string subject;
    std::string content;
    bool isAnnouncement;
};

// Key = Username, Value = User Data
using UserMap = std::unordered_map<std::string, User>;

#endif //FINAL_PROJECT_2_GLOBALS_H