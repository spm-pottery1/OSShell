#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <utility>
#include "comand.c++"

struct colonel {
    private:
    int command_id_counter = 0;
    std::unordered_map<std::string, int> command_list;
    std::vector<std::string> parsed_command;
    std::vector<command> command_objects;
    std::vector<std::string> command_args;
    
    command buildCommand(const std::string& cmd_name, const std::vector<std::string>& cmd_args) {
        command_id_counter++;
        command newCommand(cmd_name, cmd_args);
        command_objects.push_back(newCommand);
        command_list[cmd_name] = command_id_counter;
        return newCommand;
    }

    void initColonel() {
    std::string argument;
    // ...
    buildCommand("help", {});           // ID 1
    buildCommand("adduser", {"testuser"}); // ID 2
    buildCommand("mkdir", {"newDir"});   // ID 3
    buildCommand("cd", {"testDir"});     // ID 4
    buildCommand("ls", {});             // NEW: ID 5
    buildCommand("exit", {});
    
    std::cout << "DEBUG(COLONEL.C++, initColonel()): Colonel initialized with " 
              << command_list.size() << " commands." << std::endl;
}

    const std::vector<std::string>& getParsedCommand(const std::string& user_command) {
        parsed_command.clear();
        std::stringstream ss(user_command);
        std::string token;
        
        // Extract command name and arguments
        while (std::getline(ss, token, ' ')) {
            if (!token.empty()) {
                parsed_command.push_back(token);
            }
        }
        return parsed_command;
    }


    public:
    // Constructor
    colonel() {
        initColonel();
    }


    // Method to parse and execute a command
    // FIX: Add user& currentUser to the signature
    const std::string parseCommand(const std::string& user_command, user& currentUser) { 
        std::vector<std::string> tokens = getParsedCommand(user_command);
        
        if (tokens.empty()) {
            return "";
        }

        std::string command_name = tokens[0];
        std::vector<std::string> args(tokens.begin() + 1, tokens.end());
        
        std::cout << "DEBUG(COLONEL.C++, parseCommand()): Command Name: " << command_name << std::endl;
        std::cout << "DEBUG(COLONEL.C++, parseCommand()): Arguments size: " << args.size() << std::endl;


        if (command_list.count(command_name)) {
            int command_id = command_list.at(command_name);

            switch (command_id) {
                case 1: // help command
                    {
                        helpCommand helpCmd;
                        helpCmd.execute(); // No state needed for help
                        return "Help command executed.";
                    }
                case 2: // adduser command
                    {
                        addUserCommand addUserCmd(args);
                        addUserCmd.execute(); // No state needed for adding user (only file ops)
                        return "AddUser command executed.";
                    }
                case 3: // mkdir command
                    {
                        mkdirCommand mkdirCmd(args);
                        mkdirCmd.execute(currentUser); // PASSING USER STATE
                        return "Mkdir command executed.";
                    }
                case 4: // cd command
                    {
                        cdCommand cdCmd(args);
                        cdCmd.execute(currentUser); // PASSING USER STATE
                        return "Cd command executed.";
                    }
                case 5: // ls command (NEW)
                    {
                        lsCommand lsCmd(args);
                        lsCmd.execute(currentUser); // PASSING USER STATE
                        return "Ls command executed.";
                    }
                default:
                    return "Command execution logic not implemented yet.";
            }
        }
        else {
            std::cout << "Command not found: " << command_name << std::endl;
            return "";
        }
    }


}; 