#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "../file_system/directory.h"
#include "../file_system/user.h"
#include "command.h" 

// Redefining the struct for abstract base class 'command' to inherit from
// This structure is necessary because 'command.c++' is not a header.
struct chmodCommand : public command
{
public:
    chmodCommand(std::vector<std::string> args)
        : command("chmod", args) {}

    void execute(user &currentUser) override
    {
        const auto &args = getArgs();
        if (args.size() != 2)
        {
            std::cerr << "Usage: chmod <permissions> <filename>" << std::endl;
            std::cerr << "  permissions: +r, +w, +x (add) or -r, -w, -x (remove)" << std::endl;
            std::cerr << "  or direct: r, w, x, rw, rwx (set permissions)" << std::endl;
            return;
        }

        const std::string &permStr = args[0];
        const std::string &filename = args[1];

        // Find the file in the current directory (in-memory)
        Directory &currentDir = const_cast<Directory&>(currentUser.getCurrentDirectory());
        // Assumes getFilesRef() exists on Directory and returns a non-const vector<File>&
        // NOTE: This relies on 'getFilesRef()' being implemented on Directory to get a mutable reference.
        std::vector<File> &filesInDir = currentDir.getFilesRef(); 

        File *targetFile = nullptr;
        for (auto &f : filesInDir)
        {
            if (f.getFileName() == filename)
            {
                targetFile = &f;
                break;
            }
        }

        if (!targetFile)
        {
            std::cerr << "Error: File not found in current directory: " << filename << std::endl;
            return;
        }

        // Get current permissions
        std::string currentPerms = targetFile->getPermissions();
        std::string newPerms = currentPerms;

        // --- Logic to calculate newPerms (copied from your previous file) ---
        if (permStr[0] == '+')
        {
            // Add permissions
            std::string toAdd = permStr.substr(1);
            for (char c : toAdd)
            {
                if (c == 'r' || c == 'w' || c == 'x')
                {
                    if (newPerms.find(c) == std::string::npos)
                        newPerms += c;
                }
                else
                {
                    std::cerr << "Error: Invalid permission character '" << c << "'. Only r, w, x allowed." << std::endl;
                    return;
                }
            }
            std::cout << "Added permissions to " << filename << std::endl;
        }
        else if (permStr[0] == '-')
        {
            // Remove permissions
            std::string toRemove = permStr.substr(1);
            for (char c : toRemove)
            {
                if (c == 'r' || c == 'w' || c == 'x')
                {
                    size_t pos = newPerms.find(c);
                    if (pos != std::string::npos)
                        newPerms.erase(pos, 1);
                }
                else
                {
                    std::cerr << "Error: Invalid permission character '" << c << "'. Only r, w, x allowed." << std::endl;
                    return;
                }
            }
            std::cout << "Removed permissions from " << filename << std::endl;
        }
        else
        {
            // Set permissions directly
            newPerms.clear();
            for (char c : permStr)
            {
                if (c == 'r' || c == 'w' || c == 'x')
                {
                    if (newPerms.find(c) == std::string::npos)
                        newPerms += c;
                }
                else
                {
                    std::cerr << "Error: Invalid permission character '" << c << "'. Only r, w, x allowed." << std::endl;
                    return;
                }
            }
            std::cout << "Set permissions on " << filename << std::endl;
        }

        if (newPerms.empty())
        {
            std::cerr << "Warning: File now has no permissions." << std::endl;
        }

        if (newPerms.length() > 3)
        {
            std::cerr << "Error: Permissions exceed 3 character limit. Reverting." << std::endl;
            return; // Exit without applying changes
        }
        // --- End Logic to calculate newPerms ---

        // 1. Update the File object's permissions in memory
        targetFile->setPermissions(newPerms); // The File::setPermissions method handles validation

        // --- START PERSISTENCE LOGIC: Write the new permissions to disk ---
        
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
        
        // 2. Define the path for the physical permission file (<filename>.perms)
        std::filesystem::path filePath = dir / (filename + ".perms");

        // 3. Write the validated permission string to the physical .perms file, truncating any existing content
        std::ofstream ofs(filePath.string(), std::ios::trunc);
        if (!ofs.is_open())
        {
            std::cerr << "Error: cannot open permission file for writing: " << filePath << ". In-memory change made, but not persisted." << std::endl;
            return;
        }
        
        ofs << targetFile->getPermissions(); 
        ofs.close();

        // --- END PERSISTENCE LOGIC ---

        std::cout << "Permissions of " << filename << " are now [" << targetFile->getPermissions() << "]" << std::endl;
    }
};