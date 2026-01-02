//
// Created by DjMhel on 26/12/2025.
//

#ifndef FINALPROJECT2_ADMIN_H
#define FINALPROJECT2_ADMIN_H


#include <string>
#include <vector>
#include "globals.h"

class Admin {
public:
    std::string cipherPassword(std::string rawPass);
    std::vector<User> getAllUsers(const UserMap& users) const;
    void updateDescription(User &user, std::string newDesc);
    bool addConnection(User &user, std::string newFriend, const UserMap &allUsers);
    bool removeConnection(User &user, std::string friendToRemove);
    void toggleLock(User &user);
    void changePassword(User &user, std::string newPass);
    std::vector<Message> filterBySender(const std::vector<Message>& messages);
    std::vector<Message> filterByRecipient(const std::vector<Message>& messages);
    bool deleteMessage(std::vector<Message>& allMessages, const Message& target);
    void resetPasswordToDefault(User &user);
    void updateRealName(User &user, std::string newName);
    bool updateUsername(UserMap &users, std::string &targetUser, std::string newUsername);
};

bool isAdminLogin(const UserMap& users, const std::string& username, const std::string& password);

#endif //FINALPROJECT2_ADMIN_H