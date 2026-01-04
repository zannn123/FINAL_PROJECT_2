#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include <conio.h>
#include "globals.h"
#include "file_handler.h"
#include "ui_core.h"
#include "ui_admin.h"
#include "ui_user_global.h"
#include "ui_user_message.h"
#include "ui_user_connections.h"
#include "admin.h"
#include "user.h"

using namespace std;

// Global Data Containers
UserMap users;
vector<Message> allMessages;

int main() {
    // 1. Initial Setup
    initMouse();
    Beep(2000, 400);
    showLoadingView("Loading Users");
    Beep(2000, 400);

    Admin admin;
    bool isAppRunning = true;

    while (isAppRunning) {
        loadUsers(users);
        loadMessages(allMessages);
        LoginCredentials creds = showLoginView();
        if (creds.action == "CREATE") {
            if (showCreateAccountForm(users)) {
                saveUsers(users);
            }
        }
        else if (creds.action == "FORGOT") {
            if (showForgotForm(users)) {
                saveUsers(users);
            }
        }
        else if (isAdminLogin(users, creds.username, creds.password)) {
            bool isAdminSession = true;
            while (isAdminSession) {
                int choice = showAdminMenu();
                if (choice == 1) {
                    bool isManagingUsers = true;
                    while (isManagingUsers) {
                        string targetUser = "";
                        int action = showUserSubMenu(users, targetUser);
                        switch (action) {
                            case 1:
                                if (!targetUser.empty()) {
                                    system("cls");
                                    User selectedUser = users[targetUser];
                                    showUser(selectedUser, admin.cipherPassword(selectedUser.password));
                                    SetColor(90); cout << "\t\n        Press any key to return..."; _getch();
                                }
                                break;
                            case 2: // Modify
                                showModifyUserMenu(admin, users, targetUser);
                                break;
                            case 3: // Delete
                                if (!targetUser.empty() && confirmAdminPassword(creds.password)) {
                                    users.erase(targetUser);
                                    saveUsers(users);
                                    SetColor(31);
                                    cout << "\n\n\t [SYSTEM] User deleted.\n";
                                    Sleep(1500);
                                }
                                break;
                            case 4: // Reset Password
                                if (!targetUser.empty() && confirmAdminPassword(creds.password)) {
                                    admin.resetPasswordToDefault(users[targetUser]);
                                    saveUsers(users);
                                    SetColor(32);
                                    cout << "\n\n\t [SUCCESS] Password reset to 'default123'.";
                                    Sleep(2000);
                                }
                                break;
                            case 5:
                                isManagingUsers = false;
                        }
                    }
                }
                else if (choice == 2) {
                    bool isManagingMessages = true; // Sub-menu Flag

                    while (isManagingMessages) {
                        Message result = showMessageListUI(admin, allMessages, users, creds.password);

                        // Check if user pressed ESC (returned empty message)
                        if (result.sender.empty() && result.content.empty()) {
                            isManagingMessages = false;
                        }
                        else if (result.sender == "__DELETE_SIGNAL__") {
                            saveMessages(allMessages);
                        }
                        else {
                            showFullMessage(result, users);
                        }
                    }
                }
                // 3. ADMIN SECURITY
                else if (choice == 3) {
                    if (confirmAdminPassword(creds.password)) {
                        string newPass = showChangePasswordForm(creds.username);
                        if (!newPass.empty()) {
                            admin.changePassword(users[creds.username], newPass);
                            saveUsers(users);
                        }
                    }
                }
                // 4. LOGOUT
                else if (choice == 4) {
                    isAdminSession = false; // Updates flag, exits loop naturally
                }
            }
        }
        // --- OPTION D: USER LOGIN ---
        else if (isUserLoginValid(users, creds.username, creds.password)) {
            User& currentUser = users[creds.username];

            // 1. Security Check: Force Password Change if "default123"
            if (creds.password == "default123") {
                showNotification("SECURITY NOTICE", "Your password was reset by Admin.", "You must change it now.", 33);
                string newPass = showChangePasswordForm(currentUser.username);
                if (!newPass.empty() && newPass != "default123") {
                    performPasswordUpdate(currentUser, newPass);
                    saveUsers(users);
                    showNotification("SUCCESS", "Password updated!", "Logging you in...", 32);
                }
                else {
                    showNotification("CANCELLED", "You cannot proceed.", "Logging out...", 31);
                    continue; // Skip the session loop, go back to login
                }
            }
            // 2. User Session Loop
            bool isUserSession = true; // User Loop Flag
            while (isUserSession) {
                int choice = showUserView(currentUser);

                if (choice == 1) {
                    showAnnouncementFeed(currentUser, allMessages, users);
                }
                else if (choice == 2) {
                    showMessenger(currentUser, allMessages, users);
                    // MESSAGES (Inbox, Create, Sent)
                }
                else if (choice == 3) {
                    showConnectionsHub(currentUser, users);
                    // MY CONNECTIONS (View/Remove Friends)
                    // showConnectionsHub(currentUser, users); // <--- We will code this next!
                }
                else if (choice == 4) {
                    showAddFriendMenu(currentUser, users);
                    // ADD FRIEND (Browse/Search)
                    // showAddFriendMenu(currentUser, users); // <--- We will code this next!
                }
                else if (choice == 5) {
                    showProfileSettings(currentUser, users);
                    // PROFILE SETTINGS (Edit Name/Pass)
                    // showProfileSettings(currentUser, users); // <--- We will code this in ui_user_dashboard
                }
                else if (choice == 6) {
                    // LOGOUT
                    isUserSession = false; // Updates flag, exits loop naturally
                }
            }
        }
        // --- OPTION E: FAILED LOGIN ---
        else {
            User currentUser1 = users[creds.username];
            int msgX = 38;
            int msgY = 23;
            GoToXY(msgX, msgY);
            cout << "                                     ";
            GoToXY(msgX, msgY);
            if (currentUser1.isLocked) {
                SetColor(31); // Red for Danger
                cout << "  [!] ACCOUNT LOCKED. Contact Admin.";
            } else {
                SetColor(33); // Orange for Warning
                cout << "[!] Invalid Username or Password.";
            }
            Sleep(2000);
        }
    }

    return 0;
}