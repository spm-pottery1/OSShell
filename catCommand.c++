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


namespace fs = std::filesystem;
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
        fs::path base = "/home/simon/Documents/OSShellRoot";
        base.append(currentUser.getUsername());

        // Determine physical directory that corresponds to the current virtual directory
        std::string vpath = currentUser.getCurrentDirectory().getDirPath();
        fs::path dir = base;
        if (vpath != "/")
        {
            std::string rel = vpath;
            if (!rel.empty() && rel.front() == '/')
                rel.erase(0, 1);
            // *** CHANGE 2 ***
            dir.append(rel);
        }

        fs::path filePath = dir;
        filePath.append(filename);

        // Check if file exists
        if (!fs::exists(filePath))
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

        std::cout << ifs.rdbuf();

        
        ifs.close();
    }
};