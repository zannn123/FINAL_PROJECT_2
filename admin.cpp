//
// Created by DjMhel on 26/12/2025.
//

#include "admin.h"
#include "user.h"
#include <algorithm>
#include "globals.h"

using namespace std;

bool isAdminLogin(const UserMap& users, const string& username, const string& password) {
    if (users.count(username)) {
        const User& user = users.at(username);
        if (user.password == password && user.description == "ADMIN") {
            return true;
        }
    }
    return false;
}

string Admin::cipherPassword(string rawPass) {
    string ciphered = rawPass;
    for (int i = 0; i < (int)ciphered.length(); i++) {
        ciphered[i] = ciphered[i] + 3;
    }
    return ciphered;
}

std::vector<User> Admin::getAllUsers(const UserMap& users) const {
    std::vector<User> result;
    for (const auto& [_, user] : users) {
        result.push_back(user);
    }
    return result;
}

void Admin::updateDescription(User &user, string newDesc) {
    user.description = newDesc;
}

bool Admin::addConnection(User &user, string newFriend, const UserMap &allUsers) {
    if (user.username == newFriend) return false;

    if (allUsers.find(newFriend) == allUsers.end()) {
        return false;
    }
    for (const string &existingFriend : user.connections) {
        if (existingFriend == newFriend) return false;
    }
    user.connections.push_back(newFriend);
    return true;
}

bool Admin::removeConnection(User &user, string friendToRemove) {
    auto it = std::remove(user.connections.begin(), user.connections.end(), friendToRemove);
    if (it != user.connections.end()) {
        user.connections.erase(it, user.connections.end());
        return true;
    }
    return false;
}

void Admin::toggleLock(User &user) {
    user.isLocked = !user.isLocked;
}

void Admin::changePassword(User &user, string newPass) {
    user.password = newPass;
}

vector<Message> Admin::filterBySender(const vector<Message>& messages){
    vector<Message> sortedList = messages;
    sort(sortedList.begin(), sortedList.end(), [](const Message& a, const Message& b) {
        return a.sender < b.sender;
    });
    return sortedList;
}

vector<Message> Admin::filterByRecipient(const vector<Message>& messages) {
    vector<Message> sortedList = messages;
    sort(sortedList.begin(), sortedList.end(), [](const Message& a, const Message& b) {
        return a.recipient < b.recipient;
    });

    return sortedList;
}

bool Admin::deleteMessage(std::vector<Message>& allMessages, const Message& target) {
    for (auto it = allMessages.begin(); it != allMessages.end(); ++it) {

        if (it->sender == target.sender &&
            it->recipient == target.recipient &&
            it->subject == target.subject &&
            it->content == target.content) {
            allMessages.erase(it); // <--- This command removes it from the Vector (RAM)
            return true; // Success
            }
    }
    return false;
}

void Admin::resetPasswordToDefault(User &user) {
    user.password = "default123";
    user.resetRequested = false;
    user.isLocked = false;
}

void Admin::updateRealName(User &user, string newName) {
    if (!newName.empty()) {
        user.realName = newName;
    }
}

bool Admin::updateUsername(UserMap &users, string &targetUser, string newUsername) {
    if (newUsername.empty()) return false;
    if (users.find(newUsername) != users.end()) return false;

    User &oldUser = users[targetUser];
    User newUser = oldUser;
    newUser.username = newUsername;

    users[newUsername] = newUser;
    users.erase(targetUser);

    string oldName = targetUser;
    for (auto& entry : users) {
        User& u = entry.second;
        for (auto& conn : u.connections) {
            if (conn == oldName) conn = newUsername;
        }
    }

    targetUser = newUsername;
    return true;
}
