
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "directory.c++"
#include "user.c++"
#include "command.c++" // Include the base definition

// Redefining the struct for abstract base class 'command' to inherit from
// This structure is necessary because 'command.c++' is not a header.
struct catCommand : public command
{
public:
    catCommand(std::vector<std::string> args)
        : command("cat", args) {}

    void execute(user &currentUser) override
    {
        const auto &args = getArgs();
        if (args.size() != 1)
        {
            std::cerr << "Usage: cat <filename>" << std::endl;
            return;
        }
        const std::string &filename = args[0];

        // Map virtual root "/" -> physical base directory for this user
        std::filesystem::path base = "/home/simon/Documents/OSShellRoot";
        base /= currentUser.getUsername();

        // Determine physical directory that corresponds to the current virtual directory
        std::string vpath = currentUser.getCurrentDirectory().getDirPath();
        std::filesystem::path dir = base;
        if (vpath != "/")
        {
            std::string rel = vpath;
            if (!rel.empty() && rel.front() == '/')
                rel.erase(0, 1);
            dir /= rel;
        }

        std::filesystem::path filePath = dir / filename;

        // Check if file exists
        if (!std::filesystem::exists(filePath))
        {
            std::cerr << "Error: File not found: " << filePath << std::endl;
            return;
        }

        // Open and read the file
        std::ifstream ifs(filePath.string());
        if (!ifs.is_open())
        {
            std::cerr << "Error: cannot open file for reading: " << filePath << std::endl;
            return;
        }

        std::string line;
        while (std::getline(ifs, line))
        {
            std::cout << line << std::endl;
        }
        ifs.close();
    }
};