//
// Created by DjMhel on 20/12/2025.
//

#include "file_handler.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

const string USER_FILE = "user.txt";
const string MSG_FILE = "message.txt";

// Helper to split commas (for friend lists)
vector<string> split(string s, char delimiter) {
    vector<string> tokens;
    string token;
    stringstream ss(s);
    while (getline(ss, token, delimiter)) {
        if (!token.empty()) tokens.push_back(token);
    }
    return tokens;
}

void loadUsers(UserMap& users) {
    users.clear(); // Clear old data
    ifstream file(USER_FILE);

    if (!file.is_open()) {
        cout << "[Warning] Could not open users database.\n";
        return;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        User u;

        // Parse the line just like before...
        getline(ss, u.username, '|');
        getline(ss, u.password, '|');
        getline(ss, u.realName, '|');
        getline(ss, u.description, '|');
        getline(ss, u.securityAnswer, '|');

        string lockedStr;
        getline(ss, lockedStr, '|');
        u.isLocked = (lockedStr == "1");

        string connStr;
        getline(ss, connStr);
        u.connections = split(connStr, ',');

        // --- HASHING HAPPENS HERE ---
        // We insert the user into the map using their username as the Key.
        // This calculates the hash instantly.
        users[u.username] = u;
    }
    file.close();
    cout << "[System] Loaded " << users.size() << " users into Hash Map.\n";
}

void saveUsers(const UserMap& users) {
    ofstream file(USER_FILE);
    if (!file.is_open()) return;

    // Iterating through a Map is slightly different than a Vector
    // 'entry.first' is the Username (Key)
    // 'entry.second' is the User Struct (Value)
    for (const auto& entry : users) {
        const User& u = entry.second; // Get the user object

        file << u.username << "|"
             << u.password << "|"
             << u.realName << "|"
             << u.description << "|"
             << u.securityAnswer << "|"
             << (u.isLocked ? "1" : "0") << "|";

        for (size_t i = 0; i < u.connections.size(); i++) {
            file << u.connections[i];
            if (i < u.connections.size() - 1) file << ",";
        }
        file << "\n";
    }
    file.close();
}

// (Keep loadMessages/saveMessages the same as before, they use Vectors)
// I will omit them here to save space, but you need them!
void loadMessages(vector<Message>& messages) {
    messages.clear();
    ifstream file(MSG_FILE);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        Message m;

        getline(ss, m.sender, '|');
        getline(ss, m.recipient, '|');
        getline(ss, m.subject, '|');
        getline(ss, m.content, '|');

        string annStr;
        getline(ss, annStr);
        m.isAnnouncement = (annStr == "1");

        messages.push_back(m);
    }
    file.close();
}


void saveMessages(const vector<Message>& messages) {
    ofstream file(MSG_FILE);
    if (!file.is_open()) {
        cout << "[Error] Could not open " << MSG_FILE << " for writing.\n";
        return;
    }

    for (const auto& m : messages) {
        file << m.sender << "|"
             << m.recipient << "|"
             << m.subject << "|"
             << m.content << "|"
             << (m.isAnnouncement ? "1" : "0") << "\n";
    }
    file.close();
    cout << "[System] Messages saved successfully.\n";
}