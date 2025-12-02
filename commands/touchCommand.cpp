#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cctype> 
#include "../file_system/directory.h"
#include "../file_system/user.h"
#include "command.h" 



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
        std::string filename = args[0]; // Make filename mutable

        // Map virtual root "/" -> physical base directory for this user
        std::filesystem::path base = "/home/simon/Documents/OSShellRoot";
        base.append(currentUser.getUsername());

        std::string vpath = currentUser.getCurrentDirectory().getDirPath();
        std::filesystem::path dir = base;
        if (vpath != "/")
        {
            std::string rel = vpath;
            if (!rel.empty() && rel.front() == '/') rel.erase(0,1);
            dir.append(rel);
        }

        // Incrament filename logic (only if the file exists)
        std::string baseFilename = filename;
        int counter = 0;
        std::string extension;
        std::filesystem::path filePath;

        // 1. Separate extension (if any)
        size_t dot_pos = baseFilename.rfind('.');
        if (dot_pos != std::string::npos) {
            extension = baseFilename.substr(dot_pos);
            baseFilename = baseFilename.substr(0, dot_pos);
        }

        // 2. Check for existing numeric suffix on the base name (e.g., script1)
        size_t i = baseFilename.length();
        while (i > 0 && std::isdigit(baseFilename[i - 1])) {
            i--;
        }

        // Extract existing counter if present
        if (i < baseFilename.length()) {
            try {
                std::string num_str = baseFilename.substr(i);
                counter = std::stoi(num_str);
                baseFilename = baseFilename.substr(0, i);
            } catch (...) {
                // Fallback to counter 0 if conversion fails
                counter = 0; 
            }
        }

        // 3. Loop and increment until unique
        filename = baseFilename + (counter > 0 ? std::to_string(counter) : "") + extension;
        filePath = dir; 
        filePath.append(filename); 


        // Construct the new filename: root name + counter + extension
        while (std::filesystem::exists(filePath)) {
            counter++;
            filename = baseFilename + std::to_string(counter) + extension;
            filePath = dir;
            filePath.append(filename);
        }

        // Ensure directories exist, then create the file
        std::error_code ec;
        std::filesystem::create_directories(dir, ec);
        if (ec) {
            std::cerr << "Error creating directories: " << ec.message() << std::endl;
            return;
        }

        // Use the final, unique filePath
        std::ofstream ofs(filePath.string(), std::ios::app);
        if (!ofs.is_open()) {
            std::cerr << "Error: cannot create file: " << filePath << std::endl;
            return;
        }
        ofs.close();

        // Create and add File object to in-memory directory. Also, unique filename for the virtual path
        int fid = currentUser.NextFileId();
        std::string filePath_virt = (vpath == "/" ? std::string("/") + filename : vpath + "/" + filename);
        File newFile(fid, filename, filePath_virt, currentUser.getUsername(), "rw", "");
        
        Directory &currentDir = const_cast<Directory&>(currentUser.getCurrentDirectory());
        currentDir.addFile(newFile);

        std::cout << "Created file: " << filePath << std::endl;
    }
};