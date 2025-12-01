# Social Media System (C Project)

## Project Overview
This is a console-based Social Media Application built using **Modular Programming** in C. The system features a distinct separation between User and Administrator functionalities, secure authentication, and persistent data storage using text files.

## 👥 Team Members
* **Leader / Architect:** ACLAO, GLORYZANN HORILLO  - *Core Architecture, Authentication, Data Persistence*
* **User Module Dev:** SALAGOSTE, DEVINE STA. IGLESIA - *Messaging System, Connections, Profile Management*
* **Admin Module Dev:** GALLLEMIT, SCARLET PIANG - *User Management, Security, Content Moderation*

## 🏗️ Modular Architecture
The project adheres to strict modular design principles to separate logic and UI:
* **Auth Module:** Handles Login, Registration, and Password Recovery.
* **User Module:** Handles Inbox, Sent folder, Announcements, and Friend Connections.
* **Admin Module:** "Top Secret" module for managing users, unlocking accounts, and filtering messages.
* **Data Module:** Custom file handling for `users.txt` and `messages.txt` (Pipe-separated values).
* **UI Core:** Handles ASCII visuals, password masking, and menu navigation.

## 🚀 Features Implemented
### User Features
- [x] Create Account & Secure Login
- [x] Compose Private & Group Messages
- [x] View Inbox & Sent Folders
- [x] Manage Personal Connections (Add/Remove)
- [x] Reply to Messages & View Announcements

### Admin Features
- [x] Secret Admin Login
- [x] View All Users (with Ciphered Passwords)
- [x] Unlock/Lock User Accounts
- [x] Reset User Passwords (Force-change logic)
- [x] Filter Messages by Sender/Recipient
- [x] Delete Users (updates message history to "DELETED USER")

## 🛠️ Technical Details
* **Language:** C (Standard C99)
* **Compiler:** GCC (MinGW) / compatible with Dev-C++
* **Data Storage:** `.txt` files (Pipe `|` delimiter)
* **Encryption:** Caesar Cipher (+3 shift) for password visualization.

## 📂 File Structure
```text
├── main.c           # Entry point and main controller
├── globals.h        # Struct definitions (User, Message)
├── auth.c/.h        # Authentication logic
├── user_module.c/.h # User functionality
├── admin_module.c/.h# Admin functionality
├── file_handler.c/.h# File I/O operations
├── ui_core.c/.h     # UI styling and input handling
└── users.txt        # Database file
