//
// Created by DjMhel on 20/12/2025.
//

#ifndef FINAL_PROJECT_2_GLOBALS_H
#define FINAL_PROJECT_2_GLOBALS_H

#include <string>
#include <vector>
#include <unordered_map> // <--- THIS IS THE HASHING LIBRARY

using namespace std;

struct User {
    string username;
    string password;
    string realName;
    string description;
    string securityAnswer;
    bool isLocked;
    vector<string> connections; // List of friends
};

struct Message {
    string sender;
    string recipient;
    string subject;
    string content;
    bool isAnnouncement;
};

// --- THE HASH MAP DEFINITION ---
// Instead of a Vector, we define a Map.
// Key (string) = Username
// Value (User) = The User Data
using UserMap = unordered_map<string, User>;

#endif //FINAL_PROJECT_2_GLOBALS_H