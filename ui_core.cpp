#include "ui_core.h"
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <vector>
#include <cstdlib>
#include <cctype>
#include <iomanip>
using namespace std;
HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

void GoToXY(int x, int y) { cout << "\033[" << y << ";" << x << "H"; }
void SetColor(int c) { cout << "\033[" << c << "m"; }
void HideCursor() { cout << "\033[?25l"; }
void ShowCursor() { cout << "\033[?25h"; }

string getInputAt(int x, int y, bool isPassword) {
    string input = "";
    GoToXY(x, y);
    SetColor(33);

    while(true) {
        char ch = _getch();
        if (ch == 13) break;
        if (ch == 8) {
            if (!input.empty()) {
                input.pop_back();
                cout << "\b \b";
            }
        }
        else if (isalnum(ch) || ispunct(ch) || ch == ' ') {
            if (input.length() < 25) {
                input += ch;
                if (isPassword) cout << "*"; else cout << ch;
            }
        }
    }
    return input;
}

int activeInput(int x, int y, string& data, bool isPwd) {
    GoToXY(x, y);
    SetColor(33);

    if (isPwd) {
        for (size_t i = 0; i < data.length(); i++) cout << "*";
    } else {
        cout << data;
    }

    while (true) {
        int key = _getch();
        if (key == 13) return 1;
        if (key == 224) {
            key = _getch();
            if (key == 72) return -1;
            if (key == 80) return 1;
        }
        else if (key == 8) {
            if (!data.empty()) {
                data.pop_back();
                cout << "\b \b";
            }
        }
        else if (isalnum(key) || ispunct(key) || key == ' ') {
            if (data.length() < 25) {
                data += (char)key;
                if (isPwd) cout << "*"; else cout << (char)key;
            }
        }
    }
}

void initMouse() {
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hIn, &mode);
    mode &= ~ENABLE_QUICK_EDIT_MODE;
    mode |= ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS;
    SetConsoleMode(hIn, mode);
}

void drawButton(int x, int y, string text) {
    SetColor(44);
    SetColor(37);
    GoToXY(x, y); cout << " " << text << " ";
    SetColor(0);
}

void DrawStarField() {
    SetColor(90);
    for (int y = 1; y < 30; y++) {
        for (int x = 1; x < 120; x++) {
            if (rand() % 50 == 0) {
                int r = rand() % 3;
                if(r==0) cout << "."; else if(r==1) cout << "*"; else cout << "+";
            } else {
                cout << " ";
            }
        }
        cout << "\n";
    }
}

void UpdateStars(int safeX, int safeY, int safeW, int safeH) {
    for(int i=0; i<3; i++) {
        int ex = (rand() % 118) + 1;
        int ey = (rand() % 28) + 1;

        if (!(ex >= safeX && ex <= safeX + safeW && ey >= safeY && ey <= safeY + safeH)) {
             GoToXY(ex, ey); cout << " ";
        }
    }

    int x = (rand() % 118) + 1;
    int y = (rand() % 28) + 1;

    if (x >= safeX && x <= safeX + safeW && y >= safeY && y <= safeY + safeH) return;

    GoToXY(x, y);
    int magic = rand() % 20;
    if (magic == 0) { SetColor(37); cout << "*"; }
    else if (magic == 1) { SetColor(33); cout << "+"; }
    else if (magic == 2) { SetColor(90); cout << "."; }
}

void DrawCard(int x, int y, int w, int h) {
    SetColor(235);
    for(int i=1; i<=h; i++) { GoToXY(x+w, y+i); cout << "||"; }
    GoToXY(x+2, y+h+1); for(int i=0; i<w; i++) cout << "=";

    SetColor(36);
    for (int i = 0; i <= h; i++) {
        GoToXY(x, y + i);
        if (i==0 || i==h) cout << "+"; else cout << "|";
        cout << "\033[40m";
        for(int j=0; j<w-2; j++) { if(i==0||i==h) cout << "-"; else cout << " "; }
        cout << "\033[0m"; SetColor(36);
        if (i==0 || i==h) cout << "+"; else cout << "|";
    }
}

string InputWithTwinkle(int x, int y, bool isPassword, int boxX, int boxY, int boxW, int boxH) {
    string input = "";
    INPUT_RECORD ir[128];
    DWORD nRead;
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

    int btnY = boxY + 11;
    int createX_Start = boxX + 2, createX_End = boxX + 20;
    int forgotX_Start = boxX + 22, forgotX_End = boxX + 45;

    GoToXY(x, y);

    while (true) {
        UpdateStars(boxX, boxY, boxW, boxH);

        GoToXY(x + input.length(), y);
        SetColor(33);

        GetNumberOfConsoleInputEvents(hIn, &nRead);

        if (nRead > 0) {
            ReadConsoleInput(hIn, ir, 128, &nRead);
            for (size_t i = 0; i < nRead; i++) {
                if (ir[i].EventType == MOUSE_EVENT &&
                   (ir[i].Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED)) {
                    int mx = ir[i].Event.MouseEvent.dwMousePosition.X + 1;
                    int my = ir[i].Event.MouseEvent.dwMousePosition.Y + 1;
                    if (my >= btnY - 1 && my <= btnY + 1) {
                        if (mx >= createX_Start && mx <= createX_End) return "_BTN_CREATE_";
                        if (mx >= forgotX_Start && mx <= forgotX_End) return "_BTN_FORGOT_";
                    }
                }
                if (ir[i].EventType == KEY_EVENT && ir[i].Event.KeyEvent.bKeyDown) {
                    char ch = ir[i].Event.KeyEvent.uChar.AsciiChar;
                    WORD vk = ir[i].Event.KeyEvent.wVirtualKeyCode;
                    if (vk == VK_F1) return "_BTN_CREATE_";
                    if (vk == VK_F2) return "_BTN_FORGOT_";
                    if (ch == 13) return input;
                    if (ch == 8) {
                        if (!input.empty()) { cout << "\b \b"; input.pop_back(); }
                    }
                    else if ((isalnum(ch) || ispunct(ch) || ch == ' ') && input.length() < 25) {
                        input += ch;
                        if (isPassword) cout << "*"; else cout << ch;
                    }
                }
            }
        }
        Sleep(30);
    }
}

LoginCredentials showLoginView() {
    HideCursor();
    system("cls");
    DrawStarField();
    int startX = 35, startY = 8, width = 45, height = 13;
    DrawCard(startX, startY, width, height);
    SetColor(37);
    GoToXY(startX + 12, startY + 2); cout << "SOCIAL MEDIA SYSTEM";
    GoToXY(startX + 12, startY + 3); cout << "===================";
    GoToXY(startX + 5, startY + 5); cout << "User:";
    GoToXY(startX + 5, startY + 7); cout << "Pass:";
    SetColor(90);
    GoToXY(startX + 11, startY + 5); cout << "[                         ]";
    GoToXY(startX + 11, startY + 7); cout << "[                         ]";
    drawButton(startX + 2, startY + 11, "F1: Create Account");
    drawButton(startX + 22, startY + 11, "F2: Forgot Pass");
    LoginCredentials data;
    ShowCursor();
    data.username = InputWithTwinkle(startX + 12, startY + 5, false, startX, startY, width, height);
    if (data.username == "_BTN_CREATE_") { data.action = "CREATE"; return data; }
    if (data.username == "_BTN_FORGOT_") { data.action = "FORGOT"; return data; }
    data.password = InputWithTwinkle(startX + 12, startY + 7, true, startX, startY, width, height);
    if (data.password == "_BTN_CREATE_") { data.action = "CREATE"; return data; }
    if (data.password == "_BTN_FORGOT_") { data.action = "FORGOT"; return data; }
    data.action = "LOGIN";
    return data;
}

void showLoadingView(const string& finalMessage) {
    system("cls");
    HideCursor();
    int startX = 35;
    int startY = 12;
    int barWidth = 40;
    Beep(37, 50);

    string currentMsg = "";
    for(int i=0; i<finalMessage.length(); i++) currentMsg += " ";

    for (int i = 0; i < finalMessage.length(); i++) {
        for (int k = 0; k < 1; k++) {
            GoToXY(startX, startY - 2);
            SetColor(32); cout << currentMsg.substr(0, i);
            SetColor(37); cout << (char)(rand() % 25 + 65);

            Beep(3000 + (rand() % 500), 5);
        }
        currentMsg[i] = finalMessage[i];
    }
    GoToXY(startX, startY - 2);
    SetColor(37); cout << finalMessage;

    for (int percent = 0; percent <= 100; percent+=4) {
        int color = 31;
        if(percent > 40) color = 33;
        if(percent > 80) color = 32;
        GoToXY(startX, startY);
        SetColor(90); cout << "[";
        SetColor(color);
        int fill = (percent * barWidth) / 100;
        for (int j = 0; j < fill; j++) cout << "=";
        SetColor(37); cout << ">";
        SetColor(90);
        for (int j = fill; j < barWidth; j++) cout << " ";
        cout << "]";
        GoToXY(startX + barWidth + 3, startY);
        SetColor(color); cout << percent << "%";
        Beep(400 + (percent * 25), 20);
    }
    Beep(1500, 250);
    Beep(2000, 100);
    Sleep(300);
    ShowCursor();
}

bool showForgotForm(UserMap& users) {
    system("cls");
    DrawStarField();
    int x = 35, startY = 8, w = 55, h = 14;
    DrawCard(x, startY, w, h);

    SetColor(37);
    GoToXY(x + 18, startY + 2); cout << "RESET PASSWORD";
    GoToXY(x + 18, startY + 3); cout << "--------------";

    // 1. Ask for Username
    GoToXY(x + 4, startY + 6); cout << "Enter Username: ";
    string targetUser = "";
    activeInput(x + 20, startY + 6, targetUser);

    // 2. Validate User Exists
    if (users.find(targetUser) == users.end()) {
        GoToXY(x + 4, startY + 8); SetColor(31);
        cout << "Error: User not found!";
        Sleep(1500);
        return false; // <--- RETURN FALSE (Failed)
    }

    User& u = users[targetUser];

    // 3. Retrieve Question
    string questions[] = {
        "What is your favorite animal?",
        "What is your favorite interest?",
        "What is your favorite object?",
        "What city were you born in?"
    };
    string qText = (u.questionIndex >= 0 && u.questionIndex < 4) ? questions[u.questionIndex] : "Unknown";

    // 4. Ask Question
    GoToXY(x + 4, startY + 8); SetColor(37); cout << "Security Check:";
    GoToXY(x + 4, startY + 9); SetColor(36); cout << qText;
    GoToXY(x + 4, startY + 11); SetColor(37); cout << "Your Answer: ";

    string inputAnswer = "";
    activeInput(x + 17, startY + 11, inputAnswer);

    // 5. Verify Answer
    if (inputAnswer == u.securityAnswer) {
        u.resetRequested = true; // Update memory only

        GoToXY(x + 4, startY + 13); SetColor(32);
        cout << "Request Sent! Admin notified.";
        Beep(1000, 100);
        Sleep(2000);
        return true; // <--- RETURN TRUE (Success!)
    } else {
        GoToXY(x + 4, startY + 13); SetColor(31);
        cout << "Incorrect Answer. Failed.";
        Beep(200, 300);
        Sleep(2000);
        return false; // <--- RETURN FALSE (Failed)
    }
}

bool showCreateAccountForm(UserMap& users) {
    system("cls");
    DrawStarField();
    int startX = 30, startY = 5;
    DrawCard(startX, startY, 55, 20);

    SetColor(37);
    GoToXY(startX + 15, startY + 2); cout << "CREATE NEW ACCOUNT";
    GoToXY(startX + 15, startY + 3); cout << "------------------";

    string questions[] = {
        "What is your favorite animal?",
        "What is your favorite interest?",
        "What is your favorite object?",
        "What city were you born in?"
    };
    int qIndex = rand() % 4;
    string randomQ = questions[qIndex];

    GoToXY(startX + 4, startY + 5);  cout << "Name:       [                         ]";
    GoToXY(startX + 4, startY + 7);  cout << "Username:   [                         ]";
    GoToXY(startX + 4, startY + 9);  cout << "Password:   [                         ]";
    GoToXY(startX + 4, startY + 11); cout << "Re-Enter:   [                         ]";
    GoToXY(startX + 4, startY + 13); cout << "Security Q: " << randomQ;
    GoToXY(startX + 4, startY + 14); cout << "Answer:     [                         ]";

    drawButton(startX + 18, startY + 17, " SUBMIT ACCOUNT ");

    ShowCursor();
    User newUser;
    newUser.questionIndex = qIndex;

    string iName = "", iUser = "", iPass = "", iRePass = "", iAns = "";

    int step = 0;

    while (true) {
        GoToXY(startX + 35, startY + 16); cout << "                    ";

        int move = 0;

        switch(step) {
            case 0:
                move = activeInput(startX + 17, startY + 5, iName);
                if (move == 1) {
                    if (iName.empty()) {
                        GoToXY(startX + 45, startY + 5); SetColor(31); cout << "Required!";
                    } else {
                        GoToXY(startX + 45, startY + 5); cout << "         ";
                        step++;
                    }
                }
                break;

            case 1:
                move = activeInput(startX + 17, startY + 7, iUser);
                if (move == 1) {
                    if (iUser.empty()) {
                        GoToXY(startX + 45, startY + 7); SetColor(31); cout << "Required!";
                    }
                    else if (users.count(iUser)) {
                        GoToXY(startX + 17, startY + 8); SetColor(31); cout << "Taken! Try another.";
                    }
                    else {
                        GoToXY(startX + 45, startY + 7); cout << "         ";
                        GoToXY(startX + 17, startY + 8); cout << "                   ";
                        step++;
                    }
                } else {
                    step--;
                }
                break;

            case 2:
                move = activeInput(startX + 17, startY + 9, iPass, true);
                if (move == 1) {
                    if (iPass.empty()) {
                        GoToXY(startX + 45, startY + 9); SetColor(31); cout << "Required!";
                    } else {
                        GoToXY(startX + 45, startY + 9); cout << "         ";
                        step++;
                    }
                } else step--;
                break;

            case 3:
                move = activeInput(startX + 17, startY + 11, iRePass, true);
                if (move == 1) {
                    if (iPass != iRePass) {
                         GoToXY(startX + 17, startY + 12); SetColor(31); cout << "Passwords mismatch!";
                    } else {
                         GoToXY(startX + 17, startY + 12); cout << "                   ";
                         step++;
                    }
                } else step--;
                break;

            case 4:
                move = activeInput(startX + 17, startY + 14, iAns);
                if (move == 1) {
                    if (iAns.empty()) {
                        GoToXY(startX + 45, startY + 14); SetColor(31); cout << "Required!";
                    } else {
                        GoToXY(startX + 45, startY + 14); cout << "         ";
                        step++;
                    }
                } else step--;
                break;

            case 5:
                HideCursor();
                SetColor(32);
                drawButton(startX + 18, startY + 17, "> SUBMIT ACCOUNT <");

                int key = _getch();
                if (key == 13) {
                    newUser.realName = iName;
                    newUser.username = iUser;
                    newUser.password = iPass;
                    newUser.securityAnswer = iAns;
                    newUser.description = "DEFAULT_USER";
                    newUser.isLocked = false;
                    newUser.connections.clear();

                    users[newUser.username] = newUser;

                    GoToXY(startX + 4, startY + 18); SetColor(32);
                    cout << "Account Created Successfully!";
                    Beep(1000, 100);
                    Sleep(1000);
                    return true;
                }
                else if (key == 224) {
                    key = _getch();
                    if (key == 72) {
                        drawButton(startX + 18, startY + 17, " SUBMIT ACCOUNT ");
                        ShowCursor();
                        step--;
                    }
                }
                break;
        }

        if (step < 0) step = 0;
    }
}

void showUser(const User& user, const string& decryptedPassword) {
    int x = 40;
    int y = 5;
    DrawCard(x, y, 50, 14);
    GoToXY(x + 18, y + 2);
    SetColor(36); cout << "USER PROFILE";
    GoToXY(x + 2, y + 3);
    SetColor(90); for(int i=0; i<46; i++) cout << "-";
    SetColor(37);
    GoToXY(x + 4, y + 5); cout << "Username    :";
    SetColor(33); cout << " " << user.username;
    SetColor(37);
    GoToXY(x + 4, y + 7); cout << "Password    :";
    SetColor(90); cout << " " << decryptedPassword;
    SetColor(37);
    GoToXY(x + 4, y + 9); cout << "Role/Desc   :";
    SetColor(32); cout << " " << user.description;
    SetColor(37);
    GoToXY(x + 4, y + 11); cout << "Connections :";
    SetColor(35); if (user.connections.empty()) cout << "(None)";
    else { for (size_t i = 0; i < user.connections.size(); i++) cout << (i == 0 ? "" : ", ") << user.connections[i]; }
    SetColor(37);
    GoToXY(x + 4, y + 13); cout << "Status      :";
    if(user.isLocked) {
        SetColor(31); cout << " [LOCKED]";
    } else {
        SetColor(32); cout << " Active";
    }
    cout << "\n\n";
}

// ui_core.cpp

string showChangePasswordForm(const string& targetUsername) {
    system("cls");
    DrawStarField();
    int x = 35;
    int y = 8;
    int w = 50;
    int h = 16;
    DrawCard(x, y, w, h); GoToXY(x + 15, y + 2);
    SetColor(36); cout << "CHANGE PASSWORD"; GoToXY(x + 15, y + 3);
    SetColor(90); cout << "---------------"; GoToXY(x + 4, y + 5);
    SetColor(37); cout << "Target User: ";
    SetColor(33); cout << targetUsername; GoToXY(x + 4, y + 8);
    SetColor(37); cout << "New Password : [                         ]"; GoToXY(x + 4, y + 10);
    SetColor(37); cout << "Confirm Pass : [                         ]"; GoToXY(x + 4, y + 12);
    SetColor(90); cout << "[ ] Show Password (Press TAB)";
    drawButton(x + 15, y + 14, " SAVE PASSWORD ");
    string newPass = "";
    string confirmPass = "";
    bool showPwd = false;
    auto getInputWithToggle = [&](int inputX, int inputY, string &data) -> int {
        GoToXY(inputX, inputY);
        SetColor(33);

        if(showPwd) cout << data;
        else for(char c : data) cout << "*";

        while(true) {
            int key = _getch();

            if (key == 13) return 1;
            if (key == 27) return -1;

            if (key == 9) {
                showPwd = !showPwd;

                GoToXY(x + 5, y + 12);
                SetColor(showPwd ? 32 : 90);
                cout << (showPwd ? "x" : " ");

                GoToXY(inputX, inputY);
                cout << "                         ";
                GoToXY(inputX, inputY);
                SetColor(33);
                if(showPwd) cout << data;
                else for(char c : data) cout << "*";
            }
            else if (key == 8) {
                if (!data.empty()) {
                    data.pop_back();
                    cout << "\b \b";
                }
            }
            else if (isalnum(key) || ispunct(key) || key == ' ') {
                if (data.length() < 25) {
                    data += (char)key;
                    if (showPwd) cout << (char)key;
                    else cout << "*";
                }
            }
        }
    };

    if (getInputWithToggle(x + 20, y + 8, newPass) == -1) return "";

    getInputWithToggle(x + 20, y + 10, confirmPass);

    GoToXY(x + 4, y + 13);
    if (newPass.empty()) {
        SetColor(31); cout << "Error: Password cannot be empty!";
        Beep(500, 300);
        Sleep(1500);
        return "";
    }
    else if (newPass != confirmPass) {
        SetColor(31); cout << "Error: Passwords do not match!";
        Beep(500, 300);
        Sleep(1500);
        return "";
    }
    SetColor(32); cout << "Success! Updating database...   "; drawButton(x + 15, y + 14, "> SAVED <"); Beep(1000, 100); Sleep(1000);
    return newPass;
}

void showNotification(string title, string line1, string line2, int titleColor) {
    system("cls");
    DrawStarField(); // Keep the consistent background

    // 1. Draw the Centered Card
    int x = 35;
    int y = 10;
    DrawCard(x, y, 60, 8);

    // 2. Draw Title
    GoToXY(x + 5, y + 2);
    SetColor(titleColor); // Customizable Color (33=Orange, 32=Green, 31=Red)
    cout << "[!] " << title;

    // 3. Draw Message Line 1
    GoToXY(x + 5, y + 4);
    SetColor(37); // Always White for readability
    cout << line1;

    // 4. Draw Message Line 2 (Only if provided)
    if (!line2.empty()) {
        GoToXY(x + 5, y + 5);
        cout << line2;
    }

    // 5. Footer
    GoToXY(x + 5, y + 7);
    SetColor(90); // Grey
    cout << "Press any key to continue...";

    // 6. Pause
    _getch();
}