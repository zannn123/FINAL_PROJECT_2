//
// Created by DjMhel on 27/12/2025.
//

#ifndef FINALPROJECT2_USER_H
#define FINALPROJECT2_USER_H
#include <string>
#include "globals.h"

bool isUserLoginValid(const UserMap& db, const std::string& username, const std::string& password);
void performPasswordUpdate(User& user, std::string newPass);
void saveNewUserMessages(std::vector<Message>& allMessages, const std::vector<Message>& newMessages);
void serviceAddMessage(std::vector<Message>& allMessages, const Message& newMsg);
void serviceDeleteMessage(std::vector<Message>& allMessages, const Message& target);


#endif //FINALPROJECT2_USER_H