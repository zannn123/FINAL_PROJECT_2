//
// Created by DjMhel on 01/01/2026.
//

#include "ui_user_global.h"
#include "globals.h"
#include "ui_core.h"
#include "user.h"
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <cctype>
#include <iomanip>
#include <vector>

using namespace std;

// =========================================================
// HELPER: SHARED SECURITY QUESTIONS LIST
// (Must match the ones in ui_core.cpp)
// =========================================================
vector<string> getSecurityQuestions() {
    return {
        "What is your favorite animal?",   // Index 0
        "What is your favorite interest?", // Index 1
        "What is your favorite object?",   // Index 2
        "What city were you born in?"      // Index 3
    };
}

int showUserView(const User& currentUser) {
    system("cls");
    HideCursor();
    DrawStarField();

    DrawCard(5, 2, 110, 4);
    DrawCard(5, 8, 35, 23);
    DrawCard(42, 8, 73, 23);

    SetColor(36); GoToXY(10, 4); cout << "USER DASHBOARD";
    SetColor(90); cout << " | ";
    SetColor(37); cout << "Welcome, " << currentUser.realName;

    GoToXY(90, 4); SetColor(32); cout << "[ ONLINE ]";

    const int MENU_SIZE = 6;
    string menuItems[MENU_SIZE] = {
        "Announcements",    // 0
        "Messages",         // 1
        "My Connections",   // 2
        "Add Friend",       // 3
        "Profile Settings", // 4
        "Logout"            // 5
    };

    int selection = 0;
    while(true) {
        for(int i=0; i<MENU_SIZE; i++) {
            int yPos = 12 + (i * 2);
            GoToXY(8, yPos);

            if(i == selection) {
                SetColor(33);
                cout << ">> " << menuItems[i] << " <<";
            } else {
                SetColor(90);
                cout << "   " << menuItems[i] << "   ";
            }
        }

        GoToXY(45, 12); SetColor(37); cout << "PREVIEW:";
        GoToXY(45, 13); SetColor(90); cout << "---------------";
        GoToXY(45, 15); SetColor(37);

        cout << "                                                        ";
        GoToXY(45, 16); cout << "                                                        ";
        GoToXY(45, 17); cout << "                                                        ";
        GoToXY(45, 18); cout << "                                                        ";

        GoToXY(45, 15);

        if(selection == 0) {
             cout << "View Public Broadcasts:";
             GoToXY(47, 17); SetColor(90); cout << "* Read Admin Announcements";
             GoToXY(47, 18); cout << "* Reply to Broadcasts";
        }
        else if(selection == 1) {
             cout << "Private Communication Center:";
             GoToXY(47, 17); SetColor(90); cout << "* Inbox & Sent Items";
             GoToXY(47, 18); cout << "* Compose New Message";
        }
        else if(selection == 2) {
             cout << "Manage Your Network:";
             GoToXY(47, 17); SetColor(90); cout << "* View Friend List";
             GoToXY(47, 18); cout << "* Remove Connections";
        }
        else if(selection == 3) {
             cout << "Expand Your Circle:";
             GoToXY(47, 17); SetColor(90); cout << "* Browse All Users";
             GoToXY(47, 18); cout << "* Search & Add Friends";
        }
        else if(selection == 4) {
             cout << "Account Management:";
             GoToXY(47, 17); SetColor(90); cout << "* Edit Name & Bio";
             GoToXY(47, 18); cout << "* Password & Security Q";
        }
        else if(selection == 5) {
             cout << "Securely sign out of the application.";
             GoToXY(47, 17); SetColor(90); cout << "* Returns to Login Screen";
        }

        int key = _getch();
        if (key == 224) {
            key = _getch();
            if (key == 72) { // UP
                selection--;
                if (selection < 0) selection = MENU_SIZE - 1;
                Beep(600, 20);
            }
            if (key == 80) { // DOWN
                selection++;
                if (selection >= MENU_SIZE) selection = 0;
                Beep(600, 20);
            }
        }
        else if (key == 13) {
            Beep(1000, 50);
            return selection + 1;
        }
    }
}

// =========================================================
// HELPER: SECURITY POPUP (Red "Admin Style")
// =========================================================
bool confirmPasswordAction(const User& currentUser) {
    int x = 40, y = 10;
    DrawCard(x, y, 40, 8);

    GoToXY(x + 13, y + 2);
    SetColor(31); // RED TEXT
    cout << "SECURITY CHECK";

    GoToXY(x + 2, y + 3);
    SetColor(90); for(int i=0; i<36; i++) cout << "-";

    GoToXY(x + 4, y + 5);
    SetColor(37); cout << "Confirm Pass: ";

    string input = "";
    if (activeInput(x + 18, y + 5, input, true) == -1) return false;

    if (input == currentUser.password) {
        return true;
    } else {
        GoToXY(x + 12, y + 7);
        SetColor(31); cout << "WRONG PASSWORD!";
        Beep(500, 200);
        Sleep(1000);
        return false;
    }
}

// =========================================================
// SUB-MENU: EDIT PROFILE
// =========================================================
void performEditProfile(User& currentUser, UserMap& users) {
    int selection = 0;
    while(true) {
        system("cls"); DrawStarField();
        int x = 35, y = 8, w = 60, h = 18;
        DrawCard(x, y, w, h);

        GoToXY(x + 20, y + 2); SetColor(36); cout << "EDIT PROFILE";
        GoToXY(x + 2, y + 3); SetColor(90); for(int i=0; i<w-4; i++) cout << "-";

        GoToXY(x + 4, y + 5); SetColor(90); cout << "Real Name : "; SetColor(37); cout << currentUser.realName;
        GoToXY(x + 4, y + 6); SetColor(90); cout << "Username  : "; SetColor(37); cout << currentUser.username;
        GoToXY(x + 4, y + 7); SetColor(90); cout << "Bio       : "; SetColor(37);
        if (currentUser.description == "DEFAULT_USER") cout << "(None)";
        else cout << currentUser.description.substr(0, 30);

        GoToXY(x + 2, y + 9); SetColor(90); for(int i=0; i<w-4; i++) cout << "-";
        GoToXY(x + 4, y + 10); SetColor(33); cout << "Select what to change:";

        string options[] = { "Edit Real Name", "Edit Username", "Edit Bio/Description", "Back" };
        for(int i=0; i<4; i++) {
            GoToXY(x + 6, y + 12 + i);
            if(i == selection) { SetColor(33); cout << ">> " << options[i]; }
            else { SetColor(37); cout << "   " << options[i]; }
        }

        int key = _getch();
        if (key == 224) {
            key = _getch();
            if (key == 72 && selection > 0) selection--;
            if (key == 80 && selection < 3) selection++;
        }
        else if (key == 13) {
            if (selection == 3) return;

            if (selection == 0) { // Edit Name
                GoToXY(x + 4, y + 16); SetColor(37); cout << "New Name: ";
                string newName;
                if (activeInput(x + 15, y + 16, newName) != -1 && !newName.empty()) {
                    if (confirmPasswordAction(currentUser)) {
                        serviceUpdateProfile(users, currentUser, newName, currentUser.description);
                        GoToXY(x + 15, y + 17); SetColor(32); cout << "[SUCCESS] Name Updated!";
                        Sleep(1500);
                    }
                }
            }
            else if (selection == 1) { // Edit Username
                GoToXY(x + 4, y + 16); SetColor(37); cout << "New Username: ";
                string newUser;
                if (activeInput(x + 19, y + 16, newUser) != -1 && !newUser.empty()) {
                    if (confirmPasswordAction(currentUser)) {
                        string res = serviceUpdateUsername(users, currentUser, newUser);
                        GoToXY(x + 10, y + 17);
                        if (res == "Success") {
                            SetColor(32); cout << "[SUCCESS] Username changed to " << newUser;
                        } else {
                            SetColor(31); cout << "[ERROR] " << res;
                        }
                        Sleep(2000);
                    }
                }
            }
            else if (selection == 2) { // Edit Bio
                GoToXY(x + 4, y + 16); SetColor(37); cout << "New Bio: ";
                string newBio;
                if (activeInput(x + 14, y + 16, newBio) != -1 && !newBio.empty()) {
                    if (confirmPasswordAction(currentUser)) {
                        serviceUpdateProfile(users, currentUser, currentUser.realName, newBio);
                        GoToXY(x + 15, y + 17); SetColor(32); cout << "[SUCCESS] Bio Updated!";
                        Sleep(1500);
                    }
                }
            }
        }
        else if (key == 27) return;
    }
}

// =========================================================
// SUB-FEATURE: CHANGE PASSWORD
// =========================================================
void performChangePassword(User& currentUser, UserMap& users) {
    system("cls"); DrawStarField();
    int x = 35, y = 8, w = 60, h = 14;
    DrawCard(x, y, w, h);

    GoToXY(x + 18, y + 2); SetColor(36); cout << "CHANGE PASSWORD";
    GoToXY(x + 2, y + 3); SetColor(90); for(int i=0; i<w-4; i++) cout << "-";

    // 1. Security Check
    if (!confirmPasswordAction(currentUser)) return;

    // 2. Redraw Form
    system("cls"); DrawStarField(); DrawCard(x, y, w, h);
    GoToXY(x + 18, y + 2); SetColor(36); cout << "CHANGE PASSWORD";
    GoToXY(x + 2, y + 3); SetColor(90); for(int i=0; i<w-4; i++) cout << "-";

    string newPass, confirmPass;

    GoToXY(x + 4, y + 5); SetColor(37); cout << "New Password    : ";
    if (activeInput(x + 22, y + 5, newPass, true) == -1) return;

    if (newPass.length() < 4) {
        GoToXY(x + 22, y + 6); SetColor(31); cout << "Too short! (Min 4 chars)";
        Sleep(1500); return;
    }

    GoToXY(x + 4, y + 7); SetColor(37); cout << "Confirm Password: ";
    if (activeInput(x + 22, y + 7, confirmPass, true) == -1) return;

    if (newPass != confirmPass) {
        GoToXY(x + 22, y + 8); SetColor(31); cout << "Passwords do not match!";
        Sleep(1500); return;
    }

    serviceChangePassword(users, currentUser, newPass);
    GoToXY(x + 15, y + 10); SetColor(32); cout << "[SUCCESS] Password Changed!";
    Sleep(1500);
}

// =========================================================
// SUB-FEATURE: UPDATE SECURITY QUESTION (NEW!)
// =========================================================
void performUpdateSecurityQA(User& currentUser, UserMap& users) {
    vector<string> questions = getSecurityQuestions();
    int currentQIdx = currentUser.questionIndex;

    // Safety check for index
    if (currentQIdx < 0 || currentQIdx >= (int)questions.size()) currentQIdx = 0;

    system("cls"); DrawStarField();
    int x = 30, y = 8, w = 70, h = 17; // Taller to fit everything
    DrawCard(x, y, w, h);

    // --- SECTION 1: VIEW CURRENT INFO ---
    GoToXY(x + 20, y + 2); SetColor(36); cout << "UPDATE SECURITY ANSWER";
    GoToXY(x + 2, y + 3); SetColor(90); for(int i=0; i<w-4; i++) cout << "-";

    GoToXY(x + 4, y + 5); SetColor(90); cout << "Security Question:";
    GoToXY(x + 23, y + 5); SetColor(37); cout << questions[currentQIdx];

    GoToXY(x + 4, y + 6); SetColor(90); cout << "Current Answer   :";
    GoToXY(x + 23, y + 6); SetColor(37); cout << currentUser.securityAnswer;

    // --- SECURITY CHECK ---
    // User must confirm password to proceed
    if (!confirmPasswordAction(currentUser)) return;

    // --- SECTION 2: EDIT MODE (Redraw) ---
    system("cls"); DrawStarField(); DrawCard(x, y, w, h);

    GoToXY(x + 20, y + 2); SetColor(36); cout << "UPDATE SECURITY ANSWER";
    GoToXY(x + 2, y + 3); SetColor(90); for(int i=0; i<w-4; i++) cout << "-";

    // 1. Show Question (Locked/Read-Only)
    GoToXY(x + 4, y + 5); SetColor(90); cout << "Security Question:";
    GoToXY(x + 23, y + 5); SetColor(33); cout << questions[currentQIdx];

    // 2. Show Old Answer (For Reference)
    GoToXY(x + 4, y + 6); SetColor(90); cout << "Old Answer       :";
    GoToXY(x + 23, y + 6); SetColor(37); cout << currentUser.securityAnswer;

    // Divider
    GoToXY(x + 2, y + 8); SetColor(90); for(int i=0; i<w-4; i++) cout << "-";

    // 3. Input New Answer
    GoToXY(x + 4, y + 10); SetColor(32); cout << "Enter New Answer Below:";

    string newAns = "";

    GoToXY(x + 4, y + 12); SetColor(37); cout << "New Answer: ";

    // Instructions Footer
    GoToXY(x + 8, y + 15);
    SetColor(31); cout << "[ESC]"; SetColor(37); cout << " Cancel   ";
    SetColor(32); cout << "[ENTER]"; SetColor(37); cout << " Save";

    // Get Input (Visible text, not masked)
    if (activeInput(x + 16, y + 12, newAns, false) == -1) return; // Handle ESC to Cancel

    // --- SAVE LOGIC ---
    if (!newAns.empty()) {
        // Keep the same question index, only update the answer
        serviceUpdateSecurityAnswer(users, currentUser, currentQIdx, newAns);

        GoToXY(x + 20, y + 15); SetColor(32); cout << "[SUCCESS] Answer Updated!   ";
        Sleep(1500);
    } else {
        GoToXY(x + 20, y + 15); SetColor(31); cout << "[INFO] Input empty. No changes.";
        Sleep(1500);
    }
}

// =========================================================
// MENU: ACCOUNT SECURITY (Sub-Menu)
// =========================================================
void performSecuritySettings(User& currentUser, UserMap& users) {
    int selection = 0;
    while(true) {
        system("cls"); DrawStarField();
        int x = 40, y = 10, w = 45, h = 12;
        DrawCard(x, y, w, h);

        GoToXY(x + 12, y + 2); SetColor(36); cout << "ACCOUNT SECURITY";
        GoToXY(x + 2, y + 3); SetColor(90); for(int i=0; i<w-4; i++) cout << "-";

        string items[] = { "Change Password", "Update Security Question", "Back" };

        for(int i=0; i<3; i++) {
            GoToXY(x + 6, y + 5 + i);
            if (i == selection) { SetColor(33); cout << ">> " << items[i]; }
            else { SetColor(37); cout << "   " << items[i]; }
        }

        int key = _getch();
        if (key == 224) {
            key = _getch();
            if (key == 72 && selection > 0) selection--;
            if (key == 80 && selection < 2) selection++;
        }
        else if (key == 13) {
            if (selection == 0) performChangePassword(currentUser, users);
            else if (selection == 1) performUpdateSecurityQA(currentUser, users);
            else return;
        }
        else if (key == 27) return;
    }
}

// =========================================================
// MAIN SETTINGS MENU
// =========================================================
void showProfileSettings(User& currentUser, UserMap& users) {
    int selection = 0;
    while (true) {
        system("cls"); DrawStarField();

        int x = 40, y = 10, w = 40, h = 10;
        DrawCard(x, y, w, h);

        GoToXY(x + 12, y + 2); SetColor(36); cout << "PROFILE SETTINGS";
        GoToXY(x + 2, y + 3); SetColor(90); for(int i=0; i<w-4; i++) cout << "-";

        string items[3] = { "Edit Profile Info", "Account Security", "Back" };

        for (int i = 0; i < 3; ++i) {
            GoToXY(x + 4, y + 5 + i);
            if (i == selection) { SetColor(33); cout << ">> " << items[i]; }
            else { SetColor(37); cout << "   " << items[i]; }
        }

        int key = _getch();
        if (key == 224) {
            key = _getch();
            if (key == 72 && selection > 0) selection--;
            if (key == 80 && selection < 2) selection++;
        }
        else if (key == 13) {
            if (selection == 0) performEditProfile(currentUser, users);
            else if (selection == 1) performSecuritySettings(currentUser, users);
            else if (selection == 2) return;
        }
        else if (key == 27) return;
    }
}