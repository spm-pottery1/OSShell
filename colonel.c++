#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <utility>
#include <fstream>     
#include <filesystem>  
#include "command.c++"
#include "log.c++"              

// FIX 1: INCLUDE ALL REQUIRED COMMAND FILES
#include "helpCommand.c++"
#include "addUserCommand.c++"
#include "mkdirCommand.c++"
#include "cdCommand.c++"
#include "lsCommand.c++"
#include "touchCommand.c++"
#include "echoCommand.c++"
#include "catCommand.c++"
#include "rmCommand.c++"
#include "chmodCommand.c++"


struct colonel {
    private:
    int command_id_counter = 0;
    std::unordered_map<std::string, int> command_list;
    std::vector<std::string> parsed_command;
    std::vector<std::string> command_args;
    log logger;                
    
    // Base path for file system persistence
    std::string baseRoot = "/home/simon/Documents/OSShellRoot"; 


    // --- PRIVATE HELPER FUNCTIONS FOR 'exec' ---

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

    /**
     * Executes a script file by reading its content line-by-line and recursively
     * calling parseCommand for each line.
     */
    void executeScript(const std::string &filename, user &currentUser) {
        // --- 1. Find the file's metadata (in-memory) and check permissions ---
        const Directory &currentDir = currentUser.getCurrentDirectory();
        const File *targetFile = nullptr;
        for (const auto &f : currentDir.getFiles()) {
            if (f.getFileName() == filename) {
                targetFile = &f; 
                break;
            }
        }

        if (!targetFile) {
            std::cerr << "Error: Script file not found: " << filename << std::endl;
            return;
        }

        std::string perms = targetFile->getPermissions();
        if (perms.find('x') == std::string::npos) {
            std::cerr << "Error: Permission denied. File does not have 'x' (execute) permission." << std::endl;
            return;
        }
        
        // --- 2. Construct the physical path ---
        namespace fs = std::filesystem;
        fs::path physicalPath = fs::path(baseRoot) / currentUser.getUsername();
        
        // Find the parent directory's virtual path
        std::string vpath = targetFile->getFilePath(); 
        size_t lastSlash = vpath.rfind('/');
        std::string parentDirVPath = (lastSlash == std::string::npos || lastSlash == 0) ? "/" : vpath.substr(0, lastSlash);

        // Convert virtual path to relative physical path
        std::string relPath = parentDirVPath;
        if (!relPath.empty() && relPath.front() == '/') {
            relPath.erase(0, 1);
        }
        
        physicalPath /= fs::path(relPath);
        physicalPath /= filename;

        // --- 3. Read the file content from the physical disk ---
        std::string scriptContent = readPhysicalFileContent(physicalPath.string());

        if (scriptContent.empty()) {
            std::cerr << "Error: Failed to read script content from disk or file is empty: " << physicalPath.string() << std::endl;
            return;
        }

        // --- 4. Execute commands line-by-line ---
        std::stringstream ss(scriptContent);
        std::string line;
        int lineNumber = 0;
        
        std::cout << "--- Executing script: " << targetFile->getFileName() << " ---" << std::endl;

        while (std::getline(ss, line))
        {
            lineNumber++;
            // Trim whitespace and skip comments/empty lines
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);

            if (line.empty() || line.front() == '#') continue; 
            
            std::cout << "[SCRIPT:" << lineNumber << "] > " << line << std::endl;

            // Call the main dispatcher function on the current colonel instance
            (*this).parseCommand(line, currentUser);
        }

        std::cout << "--- Script execution finished ---" << std::endl;
    }
    void buildCommand(const std::string& cmd_name, const std::vector<std::string>& cmd_args) {
        command_id_counter++;
        command_list[cmd_name] = command_id_counter;
    }

    public:
    
    colonel() {
        initColonel();
    }

    void initColonel() {
        // Register commands to assign deterministic IDs
        buildCommand("help", {});       // ID 1
        buildCommand("adduser", {});    // ID 2
        buildCommand("mkdir", {});      // ID 3
        buildCommand("cd", {});         // ID 4
        buildCommand("ls", {});         // ID 5
        buildCommand("exit", {});       // ID 6
        buildCommand("touch", {});      // ID 7
        buildCommand("echo", {});       // ID 8
        buildCommand("cat", {});        // ID 9
        buildCommand("rm", {});         // ID 10
        buildCommand("chmod", {});      // ID 11
        buildCommand("exec", {});       // ID 12 
    }

    const std::string parseCommand(const std::string& user_command, user& currentUser) {
        
        // --- Tokenization Logic ---
        std::vector<std::string> args;
        std::string command_name;
        std::stringstream ss(user_command);
        
        if (!(ss >> command_name)) {
            return ""; // Empty command
        }
        std::string arg;
        while (ss >> arg) {
            args.push_back(arg);
        }
        // --- End Tokenization Logic ---

        std::string result;
        bool success = false;
                
        // --- DISPATCHER SWITCH STATEMENT ---
        // Instantiate and execute the appropriate command based on command_id
        // 1. Help
        // 2. AddUser
        // 3. Mkdir
        // 4. Cd
        // 5. Ls
        // 6. Exit
        // 7. Touch
        // 8. Echo
        // 9. Cat
        // 10. Rm
        // 11. Chmod
        // 12. Exec

        if (command_list.count(command_name)) {
            int command_id = command_list.at(command_name);
            switch (command_id) {
                case 1: { // help
                    helpCommand cmd(args);
                    cmd.execute(currentUser);
                    result = "Help displayed.";
                    success = true;
                    break;
                }
                case 2: { // adduser
                    addUserCommand cmd(args);
                    cmd.execute(currentUser);
                    result = "Adduser executed.";
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
                case 6: { // exit
                    result = "Exiting...";
                    success = true;
                    break;
                }
                case 7: { // touch
                    touchCommand cmd(args);
                    cmd.execute(currentUser);
                    result = "Touch executed.";
                    success = true;
                    break;
                }
                case 8: { // echo
                    echoCommand cmd(args);
                    cmd.execute(currentUser);
                    result = "Echo executed.";
                    success = true;
                    break;
                }
                case 9: { // cat
                    catCommand cmd(args);
                    cmd.execute(currentUser);
                    result = "Cat executed.";
                    success = true;
                    break;
                }
                case 10: { // rm
                    rmCommand cmd(args);
                    cmd.execute(currentUser);
                    result = "Rm executed.";
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
                    // Call the new private function to handle script execution
                    (*this).executeScript(args[0], currentUser);
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