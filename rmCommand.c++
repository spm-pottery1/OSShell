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


struct rmCommand : public command
{
public:
    rmCommand(std::vector<std::string> args)
        : command("rm", args) {}

    void execute(user &currentUser) override
    {
        const auto &args = getArgs();
        // Expect exactly one argument: the filename
        if (args.size() != 1)
        {
            std::cerr << "Usage: rm <filename>" << std::endl;
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

        std::error_code ec;
        if (!std::filesystem::exists(filePath, ec))
        {
            std::cerr << "Error: file not found: " << filePath << std::endl;
            return;
        }

        bool removed = std::filesystem::remove(filePath, ec);
        if (ec || !removed)
        {
            std::cerr << "Error removing file: " << (ec ? ec.message() : "unknown error") << std::endl;
            return;
        }

        std::cout << "Removed file: " << filePath << std::endl;

        // Refresh in-memory state so ls and directory lists reflect the deletion
        // loadFromDisk() is public on user and will rebuild directories/files from disk.
        currentUser.loadFromDisk();
    }
};