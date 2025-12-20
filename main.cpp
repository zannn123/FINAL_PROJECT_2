#include <iostream>
#include "globals.h"
#include "file_handler.h"

using namespace std;

int main() {
    // 1. Declare the Hash Map
    UserMap users; 
    vector<Message> messages;

    // 2. Load Data (Hashing happens automatically here)
    loadUsers(users);
    loadMessages(messages);

    // --- DEMO: THE POWER OF HASHING ---
    // Finding a user is now INSTANT (No loops needed!)
    
    string target = "paul_castillo_0"; // One of your users

    if (users.find(target) != users.end()) {
        // FOUND THEM INSTANTLY!
        User& foundUser = users[target];
        cout << "Found: " << foundUser.realName << endl;
        cout << "Password: " << foundUser.password << endl;
    } else {
        cout << "User not found." << endl;
    }

    // 3. Save Data
    saveUsers(users);

    return 0;
}