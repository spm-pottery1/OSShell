#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <ctime> 
#include "login.c++"
#include "colonel.c++"

// Reads credentials from the `login` file in the repository root.
// File format: one credential per line: "username password"
// Returns a map of username -> password.


//Making a change here as to try and generate a git put/commit


static void login_user(login &handler) {
    bool login = handler.authenticate();
    if (login) {
        std::cout << "Login successful!" << std::endl;
        handler.displayUserInfo();
    } else {
        std::cout << "Login failed." << std::endl;
    }
}
int main() {    

    login loginHandler;
    colonel shellCommander;
    std::cout << "DEBUG(MAIN) Colonel componets loaded correctly" << std::endl;

    login_user(loginHandler);
    while(loginHandler.isLoggedIn() == true) {
        std::cout << "Shell>> ";
        std::string command;
        std::getline(std::cin, command);
        if (command == "exit") {
            loginHandler.setLoggedIn(false);
            std::cout << "Logged out successfully." << std::endl;
        } else {
            // FIX: Pass the current user object to parseCommand
            shellCommander.parseCommand(command, loginHandler.getCurrentUser());
        }
    }
    return 0;
};