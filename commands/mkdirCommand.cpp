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

struct mkdirCommand : public command
{
public:
    mkdirCommand(std::vector<std::string> args)
        : command("mkdir", args) {} // Initialize base with name and args

    void execute(user &currentUser) override
    {
        const auto &args = getArgs();
        // Expect exactly one argument: the directory name
        if (args.size() != 1)
        {
            std::cerr << "Error: Incorrect number of arguments. Usage: mkdir [directoryName]" << std::endl;
            return;
        }

        const std::string &dirName = args[0];

        // 1. Get the properties from the current session
        std::string owner = currentUser.getUsername();
        int newId = currentUser.NextDirId();

        // Construct the new path based on the CWD
        std::string parentPath = currentUser.getCurrentDirectory().getDirPath();
        std::string newPath = parentPath + (parentPath == "/" ? "" : "/") + dirName;

        // 2. Create and add the Directory object
        Directory newDir(newId, dirName, newPath, owner);

        // 3. Add to user's master list (which also updates CWD's subdirectory list in the user struct)
        currentUser.addDirectory(newDir);

        // 4. Create the physical directory on disk
        std::filesystem::path base = "/home/simon/Documents/OSShellRoot";
        base /= currentUser.getUsername();

        std::string vpath = parentPath;
        std::filesystem::path dir = base;
        if (vpath != "/")
        {
            std::string rel = vpath;
            if (!rel.empty() && rel.front() == '/')
                rel.erase(0, 1);
            dir /= rel;
        }

        std::error_code ec;
        std::filesystem::create_directories(dir / dirName, ec);
        if (ec)
        {
            std::cerr << "Error creating physical directory: " << ec.message() << std::endl;
            return;
        }

        // 5. Output confirmation
        std::cout << "Successfully created directory: " << dirName << std::endl;
        std::cout << "New Path: " << newPath << std::endl;
        std::cout << "Total directories for user: " << currentUser.getDirectories().size() << std::endl;
    }
};