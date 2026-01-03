//
// Created by DjMhel on 03/01/2026.
//

#ifndef FINALPROJECT2_UI_USER_MESSAGE_H
#define FINALPROJECT2_UI_USER_MESSAGE_H
#include <vector>
#include "globals.h"


void showAnnouncementFeed(const User& currentUser, std::vector<Message>& allMessages, const UserMap& users);
void showMessenger(const User& currentUser, std::vector<Message>& allMessages, const UserMap& users);

#endif //FINALPROJECT2_UI_USER_MESSAGE_H