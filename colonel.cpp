#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <utility>
#include <fstream>     
#include <filesystem> 
#include "log.cpp" 
#include "file_system/user.h"
#include "commands/command.h"              
#include "commands/helpCommand.cpp"
#include "commands/addUserCommand.cpp"
#include "commands/mkdirCommand.cpp"
#include "commands/cdCommand.cpp"
#include "commands/lsCommand.cpp"
#include "commands/touchCommand.cpp"
#include "commands/echoCommand.cpp"
#include "commands/catCommand.cpp"
#include "commands/rmCommand.cpp"
#include "commands/chmodCommand.cpp"

namespace fs = std::filesystem;


struct colonel {
    private:
    int command_id_counter = 0;
    std::unordered_map<std::string, int> command_list;
    std::vector<std::string> parsed_command;
    std::vector<std::string> command_args;
    log logger;                
    
    // Base path for file system persistence
    std::string baseRoot = "/home/simon/Documents/OSShellRoot"; 

    // Helper to read the file content from the physical disk
    std::string readPhysicalFileContent(const std::string& physicalPath) {
        std::ifstream file(physicalPath);
        std::stringstream buffer;
        if (file.is_open()) {
            buffer << file.rdbuf();
            file.close();
            return buffer.str();
        }
        return "";
    }


    void executeScript(const std::string& filename, user& currentUser) {
        // 1. Get the physical path to the script file
        fs::path base = baseRoot;
        base.append(currentUser.getUsername());
        
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
        filePath = filePath.lexically_normal(); // Ensure path is clean

        // 2. Read file content
        std::string scriptContent = readPhysicalFileContent(filePath.string());
        
        if (scriptContent.empty()) {
            if (!fs::exists(filePath)) {
                std::cerr << "Exec Error: Script file not found: " << filePath << std::endl;
            } else {
                std::cout << "Exec: Script file is empty or unreadable." << std::endl;
            }
            return;
        }

        // 3. Process commands line by line
        std::stringstream ss(scriptContent);
        std::string line;
        
        std::cout << "--- Executing Script: " << filename << " ---" << std::endl;

        while (std::getline(ss, line)) {
            // Trim leading/trailing whitespace
            line.erase(0, line.find_first_not_of(" \t\n\r"));
            line.erase(line.find_last_not_of(" \t\n\r") + 1);

            if (line.empty() || line.front() == '#') {
                continue; // Skip empty lines or comments
            }
            
            std::cout << "$ " << line << std::endl;
            
            // Re-use the existing parseCommand function to execute the line
            parseCommand(line, currentUser);
        }
        std::cout << "--- Script Execution Finished ---" << std::endl;
    }
    
    public:

    colonel() {
        initColonel();
    }

    void initColonel() {
        command_list["help"] = 1;
        command_list["adduser"] = 2;
        command_list["mkdir"] = 3;
        command_list["cd"] = 4;
        command_list["ls"] = 5;
        command_list["touch"] = 6;
        command_list["echo"] = 7;
        command_list["cat"] = 8;
        command_list["rm"] = 9;
        command_list["exit"] = 10;
        command_list["chmod"] = 11;
        command_list["exec"] = 12; // Add exec
    }


    const std::string parseCommand(const std::string& user_command, user& currentUser) {
        

        // --- Tokenization Logic ---
        std::vector<std::string> args;
        std::string command_name;
        std::string current_token;
        char quote_char = 0;
        bool in_quotes = false;
        
        // 1. Extract command name by skipping leading whitespace
        std::stringstream ss_initial(user_command);
        if (!(ss_initial >> command_name)) {
            return ""; // Empty command
        }

        // Find the position right after the command name and separating whitespace
        size_t command_len = command_name.length();
        size_t start_pos = user_command.find_first_not_of(" \t", command_len);
        
        if (start_pos != std::string::npos) {
            // 2. Process the rest of the string for arguments
            for (size_t i = start_pos; i < user_command.length(); ++i) {
                char c = user_command[i];

                if (in_quotes) {
                    if (c == quote_char) {
                        // End of quote: The token is complete, push it and reset
                        in_quotes = false;
                        quote_char = 0;
                        args.push_back(current_token);
                        current_token.clear();
                    } else {
                        // Inside quotes: Everything is part of the token
                        current_token += c;
                    }
                } else {
                    if (c == '"' || c == '\'') {
                        // Start of quote: Set state
                        in_quotes = true;
                        quote_char = c;
                    } else if (c == ' ' || c == '\t') {
                        // Whitespace outside quotes is a separator
                        if (!current_token.empty()) {
                            args.push_back(current_token);
                            current_token.clear();
                        }
                    } else {
                        // Regular character
                        current_token += c;
                    }
                }
            }

            // 3. Push any remaining token after loop finishes (e.g., last non-quoted word)
            if (!current_token.empty()) {
                args.push_back(current_token);
            }
        }

        std::string result;
        bool success = false;

        if (command_list.count(command_name)) {
            int command_id = command_list[command_name];
            switch (command_id) {
                case 1: { // help
                    helpCommand cmd(args);
                    cmd.execute(currentUser);
                    result = "Help executed.";
                    success = true;
                    break;
                }
                case 2: { // adduser
                    addUserCommand cmd(args);
                    cmd.execute(currentUser);
                    result = "AddUser executed.";
                    success = true;
                    break;
                }
                case 3: { // mkdir
                    mkdirCommand cmd(args);
                    cmd.execute(currentUser);
                    result = "Mkdir executed.";
                    success = true;
                    break;
                }
                case 4: { // cd
                    cdCommand cmd(args);
                    cmd.execute(currentUser);
                    result = "Cd executed.";
                    success = true;
                    break;
                }
                case 5: { // ls
                    lsCommand cmd(args);
                    cmd.execute(currentUser);
                    result = "Ls executed.";
                    success = true;
                    break;
                }
                case 6: { // touch
                    touchCommand cmd(args);
                    cmd.execute(currentUser);
                    result = "Touch executed.";
                    success = true;
                    break;
                }
                case 7: { // echo
                    echoCommand cmd(args);
                    cmd.execute(currentUser);
                    result = "Echo executed.";
                    success = true;
                    break;
                }
                case 8: { // cat
                    catCommand cmd(args);
                    cmd.execute(currentUser);
                    result = "Cat executed.";
                    success = true;
                    break;
                }
                case 9: { // rm
                    rmCommand cmd(args);
                    cmd.execute(currentUser);
                    result = "Rm executed.";
                    success = true;
                    break;
                }
                case 10: { // exit
                    // Handled in main shell loop
                    result = "Exit command.";
                    success = true;
                    break;
                }
                case 11: { // chmod
                    chmodCommand cmd(args);
                    cmd.execute(currentUser);
                    result = "Chmod executed.";
                    success = true;
                    break;
                }
                case 12: { // exec
                    if (args.size() != 1) {
                        std::cerr << "Usage: exec <filename>" << std::endl;
                        result = "Invalid usage of exec.";
                        success = false;
                        break;
                    }
                    executeScript(args[0], currentUser);
                    result = "Exec script execution cycle finished.";
                    success = true;
                    break;
                }
                default: {
                    success = false;
                    break;
                }
            }
        } else {
            std::cout << "Command not found: " << command_name << std::endl;
            result = "Command not found: " + command_name;
            success = false;
        }

        // log the command using logIt
        std::ostringstream msg;
        msg << (success ? "OK" : "FAIL") << " | " << currentUser.getUsername() << " | " << command_name;
        for (const auto &a : args) msg << " " << a;
        msg << " | " << result;
        logger.logIt(msg.str());

        return result;
    }
};