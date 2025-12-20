//
// Created by DjMhel on 20/12/2025.
//

#ifndef FINAL_PROJECT_2_FILE_HANDLER_H
#define FINAL_PROJECT_2_FILE_HANDLER_H

#include "globals.h"

// LOAD: Reads text file -> Puts into Hash Map
void loadUsers(UserMap& users);
void loadMessages(vector<Message>& messages);

// SAVE: Reads Hash Map -> Writes to text file
void saveUsers(const UserMap& users);
void saveMessages(const vector<Message>& messages);

#endif //FINAL_PROJECT_2_FILE_HANDLER_H