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
        // 1. FILTER: Get Only Parent Posts (Newest First)
        vector<Message> parents;
        for(const auto& msg : allMessages) {
            // We show it if it's an Announcement AND not a reply (doesn't start with RE:)
            if (msg.isAnnouncement && msg.subject.substr(0, 3) != "RE:") {
                parents.push_back(msg);
            }
        }
        reverse(parents.begin(), parents.end());

        // 2. SETUP DRAWING
        system("cls"); DrawStarField();

        int feedX = 10, feedY = 2, feedW = 75, feedH = 26;
        int sideX = feedX + feedW + 2, sideY = feedY, sideW = 30, sideH = 12;

        // 3. DRAW SIDEBAR CONTROLS
        DrawCard(sideX, sideY, sideW, sideH);
        GoToXY(sideX + 8, sideY + 2); SetColor(36); cout << "CONTROLS";
        GoToXY(sideX + 2, sideY + 3); SetColor(90); for(int i=0; i<sideW-4; i++) cout << "-";
        GoToXY(sideX + 4, sideY + 5); SetColor(37); cout << "[UP/DN]"; SetColor(90); cout << " Scroll";
        GoToXY(sideX + 4, sideY + 6); SetColor(37); cout << "[ENTER]"; SetColor(90); cout << " View Thread";
        GoToXY(sideX + 4, sideY + 7); SetColor(37); cout << "[R]    "; SetColor(90); cout << " Reply";
        GoToXY(sideX + 4, sideY + 8); SetColor(37); cout << "[C]    "; SetColor(90); cout << " New Post";
        GoToXY(sideX + 4, sideY + 9); SetColor(33); cout << "[D]    "; SetColor(90); cout << " Delete Post";
        GoToXY(sideX + 4, sideY + 10); SetColor(31); cout << "[ESC]  "; SetColor(90); cout << " Back";

        // 4. DRAW FEED HEADER
        DrawCard(feedX, feedY, feedW, feedH);
        GoToXY(feedX + 25, feedY + 2); SetColor(36); cout << "ANNOUNCEMENTS";
        GoToXY(feedX + 2, feedY + 3); SetColor(90); for(int i=0; i<feedW-4; i++) cout << "=";

        if (parents.empty()) { GoToXY(feedX + 25, feedY + 12); SetColor(90); cout << "No announcements yet."; }

        // 5. RENDER CARDS LOOP
        int yPointer = feedY + 5;
        int cardsRendered = 0;

        for (size_t i = scrollOffset; i < parents.size(); i++) {
            Message& post = parents[i];
            bool isSelected = (i == selection);

            // --- THE FIX: Define Clean Variables First ---
            string cleanText = getCleanContent(post.content);
            vector<string> groupMembers = getGroupParticipants(post.content);
            vector<string> lines = wrapText(cleanText, 60);

            // Get replies for preview
            vector<Message> replies = getReplies(post, allMessages);
            int replyCount = replies.size();
            int previewLines = (replyCount > 0) ? 2 : 0;

            // Calculate height
            int cardH = 2 + lines.size() + 1 + previewLines;

            // Stop if screen is full
            if (yPointer + cardH > feedY + feedH - 2) break;

            // Draw Cursor
            if (isSelected) { SetColor(33); GoToXY(feedX - 2, yPointer); cout << ">>"; } else { SetColor(90); }

            // Draw Card Top Line
            int cardX = feedX + 2;
            GoToXY(cardX, yPointer); for(int k=0; k<65; k++) cout << "-";

            // Draw Header: [Name] [GroupTag] posted: [Topic]
            GoToXY(cardX + 2, yPointer + 1);
            if (post.sender == currentUser.username) SetColor(33); else SetColor(32);
            cout << post.sender;

            // Draw Group Tag (if applicable)
            if (!groupMembers.empty()) {
                SetColor(36); cout << " (w/ " << groupMembers.size() << " others)";
            }

            SetColor(90); cout << " posted: "; SetColor(37); cout << post.subject;

            // Draw Content Body
            SetColor(90);
            for(size_t l=0; l<lines.size(); l++) {
                GoToXY(cardX + 2, yPointer + 2 + l);
                cout << lines[l];
            }

            // Draw Comment Preview (Footer)
            if (replyCount > 0) {
                int footerY = yPointer + 2 + lines.size();
                GoToXY(cardX + 2, footerY); SetColor(33); cout << "--- " << replyCount << " comment(s) ---";

                // Show last reply (Cleaned)
                Message& lastRep = replies.back();
                string cleanRep = getCleanContent(lastRep.content);

                GoToXY(cardX + 4, footerY + 1); SetColor(36); cout << lastRep.sender << ": ";
                SetColor(90); cout << cleanRep.substr(0, 40);
            }

            // Draw Card Bottom Line
            GoToXY(cardX, yPointer + cardH); for(int k=0; k<65; k++) cout << "-";

            yPointer += cardH + 1;
            cardsRendered++;
        }

        // 6. INPUT HANDLING
        int key = _getch();
        if (key == 27) return; // ESC -> Back to Main

        // Navigation
        if (key == 224) {
            key = _getch();
            if (key == 72 && selection > 0) { selection--; if (selection < scrollOffset) scrollOffset--; }
            if (key == 80 && selection < parents.size() - 1) { selection++; if (selection >= scrollOffset + cardsRendered - 1) scrollOffset++; }
        }

        // [C] Create Post
        else if (tolower(key) == 'c') {
            vector<Message> newMsgs = performCompose(currentUser, "", false);
            if (!newMsgs.empty()) {
                serviceAddMessage(allMessages, newMsgs[0]);
            }
        }

        // [R] Reply to Selected
        else if (tolower(key) == 'r') {
            if (!parents.empty()) {
                string sub = parents[selection].subject;
                if (sub.find("@") == string::npos) sub = "RE: " + sub + " (@" + parents[selection].sender + ")";

                vector<Message> newMsgs = performCompose(currentUser, sub, true);
                if (!newMsgs.empty()) {
                    serviceAddMessage(allMessages, newMsgs[0]);
                }
            }
        }

        // [D] Delete Post
        else if (tolower(key) == 'd') {
            if (!parents.empty()) {
                Message& target = parents[selection];
                if (target.sender == currentUser.username) {
                    GoToXY(feedX + 2, feedY + feedH - 2); SetColor(31); cout << "Delete this post? (Y/N): ";
                    char confirm = _getch();
                    if (tolower(confirm) == 'y') {
                        serviceDeleteMessage(allMessages, target);
                        selection = 0;
                        GoToXY(feedX + 25, feedY + 12); cout << "Deleting..."; Sleep(500);
                    }
                } else {
                    GoToXY(feedX + 2, feedY + feedH - 2); SetColor(31); cout << "You can only delete your own posts!"; Sleep(1000);
                }
            }
        }

        // [ENTER] View Full Thread
        else if (key == 13) {
            if (!parents.empty()) {
                Message& target = parents[selection];
                vector<Message> threadReplies = getReplies(target, allMessages);
                showThreadView(currentUser, target, threadReplies, allMessages, users);
            }
        }
    }
}

void showFullMessageView(const User& currentUser, const Message& selectedMsg, const vector<Message>& allMessages) {
    system("cls"); DrawStarField();

    // 1. GET HISTORY
    vector<Message> rawThread = getConversationThread(currentUser, selectedMsg.subject, allMessages);

    // --- FIXED DEDUPLICATION ---
    vector<Message> uniqueThread;
    vector<string> seenHashes;

    for (const auto& msg : rawThread) {
        // CLEAN THE CONTENT FIRST!
        // This removes "{GRP:paul,ivy}" so we just get "Hiiiiii"
        string cleanBody = getCleanContent(msg.content);

        // Create Fingerprint: SENDER + CLEAN_CONTENT
        // Now "zann2||Hiiiiii" matches "zann2||Hiiiiii" regardless of group tags
        string fingerprint = msg.sender + "||" + cleanBody;

        bool alreadyAdded = false;
        for (const string& h : seenHashes) {
            if (h == fingerprint) {
                alreadyAdded = true;
                break;
            }
        }

        if (!alreadyAdded) {
            uniqueThread.push_back(msg);
            seenHashes.push_back(fingerprint);
        }
    }
    // ---------------------------

    int x = 15, y = 3, w = 90, h = 22;
    DrawCard(x, y, w, h);

    // 2. HEADER
    GoToXY(x + 35, y + 2); SetColor(36); cout << "CONVERSATION HISTORY";
    GoToXY(x + 2, y + 3); SetColor(90); for(int i=0; i<w-4; i++) cout << "-";

    GoToXY(x + 4, y + 4); SetColor(37); cout << "Topic: ";
    SetColor(33); cout << selectedMsg.subject;

    GoToXY(x + 2, y + 5); SetColor(90); for(int i=0; i<w-4; i++) cout << "-";

    // 3. RENDER CHAT LOOP
    int yPtr = y + 6;

    for (const auto& msg : uniqueThread) {
        if (yPtr >= y + h - 2) break;

        string cleanContent = getCleanContent(msg.content);
        vector<string> group = getGroupParticipants(msg.content);
        vector<string> lines = wrapText(cleanContent, 75);

        // Sender Header
        GoToXY(x + 4, yPtr);
        if (msg.sender == currentUser.username) {
            SetColor(32); cout << "ME: ";
        } else {
            SetColor(36); cout << msg.sender << ": ";
        }

        if (!group.empty()) {
            SetColor(90); cout << "(to group) ";
        }

        yPtr++;

        // Message Body
        SetColor(37);
        for (const string& line : lines) {
            if (yPtr >= y + h - 2) break;
            GoToXY(x + 8, yPtr++);
            cout << line;
        }

        yPtr++;
    }

    // 4. FOOTER
    GoToXY(x + 2, y + h + 1); SetColor(90); cout << "[ESC] Back";

    while(true) {
        if (_getch() == 27) return;
    }
}

string selectFriendRecipients(const User& currentUser) {
    // 1. Get Friends List (Directly from the vector)
    // No need to parse commas because it's already a list!
    vector<string> friends = currentUser.connections;

    // Check if empty
    if (friends.empty()) {
        int x = 30, y = 10;
        DrawCard(x, y, 40, 5);
        GoToXY(x + 5, y + 2); SetColor(31); cout << "You have no added connections!";
        Sleep(1500);
        return "";
    }

    // 2. Selection Loop
    vector<bool> selected(friends.size(), false);
    int idx = 0;

    while (true) {
        system("cls"); DrawStarField();
        int x = 30, y = 5, w = 50, h = friends.size() + 8;
        DrawCard(x, y, w, h);

        GoToXY(x + 15, y + 2); SetColor(36); cout << "SELECT RECIPIENTS";
        GoToXY(x + 2, y + 3); SetColor(90); for(int i=0; i<w-4; i++) cout << "-";

        GoToXY(x + 4, y + 4); SetColor(37); cout << "[UP/DN] Move   [SPACE] Toggle   [ENTER] Confirm";

        for (size_t i = 0; i < friends.size(); ++i) {
            int row = y + 6 + i;
            GoToXY(x + 4, row);

            // Draw Selection Arrow
            if (i == idx) { SetColor(33); cout << ">> "; }
            else { cout << "   "; }

            // Draw Checkbox
            SetColor(37); cout << "[";
            if (selected[i]) { SetColor(32); cout << "X"; }
            else { cout << " "; }
            SetColor(37); cout << "] ";

            // Draw Name
            if (i == idx) SetColor(33); else SetColor(90);
            cout << friends[i];
        }

        int key = _getch();
        if (key == 224) {
            key = _getch();
            if (key == 72 && idx > 0) idx--;
            if (key == 80 && idx < friends.size() - 1) idx++;
        }
        else if (key == 32) { // SPACE -> Toggle
            selected[idx] = !selected[idx];
        }
        else if (key == 13) { // ENTER -> Confirm
            string result = "";
            for (size_t i = 0; i < friends.size(); ++i) {
                if (selected[i]) {
                    if (!result.empty()) result += ",";
                    result += friends[i];
                }
            }
            return result; // Returns "friend1,friend2"
        }
        else if (key == 27) return ""; // ESC -> Cancel
    }
}

// =========================================================
// UPDATED COMPOSE FUNCTION
// =========================================================
vector<Message> performPrivateCompose(const User& currentUser, const UserMap& users, string defaultRecipient = "", string defaultSubject = "") {
    bool isReply = !defaultSubject.empty();
    string recipient = defaultRecipient;

    // --- STEP 1: CHOOSE TYPE (If not replying) ---
    if (!isReply) {
        system("cls"); DrawStarField();
        int x = 30, y = 8, w = 50, h = 8;
        DrawCard(x, y, w, h);
        GoToXY(x + 15, y + 2); SetColor(36); cout << "COMPOSE MESSAGE";
        GoToXY(x + 5, y + 4); SetColor(37); cout << "[1] Personal Message (Manual Input)";
        GoToXY(x + 5, y + 5); SetColor(37); cout << "[2] Personal Connection (Group)";

        char choice = _getch();
        if (choice == '2') {
            // Open Friend Selector
            recipient = selectFriendRecipients(currentUser);
            if (recipient.empty()) return {}; // Cancelled or empty selection
        }
        else if (choice != '1') {
            return {}; // Cancel if they didn't pick 1 or 2
        }
        // If '1', recipient remains empty and we proceed to manual input below
    }

    // --- STEP 2: COMPOSE FORM ---
    system("cls"); DrawStarField();
    int x = 30, y = 5, w = 60, h = 18; DrawCard(x, y, w, h);

    GoToXY(x + 22, y + 2); SetColor(36); cout << "NEW MESSAGE";
    GoToXY(x + 2, y + 3); SetColor(90); for(int i=0; i<w-4; i++) cout << "-";

    // RECIPIENT FIELD
    GoToXY(x + 4, y + 5); SetColor(37); cout << "To (User)   : ";
    if (!recipient.empty()) {
        // If we selected friends or it's a reply, lock this field
        SetColor(33);
        if (recipient.length() > 35) cout << recipient.substr(0, 32) << "...";
        else cout << recipient;
    } else {
        // Manual Input (If 'Personal' was chosen)
        if (activeInput(x + 18, y + 5, recipient) == -1) return {};
    }

    string subject = defaultSubject;
    GoToXY(x + 4, y + 7); SetColor(37); cout << "Subject     : ";
    if (isReply) {
        SetColor(33); cout << subject;
    } else {
        if (activeInput(x + 18, y + 7, subject) == -1) return {};
    }

    // MESSAGE BODY
    GoToXY(x + 4, y + 9); SetColor(37); cout << "Message     : ";
    GoToXY(x + 4, y + 10); SetColor(90); for(int i=0; i<w-4; i++) cout << "-";
    GoToXY(x + 4, y + 15); SetColor(90); cout << "[ENTER] Send   [ESC] Cancel";
    GoToXY(x + 4, y + 11); SetColor(37);

    string content = "";
    while(true) {
        int key = _getch();
        if (key == 27) return {};
        else if (key == 13) { if (!content.empty()) break; }
        else if (key == 8) { if (!content.empty()) { content.pop_back(); cout << "\b \b"; } }
        else if (content.length() < 55 && key >= 32 && key <= 126) { content += (char)key; cout << (char)key; }
    }

    if (content.empty() || recipient.empty()) return {};

    // Validate Recipients (Check if they exist)
    stringstream ss(recipient);
    string name;
    bool allValid = true;
    while (getline(ss, name, ',')) {
        name.erase(remove(name.begin(), name.end(), ' '), name.end());
        if (users.find(name) == users.end()) {
             GoToXY(x + 20, y + 16); SetColor(31);
             cout << "User '" << name << "' not found!";
             Sleep(1000);
             allValid = false;
             break;
        }
    }
    if (!allValid) return {};

    Message newMsg;
    newMsg.sender = currentUser.username;
    newMsg.recipient = recipient;
    newMsg.subject = subject;
    newMsg.content = content;
    newMsg.isAnnouncement = false;

    return { newMsg };
}

// =========================================================
// MAIN UI: MESSENGER (INBOX & SENT)
// =========================================================
void showMessenger(const User& currentUser, vector<Message>& allMessages, const UserMap& users) {
    int selection = 0;
    int scrollOffset = 0;
    bool viewInbox = true;

    while(true) {
        vector<Message> displayList;
        if (viewInbox) displayList = getInboxMessages(currentUser, allMessages);
        else displayList = getSentMessages(currentUser, allMessages);

        system("cls"); DrawStarField();

        int feedX = 10, feedY = 2, feedW = 75, feedH = 26;
        int sideX = feedX + feedW + 2, sideY = feedY, sideW = 30, sideH = 12;

        // FIXED CONTROLS DRAWING
        DrawCard(sideX, sideY, sideW, sideH);
        GoToXY(sideX + 8, sideY + 2); SetColor(36); cout << "MESSENGER";
        GoToXY(sideX + 2, sideY + 3); SetColor(90); for(int i=0; i<sideW-4; i++) cout << "-";

        int descX = sideX + 13;
        GoToXY(sideX + 3, sideY + 5); SetColor(37); cout << "[UP/DN]"; GoToXY(descX, sideY + 5); SetColor(90); cout << "Scroll";
        GoToXY(sideX + 3, sideY + 6); SetColor(37); cout << "[ENTER]"; GoToXY(descX, sideY + 6); SetColor(90); cout << "Read Thread"; // Updated
        GoToXY(sideX + 3, sideY + 7); SetColor(37); cout << "[C]";     GoToXY(descX, sideY + 7); SetColor(90); cout << "Compose";
        GoToXY(sideX + 3, sideY + 8); SetColor(37); cout << "[R]";     GoToXY(descX, sideY + 8); SetColor(90); cout << "Reply All";
        GoToXY(sideX + 3, sideY + 9); SetColor(37); cout << "[D]";     GoToXY(descX, sideY + 9); SetColor(90); cout << "Delete";

        GoToXY(sideX + 3, sideY + 10);
        if(viewInbox) { SetColor(33); cout << "[S]"; GoToXY(descX, sideY + 10); SetColor(90); cout << "Sent Folder"; }
        else          { SetColor(32); cout << "[I]"; GoToXY(descX, sideY + 10); SetColor(90); cout << "Inbox"; }

        GoToXY(sideX + 3, sideY + 11); SetColor(31); cout << "[ESC]";  GoToXY(descX, sideY + 11); SetColor(90); cout << "Back";

        // INBOX DRAWING
        DrawCard(feedX, feedY, feedW, feedH);
        GoToXY(feedX + 25, feedY + 2);
        if (viewInbox) { SetColor(32); cout << "INBOX (Received)"; }
        else           { SetColor(33); cout << "SENT (Outbox)"; }
        GoToXY(feedX + 2, feedY + 3); SetColor(90); for(int i=0; i<feedW-4; i++) cout << "=";

        if (displayList.empty()) { GoToXY(feedX + 25, feedY + 12); SetColor(90); cout << "Folder is empty."; }

        int yPointer = feedY + 5;
        int cardsRendered = 0;

        for (size_t i = scrollOffset; i < displayList.size(); i++) {
            Message& msg = displayList[i];
            bool isSelected = (i == selection);

            string cleanText = getCleanContent(msg.content);
            vector<string> groupMembers = getGroupParticipants(msg.content);
            vector<string> lines = wrapText(cleanText, 60);

            int cardH = 2 + lines.size() + 1;
            if (yPointer + cardH > feedY + feedH - 2) break;

            if (isSelected) { SetColor(33); GoToXY(feedX - 2, yPointer); cout << ">>"; }
            else { SetColor(90); }

            int cardX = feedX + 2;
            GoToXY(cardX, yPointer); for(int k=0; k<65; k++) cout << "-";

            GoToXY(cardX + 2, yPointer + 1);
            if (viewInbox) { SetColor(32); cout << "From: " << msg.sender; }
            else { SetColor(33); cout << "To: " << msg.recipient; }

            if (!groupMembers.empty()) { SetColor(36); cout << " (Group)"; }

            SetColor(90); cout << " | Subject: "; SetColor(37); cout << msg.subject;

            SetColor(90);
            for(size_t l=0; l<lines.size(); l++) {
                GoToXY(cardX + 2, yPointer + 2 + l);
                cout << lines[l];
            }
            GoToXY(cardX, yPointer + cardH); for(int k=0; k<65; k++) cout << "-";
            yPointer += cardH + 1;
            cardsRendered++;
        }

        int key = _getch();
        if (key == 27) return;

        if (key == 224) {
            key = _getch();
            if (key == 72 && selection > 0) { selection--; if (selection < scrollOffset) scrollOffset--; }
            if (key == 80 && selection < displayList.size() - 1) { selection++; if (selection >= scrollOffset + cardsRendered - 1) scrollOffset++; }
        }

        // --- [ENTER] VIEW CONVERSATION THREAD ---
        else if (key == 13) {
            if (!displayList.empty()) {
                // Pass allMessages so we can find the thread history
                showFullMessageView(currentUser, displayList[selection], allMessages);
            }
        }

        else if (tolower(key) == 's' || tolower(key) == 'i') {
            viewInbox = !viewInbox;
            selection = 0; scrollOffset = 0;
        }

        // [C] Compose
        else if (tolower(key) == 'c') {
            vector<Message> newMsgs = performPrivateCompose(currentUser, users);
            if (!newMsgs.empty()) serviceAddMessage(allMessages, newMsgs[0]);
        }

        // [R] Reply All
        else if (tolower(key) == 'r') {
            if (!displayList.empty()) {
                Message& target = displayList[selection];
                string replyTo = "";

                if (viewInbox) {
                    vector<string> allRecipients;
                    allRecipients.push_back(target.sender);
                    vector<string> group = getGroupParticipants(target.content);
                    for(const string& member : group) allRecipients.push_back(member);

                    for (const string& person : allRecipients) {
                        if (person != currentUser.username) {
                            if (replyTo.length() > 0) replyTo += ", ";
                            replyTo += person;
                        }
                    }
                } else {
                    replyTo = target.recipient;
                    vector<string> group = getGroupParticipants(target.content);
                    for(const string& member : group) {
                         if (replyTo.find(member) == string::npos) replyTo += ", " + member;
                    }
                }

                // AUTO-FILL SUBJECT: Check if it already starts with RE:
                string replySub = target.subject;
                if (replySub.find("RE:") == string::npos) replySub = "RE: " + replySub;

                // Pass the subject so it gets LOCKED
                vector<Message> newMsgs = performPrivateCompose(currentUser, users, replyTo, replySub);
                if (!newMsgs.empty()) serviceAddMessage(allMessages, newMsgs[0]);
            }
        }

        else if (tolower(key) == 'd') {
            if (!displayList.empty()) {
                Message& target = displayList[selection];
                GoToXY(feedX + 2, feedY + feedH - 2); SetColor(31); cout << "Delete this message? (Y/N): ";
                if (tolower(_getch()) == 'y') {
                    serviceDeleteMessage(allMessages, target);
                    selection = 0;
                }
            }
        }
    }
}