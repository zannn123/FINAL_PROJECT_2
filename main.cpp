#include <conio.h>
#include <filesystem>
#include "globals.h"
#include "file_handler.h"
#include "ui_core.h"
#include "ui_admin.h"
#include "ui_user.h"
#include "admin.h"
#include "user.h"
#include <iostream>
#include <windows.h>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

UserMap users;
vector<Message> allMessages;

int main() {
    initMouse();
    Beep(2000, 400);
    showLoadingView("Loading Users");
    Beep(2000, 400);
    Admin admin;

    while (true) {
        loadUsers(users);
        loadMessages(allMessages);
        LoginCredentials creds = showLoginView();
        if (creds.action == "CREATE") {
            if (showCreateAccountForm(users)) {
                saveUsers(users);
            }
        }
        else if (creds.action == "FORGOT") {
            showForgotForm();
        }
        else if (isAdminLogin(users ,creds.username, creds.password)) {//admin side
            while (true) {
                int choice = showAdminMenu();
                if (choice == 1) {//users
                    while(true) {
                        string targetUser = ""; // Variable to hold the result
                        int action = showUserSubMenu(users, targetUser);

                        switch (action) {
                            case 1:
                                if (!targetUser.empty()) {
                                    system("cls");
                                    User selectedUser = users[targetUser];
                                    showUser(selectedUser, admin.cipherPassword(selectedUser.password));
                                    SetColor(90);
                                    cout << "\t   Press any key to return to menu...";
                                    _getch();
                                }
                                break;
                            case 2:
                                showModifyUserMenu(admin, users, targetUser);
                                break;
                            case 3:
                                if (!targetUser.empty()) {
                                    if (confirmAdminPassword(creds.password)) {
                                        users.erase(targetUser);
                                        saveUsers(users);
                                        SetColor(31);
                                        cout << "\n\n\t                                 [SYSTEM] User '" << targetUser << "' has been deleted.\n";
                                        Beep(500, 300);
                                        Sleep(1500);
                                    }
                                }
                                break;
                            case 4: // Reset Password
                                break;
                        }if (action == 5) break;
                    }
                }
                else if (choice == 2) {//messages
                    while(true) {
                        Message result = showMessageListUI(admin, allMessages, users, creds.password);
                        if (result.sender.empty() && result.content.empty()) break;


                        if (result.sender == "__DELETE_SIGNAL__") {
                            saveMessages(allMessages); // Main saves file
                            continue; // Reload list
                        }

                        showFullMessage(result, users);
                    }
                }
                else if (choice == 3) {//security
                    if (confirmAdminPassword(creds.password)) {
                        string newPass = showChangePasswordForm(creds.username);
                        if (!newPass.empty()) {
                            admin.changePassword(users[creds.username], newPass);
                            saveUsers(users);
                        }
                    }
                }
                else if (choice == 4) {
                    break;
                }
            }
        }else if (isUserLoginValid(users,creds.username, creds.password)) {//
            User currentUser = users[creds.username];
            while(true) {
                // Returns: 1=Feed, 2=Post, 3=Friends, 4=Profile, 5=Logout
                int choice = showUserView(currentUser);

                if (choice == 1) {
                    // Call View Feed Function (we will build this next)
                    // viewFeed(allMessages);
                }
                else if (choice == 2) {
                    // Call Post Function
                }
                else if (choice == 5) {
                    // Logout
                    break; // Breaks the inner loop, goes back to Login Screen
                }
            }
        }
        else {
            User currentUser1 = users[creds.username];
            if (currentUser1.isLocked) {
                cout << "\n\n\t                                 [!] ACCOUNT LOCKED.\n";
            }else {
                cout << "\n\n\t                                 [!] Invalid Username or Password.\n";
            }

            Sleep(2000);
        }
    }
    return 0;
}

