//
// Created by DjMhel on 20/12/2025.
//

#include "file_handler.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

const string USER_FILE = "C:\\Users\\DjMhel\\Documents\\finalProject2\\user.txt";
const string MSG_FILE = "C:\\Users\\DjMhel\\Documents\\finalProject2\\message.txt";

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
    users.clear();
    ifstream file(USER_FILE);

    if (!file.is_open()) {
        cout << "[Warning] Could not open users database at: " << USER_FILE << "\n";
        return;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string segment;
        vector<string> tokens;

        // Split by pipe '|'
        while(getline(ss, segment, '|')) {
            tokens.push_back(segment);
        }

        // We need at least 6 columns (up to Answer) to be valid basics
        if (tokens.size() >= 6) {
            User u;
            u.username       = tokens[0];
            u.password       = tokens[1];
            u.realName       = tokens[2];
            u.description    = tokens[3];

            try { u.questionIndex = stoi(tokens[4]); }
            catch (...) { u.questionIndex = 0; }

            u.securityAnswer = tokens[5];

            // --- FIX START: READ IN CORRECT ORDER (MATCHING SAVE) ---

            // 1. READ CONNECTIONS FIRST (Token 6)
            // (Your save function puts connections right after security answer)
            if (tokens.size() > 6) {
                string connStr = tokens[6];
                if (connStr != " " && !connStr.empty() && connStr != "0") {
                    stringstream css(connStr);
                    string friendName;
                    while(getline(css, friendName, ',')) {
                        // FILTER: Only add if valid and NOT "0"
                        if(!friendName.empty() && friendName != "0") {
                            u.connections.push_back(friendName);
                        }
                    }
                }
            }

            // 2. READ LOCKED STATUS LAST (Token 7)
            // (Your save function puts this at the very end)
            if (tokens.size() > 7) {
                try {
                    u.isLocked = (stoi(tokens[7]) == 1);
                }
                catch (...) { u.isLocked = false; }
            } else {
                u.isLocked = false; // Default if column missing
            }

            // --- FIX END ---

            users[u.username] = u;
        }
    }
    file.close();
    cout << "\n [System] Loaded " << users.size() << " users.\n";
}

void saveUsers(const UserMap& users) {
    ofstream file(USER_FILE);// Or use USER_FILE if you have that defined
    if (!file.is_open()) return;

    for (const auto& entry : users) {
        const User& u = entry.second;

        // 1. Write the Fixed Columns (Username -> Security Answer)
        file << u.username << "|"
             << u.password << "|"
             << u.realName << "|"
             << u.description << "|"
             << u.questionIndex << "|"     // <--- ADDED THIS (was missing)
             << u.securityAnswer << "|";

        // 2. Write Connections (Column 7)
        // We write them as a comma-separated list INSIDE the pipe
        if (u.connections.empty()) {
            file << " "; // Write a space if no friends (keeps the column valid)
        } else {
            for (size_t i = 0; i < u.connections.size(); i++) {
                file << u.connections[i];
                if (i < u.connections.size() - 1) {
                    file << ","; // Comma between friends
                }
            }
        }

        // 3. Write Lock Status (Column 8)
        // We put this last to be safe
        file << "|" << (u.isLocked ? "1" : "0") << endl;
    }

    file.close();
}

// (Keep loadMessages/saveMessages the same as before, they use Vectors)
// I will omit them here to save space, but you need them!
void loadMessages(vector<Message>& messages) {
    messages.clear();
    ifstream file(MSG_FILE);

    if (!file.is_open()) {
        cout << "[Warning] Could not open messages file (" << MSG_FILE << ")\n";
        return;
    }

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
    cout << "[System] Loaded " << messages.size() << " messages.\n";
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
    cout << "\n [System] Messages saved successfully.\n";
}