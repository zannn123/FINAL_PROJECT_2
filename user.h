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

std::string getCleanContent(const std::string& rawContent);

// Extracts ["ben"] from "{GRP:ben} Hello"
std::vector<std::string> getGroupParticipants(const std::string& rawContent);

// Creates "{GRP:ben} Hello"
std::string formatGroupMessage(const std::string& realMessage, const std::vector<std::string>& participants);
std::vector<Message> getInboxMessages(const User& currentUser, const std::vector<Message>& allMessages);
std::vector<Message> getSentMessages(const User& currentUser, const std::vector<Message>& allMessages);
std::vector<Message> getConversationThread(const User& currentUser, const std::string& rootSubject, const std::vector<Message>& allMessages);


#endif //FINALPROJECT2_USER_H