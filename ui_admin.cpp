#include "ui_admin.h"
#include "ui_core.h"
#include "file_handler.h"
#include "admin.h"
#include <iostream>
#include <conio.h>
#include <iomanip>
#include <vector>
#include <windows.h>

using namespace std;

int showAdminMenu() {
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    system("cls");
    HideCursor();
    DrawStarField();

    DrawCard(5, 2, 110, 4);
    DrawCard(5, 8, 30, 20);
    DrawCard(37, 8, 78, 20);

    SetColor(36);
    GoToXY(10, 4); cout << "SYSTEM MODE: ";
    SetColor(31);
    cout << "ADMINISTRATOR";

    GoToXY(45, 4); SetColor(36); cout << "ACCESS LEVEL: ";
    SetColor(33);
    cout << "Root / Full Control";

    GoToXY(80, 4); SetColor(36); cout << "SESSION: ";
    SetColor(32);
    cout << "Secure";

    const int MENU_SIZE = 4;
    string menuItems[MENU_SIZE] = {
        "Users",
        "Messages",
        "Security",
        "Logout"
    };

    int selection = 0;
    while(true) {
        for(int i=0; i<MENU_SIZE; i++) {
            int yPos = 12 + (i * 3);
            GoToXY(8, yPos);

            if(i == selection) {
                SetColor(31);
                cout << ">> " << menuItems[i];
            } else {
                SetColor(90);
                cout << "   " << menuItems[i];
            }
        }

        GoToXY(45, 12); SetColor(37); cout << "COMMAND DESCRIPTION:";
        GoToXY(45, 13); SetColor(90); cout << "--------------------";
        GoToXY(45, 15); SetColor(37);

        cout << "                                                        ";
        GoToXY(45, 15);

        if(selection == 0) cout << "Manage Accounts: View list, Ban users, or Reset passwords.";
        if(selection == 1) cout << "System Surveillance: Read or delete user messages.";
        if(selection == 2) cout << "Root Security: Change the Administrator password.";
        if(selection == 3) cout << "End Session: Return to the main Login Screen.";

        GoToXY(45, 24); SetColor(31);
        cout << "[!] WARNING: You have full access. Proceed with caution.";

        int key = _getch();

        if (key == 224) {
            key = _getch();
            if (key == 72) {
                selection--;
                if (selection < 0) selection = MENU_SIZE - 1;
                Beep(600, 20);
            }
            if (key == 80) {
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

int showUserSubMenu(const UserMap& users, string &targetUser) {
    system("cls");
    HideCursor();
    DrawStarField();

    // --- SETUP DATA FOR SCROLLING ---
    vector<string> userKeys;
    for (const auto& pair : users) {
        userKeys.push_back(pair.first);
    }

    int menuX = 5;
    int menuY = 6;
    int listX = 50;
    int listY = 6;

    // Menu Options
    const int MENU_SIZE = 5;
    string menuItems[MENU_SIZE] = {
        "Explore Users",    // 0
        "Modify Status",    // 1
        "Delete User",      // 2
        "Reset Password",   // 3
        "Back to Main"      // 4
    };

    int menuSelection = 0;
    int listSelection = 0;
    int scrollOffset = 0;
    bool focusOnList = false; // FALSE = Left Menu, TRUE = Right List

    while (true) {
        // --- DRAW LEFT CARD (MENU) ---
        int menuColor = focusOnList ? 90 : 36;
        DrawCard(menuX, menuY, 40, 18);
        GoToXY(menuX + 10, menuY + 2);
        SetColor(menuColor); cout << "USER ACTIONS";
        GoToXY(menuX + 2, menuY + 4);
        SetColor(90); for (int i = 0; i < 36; i++) cout << "-";

        for (int i = 0; i < MENU_SIZE; i++) {
            int yPos = menuY + 6 + (i * 2);
            GoToXY(menuX + 4, yPos);

            if (!focusOnList && i == menuSelection) {
                SetColor(31); // Active Selection
                cout << ">> " << menuItems[i] << " <<";
            }
            else {
                SetColor(focusOnList ? 90 : 37); // Dim if focus is on right
                cout << "   " << menuItems[i] << "   ";
            }
        }

        // --- DRAW RIGHT CARD (LIST) ---
        int listColor = focusOnList ? 36 : 90;
        DrawCard(listX, listY, 60, 18);
        GoToXY(listX + 20, listY + 2);
        SetColor(listColor); cout << "SELECT TARGET USER";
        GoToXY(listX + 2, listY + 4);
        SetColor(90); for (int i = 0; i < 56; i++) cout << "-";

        // Draw Visible Users (Limit 10)
        int maxVisible = 10;
        for (int i = 0; i < maxVisible; i++) {
            int dataIndex = scrollOffset + i;
            if (dataIndex >= userKeys.size()) break;

            string currentName = userKeys[dataIndex];
            const User& u = users.at(currentName);

            GoToXY(listX + 4, listY + 6 + i);

            // Logic for highlighting the selected row on the right
            if (focusOnList && dataIndex == listSelection) {
                SetColor(224); // Black text on white background (Inverted)
                cout << " > " << currentName;
                for(int s=0; s < 40 - currentName.length(); s++) cout << " ";
            }
            else {
                // Normal Color Logic
                if (u.isLocked) SetColor(90);
                else if (u.description == "ADMIN") SetColor(31);
                else SetColor(32);

                cout << "  " << currentName;
                if(u.isLocked) cout << " [LOCKED]";
                SetColor(0); cout << "                                ";
            }
        }

        // Scroll Indicators
        GoToXY(listX + 50, listY + 6);
        if(scrollOffset > 0) { SetColor(36); cout << "^"; } else cout << " ";

        GoToXY(listX + 50, listY + 15);
        if(scrollOffset + maxVisible < userKeys.size()) { SetColor(36); cout << "v"; } else cout << " ";


        // --- INFO BAR ---
        GoToXY(menuX + 2, menuY + 16); SetColor(90);
        cout << "                                    ";
        GoToXY(menuX + 2, menuY + 16);
        if(!focusOnList) {
            if(menuSelection == 0) cout << "INFO: View list (No Action).";
            if(menuSelection == 1) cout << "INFO: Select user to LOCK.";
            if(menuSelection == 2) cout << "INFO: Select user to DELETE.";
            if(menuSelection == 3) cout << "INFO: Select user to RESET.";
            if(menuSelection == 4) cout << "INFO: Return to menu.";
        } else {
            SetColor(33);
            cout << "Press ENTER to confirm target.";
        }

        // --- INPUT HANDLING ---
        int key = _getch();
        if (key == 224) {
            key = _getch();
            if (!focusOnList) {
                if (key == 72) { menuSelection--; if (menuSelection < 0) menuSelection = MENU_SIZE - 1; }
                if (key == 80) { menuSelection++; if (menuSelection >= MENU_SIZE) menuSelection = 0; }
            }
            else {
                if (key == 72) { // UP
                    if (listSelection > 0) {
                        listSelection--;
                        if (listSelection < scrollOffset) scrollOffset--;
                    }
                }
                if (key == 80) { // DOWN
                    if (listSelection < userKeys.size() - 1) {
                        listSelection++;
                        if (listSelection >= scrollOffset + maxVisible) scrollOffset++;
                    }
                }
            }
        }
        else if (key == 13) { // ENTER KEY
            if (!focusOnList) {
                if (menuSelection == 4) return 5;
                focusOnList = true;
            }
            else {
                targetUser = userKeys[listSelection];
                return menuSelection + 1;
            }
        }
        else if (key == 27) { // ESC KEY
            if (focusOnList) focusOnList = false;
            else return 5;
        }
    }
}

void showModifyUserMenu(Admin &admin, UserMap &users, string target) {
    if (users.find(target) == users.end()) return;
    User &u = users[target];
    while(true) {
        system("cls");
        SetColor(36); cout << "\n   EDITING USER: " << target << "\n";
        SetColor(90); cout << "   --------------------------------\n";
        SetColor(37); cout << "   [1] Edit Description  ";
        SetColor(33); cout << "(" << u.description << ")\n";
        SetColor(37); cout << "   [2] Add Friend        ";
        SetColor(35); cout << "(Cnt: " << u.connections.size() << ")\n";
        SetColor(37); cout << "   [3] Remove Friend     \n";
        SetColor(37); cout << "   [4] Toggle Lock       ";
        SetColor(u.isLocked ? 31 : 32); cout << "(" << (u.isLocked ? "LOCKED" : "ACTIVE") << ")\n";
        SetColor(37); cout << "   [5] Finish Editing\n";
        cout << "\n   Select Option: ";
        int choice;
        if (!(cin >> choice)) {
            cin.clear(); cin.ignore(1000, '\n'); choice = 0;
        }
        if (choice == 1) {
            cout << "   Enter new description: ";
            string desc;
            cin.ignore();
            getline(cin, desc);
            admin.updateDescription(u, desc);
            saveUsers(users); // <--- SAVED!
            cout << "   [SAVED] Description updated.\n";
        }
        else if (choice == 2) {
            cout << "   Enter username to add: ";
            string name;
            cin >> name;
            if (admin.addConnection(u, name, users)) {
                saveUsers(users); // <--- SAVED!
                cout << "   [SAVED] Added " << name << ".\n";
            } else {
                cout << "   [ERROR] Invalid user.\n";
            }
        }
        else if (choice == 3) {
            cout << "   Enter username to remove: ";
            string name;
            cin >> name;
            if (admin.removeConnection(u, name)) {
                saveUsers(users); // <--- SAVED!
                cout << "   [SUCCESS] Removed " << name << ".\n";
            } else {
                cout << "   [ERROR] Friend not found.\n";
            }
        }
        else if (choice == 4) {
            admin.toggleLock(u);
            saveUsers(users); // <--- SAVED!
            cout << "   [SUCCESS] Status toggled.\n";
        }
        else if (choice == 5) {
            break;
        }

        if(choice != 5) {
            SetColor(90); cout << "   Press any key to continue...";
            _getch();
        }
    }
}

bool confirmAdminPassword(string correctPassword) {
    // 1. Draw a Red "Danger" Popup
    int x = 40, y = 10;
    DrawCard(x, y, 40, 8); // Small box in center

    GoToXY(x + 10, y + 2);
    SetColor(31); // RED TEXT
    cout << "SECURITY CHECK";

    GoToXY(x + 2, y + 3);
    SetColor(90); for(int i=0; i<36; i++) cout << "-";

    GoToXY(x + 4, y + 5);
    SetColor(37); cout << "Confirm Pass: ";

    // 2. Get Input (Masked with *)
    string input = "";
    activeInput(x + 18, y + 5, input, true); // true = password mode

    // 3. Verify
    if (input == correctPassword) {
        return true;
    } else {
        GoToXY(x + 12, y + 7);
        SetColor(31); cout << "WRONG PASSWORD!";
        Beep(500, 200);
        Sleep(1000);
        return false;
    }
}

Message showMessageListUI(Admin& admin, std::vector<Message>& allMessages, const UserMap& users, std::string adminPass) {
    while (_kbhit()) _getch(); // Clear buffer
    system("cls");
    HideCursor();
    DrawStarField();

    int menuX = 5, menuY = 6;
    int listX = 50, listY = 6;
    int listSelection = 0;
    int scrollOffset = 0;

    // 1. CREATE A LOCAL COPY OF MESSAGES TO DISPLAY
    vector<Message> displayList = admin.filterBySender(allMessages);
    bool filterBySender = true;
    bool focusOnList = false;

    while (true) {
        // --- LEFT CARD (FILTERS) ---
        int menuColor = focusOnList ? 90 : 36;
        DrawCard(menuX, menuY, 40, 18);

        GoToXY(menuX + 10, menuY + 2); SetColor(menuColor); cout << "MESSAGE FILTERS";
        GoToXY(menuX + 2, menuY + 4); SetColor(90); for (int i = 0; i < 36; i++) cout << "-";

        // Filter Checkboxes
        GoToXY(menuX + 4, menuY + 7);
        SetColor(filterBySender ? 32 : 90);
        cout << (filterBySender ? "[X]" : "[ ]") << " By Sender Name";

        GoToXY(menuX + 4, menuY + 9);
        SetColor(!filterBySender ? 32 : 90);
        cout << (!filterBySender ? "[X]" : "[ ]") << " By Recipient Name";

        // --- DELETE INSTRUCTION (RED) ---
        GoToXY(menuX + 4, menuY + 12);
        if (focusOnList) {
            SetColor(31); cout << "Press [X] to DELETE Msg";

        } else {
            SetColor(90); cout << "                      "; // Clear text
        }

        // --- NAVIGATION INSTRUCTIONS (FIXED HERE) ---
        GoToXY(menuX + 4, menuY + 15);
        SetColor(37); // White Color

        if (focusOnList) {
            // Line 1:
            cout << "Press [TAB] to Filters  ";

            // Line 2: Move Cursor DOWN by 1 (to Y + 16)
            GoToXY(menuX + 4, menuY + 16);
            cout << "Press [ESC] to Exit     ";
        }
        else {
            // Line 1:
            cout << "Press [ENTER] View List ";

            // Line 2: Move Cursor DOWN by 1
            GoToXY(menuX + 4, menuY + 16);
            SetColor(31); // Red Color (optional, for global exit)
            cout << "Press [ESC] to Exit     ";
        }

        // --- RIGHT CARD (DATA LIST) ---
        int listColor = focusOnList ? 36 : 90;
        DrawCard(listX, listY, 65, 18);

        GoToXY(listX + 22, listY + 2); SetColor(listColor);
        cout << (filterBySender ? "FROM (SENDER)" : "TO (RECIPIENT)");

        GoToXY(listX + 4, listY + 5); SetColor(37);
        cout << "NAME             PREVIEW";

        if (displayList.empty()) {
             GoToXY(listX + 20, listY + 10); SetColor(31); cout << "NO MESSAGES FOUND";
        }

        // --- RENDER ROWS ---
        int maxVisible = 10;
        for (int i = 0; i < maxVisible; i++) {
            int dataIndex = scrollOffset + i;
            if (dataIndex >= displayList.size()) break;

            const Message& msg = displayList[dataIndex];

            string nameDisplay = filterBySender ? msg.sender : msg.recipient;
            bool isDeleted = (users.find(nameDisplay) == users.end());

            string bodyPreview = msg.content;
            if (bodyPreview.length() > 25) bodyPreview = bodyPreview.substr(0, 25) + "...";

            GoToXY(listX + 4, listY + 7 + i);

            if (focusOnList && dataIndex == listSelection) {
                SetColor(224); // Selected Highlight
                cout << " > " << std::left << std::setw(15) << (isDeleted ? "DELETED USER" : nameDisplay)
                     << " | " << bodyPreview;
                for(int s=0; s < 10; s++) cout << " ";
            }
            else {
                if (isDeleted) SetColor(31); else SetColor(32);
                cout << "   " << std::left << std::setw(15) << (isDeleted ? "DELETED USER" : nameDisplay);
                SetColor(90); cout << " | " << bodyPreview;
                cout << "          ";
            }
        }

        // --- INPUT HANDLING ---
        int key = _getch();

        if (key == 9) { // TAB
            filterBySender = !filterBySender;
            if (filterBySender) displayList = admin.filterBySender(allMessages);
            else displayList = admin.filterByRecipient(allMessages);
            listSelection = 0; scrollOffset = 0;
            Beep(400, 50);
        }
        else if (key == 13) { // ENTER
            if (!focusOnList) focusOnList = true;
            else return displayList[listSelection];
        }
        else if (key == 27) { // ESC
            if (focusOnList) focusOnList = false; // Go back to filters
            else return Message(); // Return empty message (triggers exit in Main)
        }
        // DELETE LOGIC (Press X)
        else if (focusOnList && (tolower(key) == 'x')) {
            if (!displayList.empty()) {
                Message target = displayList[listSelection];
                if (confirmAdminPassword(adminPass)) {
                    if (admin.deleteMessage(allMessages, target)) {
                        Message signal;
                        signal.sender = "__DELETE_SIGNAL__";
                        return signal;
                    }
                }
                system("cls"); DrawStarField();
            }
        }
        // ARROW KEYS
        else if (key == 224) {
             key = _getch();
             if (focusOnList) {
                if (key == 72 && listSelection > 0) {
                    listSelection--;
                    if (listSelection < scrollOffset) scrollOffset--;
                }
                if (key == 80 && listSelection < displayList.size() - 1) {
                    listSelection++;
                    if (listSelection >= scrollOffset + maxVisible) scrollOffset++;
                }
             }
        }
    }
}

void showFullMessage(const Message& msg, const UserMap& users) {
    system("cls"); // Clear screen to show this view exclusively
    HideCursor();
    DrawStarField(); // Keep the background consistent

    int x = 30; // Centered
    int y = 5;
    int w = 70; // Wider to fit text
    int h = 20; // Taller for the body

    // 1. Draw Container
    DrawCard(x, y, w, h);

    // 2. Header
    GoToXY(x + 25, y + 2);
    SetColor(36); cout << "MESSAGE DETAILS";

    GoToXY(x + 2, y + 3);
    SetColor(90); for(int i=0; i<w-4; i++) cout << "-";

    // 3. Metadata Fields
    // --- SENDER ---
    GoToXY(x + 4, y + 5);
    SetColor(37); cout << "From       :";

    // Check if sender is deleted to color it Red
    if (users.find(msg.sender) == users.end()) {
        SetColor(31); cout << " DELETED USER";
        SetColor(90); cout << " (was " << msg.sender << ")";
    } else {
        SetColor(32); cout << " " << msg.sender;
    }

    // --- RECIPIENT ---
    GoToXY(x + 4, y + 7);
    SetColor(37); cout << "To         :";
    SetColor(32); cout << " " << msg.recipient;

    // --- SUBJECT ---
    GoToXY(x + 4, y + 9);
    SetColor(37); cout << "Subject    :";
    SetColor(33); cout << " " << msg.subject;

    // 4. Message Body Area
    GoToXY(x + 2, y + 11);
    SetColor(90); for(int i=0; i<w-4; i++) cout << "-";

    GoToXY(x + 4, y + 12);
    SetColor(36); cout << "Content :";

    // 5. Text Wrapping Logic
    // This loop prints characters one by one. If it hits the right edge,
    // it moves down to the next line so the box doesn't break.
    SetColor(37);
    int currentX = x + 4;
    int currentY = y + 14;
    int rightLimit = x + w - 4;
    int bottomLimit = y + h - 2;

    GoToXY(currentX, currentY);

    for (char c : msg.content) {
        cout << c;
        currentX++;

        // If we hit the right wall, move down
        if (currentX >= rightLimit) {
            currentX = x + 4;
            currentY++;

            // Stop printing if we run out of vertical space
            if (currentY >= bottomLimit) break;

            GoToXY(currentX, currentY);
        }
    }

    // 6. Footer
    GoToXY(x + 20, y + h + 2);
    SetColor(90); cout << "Press any key to return to list...";
    _getch();
}