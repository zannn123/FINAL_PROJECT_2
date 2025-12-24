#include "ui_core.h"
#include <iostream>
#include <windows.h>
#include <conio.h> // Required for _kbhit() and _getch()
#include <vector>

using namespace std;

// --- HIDDEN TOOLS ---
void GoToXY(int x, int y) { cout << "\033[" << y << ";" << x << "H"; }
void SetColor(int c) { cout << "\033[" << c << "m"; }
void HideCursor() { cout << "\033[?25l"; } // Hides the blinking underscore
void ShowCursor() { cout << "\033[?25h"; }

// --- BACKGROUND: STATIC STAR FIELD ---
void DrawStarField() {
    SetColor(90); // Dark Gray (Dim stars)
    for (int y = 1; y < 30; y++) {
        for (int x = 1; x < 120; x++) {
            // 2% chance to place a star initially
            if (rand() % 50 == 0) {
                // Determine star type
                int r = rand() % 3;
                if(r==0) cout << ".";
                else if(r==1) cout << "*";
                else cout << "+";
            } else {
                cout << " ";
            }
        }
        cout << "\n";
    }
}

// --- ANIMATION: TWINKLE EFFECT ---
// This picks a random spot. If it's a star, it changes color.
// If it's empty, it might make a new star.
void UpdateStars(int safeX, int safeY, int safeW, int safeH) {
    // Pick 5 random stars to update per frame
    for(int i=0; i<5; i++) {
        int x = (rand() % 118) + 1;
        int y = (rand() % 28) + 1;

        // CRITICAL: Don't draw ON TOP of the Login Box!
        // If x,y is inside the box area, skip it.
        if (x >= safeX && x <= safeX + safeW && y >= safeY && y <= safeY + safeH) {
            continue;
        }

        GoToXY(x, y);
        int magic = rand() % 20;

        if (magic == 0) { SetColor(37); cout << "*"; } // Flash Bright White
        else if (magic == 1) { SetColor(33); cout << "+"; } // Flash Yellow
        else if (magic == 2) { SetColor(90); cout << "."; } // Dim Gray
        else if (magic > 15) { cout << " "; } // Erase star (blink off)
    }
}

// --- UI: DRAW THE CARD ---
void DrawCard(int x, int y, int w, int h) {
    // Shadow
    SetColor(235); // Black/Dark
    for(int i=1; i<=h; i++) { GoToXY(x+w, y+i); cout << "||"; }
    GoToXY(x+2, y+h+1); for(int i=0; i<w; i++) cout << "=";

    // Box Body
    SetColor(36); // Cyan
    for (int i = 0; i <= h; i++) {
        GoToXY(x, y + i);
        if (i==0 || i==h) cout << "+"; else cout << "|";

        // Fill background with Black so stars don't show through text
        cout << "\033[40m";
        for(int j=0; j<w-2; j++) {
            if (i==0 || i==h) cout << "-"; else cout << " ";
        }
        cout << "\033[0m"; // Reset background
        SetColor(36);      // Restore border color

        if (i==0 || i==h) cout << "+"; else cout << "|";
    }

    // Title

    GoToXY(x + 10, y + 2);
    SetColor(37); cout << "SOCIAL MEDIA SYSTEM";
    GoToXY(x + 10, y + 3); cout << "===================";
}

// --- SMART INPUT LOOP (Handles Animation + Typing) ---
// This replaces 'cin >>'
string InputWithTwinkle(int x, int y, bool isPassword, int boxX, int boxY, int boxW, int boxH) {
    string input = "";

    // Ensure we are inside the bracket
    GoToXY(x, y);

    while (true) {
        // 1. ANIMATION STEP: Twinkle stars while waiting
        UpdateStars(boxX, boxY, boxW, boxH);

        // Return cursor to input box so user knows where they are
        GoToXY(x + input.length(), y);
        SetColor(33); // Yellow Text

        // 2. INPUT CHECK: Did the user press a key?
        if (_kbhit()) {
            char ch = _getch(); // Get the key

            if (ch == 13) { // ENTER key
                break;
            }
            else if (ch == 8) { // BACKSPACE key
                if (!input.empty()) {
                    // Visual delete
                    cout << "\b \b";
                    input.pop_back();
                }
            }
            else if (input.length() < 25) { // Limit length
                input += ch;
                if (isPassword) cout << "*";
                else cout << ch;
            }
        }

        // 3. SPEED CONTROL: Small delay to stop CPU from overheating
        Sleep(30);
    }
    return input;
}

//BELOW KAY MGA VIEWS NA!
LoginCredentials showLoginView() {
    HideCursor(); // Make it look cleaner

    // 1. Draw Static Background
    system("cls");
    DrawStarField();

    // 2. Draw Login Box
    int startX = 35, startY = 8, width = 40, height = 10;
    DrawCard(startX, startY, width, height);

    // Labels
    SetColor(37); // White
    GoToXY(startX + 5, startY + 5); cout << "User:";
    GoToXY(startX + 5, startY + 7); cout << "Pass:";

    // Brackets
    SetColor(90);
    GoToXY(startX + 11, startY + 5); cout << "[                         ]";
    GoToXY(startX + 11, startY + 7); cout << "[                         ]";

    LoginCredentials data;
    ShowCursor(); // Show cursor for typing

    // 3. GET INPUTS (Using our new Twinkle-Friendly Loop)
    // We pass the Box coordinates so stars don't draw over the text!
    data.username = InputWithTwinkle(startX + 12, startY + 5, false, startX, startY, width, height);
    data.password = InputWithTwinkle(startX + 12, startY + 7, true,  startX, startY, width, height);

    return data;
}

// --- CREATIVE LOADING SCREEN ---
void showLoadingView(const string& finalMessage) {
    system("cls");
    HideCursor();

    // Optional: Draw Background Stars if you have the function
    // DrawStarField();

    int startX = 35;
    int startY = 12;
    int barWidth = 40;

    // --- PART 1: GLITCH TEXT DECODING SOUNDS ---
    string currentMsg = "";
    for(int i=0; i<finalMessage.length(); i++) currentMsg += " ";

    for (int i = 0; i < finalMessage.length(); i++) {
        // "Scramble" the remaining letters
        for (int k = 0; k < 3; k++) {
            GoToXY(startX, startY - 2);
            SetColor(32); // Green
            cout << currentMsg.substr(0, i);

            SetColor(37); // White
            cout << (char)(rand() % 25 + 65);

            // SOUND EFFECT: High pitched digital "chirp"
            // Frequency: Random between 2000Hz and 2500Hz (Data noise)
            // Duration: 5ms (Very fast)
            // Note: Beep() pauses execution, so we remove Sleep(10) to keep it fast
            Beep(2000 + (rand() % 500), 5);
        }
        currentMsg[i] = finalMessage[i];
    }

    // Final print of the stable title
    GoToXY(startX, startY - 2);
    SetColor(37); cout << finalMessage;

    // Confirmation Sound for Text Done
    Beep(1500, 50);

    // --- PART 2: PROGRESS BAR RISING TONE ---
    for (int percent = 0; percent <= 100; percent+=2) {

        // Color Logic
        int color = 31;
        if(percent > 40) color = 33;
        if(percent > 80) color = 32;

        // Draw Bar
        GoToXY(startX, startY);
        SetColor(90); cout << "[";

        SetColor(color);
        int fill = (percent * barWidth) / 100;
        for (int j = 0; j < fill; j++) cout << "=";

        SetColor(37); cout << ">";

        SetColor(90);
        for (int j = fill; j < barWidth; j++) cout << " ";
        cout << "]";

        // Draw Percentage
        GoToXY(startX + barWidth + 3, startY);
        SetColor(color);
        cout << percent << "%";

        // Binary Rain
        GoToXY(startX, startY + 2);
        SetColor(90);
        cout << "System: ";
        for(int b=0; b<20; b++) cout << (rand()%2);

        // SOUND EFFECT: Rising "Charging" Tone
        // Frequency: Starts low (400Hz) and goes high (up to 2400Hz) based on percent
        int pitch = 400 + (percent * 20);

        // Play sound for 10ms
        Beep(pitch, 10);

        // We reduce Sleep() because Beep() already took 10ms
        Sleep(30);
    }

    // --- PART 3: SUCCESS SOUND ---
    // A nice "Ding-Dong" effect to show completion
    Sleep(200);
    Beep(2000, 100);
    Beep(2500, 150);

    Sleep(500);
    ShowCursor();
}