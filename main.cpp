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
    showLoadingView("Loading Users");
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
                int adminChoice;
                if (!(cin >> adminChoice)) { // Error check if they type letters
                    cin.clear(); cin.ignore(1000, '\n');
                    continue;
                }
                if (adminChoice == 5) break;
                cout << "\n[System] Feature coming soon...\n";
                Sleep(1000);
            }
        }else if (isUserLoginValid(users,creds.username, creds.password)) {
                cout<<"welcome";

        }
        else {
            cout << "\n\n\t                                 [!] Invalid Username or Password.\n";
            Sleep(2000);
        }
    }
    return 0;
}

