#ifndef UI_CORE_H
#define UI_CORE_H
#include <string>


struct LoginCredentials {
    std::string username;
    std::string password;
};

// Functions
LoginCredentials showLoginView();
void showLoadingView(const std::string& message);

#endif