#pragma once
#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>
#include "user.c++"

class login {
    private:
        std::string checkUsername;
        std::string filePath = "/home/simon/Documents/OSShellRoot/users.txt"; 
        user currentUser = user("root", "root");
        bool loggedIn = false;
    
    public:
        login() {
            checkFirstLogin();
        }

        void displayUserInfo() const {
            if (loggedIn == true) {
                std::cout << currentUser.toString() << std::endl;
            } else {
                std::cout << "No user is currently logged in." << std::endl;
            }
        }
        user& getCurrentUser() {
            return currentUser;
        }

        void setLoggedIn(bool status) {
            loggedIn = status;
        }

        bool isLoggedIn() const {
            return loggedIn;
        }

        std::string getFilePath() const {
            return filePath;
        }

        void checkFirstLogin() {
            // 1. Get the directory path from the file path (Should be /home/simon/Documents/OSShellRoot)
            std::filesystem::path file_path_obj(filePath);
            std::filesystem::path dir_path_obj = file_path_obj.parent_path();

            // 2. Check if the directory exists and create it if it doesn't.
            if (!std::filesystem::exists(dir_path_obj)) {
                try {
                    std::filesystem::create_directories(dir_path_obj);
                    std::cout << "Created directory: " << dir_path_obj.string() << std::endl;
                } catch (const std::exception& e) {
                    std::cout << "Error creating directory: " << e.what() << std::endl;
                    return; 
                }
            }

            // 3. Check if the users.txt file exists and create it if it doesn't.
            std::ifstream usersFileCheck(filePath);
            if (!usersFileCheck.is_open()) {
                std::ofstream usersFile(filePath);
                if (usersFile.is_open()) {
                    usersFile << "root root" << std::endl; 
                    usersFile.close();
                    std::cout << "Created new users file with default root user." << std::endl;
                } else {
                    std::cout << "Error: Unable to create users file at: " << filePath << std::endl;
                }
            }
        }
        
        // Helper function to check if a user exists and return the user object
        user checkUser(const std::string& inputUsername) {
            std::ifstream usersFile(filePath);
            if (!usersFile.is_open()) {
                std::cerr << "Error: Unable to open users file." << std::endl;
                return user("", ""); 
            }

            std::string line;
            std::string storedUsername, storedPassword;
            while (std::getline(usersFile, line)) {
                std::istringstream iss(line);
                if (iss >> storedUsername >> storedPassword) {
                    if (storedUsername == inputUsername) {
                        // Found user. Create and return the user object.
                        usersFile.close();
                        return user(storedUsername, storedPassword); 
                    }
                }
            }
            usersFile.close();
            // User not found. Return a user object with empty username.
            return user("", ""); 
        }

        // Authentication process method
        bool authenticate() {
            std::string inputUsername;
            std::string inputPassword;
            
            // --- Username Validation ---
            do {
                std::cout << "Enter username: ";
                std::cin >> inputUsername;

                // Try to find the user in the file
                currentUser = checkUser(inputUsername);
                
                // If user not found 
                if (currentUser.getUsername() == "") {
                    std::cout << "User not found or file error. Please try again." << std::endl;
                }

            // Keep looping while the username is empty/not found
            } while (currentUser.getUsername() == ""); 

            // Username accepted
            std::cout << "Username accepted: " << currentUser.getUsername() << std::endl; 

            // --- Password Check ---//
            std::cout << "Enter password: ";
            std::cin >> inputPassword;
 
            // Check if the retrieved user's password matches the input
            setLoggedIn(currentUser.getPassword() == inputPassword);
            return isLoggedIn();

        }

        // Password authentication method
        bool passAuthenticate() {
            
            std::string inputPassword;
            std::cout << "Enter password: ";
            std::cin >> inputPassword;

            return (currentUser.getPassword() == inputPassword);
        }

};