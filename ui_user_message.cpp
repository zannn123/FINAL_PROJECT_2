#include "ui_user_message.h"
#include "ui_core.h"
#include "user.h"     // <--- CONNECTS TO LOGIC LAYER (Service functions)
#include <iostream>
#include <conio.h>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <sstream>
#include <windows.h>

using namespace std;

// =========================================================
// HELPER FUNCTIONS
// =========================================================

// --- WRAP TEXT: Splits long strings into lines for card display ---
vector<string> wrapText(string text, int limit) {
    vector<string> lines;
    string currentLine = "";
    string word;
    stringstream ss(text);
    while (ss >> word) {
        if (currentLine.length() + word.length() + 1 > limit) {
            lines.push_back(currentLine);
            currentLine = word + " ";
        } else {
            currentLine += word + " ";
        }
    }
    if (!currentLine.empty()) lines.push_back(currentLine);
    return lines;
}

// --- GET REPLIES: Finds comments for a specific post ---
vector<Message> getReplies(const Message& parent, const vector<Message>& allMessages) {
    vector<Message> replies;
    string genericTarget = "RE: " + parent.subject;
    string specificTarget = "@" + parent.sender;

    for (const auto& msg : allMessages) {
        if (!msg.isAnnouncement) continue;

        // Check 1: Is it a reply to this Subject?
        bool subjectMatch = (msg.subject.find(parent.subject) != string::npos) && (msg.subject.find("RE:") != string::npos);

        // Check 2: Does it tag the specific User?
        bool userMatch = (msg.subject.find(specificTarget) != string::npos);

        // Logic: If subject is generic "General", we NEED the user tag.
        if (parent.subject == "General") {
            if (subjectMatch && userMatch) replies.push_back(msg);
        } else {
            if (subjectMatch) replies.push_back(msg);
        }
    }
    return replies;
}

// --- COMPOSE POPUP: Handles Input for New Posts & Replies ---
vector<Message> performCompose(const User& currentUser, string defaultSubject, bool isReply) {
    system("cls"); DrawStarField();
    int x = 30, y = 5, w = 60, h = 18; DrawCard(x, y, w, h);

    GoToXY(x + 20, y + 2); SetColor(36); cout << (isReply ? "WRITE A COMMENT" : "NEW ANNOUNCEMENT");
    GoToXY(x + 2, y + 3); SetColor(90); for(int i=0; i<w-4; i++) cout << "-";

    string subject = defaultSubject;
    GoToXY(x + 4, y + 7); SetColor(37); cout << "Topic    : ";
    if (isReply) {
        SetColor(90);
        if (subject.length() > 35) cout << subject.substr(0, 32) << "...";
        else cout << subject;
    }
    else {
        activeInput(x + 15, y + 7, subject);
    }

    GoToXY(x + 4, y + 9); SetColor(37); cout << "Message  : ";
    GoToXY(x + 4, y + 10); SetColor(90); for(int i=0; i<w-4; i++) cout << "-";
    GoToXY(x + 4, y + 15); SetColor(90); cout << "[ENTER] Post   [ESC] Cancel";
    GoToXY(x + 4, y + 11); SetColor(37);

    string content = "";
    while(true) {
        int key = _getch();
        if (key == 27) return {}; // ESC -> Cancel
        else if (key == 13) { if (!content.empty()) break; } // ENTER -> Finish
        else if (key == 8) { if (!content.empty()) { content.pop_back(); cout << "\b \b"; } }
        else if (content.length() < 55 && key >= 32 && key <= 126) { content += (char)key; cout << (char)key; }
    }

    if (content.empty() || subject.empty()) return {};

    Message newMsg;
    newMsg.sender = currentUser.username;
    newMsg.recipient = "ALL";
    newMsg.subject = subject;
    newMsg.content = content;
    newMsg.isAnnouncement = true;

    return { newMsg };
}

// =========================================================
// SUB-VIEW: THREAD DETAILS
// =========================================================
void showThreadView(const User& currentUser, const Message& parentPost, const vector<Message>& allReplies, vector<Message>& allMessages, const UserMap& users) {
    int scroll = 0;
    while(true) {
        system("cls"); DrawStarField();

        // 1. Draw Parent Post
        int px = 20, py = 2, pw = 80; DrawCard(px, py, pw, 8);
        GoToXY(px + 2, py + 1); SetColor(36); cout << parentPost.sender;
        SetColor(90); cout << " posted: " << parentPost.subject;

        vector<string> parentBody = wrapText(parentPost.content, 70);
        for(size_t i=0; i<parentBody.size() && i<4; i++) {
            GoToXY(px + 2, py + 3 + i); SetColor(37); cout << parentBody[i];
        }

        // 2. Draw Replies List
        int rx = 20, ry = 11, rw = 80, rh = 16; DrawCard(rx, ry, rw, rh);
        GoToXY(rx + 2, ry - 1); SetColor(33); cout << "COMMENTS (" << allReplies.size() << ")";

        if (allReplies.empty()) {
            GoToXY(rx + 30, ry + 8); SetColor(90); cout << "No comments yet.";
        } else {
            int yPtr = ry + 2;
            for(size_t i = scroll; i < allReplies.size(); i++) {
                if (yPtr >= ry + rh - 2) break; // Stop if full
                const Message& r = allReplies[i];
                GoToXY(rx + 4, yPtr); SetColor(32); cout << r.sender << ": ";
                SetColor(90); cout << r.content.substr(0, 60);
                yPtr++;
            }
        }

        // 3. Footer
        GoToXY(rx, ry + rh + 1); SetColor(37); cout << "[ESC] Back   [R] Add Comment   [UP/DOWN] Scroll";

        int key = _getch();
        if (key == 27) return;

        if (key == 224) {
            key = _getch();
            if (key == 72 && scroll > 0) scroll--;
            if (key == 80 && scroll < (int)allReplies.size() - 5) scroll++;
        }

        // --- REPLY INSIDE THREAD ---
        if (tolower(key) == 'r') {
            string sub = parentPost.subject;
            if (sub.find("@") == string::npos) sub = "RE: " + sub + " (@" + parentPost.sender + ")";

            vector<Message> newMsgs = performCompose(currentUser, sub, true);
            if (!newMsgs.empty()) {
                // ARCHITECTURE CALL: UI -> Logic Layer -> File
                serviceAddMessage(allMessages, newMsgs[0]);

                // Recursively refresh to show new comment immediately
                showThreadView(currentUser, parentPost, getReplies(parentPost, allMessages), allMessages, users);
                return;
            }
        }
    }
}

// =========================================================
// MAIN UI: FACEBOOK FEED (Service Layer Implemented)
// =========================================================
void showAnnouncementFeed(const User& currentUser, vector<Message>& allMessages, const UserMap& users) {
    int selection = 0;
    int scrollOffset = 0;

    while(true) {
        // FILTER: Only Parent Posts
        vector<Message> parents;
        for(const auto& msg : allMessages) {
            if (msg.isAnnouncement && msg.subject.substr(0, 3) != "RE:") {
                parents.push_back(msg);
            }
        }
        reverse(parents.begin(), parents.end());

        system("cls"); DrawStarField();

        int feedX = 10, feedY = 2, feedW = 75, feedH = 26;
        int sideX = feedX + feedW + 2, sideY = feedY, sideW = 30, sideH = 12;

        // SIDEBAR CONTROLS
        DrawCard(sideX, sideY, sideW, sideH);
        GoToXY(sideX + 8, sideY + 2); SetColor(36); cout << "CONTROLS";
        GoToXY(sideX + 2, sideY + 3); SetColor(90); for(int i=0; i<sideW-4; i++) cout << "-";
        GoToXY(sideX + 4, sideY + 5); SetColor(37); cout << "[UP/DN]"; SetColor(90); cout << " Scroll";
        GoToXY(sideX + 4, sideY + 6); SetColor(37); cout << "[ENTER]"; SetColor(90); cout << " View Thread";
        GoToXY(sideX + 4, sideY + 7); SetColor(37); cout << "[R]    "; SetColor(90); cout << " Reply";
        GoToXY(sideX + 4, sideY + 8); SetColor(37); cout << "[C]    "; SetColor(90); cout << " New Post";
        GoToXY(sideX + 4, sideY + 9); SetColor(33); cout << "[D]    "; SetColor(90); cout << " Delete Post";
        GoToXY(sideX + 4, sideY + 10); SetColor(31); cout << "[ESC]  "; SetColor(90); cout << " Back";

        // FEED DISPLAY
        DrawCard(feedX, feedY, feedW, feedH);
        GoToXY(feedX + 25, feedY + 2); SetColor(36); cout << "ANNOUNCEMENTS";
        GoToXY(feedX + 2, feedY + 3); SetColor(90); for(int i=0; i<feedW-4; i++) cout << "=";

        if (parents.empty()) { GoToXY(feedX + 25, feedY + 12); SetColor(90); cout << "No announcements yet."; }

        int yPointer = feedY + 5;
        int cardsRendered = 0;

        for (size_t i = scrollOffset; i < parents.size(); i++) {
            Message& post = parents[i];
            bool isSelected = (i == selection);

            vector<Message> replies = getReplies(post, allMessages);
            int replyCount = replies.size();
            vector<string> lines = wrapText(post.content, 60);
            int previewLines = (replyCount > 0) ? 2 : 0;
            int cardH = 2 + lines.size() + 1 + previewLines;

            if (yPointer + cardH > feedY + feedH - 2) break;

            if (isSelected) { SetColor(33); GoToXY(feedX - 2, yPointer); cout << ">>"; } else { SetColor(90); }

            // Draw Card Body
            int cardX = feedX + 2;
            GoToXY(cardX, yPointer); for(int k=0; k<65; k++) cout << "-";
            GoToXY(cardX + 2, yPointer + 1);
            if (post.sender == currentUser.username) SetColor(33); else SetColor(32);
            cout << post.sender; SetColor(90); cout << " posted: "; SetColor(37); cout << post.subject;

            SetColor(90);
            for(size_t l=0; l<lines.size(); l++) { GoToXY(cardX + 2, yPointer + 2 + l); cout << lines[l]; }

            // Draw Preview
            if (replyCount > 0) {
                int footerY = yPointer + 2 + lines.size();
                GoToXY(cardX + 2, footerY); SetColor(33); cout << "--- " << replyCount << " comment(s) ---";
                Message& lastRep = replies.back();
                GoToXY(cardX + 4, footerY + 1); SetColor(36); cout << lastRep.sender << ": "; SetColor(90); cout << lastRep.content.substr(0, 40);
            }

            GoToXY(cardX, yPointer + cardH); for(int k=0; k<65; k++) cout << "-";
            yPointer += cardH + 1;
            cardsRendered++;
        }

        int key = _getch();
        if (key == 27) return; // Exit to Main

        // --- NAVIGATION ---
        if (key == 224) {
            key = _getch();
            if (key == 72 && selection > 0) { selection--; if (selection < scrollOffset) scrollOffset--; }
            if (key == 80 && selection < parents.size() - 1) { selection++; if (selection >= scrollOffset + cardsRendered - 1) scrollOffset++; }
        }

        // --- [C] CREATE POST ---
        else if (tolower(key) == 'c') {
            vector<Message> newMsgs = performCompose(currentUser, "", false);
            if (!newMsgs.empty()) {
                // ARCHITECTURE CALL: UI -> Logic Layer -> File
                serviceAddMessage(allMessages, newMsgs[0]);
            }
        }

        // --- [R] REPLY ---
        else if (tolower(key) == 'r') {
            if (!parents.empty()) {
                string sub = parents[selection].subject;
                if (sub.find("@") == string::npos) sub = "RE: " + sub + " (@" + parents[selection].sender + ")";

                vector<Message> newMsgs = performCompose(currentUser, sub, true);
                if (!newMsgs.empty()) {
                    // ARCHITECTURE CALL: UI -> Logic Layer -> File
                    serviceAddMessage(allMessages, newMsgs[0]);
                }
            }
        }

        // --- [D] DELETE ---
        else if (tolower(key) == 'd') {
            if (!parents.empty()) {
                Message& target = parents[selection];
                if (target.sender == currentUser.username) {
                    GoToXY(feedX + 2, feedY + feedH - 2); SetColor(31); cout << "Delete this post? (Y/N): ";
                    char confirm = _getch();
                    if (tolower(confirm) == 'y') {
                        // ARCHITECTURE CALL: UI -> Logic Layer -> File
                        // Note: Logic Layer handles recursive deletion
                        serviceDeleteMessage(allMessages, target);

                        selection = 0; // Reset to avoid index errors
                        GoToXY(feedX + 25, feedY + 12); cout << "Deleting..."; Sleep(500);
                    }
                } else {
                    GoToXY(feedX + 2, feedY + feedH - 2); SetColor(31); cout << "You can only delete your own posts!"; Sleep(1000);
                }
            }
        }

        // --- [ENTER] VIEW THREAD ---
        else if (key == 13) {
            if (!parents.empty()) {
                Message& target = parents[selection];
                vector<Message> threadReplies = getReplies(target, allMessages);

                showThreadView(currentUser, target, threadReplies, allMessages, users);
            }
        }
    }
}