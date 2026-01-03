// ui_core.h
#ifndef UI_CORE_H
#define UI_CORE_H
#include <string>
#include "globals.h"

struct LoginCredentials {
    std::string username;
    std::string password;
    std::string action; // "LOGIN", "CREATE", "FORGOT"
};

// --- DRAWING UTILITIES (Shared) ---
void GoToXY(int x, int y);
void SetColor(int c);
void HideCursor();
void ShowCursor();
void initMouse();

void DrawCard(int x, int y, int w, int h);
void DrawStarField();
void drawButton(int x, int y, std::string text);
int activeInput(int x, int y, std::string& data, bool isPwd = false);
std::string InputWithTwinkle(int x, int y, bool isPassword, int boxX, int boxY, int boxW, int boxH);

// --- CORE SCREENS ---
LoginCredentials showLoginView();
void showLoadingView(const std::string& message);
bool showCreateAccountForm(UserMap& users);
bool showForgotForm(UserMap& users);
void showUser(const User& user, const std::string& decryptedPassword);
std::string showChangePasswordForm(const std::string& targetUsername);
void showNotification(std::string title, std::string line1, std::string line2 = "", int titleColor = 33);

std::vector<std::string> wrapText(std::string text, int limit);


#endif