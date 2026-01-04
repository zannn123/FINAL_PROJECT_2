//
// Created by DjMhel on 20/12/2025.
//

#ifndef FINAL_PROJECT_2_FILE_HANDLER_H
#define FINAL_PROJECT_2_FILE_HANDLER_H

#include "globals.h"

void loadUsers(UserMap& users);
void loadMessages(std::vector<Message>& messages);
void saveUsers(const UserMap& users);
void saveMessages(const std::vector<Message>& messages);

#endif //FINAL_PROJECT_2_FILE_HANDLER_H