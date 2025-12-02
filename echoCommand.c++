#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cctype>
#include "directory.c++"
#include "user.c++"
#include "command.c++"


namespace fs = std::filesystem;

// Redefining the struct for abstract base class 'command' to inherit from
struct echoCommand : public command
{
public:
    echoCommand(std::vector<std::string> args)
        : command("echo", args) {}

    void execute(user &currentUser) override
    {
        const auto args = getArgs();
        if (args.empty())
        {
            std::cerr << "Usage: echo <text> [> | >>] <filename>" << std::endl;
            return;
        }      

        // --- Argument Parsing ---
        bool append = false;
        std::string filename;
        std::vector<std::string> textParts;
        
        // Find the index of the redirection token
        size_t redirectIndex = args.size(); 
        // 1. Search for explicit tokens ">" or ">>"
        for (size_t i = 0; i < args.size(); ++i)
        {
            const std::string &a = args[i];
            if (a == ">" || a == ">>")
            {
                redirectIndex = i;
                append = (a == ">>");
                break;
            }
        }
        // 2. If explicit token found, extract the filename from the next argument
        if (redirectIndex < args.size())
        {
            if (redirectIndex + 1 < args.size())
                filename = args[redirectIndex + 1];
            else {
                std::cerr << "Error: no filename after redirection operator." << std::endl;
                return;
            }
            for (size_t j = 0; j < redirectIndex; ++j)
                textParts.push_back(args[j]);
        }
        // 3. Handle bundled tokens (e.g., ">>file.txt") in the last argument
        else if (!args.empty())
        {
            const std::string &lastArg = args.back();
            if (lastArg.rfind(">>", 0) == 0) 
            {
                append = true;
                filename = lastArg.substr(2);
                for (size_t j = 0; j + 1 < args.size(); ++j)
                    textParts.push_back(args[j]);
            }
            else if (lastArg.rfind(">", 0) == 0) 
            {
                append = false;
                filename = lastArg.substr(1);
                for (size_t j = 0; j + 1 < args.size(); ++j)
                    textParts.push_back(args[j]);
            }
            else
            {
                // 'echo' without redirection prints to stdout.
                if (args.size() == 1) {
                    std::cout << args[0] << std::endl;
                    return;
                }
                
                // Fallback: Assume the last argument is the filename for OVERWRITE
                filename = args.back();
                append = false;
                for (size_t j = 0; j + 1 < args.size(); ++j)
                    textParts.push_back(args[j]);
            }
        }
        
        // Check if we managed to extract a filename
        if (filename.empty())
        {
            if (!args.empty() && textParts.empty())
                std::cout << args[0] << std::endl;
            else 
                std::cerr << "Error: Invalid command format or missing filename." << std::endl;
            return;
        }
        
        // Join text parts into one string
        std::string text;
        for (size_t k = 0; k < textParts.size(); ++k)
        {
            if (k) text += " ";
            text += textParts[k];
        }

        // Strip surrounding quotes if present
        if (text.size() >= 2)
        {
            if ((text.front() == '"' && text.back() == '"') || (text.front() == '\'' && text.back() == '\''))
                text = text.substr(1, text.size() - 2);
        }

        // Map virtual root "/" -> physical base directory for this user
        fs::path base = "/home/simon/Documents/OSShellRoot";
        base.append(currentUser.getUsername()); // Explicit append

        // Determine physical directory that corresponds to the current virtual directory
        std::string vpath = currentUser.getCurrentDirectory().getDirPath(); 
        fs::path dir = base;
        if (vpath != "/")
        {
            std::string rel = vpath;
            if (!rel.empty() && rel.front() == '/')
                rel.erase(0, 1);
            dir.append(rel); // Explicit append
        }

        // --- PATH CONSTRUCTION (Standard shell behavior) ---
        // Save the current in-memory file existence status before attempting to write
        // Note: hasFile() was added to directory.c++ in the previous step.
        bool file_existed_in_memory = currentUser.getCurrentDirectory().hasFile(filename);

        fs::path filePath = dir;
        filePath.append(filename); // Explicit append (using the requested filename)
        // --- END Standard touch logic ---
        
        // Ensure directories exist
        std::error_code ec;
        fs::create_directories(dir, ec);
        if (ec)
        {
            std::cerr << "Error creating directories: " << ec.message() << std::endl;
            return;
        }

        // Open the file. The standard flags handle creation, overwriting, or appending.
        std::ofstream ofs;
        
        if (append)
            // std::ios::app ensures content is added to the end.
            ofs.open(filePath.string(), std::ios::app);
        else
            // std::ios::trunc ensures the file is created or overwritten (truncated).
            ofs.open(filePath.string(), std::ios::trunc); 

        if (!ofs.is_open())
        {
            std::cerr << "Error: cannot open file for writing: " << filePath << std::endl;
            return;
        }

        // Write text and a newline
        ofs << text << std::endl;
        ofs.close();

        // Update in-memory state only if the file was newly created (it didn't exist in memory)
        if (!file_existed_in_memory) {
            currentUser.loadFromDisk();
        }

        std::cout << (append ? "Appended to file: " : "Wrote to file: ") << filename << std::endl;
    }
};