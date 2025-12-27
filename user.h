//
// Created by DjMhel on 27/12/2025.
//

#ifndef FINALPROJECT2_USER_H
#define FINALPROJECT2_USER_H
#include <string>
#include "globals.h"

bool isUserLoginValid(const UserMap& db, const std::string& username, const std::string& password);


#endif //FINALPROJECT2_USER_H