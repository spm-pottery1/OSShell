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

struct touchCommand : public command
{
public:
    touchCommand(std::vector<std::string> args)
        : command("touch", args) {}

    void execute(user &currentUser) override
    {
        const auto &args = getArgs();
        if (args.size() != 1) {
            std::cerr << "Usage: touch <filename>" << std::endl;
            return;
        }
        const std::string& filename = args[0];

        // Map virtual root "/" -> physical base directory for this user
        std::filesystem::path base = "/home/simon/Documents/OSShellRoot";
        base /= currentUser.getUsername();

        // Determine physical directory that corresponds to the current virtual directory
        std::string vpath = currentUser.getCurrentDirectory().getDirPath();
        std::filesystem::path dir = base;
        if (vpath != "/")
        {
            std::string rel = vpath;
            if (!rel.empty() && rel.front() == '/') rel.erase(0,1);
            dir /= rel;
        }

        // Ensure directories exist, then create the file
        std::error_code ec;
        std::filesystem::create_directories(dir, ec);
        if (ec) {
            std::cerr << "Error creating directories: " << ec.message() << std::endl;
            return;
        }

        std::filesystem::path filePath = dir / filename;
        std::ofstream ofs(filePath.string(), std::ios::app);
        if (!ofs.is_open()) {
            std::cerr << "Error: cannot create file: " << filePath << std::endl;
            return;
        }
        ofs.close();

        // Create and add File object to in-memory directory
        int fid = currentUser.NextFileId();
        std::string filePath_virt = (vpath == "/" ? std::string("/") + filename : vpath + "/" + filename);
        File newFile(fid, filename, filePath_virt, currentUser.getUsername(), "rw", "");
        
        Directory &currentDir = const_cast<Directory&>(currentUser.getCurrentDirectory());
        currentDir.addFile(newFile);

        std::cout << "Created file: " << filePath << std::endl;
    }
};

