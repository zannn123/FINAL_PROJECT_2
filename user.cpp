//
// Created by DjMhel on 27/12/2025.
//

#include "user.h"

#include <algorithm>

#include "globals.h"
#include <iostream>

#include "file_handler.h"
using namespace std;

bool isUserLoginValid(const UserMap& db, const string& username, const string& password) {
    if (db.find(username) == db.end()) {
        return false;
    }
    const User& user = db.at(username);

    if (user.isLocked) {
        return false;
    }
    if (user.password == password) {
        return true;
    }
    return false;
}

void performPasswordUpdate(User& user, std::string newPass) {
    user.password = newPass;      // Update the password
    user.resetRequested = false;  // Clear the "Reset Requested" flag
    user.isLocked = false;        // Ensure account is unlocked
}

void saveNewUserMessages(vector<Message>& allMessages, const vector<Message>& newMessages) {
    if (newMessages.empty()) {
        return;
    }
    allMessages.insert(allMessages.end(), newMessages.begin(), newMessages.end());
    saveMessages(allMessages);
}

void serviceAddMessage(vector<Message>& allMessages, const Message& newMsg) {
    // 1. Update RAM
    allMessages.push_back(newMsg);
    // 2. Update Disk
    saveMessages(allMessages);
}

void serviceDeleteMessage(vector<Message>& allMessages, const Message& target) {
    // 1. Delete Target
    auto it = remove_if(allMessages.begin(), allMessages.end(), [&](const Message& m) {
        return m.sender == target.sender && m.subject == target.subject && m.content == target.content;
    });
    allMessages.erase(it, allMessages.end());

    // 2. Delete Replies (Cascade)
    string genericTarget = "RE: " + target.subject;
    string specificTarget = "@" + target.sender;

    auto it2 = remove_if(allMessages.begin(), allMessages.end(), [&](const Message& m) {
        if (!m.isAnnouncement) return false;
        bool isReply = false;
        bool subjectMatch = (m.subject.find(target.subject) != string::npos) && (m.subject.find("RE:") != string::npos);
        bool userMatch = (m.subject.find(specificTarget) != string::npos);

        if (target.subject == "General") {
            if (subjectMatch && userMatch) isReply = true;
        } else {
            if (subjectMatch) isReply = true;
        }
        return isReply;
    });
    allMessages.erase(it2, allMessages.end());

    // 3. Update Disk
    saveMessages(allMessages);
}