#ifndef UI_CORE_H
#define UI_CORE_H
#include <string>

struct LoginCredentials {
    std::string username;
    std::string password;
    std::string action; // NEW: Stores "LOGIN", "CREATE", or "FORGOT"
};

// Functions
void initMouse(); // Call this in main!
LoginCredentials showLoginView();
void showLoadingView(const std::string& message);
void showAdminMenu();
void showCreateAccountForm();
void showForgotForm();

#endif