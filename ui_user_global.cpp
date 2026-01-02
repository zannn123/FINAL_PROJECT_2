//
// Created by DjMhel on 01/01/2026.
//

#include "ui_user_global.h"
#include "globals.h"
#include "ui_core.h"
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <vector>
#include <cstdlib>
#include <cctype>
#include <iomanip>

using namespace std;

int showUserView(const User& currentUser) {
    system("cls");
    HideCursor();
    DrawStarField();

    DrawCard(5, 2, 110, 4);
    DrawCard(5, 8, 35, 23); // Increased height slightly for extra option
    DrawCard(42, 8, 73, 23);

    SetColor(36); GoToXY(10, 4); cout << "USER DASHBOARD";
    SetColor(90); cout << " | ";
    SetColor(37); cout << "Welcome, " << currentUser.realName;

    GoToXY(90, 4); SetColor(32); cout << "[ ONLINE ]";

    // --- UPDATED MENU LIST ---
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
        // Draw Menu Items
        for(int i=0; i<MENU_SIZE; i++) {
            int yPos = 12 + (i * 2); // Reduced spacing slightly to fit 6 items neatly
            GoToXY(8, yPos);

            if(i == selection) {
                SetColor(33);
                cout << ">> " << menuItems[i] << " <<";
            } else {
                SetColor(90);
                cout << "   " << menuItems[i] << "   ";
            }
        }

        // Draw Preview Section
        GoToXY(45, 12); SetColor(37); cout << "PREVIEW:";
        GoToXY(45, 13); SetColor(90); cout << "---------------";
        GoToXY(45, 15); SetColor(37);

        // Clear previous preview text
        cout << "                                                        ";
        GoToXY(45, 16); cout << "                                                        ";
        GoToXY(45, 17); cout << "                                                        ";
        GoToXY(45, 18); cout << "                                                        ";

        GoToXY(45, 15);

        // --- UPDATED DESCRIPTIONS ---
        if(selection == 0) { // Announcements
             cout << "View Public Broadcasts:";
             GoToXY(47, 17); SetColor(90); cout << "* Read Admin Announcements";
             GoToXY(47, 18); cout << "* Reply to Broadcasts";
        }
        else if(selection == 1) { // Messages
             cout << "Private Communication Center:";
             GoToXY(47, 17); SetColor(90); cout << "* Inbox & Sent Items";
             GoToXY(47, 18); cout << "* Compose New Message";
        }
        else if(selection == 2) { // My Connections
             cout << "Manage Your Network:";
             GoToXY(47, 17); SetColor(90); cout << "* View Friend List";
             GoToXY(47, 18); cout << "* Remove Connections";
        }
        else if(selection == 3) { // Add Friend
             cout << "Expand Your Circle:";
             GoToXY(47, 17); SetColor(90); cout << "* Browse All Users";
             GoToXY(47, 18); cout << "* Search & Add Friends";
        }
        else if(selection == 4) { // Profile Settings
             cout << "Account Management:";
             GoToXY(47, 17); SetColor(90); cout << "* Edit Name & Description";
             GoToXY(47, 18); cout << "* Change Password";
        }
        else if(selection == 5) { // Logout
             cout << "Securely sign out of the application.";
             GoToXY(47, 17); SetColor(90); cout << "* Returns to Login Screen";
        }

        // Input Handling
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
        else if (key == 13) { // ENTER
            Beep(1000, 50);
            return selection + 1; // Returns 1-6
        }
    }
}