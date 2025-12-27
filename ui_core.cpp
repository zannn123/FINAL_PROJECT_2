#include "ui_core.h"
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <vector>

using namespace std;

// --- INTERNAL HELPERS ---
HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

void GoToXY(int x, int y) { cout << "\033[" << y << ";" << x << "H"; }
void SetColor(int c) { cout << "\033[" << c << "m"; }
void HideCursor() { cout << "\033[?25l"; }
void ShowCursor() { cout << "\033[?25h"; }

// --- MOUSE SETUP (Strengthened) ---
void initMouse() {
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hIn, &mode);

    // Force OFF QuickEdit (Prevents freezing)
    mode &= ~ENABLE_QUICK_EDIT_MODE;

    // Force ON Mouse Input + Extended Flags
    mode |= ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS;

    SetConsoleMode(hIn, mode);
}

// --- BUTTON DRAWER ---
void drawButton(int x, int y, string text) {
    SetColor(44); // Blue Background
    SetColor(37); // White Text
    GoToXY(x, y); cout << " " << text << " ";
    SetColor(0);  // Reset
}

// --- BACKGROUND EFFECTS ---
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
    for(int i=0; i<5; i++) {
        int x = (rand() % 118) + 1;
        int y = (rand() % 28) + 1;
        if (x >= safeX && x <= safeX + safeW && y >= safeY && y <= safeY + safeH) continue;
        GoToXY(x, y);
        int magic = rand() % 20;
        if (magic == 0) { SetColor(37); cout << "*"; }
        else if (magic == 1) { SetColor(33); cout << "+"; }
        else if (magic == 2) { SetColor(90); cout << "."; }
        else if (magic > 15) { cout << " "; }
    }
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

// --- SMART INPUT (Fail-Safe Version) ---
string InputWithTwinkle(int x, int y, bool isPassword, int boxX, int boxY, int boxW, int boxH) {
    string input = "";
    INPUT_RECORD ir[128];
    DWORD nRead;
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

    // Button Coordinates
    int btnY = boxY + 11;
    int createX_Start = boxX + 2, createX_End = boxX + 20;
    int forgotX_Start = boxX + 22, forgotX_End = boxX + 45;

    GoToXY(x, y);

    while (true) {
        UpdateStars(boxX, boxY, boxW, boxH);

        // Ensure cursor is at typing position
        GoToXY(x + input.length(), y);
        SetColor(33); // Yellow Text

        GetNumberOfConsoleInputEvents(hIn, &nRead);

        if (nRead > 0) {
            ReadConsoleInput(hIn, ir, 128, &nRead);

            for (size_t i = 0; i < nRead; i++) {

                // [1] MOUSE CHECK
                if (ir[i].EventType == MOUSE_EVENT &&
                   (ir[i].Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED)) {

                    int mx = ir[i].Event.MouseEvent.dwMousePosition.X + 1;
                    int my = ir[i].Event.MouseEvent.dwMousePosition.Y + 1;

                    // Expanded Y-Range (Hit row above/below button too)
                    if (my >= btnY - 1 && my <= btnY + 1) {
                        if (mx >= createX_Start && mx <= createX_End) return "_BTN_CREATE_";
                        if (mx >= forgotX_Start && mx <= forgotX_End) return "_BTN_FORGOT_";
                    }
                }

                // [2] KEYBOARD CHECK
                if (ir[i].EventType == KEY_EVENT && ir[i].Event.KeyEvent.bKeyDown) {
                    char ch = ir[i].Event.KeyEvent.uChar.AsciiChar;
                    WORD vk = ir[i].Event.KeyEvent.wVirtualKeyCode; // Virtual Key Code

                    // --- FAIL-SAFE SHORTCUTS ---
                    if (vk == VK_F1) return "_BTN_CREATE_"; // Press F1 to Create Account
                    if (vk == VK_F2) return "_BTN_FORGOT_"; // Press F2 for Forgot Password
                    // ---------------------------

                    if (ch == 13) return input; // Enter Key

                    if (ch == 8) { // Backspace
                        if (!input.empty()) {
                            cout << "\b \b";
                            input.pop_back();
                        }
                    }
                    else if ((isalnum(ch) || ispunct(ch) || ch == ' ') && input.length() < 25) {
                        input += ch;
                        if (isPassword) cout << "*";
                        else cout << ch;
                    }
                }
            }
        }
        Sleep(30);
    }
}

// --- LOGIN VIEW ---
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

    // [DRAW BUTTONS WITH HINTS]
    drawButton(startX + 2, startY + 11, "F1: Create Account"); // Added "F1" Hint
    drawButton(startX + 22, startY + 11, "F2: Forgot Pass");   // Added "F2" Hint

    LoginCredentials data;
    ShowCursor();

    // 1. INPUT USERNAME
    data.username = InputWithTwinkle(startX + 12, startY + 5, false, startX, startY, width, height);
    if (data.username == "_BTN_CREATE_") { data.action = "CREATE"; return data; }
    if (data.username == "_BTN_FORGOT_") { data.action = "FORGOT"; return data; }

    // 2. INPUT PASSWORD
    data.password = InputWithTwinkle(startX + 12, startY + 7, true, startX, startY, width, height);
    if (data.password == "_BTN_CREATE_") { data.action = "CREATE"; return data; }
    if (data.password == "_BTN_FORGOT_") { data.action = "FORGOT"; return data; }

    data.action = "LOGIN";
    return data;
}

// --- LOADING SCREEN (Keep this) ---
void showLoadingView(const string& finalMessage) {
    system("cls");
    HideCursor();

    // --- FIX 1: WAKE UP THE SPEAKERS ---
    // Play a silent sound to force the audio driver to wake up
    // before the real animation starts.
    Beep(37, 200); // 37Hz is very low (sub-bass), often inaudible but active

    int startX = 35;
    int startY = 12;
    int barWidth = 40;

    // --- GLITCH TEXT LOOP ---
    string currentMsg = "";
    for(int i=0; i<finalMessage.length(); i++) currentMsg += " ";

    for (int i = 0; i < finalMessage.length(); i++) {
        for (int k = 0; k < 3; k++) {
            GoToXY(startX, startY - 2);
            SetColor(32); cout << currentMsg.substr(0, i);
            SetColor(37); cout << (char)(rand() % 25 + 65);

            // FIX 2: Longer duration (40ms) so it doesn't get skipped
            Beep(2000 + (rand() % 500), 40);
        }
        currentMsg[i] = finalMessage[i];
    }
    GoToXY(startX, startY - 2);
    SetColor(37); cout << finalMessage;

    // Confirmation Sound
    Beep(1000, 100);

    // --- PROGRESS BAR LOOP ---
    for (int percent = 0; percent <= 100; percent+=5) {
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

        // FIX 3: Consistent rhythm
        // Pitch rises from 400Hz to 1900Hz
        Beep(400 + (percent * 15), 500);

        // Small pause to let the sound "breathe"
        Sleep(10);
    }

    Sleep(200);
    ShowCursor();
}

// --- ADMIN MENU ---
void showAdminMenu() {
    system("cls");
    cout << "====================================\n";
    cout << "      ADMINISTRATOR DASHBOARD       \n";
    cout << "====================================\n";
    cout << "[1] View Users\n";
    cout << "[2] Modify Users\n";
    cout << "[3] Delete Users\n";
    cout << "[4] View Messages\n";
    cout << "[5] Logout\n";
    cout << "------------------------------------\n";
    cout << "Enter choice: ";
}

// --- FORGOT FORM ---
void showForgotForm() {
    system("cls");
    DrawStarField();
    int startX = 35, startY = 8, w = 40, h = 10;
    DrawCard(startX, startY, w, h);
    SetColor(37);
    GoToXY(startX + 12, startY + 2); cout << "RESET PASSWORD";
    GoToXY(startX + 12, startY + 3); cout << "--------------";
    GoToXY(startX + 4, startY + 6); cout << "Enter Username: [               ]";
    GoToXY(startX + 21, startY + 6);
    string dummy;
    cin >> dummy;
}

// --- CREATE ACCOUNT FORM ---
void showCreateAccountForm() {
    system("cls");
    DrawStarField();
    int startX = 30, startY = 5, w = 50, h = 18;
    DrawCard(startX, startY, w, h);
    SetColor(37);
    GoToXY(startX + 15, startY + 2); cout << "CREATE NEW ACCOUNT";
    GoToXY(startX + 15, startY + 3); cout << "------------------";
    GoToXY(startX + 4, startY + 5);  cout << "Name:       [                         ]";
    GoToXY(startX + 4, startY + 7);  cout << "Username:   [                         ]";
    GoToXY(startX + 4, startY + 9);  cout << "Password:   [                         ]";
    GoToXY(startX + 4, startY + 11); cout << "Re-Enter:   [                         ]";
    GoToXY(startX + 4, startY + 13); cout << "Security Q: [                         ]";
    SetColor(90);
    GoToXY(startX + 4, startY + 16); cout << "(Press ENTER to submit)";
    GoToXY(startX + 17, startY + 5);
    string dummy;
    cin >> dummy;
}

