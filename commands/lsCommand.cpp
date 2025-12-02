#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "../file_system/directory.h"
#include "../file_system/user.h"
#include "command.h"

struct lsCommand : public command
{
public:
    // Constructor
    lsCommand(std::vector<std::string> args)
        : command("ls", args) {

        } 
    void execute(user &currentUser) override
    {
        const auto &args = getArgs();
        // ls takes no arguments
        if (!args.empty())
        {
            std::cerr << "Error: ls takes no args." << std::endl;
            return;
        }

        // 1. Get the current directory object
        const Directory &currentDir = currentUser.getCurrentDirectory();

        std::cout << "Contents of " << currentDir.getDirPath() << ":" << std::endl;

        // 2. List subdirectories (only direct children)
        std::cout << "Directories: ";
        if (currentDir.getSubdirectories().empty())
        {
            std::cout << "(none)";
        }
        else
        {
            std::cout << currentDir.getSubdirectoryNames();
        }
        std::cout << std::endl;

        // 3. List files (only files in this directory, NOT subdirectories)
        std::cout << "Files:";
        const auto& filesInDir = currentDir.getFiles();
        if (filesInDir.empty())
        {
            std::cout << "(none)";
        }
        else
        {

            std::cout << currentDir.getFileNames(true) << "\n";
        }
        std::cout << std::endl;
    }
};