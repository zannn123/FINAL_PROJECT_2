//
// Created by DjMhel on 01/01/2026.
//

#ifndef FINALPROJECT2_UI_ADMIN_H
#define FINALPROJECT2_UI_ADMIN_H
#include "globals.h"

class Admin;

int showAdminMenu();
int showUserSubMenu(const UserMap& users, std::string &targetUser);
void showModifyUserMenu(Admin &admin, UserMap &users, std::string target);
bool confirmAdminPassword(std::string correctPassword);
void showFullMessage(const Message& msg, const UserMap& users);
Message showMessageListUI(Admin& admin, std::vector<Message>& allMessages, const UserMap& users, std::string adminPass);

#endif //FINALPROJECT2_UI_ADMIN_H