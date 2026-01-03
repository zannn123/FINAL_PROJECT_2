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
    // 1. Check if multiple recipients (Fan-Out Logic)
    if (newMsg.recipient.find(',') != string::npos) {
        vector<string> recipients = splitRecipients(newMsg.recipient);

        // Create a separate copy for EACH person
        for (const string& r : recipients) {
            Message copy = newMsg;
            copy.recipient = r; // Set specific owner

            // Calculate who else is in the group (Everyone except 'r')
            vector<string> others;
            for (const string& other : recipients) {
                if (other != r) others.push_back(other);
            }

            // Add {GRP} tag so the UI knows it's a group chat
            if (!others.empty()) {
                // Only wrap it if it's not already wrapped!
                if (copy.content.find("{GRP:") == string::npos) {
                    copy.content = formatGroupMessage(newMsg.content, others);
                }
            }

            allMessages.push_back(copy);
        }
    } else {
        // Normal Single Message
        allMessages.push_back(newMsg);
    }

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

string getCleanContent(const string& rawContent) {
    // Check if it starts with the Group Tag
    if (rawContent.substr(0, 5) == "{GRP:") {
        size_t endPos = rawContent.find('}');
        if (endPos != string::npos) {
            // Return everything AFTER the '}'
            string clean = rawContent.substr(endPos + 1);

            // Optional: Remove a leading space if it exists
            if (!clean.empty() && clean[0] == ' ') {
                clean.erase(0, 1);
            }
            return clean;
        }
    }
    // If not a group message, return valid content as is
    return rawContent;
}

// 2. UNWRAP: Get the list of names
vector<string> getGroupParticipants(const string& rawContent) {
    vector<string> participants;

    if (rawContent.substr(0, 5) == "{GRP:") {
        size_t endPos = rawContent.find('}');
        if (endPos != string::npos) {
            // Extract the part between {GRP: and }
            string namesList = rawContent.substr(5, endPos - 5);

            stringstream ss(namesList);
            string name;
            // Split by comma
            while (getline(ss, name, ',')) {
                if (!name.empty()) participants.push_back(name);
            }
        }
    }
    return participants;
}

// 3. WRAP: Create the tagged string for saving
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
        // 1. Must NOT be an Announcement
        if (msg.isAnnouncement) continue;

        // 2. Must be sent TO me (Directly)
        bool isDirect = (msg.recipient == currentUser.username);

        // 3. OR sent TO me via Group (Recipient field might be one person, but I might be in the GRP tag)
        // Note: For your current file structure, fan-out means I get my own line.
        // So 'msg.recipient == currentUser.username' is actually sufficient!

        if (isDirect) {
            inbox.push_back(msg);
        }
    }
    // Sort? The file is usually chronological, so we might want to reverse it to see newest first.
    reverse(inbox.begin(), inbox.end());
    return inbox;
}

vector<Message> getSentMessages(const User& currentUser, const vector<Message>& allMessages) {
    vector<Message> sent;
    for (const auto& msg : allMessages) {
        // 1. Must NOT be an Announcement
        if (msg.isAnnouncement) continue;

        // 2. Must be sent BY me
        if (msg.sender == currentUser.username) {
            sent.push_back(msg);
        }
    }
    // Sort newest first
    reverse(sent.begin(), sent.end());
    return sent;
}

vector<Message> getConversationThread(const User& currentUser, const string& subject, const vector<Message>& allMessages) {
    vector<Message> thread;

    // 1. clean the subject (Remove "RE: " if it exists to find the original)
    string cleanSubject = subject;
    if (cleanSubject.substr(0, 4) == "RE: ") {
        cleanSubject = cleanSubject.substr(4);
    }

    for (const auto& msg : allMessages) {
        if (msg.isAnnouncement) continue; // Skip public posts

        // 2. Check if Subject Matches (Look for the root topic)
        // We check if msg.subject CONTAINS the cleanSubject
        if (msg.subject.find(cleanSubject) != string::npos) {

            // 3. Security: Am I involved? (Sender OR Recipient)
            // (In a group message, I might be inside the {GRP} tag, but usually I am recipient in my own file copy)
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

    // 3. Update the global UserMap with my changes
    // (Since currentUser is a reference, it might already be updated, but this ensures safety)
    users[currentUser.username] = currentUser;

    // 4. Save Changes to Disk (Permanent)
    saveUsers(users);
}

vector<User> serviceSearchUsers(const UserMap& users, const string& query) {
    vector<User> results;
    string q = query;
    transform(q.begin(), q.end(), q.begin(), ::tolower);

    for (const auto& entry : users) {
        User u = entry.second;

        // 1. Filter out Admin
        if (u.username == "admin") continue;

        // 2. If Query is Empty, Return EVERYONE
        if (q.empty()) {
            results.push_back(u);
            continue;
        }

        // 3. Otherwise, Check for Match
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
    // 1. Update the Object in Memory
    currentUser.realName = newName;
    currentUser.description = newBio;

    // 2. Update the Global Map
    users[currentUser.username] = currentUser;

    // 3. Save to Disk
    saveUsers(users);
}

void serviceChangePassword(UserMap& users, User& currentUser, const string& newPass) {
    // 1. Update the Object
    currentUser.password = newPass;
    currentUser.isLocked = false; // Safety unlock

    // 2. Update the Global Map
    users[currentUser.username] = currentUser;

    // 3. Save to Disk
    saveUsers(users);
}

string serviceUpdateUsername(UserMap& users, User& currentUser, const string& newUsername) {
    // 1. Validation
    if (newUsername.empty()) return "Username cannot be empty.";
    if (newUsername.length() < 3) return "Username too short.";

    // 2. Check if Taken (Collision Detection)
    if (users.find(newUsername) != users.end()) {
        return "Username already exists!";
    }

    // 3. Prepare Data Migration
    string oldUsername = currentUser.username;

    // Create new entry with new key
    User newUser = currentUser;
    newUser.username = newUsername; // Update internal field

    // 4. Update Database (Swap Keys)
    users.erase(oldUsername);       // Delete old key
    users[newUsername] = newUser;   // Insert new key

    // 5. Update Local Object (So the UI doesn't break)
    currentUser.username = newUsername;

    // 6. Fix Friend Lists (Critical: Update everyone else's connections)
    for (auto& entry : users) {
        User& u = entry.second;
        for (string& friendName : u.connections) {
            if (friendName == oldUsername) {
                friendName = newUsername; // Replace old name with new
            }
        }
    }

    // 7. Save Everything
    saveUsers(users);

    return "Success";
}

void serviceUpdateSecurityAnswer(UserMap& users, User& currentUser, int newQuestionIdx, const string& newAnswer) {
    currentUser.questionIndex = newQuestionIdx;
    currentUser.securityAnswer = newAnswer;

    // Update Global Map & Save
    users[currentUser.username] = currentUser;
    saveUsers(users);
}