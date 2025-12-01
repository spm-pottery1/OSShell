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

        // Detect redirection token and filename
        bool append = false;
        std::string filename;
        std::vector<std::string> textParts;

        // Look for explicit redirection tokens (">" or ">>") or bundled (">>file" or ">file")
        size_t i = 0;
        bool foundRedirect = false;
        for (; i < args.size(); ++i)
        {
            const std::string &a = args[i];
            if (a == ">" || a == ">>")
            {
                foundRedirect = true;
                append = (a == ">>");
                // filename should be next token
                if (i + 1 < args.size())
                    filename = args[i + 1];
                else
                {
                    std::cerr << "Error: no filename after redirection operator." << std::endl;
                    return;
                }
                break;
            }
            // support ">>file.txt" or ">file.txt"
            if (a.rfind(">>", 0) == 0)
            {
                foundRedirect = true;
                append = true;
                filename = a.substr(2);
                break;
            }
            if (a.rfind(">", 0) == 0)
            {
                foundRedirect = true;
                append = false;
                filename = a.substr(1);
                break;
            }
            // otherwise part of the text
            textParts.push_back(a);
        }

        if (!foundRedirect)
        {
            // No explicit redirection token: assume last arg is filename, rest is text
            if (args.size() < 2)
            {
                std::cerr << "Usage: echo <text> <filename> (or echo <text> >> <filename>)" << std::endl;
                return;
            }
            filename = args.back();
            // textParts already contains all up to first redirect (or empty); rebuild from args[0..n-2]
            textParts.clear();
            for (size_t j = 0; j + 1 < args.size(); ++j)
                textParts.push_back(args[j]);
        }

        // Join text parts into one string
        std::string text;
        for (size_t k = 0; k < textParts.size(); ++k)
        {
            if (k)
                text += " ";
            text += textParts[k];
        }

        // Strip surrounding quotes if present
        if (text.size() >= 2)
        {
            if ((text.front() == '"' && text.back() == '"') || (text.front() == '\'' && text.back() == '\''))
            {
                text = text.substr(1, text.size() - 2);
            }
        }

        // Map virtual root "/" -> physical base directory for this user
        std::filesystem::path base = "/home/simon/Documents/OSShellRoot";
        base /= currentUser.getUsername();

        // Determine physical directory that corresponds to the current virtual directory
        std::string vpath = currentUser.getCurrentDirectory().getDirPath(); // e.g. "/docs"
        std::filesystem::path dir = base;
        if (vpath != "/")
        {
            std::string rel = vpath;
            if (!rel.empty() && rel.front() == '/')
                rel.erase(0, 1);
            dir /= rel;
        }

        // Ensure directories exist, then open the file
        std::error_code ec;
        std::filesystem::create_directories(dir, ec);
        if (ec)
        {
            std::cerr << "Error creating directories: " << ec.message() << std::endl;
            return;
        }

        std::filesystem::path filePath = dir / filename;
        std::ofstream ofs;
        if (append)
            ofs.open(filePath.string(), std::ios::app);
        else
            ofs.open(filePath.string(), std::ios::trunc);

        if (!ofs.is_open())
        {
            std::cerr << "Error: cannot open file for writing: " << filePath << std::endl;
            return;
        }

        // Write text and a newline (behaves like shell echo)
        ofs << text << std::endl;
        ofs.close();

        std::cout << (append ? "Appended to file: " : "Wrote to file: ") << filePath << std::endl;
    }
};