//
// Created by DjMhel on 27/12/2025.
//

#include "user.h"
#include <algorithm>
#include "globals.h"
#include <iostream>
#include <sstream>
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
    user.password = newPass;
    user.resetRequested = false;
    user.isLocked = false;
}

void saveNewUserMessages(vector<Message>& allMessages, const vector<Message>& newMessages) {
    if (newMessages.empty()) {
        return;
    }
    allMessages.insert(allMessages.end(), newMessages.begin(), newMessages.end());
    saveMessages(allMessages);
}

vector<string> splitRecipients(string s) {
    vector<string> tokens;
    string token;
    stringstream ss(s);
    while (getline(ss, token, ',')) {
        // Remove spaces
        token.erase(remove(token.begin(), token.end(), ' '), token.end());
        if (!token.empty()) tokens.push_back(token);
    }
    return tokens;
}

void serviceAddMessage(vector<Message>& allMessages, const Message& newMsg) {
    if (newMsg.recipient.find(',') != string::npos) {
        vector<string> recipients = splitRecipients(newMsg.recipient);

        for (const string& r : recipients) {
            Message copy = newMsg;
            copy.recipient = r;

            vector<string> others;
            for (const string& other : recipients) {
                if (other != r) others.push_back(other);
            }
            if (!others.empty()) {
                if (copy.content.find("{GRP:") == string::npos) {
                    copy.content = formatGroupMessage(newMsg.content, others);
                }
            }

            allMessages.push_back(copy);
        }
    } else {
        allMessages.push_back(newMsg);
    }
    saveMessages(allMessages);
}

void serviceDeleteMessage(vector<Message>& allMessages, const Message& target) {
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
    saveMessages(allMessages);
}

string getCleanContent(const string& rawContent) {
    if (rawContent.substr(0, 5) == "{GRP:") {
        size_t endPos = rawContent.find('}');
        if (endPos != string::npos) {
            string clean = rawContent.substr(endPos + 1);

            if (!clean.empty() && clean[0] == ' ') {
                clean.erase(0, 1);
            }
            return clean;
        }
    }
    // If not a group message, return valid content as is
    return rawContent;
}

vector<string> getGroupParticipants(const string& rawContent) {
    vector<string> participants;
    if (rawContent.substr(0, 5) == "{GRP:") {
        size_t endPos = rawContent.find('}');
        if (endPos != string::npos) {
            string namesList = rawContent.substr(5, endPos - 5);
            stringstream ss(namesList);
            string name;
            while (getline(ss, name, ',')) {
                if (!name.empty()) participants.push_back(name);
            }
        }
    }
    return participants;
}

string formatGroupMessage(const string& realMessage, const vector<string>& participants) {
    if (participants.empty()) return realMessage;
    string result = "{GRP:";
    for (size_t i = 0; i < participants.size(); i++) {
        result += participants[i];
        if (i < participants.size() - 1) {
            result += ",";
        }
    }
    result += "} " + realMessage;
    return result;
}

vector<Message> getInboxMessages(const User& currentUser, const vector<Message>& allMessages) {
    vector<Message> inbox;
    for (const auto& msg : allMessages) {
        if (msg.isAnnouncement) continue;
        bool isDirect = (msg.recipient == currentUser.username);
        if (isDirect) {
            inbox.push_back(msg);
        }
    }
    reverse(inbox.begin(), inbox.end());
    return inbox;
}

vector<Message> getSentMessages(const User& currentUser, const vector<Message>& allMessages) {
    vector<Message> sent;
    for (const auto& msg : allMessages) {
        if (msg.isAnnouncement) continue;
        if (msg.sender == currentUser.username) {
            sent.push_back(msg);
        }
    }
    reverse(sent.begin(), sent.end());
    return sent;
}

vector<Message> getConversationThread(const User& currentUser, const string& subject, const vector<Message>& allMessages) {
    vector<Message> thread;
    string cleanSubject = subject;
    if (cleanSubject.substr(0, 4) == "RE: ") {
        cleanSubject = cleanSubject.substr(4);
    }

    for (const auto& msg : allMessages) {
        if (msg.isAnnouncement) continue; // Skip public posts
        if (msg.subject.find(cleanSubject) != string::npos) {
            bool involved = (msg.sender == currentUser.username || msg.recipient == currentUser.username);

            if (involved) {
                thread.push_back(msg);
            }
        }
    }
    return thread;
}

void removeStringFromVector(vector<string>& vec, const string& target) {
    vec.erase(remove(vec.begin(), vec.end(), target), vec.end());
}

void serviceRemoveFriend(UserMap& users, User& currentUser, const string& targetUsername) {
    removeStringFromVector(currentUser.connections, targetUsername);
    if (users.find(targetUsername) != users.end()) {
        removeStringFromVector(users[targetUsername].connections, currentUser.username);
    }
    users[currentUser.username] = currentUser;
    saveUsers(users);
}

vector<User> serviceSearchUsers(const UserMap& users, const string& query) {
    vector<User> results;
    string q = query;
    transform(q.begin(), q.end(), q.begin(), ::tolower);

    for (const auto& entry : users) {
        User u = entry.second;
        if (u.username == "admin") continue;
        if (q.empty()) {
            results.push_back(u);
            continue;
        }
        string lowUser = u.username;
        string lowName = u.realName;
        transform(lowUser.begin(), lowUser.end(), lowUser.begin(), ::tolower);
        transform(lowName.begin(), lowName.end(), lowName.begin(), ::tolower);

        if (lowUser.find(q) != string::npos || lowName.find(q) != string::npos) {
            results.push_back(u);
        }
    }
    return results;
}

string serviceAddConnection(UserMap& users, User& currentUser, const string& targetUsername) {
    if (targetUsername == currentUser.username) return "You cannot add yourself.";

    for (const string& friendName : currentUser.connections) {
        if (friendName == targetUsername) return "User is already in your connections.";
    }

    if (users.find(targetUsername) == users.end()) return "User not found.";
    currentUser.connections.push_back(targetUsername);
    users[currentUser.username] = currentUser;
    saveUsers(users);
    return "Success";
}

void serviceUpdateProfile(UserMap& users, User& currentUser, const string& newName, const string& newBio) {
    currentUser.realName = newName;
    currentUser.description = newBio;
    users[currentUser.username] = currentUser;
    saveUsers(users);
}

void serviceChangePassword(UserMap& users, User& currentUser, const string& newPass) {
    currentUser.password = newPass;
    currentUser.isLocked = false; // Safety unlock
    users[currentUser.username] = currentUser;
    saveUsers(users);
}

string serviceUpdateUsername(UserMap& users, User& currentUser, const string& newUsername) {
    if (newUsername.empty()) return "Username cannot be empty.";
    if (newUsername.length() < 3) return "Username too short.";
    if (users.find(newUsername) != users.end()) {
        return "Username already exists!";
    }
    string oldUsername = currentUser.username;
    User newUser = currentUser;
    newUser.username = newUsername;
    users.erase(oldUsername);
    users[newUsername] = newUser;
    currentUser.username = newUsername;
    for (auto& entry : users) {
        User& u = entry.second;
        for (string& friendName : u.connections) {
            if (friendName == oldUsername) {
                friendName = newUsername;
            }
        }
    }
    saveUsers(users);

    return "Success";
}

void serviceUpdateSecurityAnswer(UserMap& users, User& currentUser, int newQuestionIdx, const string& newAnswer) {
    currentUser.questionIndex = newQuestionIdx;
    currentUser.securityAnswer = newAnswer;
    users[currentUser.username] = currentUser;
    saveUsers(users);
}