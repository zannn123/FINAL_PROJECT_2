

---

# 📟 ConsoleConnect (C++ Social Platform)

> **"Who needs a mouse when you have a keyboard?"**
> A fully functional social media platform built entirely in the **C++ Console**. No external GUI libraries. No drag-and-drop tools. Just raw logic, memory management, and ASCII art.

---

## 📸 Preview

<img width="955" height="481" alt="image" src="https://github.com/user-attachments/assets/2d566b6d-dc5f-4bf0-bd3d-6fae1edd2108" />

---

## 🚀 Why This Project?

In an era of React and Flutter, we went back to the roots. The goal of this project was not just to build a chat app, but to master the **fundamentals of Computer Engineering**:

* **Memory Management:** Passing heavy data structures (`UserMap`) by reference to avoid copies.
* **Data Structures:** Using `std::unordered_map` for O(1) user lookups and `std::vector` for message history.
* **Security:** Implementing custom **Caesar Cipher** encryption for password storage.
* **Architecture:** A strict separation of **UI (Frontend)** and **Logic (Backend)** within a console environment.

---

## 🛠️ Tech Stack & Features

### The Core Engine

* **Language:** C++ (Standard 11+)
* **Interface:** Custom TUI (Text-Based User Interface) using `ui_core.h`.
* **Database:** Local File I/O (`.txt` / `.csv`) for persistence.

### Key Features

| Role | Capabilities |
| --- | --- |
| **User** | • Real-time Messaging Simulation<br>

<br>• News Feed Navigation<br>

<br>• Profile Customization<br>

<br>• Secure Login/Registration |
| **Admin** | • User Management Table (Sort/Filter)<br>

<br>• Ban/Unban Logic<br>

<br>• System Oversight |
| **System** | • **Flicker-Free Rendering:** Optimized cursor movement (`GoToXY`) logic.<br>

<br>• **Input Masking:** Password fields display `****`. |

---

## 🏗️ Architecture (The "No Globals" Rule)

We adhered to strict coding standards to avoid global variables. The application uses **Dependency Injection** starting from `main()`.

```cpp
// Simplified Architecture Flow

int main() {
    // 1. The Single Source of Truth (Created in Stack)
    UserMap users; 
    vector<Message> messages;

    // 2. Data Loading (File -> Memory)
    loadUsers(users);

    // 3. Injection (Memory -> Modules)
    // The 'users' map is passed by reference (&) to ensure
    // changes in the Admin panel reflect instantly in the User panel.
    showLoginMenu(users, messages); 
}

```

---

## 📂 Project Structure

* `main.cpp` - The entry point and memory initializer.
* `globals.h` - **Type Definitions Only** (Structs for `User`, `Message`). No variables.
* `ui_core.cpp` - The graphics engine (Boxes, Colors, Cursor handling).
* `user.cpp` - Frontend logic for the User interface.
* `admin.cpp` - Frontend logic for the Admin panel.
* `service.cpp` - The backend logic (File handling, Encryption, Validation).

---

## 👥 The Team

We split the development based on a **Micro-service** philosophy:

* **👑 The Architect (Leader)**
* Designed the System Architecture.
* Built the `ui_core` (Visuals, Input Handling, Navigation).
* Integrated the Frontend and Backend.


* **🛡️ The Admin Logic Dev**
* Developed the Admin algorithms (Sorting users, Banning logic).
* Implemented Security protocols.


* **💬 The User Experience Dev**
* Built the Messaging System logic.
* Designed the User Profile and Feed interactions.



---

## 💿 Installation & Usage

1. **Clone the repo:**
```bash
git clone https://github.com/yourusername/console-connect.git

```


2. **Compile:**
* Open in **DevC++** or **VS Code**.
* Ensure all `.cpp` files are linked.
* Compile using `g++ *.cpp -o app`.


3. **Run:**
* Launch `app.exe`.
* Use **Arrow Keys** to navigate and **Enter** to select.



---

### 📝 Note on "Global Variables"

*This project strictly avoids global variables. `globals.h` is used exclusively for `struct` definitions and shared constants, ensuring thread-safety concepts and clean memory management.*

FOR ADMIN LOG IN:
username: admin
password: admin123
---

*Made with ❤️ and a lot of caffeine by BSCPE JRMSU.*
