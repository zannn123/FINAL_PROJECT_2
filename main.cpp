#include "globals.h"
#include "file_handler.h"
#include "ui_core.h"
#include "user.h"
#include "admin.h"
#include <iostream>
#include <windows.h>

using namespace std;

UserMap users;
vector<Message> allMessages;

int main() {
    initMouse();
    Beep(2000, 400);
    showLoadingView("Loading Users");
    Beep(2000, 400);
    loadUsers(users);
    loadMessages(allMessages);

    while (true) {
        LoginCredentials creds = showLoginView();
        if (creds.action == "CREATE") {
            showCreateAccountForm();
        }
        else if (creds.action == "FORGOT") {
            showForgotForm();
        }
        else if (isAdminLogin(creds.username, creds.password)) {
            while (true) {
                showAdminMenu();
                int choice = showAdminMenu();
                if (choice == 1) {
                    // Call View Users Function
                    // adminViewUsers(users);
                }
                else if (choice == 2) {
                    // Call Modify Users
                }
                else if (choice == 3) {
                    // Call Delete Users
                }
                else if (choice == 4) {
                    // Call View Messages
                }
                else if (choice == 5) {
                    break; // Logout
                }
            }
        }else if (isUserLoginValid(users,creds.username, creds.password)) {
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
            cout << "\n\n\t                                 [!] Invalid Username or Password.\n";
            Sleep(2000);
        }
    }
    return 0;
}

