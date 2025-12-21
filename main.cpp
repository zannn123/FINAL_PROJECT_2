#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include "globals.h"      // Contains UserMap and Message structs
#include "file_handler.h"  // Contains loadUsers and loadMessages

using namespace std;

int main() {
    // 1. DATA STRUCTURES
    UserMap users;
    vector<Message> messages;

    // 2. LOAD DATA
    // This pulls from your user.txt and the newly generated message.txt
    loadUsers(users);
    loadMessages(messages);

    cout << "============================================================" << endl;
    cout << "                USER OUTBOX VERIFICATION REPORT             " << endl;
    cout << "============================================================" << endl;

    if (users.empty()) {
        cout << "[ERROR] No users found. Check your user.txt file path." << endl;
        return 1;
    }

    // 3. ITERATE THROUGH EVERY USER
    for (auto const& [username, data] : users) {
        cout << "\n[USER]: " << left << setw(15) << data.username
             << " | NAME: " << data.realName << endl;

        bool foundSent = false;
        int sentCount = 0;

        // 4. FILTER MESSAGES BY SENDER
        for (const auto& msg : messages) {
            if (msg.sender == username) {
                // If this is the first message found, print a header
                if (!foundSent) {
                    cout << "  MESSAGES SENT TO:" << endl;
                    foundSent = true;
                }

                sentCount++;
                cout << "    (" << sentCount << ") To: " << left << setw(15) << msg.recipient
                     << " | Content: " << msg.content << endl;
            }
        }

        if (!foundSent) {
            cout << "  (No messages sent by this user)" << endl;
        }

        cout << "------------------------------------------------------------" << endl;
    }

    cout << "\n[SYSTEM] Scan complete. Total Users: " << users.size() << endl;
    cout << "Press Enter to close..." << endl;
    cin.get();

    return 0;
}