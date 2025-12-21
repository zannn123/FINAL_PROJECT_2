#include <iostream>
#include <vector>
#include <string>
#include "globals.h"
#include "file_handler.h"

using namespace std;

int main() {
    // 1. Initialize data structures
    UserMap users;
    vector<Message> messages;

    // 2. Load data from text files
    loadUsers(users);
    loadMessages(messages);

    cout << "==========================================================" << endl;
    cout << "                MASTER MESSAGE LOG REPORT                 " << endl;
    cout << "==========================================================" << endl;

    // 3. DISPLAY EVERY MESSAGE IN THE DATABASE
    if (messages.empty()) {
        cout << "[!] No messages found in message.txt" << endl;
    } else {
        cout << "TOTAL MESSAGES FOUND: " << messages.size() << "\n" << endl;

        for (size_t i = 0; i < messages.size(); i++) {
            const auto& m = messages[i];

            cout << "Message #" << (i + 1) << ":" << endl;
            cout << "  FROM:    " << m.sender << " (" << (users.count(m.sender) ? users[m.sender].realName : "Unknown") << ")" << endl;
            cout << "  TO:      " << m.recipient << " (" << (users.count(m.recipient) ? users[m.recipient].realName : "Unknown") << ")" << endl;
            cout << "  SUBJECT: " << m.subject << endl;
            cout << "  CONTENT: " << m.content << endl;
            cout << "  TYPE:    " << (m.isAnnouncement ? "ANNOUNCEMENT" : "PRIVATE CHAT") << endl;
            cout << "----------------------------------------------------------" << endl;
        }
    }

    // 4. USER-CENTRIC VIEW (RELATIONSHIP CHECK)
    cout << "\n\n==========================================================" << endl;
    cout << "             USER RELATIONSHIP SUMMARY                    " << endl;
    cout << "==========================================================" << endl;

    for (auto const& [username, data] : users) {
        cout << "\nUSER: " << data.username << " (" << data.realName << ")" << endl;

        bool found = false;
        for (const auto& msg : messages) {
            // Check if current user is involved in the message
            if (msg.sender == username || msg.recipient == username) {
                string partner = (msg.sender == username) ? msg.recipient : msg.sender;
                string tag = (msg.sender == username) ? "[SENT TO " : "[RCVD FROM ";

                cout << "  -> " << tag << partner << "]: " << msg.content << endl;
                found = true;
            }
        }

        if (!found) cout << "  -> (No history)" << endl;
    }

    return 0;
}