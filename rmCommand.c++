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

struct rmCommand : public command
{
public:
    rmCommand(std::vector<std::string> args)
        : command("rm", args) {}

    void execute(user &currentUser) override
    {
        const auto &args = getArgs();
        // Expect exactly one argument: the filename (no support for relative paths like ../file.txt yet)
        if (args.size() != 1)
        {
            std::cerr << "Usage: rm <filename>" << std::endl;
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
            dir.append(rel);
        }

        
        fs::path filePath = dir;
        filePath.append(filename);

        filePath = filePath.lexically_normal();


        std::error_code ec;
        if (!fs::exists(filePath, ec))
        {
            std::cerr << "Error: file not found: " << filePath << std::endl;
            return;
        }

        bool removed = fs::remove(filePath, ec);
        if (ec || !removed)
        {
            std::cerr << "Error removing file: " << (ec ? ec.message() : "unknown error") << std::endl;
            return;
        }

        std::cout << "Removed file: " << filePath << std::endl;

        // Refresh in-memory state so ls and directory lists reflect the deletion
        currentUser.loadFromDisk();
    }
};