#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "directory.c++"
#include "user.c++"
#include "command.c++"

struct cdCommand : public command
{
public:
    cdCommand(std::vector<std::string> args)
        : command("cd", args) {} // Initialize base with name and args

    void execute(user &currentUser) override
    {
        const auto &args = getArgs();
        // Expect exactly one argument: the target directory name or ".."
        if (args.empty() || args.size() > 1)
        {
            std::cerr << "Error: Usage: cd [directoryName] or cd .." << std::endl;
            return;
        }

        const std::string &targetDirName = args[0];

        // 1. Check for '..' (parent directory)
        if (targetDirName == "..")
        {
            // If the CWD is the root (index 0), don't move up.
            if (currentUser.getCurrentDirectory().getDirName() != "/")
            {
                std::string currentPath = currentUser.getCurrentDirectory().getDirPath();
                size_t lastSlash = currentPath.find_last_of('/');
                std::string parentPathName = currentPath.substr(0, lastSlash);

                if (parentPathName.empty())
                {
                    parentPathName = "/";
                }

                // Search for the directory index matching the parent's full path
                int targetIndex = -1;
                for (size_t i = 0; i < currentUser.getDirectories().size(); ++i)
                {
                    if (currentUser.getDirectories()[i].getDirPath() == parentPathName)
                    {
                        targetIndex = i;
                        break;
                    }
                }

                if (targetIndex != -1)
                {
                    currentUser.setCurrentDirectoryIndex(targetIndex);
                    std::cout << "Changed directory to: " << parentPathName << std::endl;
                    return;
                }
                else if (currentUser.getCurrentDirectory().getDirName() != "/")
                {
                    // Fallback if parent is root "/"
                    currentUser.setCurrentDirectoryIndex(0);
                    std::cout << "Changed directory to: /" << std::endl;
                    return;
                }
            }
            else
            {
                std::cout << "Already at the root directory." << std::endl;
                return;
            }
        }

        // 2. Check for a subdirectory name

        // Check if the target is in the current directory's list of subdirectories
        const std::vector<Directory> &subdirs = currentUser.getCurrentDirectory().getSubdirectories();

        for (const auto &subdir : subdirs)
        {
            if (subdir.getDirName() == targetDirName)
            {
                // Found it. Now find its index in the user's master list
                int targetIndex = currentUser.findDirectoryIndexByName(targetDirName);
                if (targetIndex != -1)
                {
                    currentUser.setCurrentDirectoryIndex(targetIndex);
                    std::cout << "Changed directory to: " << targetDirName << std::endl;
                    return;
                }
            }
        }

        std::cerr << "Error: Directory not found: " << targetDirName << std::endl;
    }
};