#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "../file_system/directory.h"
#include "../file_system/user.h"
#include "command.h" 



struct addUserCommand : public command
{
public:
    // Constructor initializes the base class with the command name and arguments
    addUserCommand(std::vector<std::string> args)
        : command("adduser", args) {}

    // The required execute implementation
    void execute(user &currentUser) override
    {
        // 1. Get the arguments vector
        const auto &args = getArgs(); // Inherited method

        // 2. Check for the correct number of arguments (must be exactly 1: the username)
        if (args.size() != 1)
        {
            std::cerr << "Error: Incorrect number of arguments. Usage: adduser [username] " << std::endl;
            return;
        }
        const std::string &username = args[0];

        std::cout << "Executing adduser command for user: " << username << std::endl;

        // --- File Operations ---
        std::string filePath = "/home/simon/Documents/OSShellRoot/users.txt";

        // Check if user already exists by reading the file (if it exists)
        
            std::ifstream usersFileRead(filePath);
            if (usersFileRead.is_open())
            {
                std::string line;
                while (std::getline(usersFileRead, line))
                {
                    std::istringstream iss(line);
                    std::string existingUser;
                    if (iss >> existingUser)
                    {
                        if (existingUser == username)
                        {
                            std::cerr << "Error: User already exists: " << username << std::endl;
                            return;
                        }
                    }
                }
            }

        // Now open for append and add the new user
        std::ofstream usersFile(filePath, std::ios::app);
        if (!usersFile.is_open())
        {
            std::cerr << "Error: Unable to open or create users file at: " << filePath << std::endl;
            return;
        }

        // 4. Prompt for and read the password
        std::cout << "Enter user password: ";
        std::string password;
        std::cin >> password;

        // 5. Append username and password (separated by a space, on a new line)
        usersFile << username << " " << password << std::endl;

        // 6. Close the file and confirm
        usersFile.close();
        std::cout << "User added successfully: " << username << std::endl;
    }
};