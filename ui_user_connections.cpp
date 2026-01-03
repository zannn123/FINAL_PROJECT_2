#include "ui_user_connections.h"
#include "ui_core.h"
#include "user.h" // Logic Layer
#include <iostream>
#include <conio.h>
#include <algorithm>
#include <windows.h>

using namespace std;

void showConnectionsHub(User& currentUser, UserMap& users) {
    int selection = 0;
    int scrollOffset = 0;

    while (true) {
        // 1. Get current list of friends
        vector<string>& friends = currentUser.connections;

        // 2. Setup Screen Background
        system("cls"); DrawStarField();

        // --- GEOMETRY FIX: SHIFT LEFT & TIGHTEN ---
        // Old: listX = 25. New: listX = 15 (Moved Left)
        // Main Box Width: 70
        // Sidebar Width: 25
        int listX = 15, listY = 4, listW = 70, listH = 20;
        int sideX = listX + listW + 2, sideY = listY, sideW = 25, sideH = 10;

        // 3. Draw Main Container
        DrawCard(listX, listY, listW, listH);
        GoToXY(listX + 25, listY + 2); SetColor(36); cout << "MY CONNECTIONS (" << friends.size() << ")";
        GoToXY(listX + 2, listY + 3); SetColor(90); for(int i=0; i<listW-4; i++) cout << "=";

        // 4. Draw Column Headers
        GoToXY(listX + 6, listY + 4); SetColor(33); cout << "USERNAME";
        GoToXY(listX + 35, listY + 4); SetColor(33); cout << "FULL NAME";
        GoToXY(listX + 2, listY + 5); SetColor(90); for(int i=0; i<listW-4; i++) cout << "-";

        // 5. Draw Sidebar Controls
        DrawCard(sideX, sideY, sideW, sideH);
        GoToXY(sideX + 6, sideY + 2); SetColor(36); cout << "ACTIONS";
        GoToXY(sideX + 2, sideY + 3); SetColor(90); for(int i=0; i<sideW-4; i++) cout << "-";

        GoToXY(sideX + 3, sideY + 5); SetColor(37); cout << "[UP/DN] Scroll";
        GoToXY(sideX + 3, sideY + 6); SetColor(33); cout << "[D]     Remove";
        GoToXY(sideX + 3, sideY + 7); SetColor(31); cout << "[ESC]   Back";

        // 6. Draw the Friend List
        if (friends.empty()) {
            GoToXY(listX + 22, listY + 10); SetColor(90); cout << "You have no connections yet.";
        }
        else {
            int yPtr = listY + 6;

            for (size_t i = scrollOffset; i < friends.size(); i++) {
                if (yPtr >= listY + listH - 1) break;

                string friendUsername = friends[i];
                string friendRealName = "Unknown User";

                if (users.find(friendUsername) != users.end()) {
                    friendRealName = users[friendUsername].realName;
                }

                // Draw Selection Cursor
                if (i == selection) { SetColor(33); GoToXY(listX + 3, yPtr); cout << ">>"; }
                else { SetColor(90); }

                // COLUMN 1: Username
                GoToXY(listX + 6, yPtr); SetColor(37); cout << friendUsername;

                // COLUMN 2: Real Name
                GoToXY(listX + 35, yPtr); SetColor(90); cout << "| ";
                SetColor(37); cout << friendRealName;

                yPtr++;
            }
        }

        // 7. Input Logic
        int key = _getch();

        if (key == 27) return;

        if (key == 224) {
            key = _getch();
            if (key == 72 && selection > 0) {
                selection--;
                if (selection < scrollOffset) scrollOffset--;
            }
            if (key == 80 && selection < friends.size() - 1) {
                selection++;
                if (selection >= scrollOffset + (listH - 8)) scrollOffset++;
            }
        }

        else if (tolower(key) == 'd') {
            if (!friends.empty()) {
                string target = friends[selection];

                GoToXY(listX + 5, listY + listH - 2);
                SetColor(31); cout << "Remove " << target << "? (Y/N): ";

                char confirm = _getch();
                if (tolower(confirm) == 'y') {
                    serviceRemoveFriend(users, currentUser, target);

                    GoToXY(listX + 5, listY + listH - 2);
                    SetColor(32); cout << "User removed successfully!     ";
                    Sleep(1000);

                    if (selection >= friends.size() && selection > 0) selection--;
                } else {
                    GoToXY(listX + 5, listY + listH - 2);
                    cout << "                                  ";
                }
            }
        }
    }
}

void showAddFriendMenu(User& currentUser, UserMap& users) {
    string searchQuery = "";
    vector<User> displayList = serviceSearchUsers(users, "");

    int selection = 0;
    int scrollOffset = 0;

    // --- GEOMETRY FIX (MATCHING CENTERED LAYOUT) ---
    int listX = 6, listY = 4, listW = 72, listH = 20;
    int sideX = listX + listW + 2, sideY = listY, sideW = 24, sideH = 12;

    while (true) {
        system("cls"); DrawStarField();

        // 1. Header
        DrawCard(listX, listY, listW, listH);
        GoToXY(listX + 2, listY + 2); SetColor(36);
        if(searchQuery.empty()) cout << "BROWSE USERS (" << displayList.size() << ")";
        else cout << "RESULTS (" << displayList.size() << ")";

        GoToXY(listX + 2, listY + 3); SetColor(90); for(int i=0; i<listW-4; i++) cout << "-";

        GoToXY(listX + 4, listY + 4); SetColor(37); cout << "Search: ";
        SetColor(33);
        if (searchQuery.empty()) cout << "[Type name...]";
        else cout << searchQuery;

        // 2. Column Headers
        GoToXY(listX + 2, listY + 5); SetColor(90); for(int i=0; i<listW-4; i++) cout << "-";
        GoToXY(listX + 5, listY + 6); SetColor(33); cout << "USERNAME";
        GoToXY(listX + 26, listY + 6); SetColor(33); cout << "FULL NAME";
        GoToXY(listX + 50, listY + 6); SetColor(33); cout << "BIO";

        // 3. Side Controls
        DrawCard(sideX, sideY, sideW, sideH);
        GoToXY(sideX + 6, sideY + 2); SetColor(36); cout << "ACTIONS";
        GoToXY(sideX + 2, sideY + 3); SetColor(90); for(int i=0; i<sideW-4; i++) cout << "-";

        GoToXY(sideX + 3, sideY + 5); SetColor(37); cout << "[A-Z]  "; SetColor(90); cout << " Type";
        GoToXY(sideX + 3, sideY + 6); SetColor(37); cout << "[ARROWS]"; SetColor(90); cout << " Move";
        GoToXY(sideX + 3, sideY + 8); SetColor(33); cout << "[ENTER]"; SetColor(90); cout << " Add";
        GoToXY(sideX + 3, sideY + 9); SetColor(32); cout << "[V]    "; SetColor(90); cout << " View";
        GoToXY(sideX + 3, sideY + 10); SetColor(31); cout << "[ESC]  "; SetColor(90); cout << " Back";

        // 4. Render List
        if (displayList.empty()) {
             GoToXY(listX + 25, listY + 10); SetColor(31); cout << "No users found.";
        }
        else {
            int yPtr = listY + 8;

            for (size_t i = scrollOffset; i < displayList.size(); i++) {
                if (yPtr >= listY + listH - 1) break;

                const User& u = displayList[i];

                bool isFriend = false;
                for(const string& f : currentUser.connections) if(f == u.username) isFriend = true;
                if (u.username == currentUser.username) isFriend = true;

                // Cursor
                if (i == selection) { SetColor(33); GoToXY(listX + 2, yPtr); cout << ">>"; }
                else { SetColor(90); }

                // Col 1: Username
                GoToXY(listX + 5, yPtr);
                if (isFriend) SetColor(32); else SetColor(37);
                cout << u.username;

                // Col 2: Real Name
                GoToXY(listX + 26, yPtr); SetColor(90); cout << "| ";
                SetColor(37); cout << u.realName.substr(0, 19);

                // Col 3: Bio
                string bio = u.description;
                if (bio == "DEFAULT_USER") bio = "";

                GoToXY(listX + 50, yPtr); SetColor(90); cout << "| ";
                SetColor(37); cout << bio.substr(0, 16);

                // Status Tag
                if (isFriend) {
                    GoToXY(listX + 68, yPtr); SetColor(32); cout << "(A)";
                }

                yPtr++;
            }
        }

        // 5. Input Handling
        int key = _getch();

        if (key == 27) return;

        if (key == 224) {
            key = _getch();
            if (key == 72 && selection > 0) {
                selection--; if(selection < scrollOffset) scrollOffset--;
            }
            if (key == 80 && selection < displayList.size()-1) {
                selection++; if(selection >= scrollOffset + (listH - 10)) scrollOffset++;
            }
        }
        else if (key == 13) {
            if (!displayList.empty()) {
                User& target = displayList[selection];
                string status = serviceAddConnection(users, currentUser, target.username);

                GoToXY(listX + 4, listY + listH - 2);
                if (status == "Success") {
                    SetColor(32); cout << "Added " << target.realName << "!              ";
                } else {
                    SetColor(31); cout << status << "              ";
                }
                Sleep(1000);
            }
        }
        else if (tolower(key) == 'v') {
             if (!displayList.empty()) {
                const User& u = displayList[selection];

                // POPUP CARD (Adjusted position)
                int popX = listX + 10, popY = listY + 5;
                DrawCard(popX, popY, 50, 10);

                GoToXY(popX + 18, popY + 2); SetColor(36); cout << "USER PROFILE";
                GoToXY(popX + 2, popY + 3); SetColor(90); for(int k=0; k<46; k++) cout << "-";

                GoToXY(popX + 4, popY + 4); SetColor(90); cout << "Username : "; SetColor(37); cout << u.username;
                GoToXY(popX + 4, popY + 5); SetColor(90); cout << "Full Name: "; SetColor(37); cout << u.realName;
                GoToXY(popX + 4, popY + 6); SetColor(90); cout << "Bio      : "; SetColor(33);

                if (u.description == "DEFAULT_USER") cout << "No bio available.";
                else cout << u.description;

                GoToXY(popX + 4, popY + 8); SetColor(37); cout << "Press any key to close...";
                _getch();
             }
        }
        else if (key == 8) {
            if (!searchQuery.empty()) {
                searchQuery.pop_back();
                displayList = serviceSearchUsers(users, searchQuery);
                selection = 0; scrollOffset = 0;
            }
        }
        else if (key >= 32 && key <= 126) {
            if (searchQuery.length() < 25) {
                searchQuery += (char)key;
                displayList = serviceSearchUsers(users, searchQuery);
                selection = 0; scrollOffset = 0;
            }
        }
    }
}