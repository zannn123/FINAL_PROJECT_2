//
// Created by DjMhel on 27/12/2025.
//

#include "user.h"
#include "globals.h"
#include <iostream>
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

