#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "directory.c++" 
#include "user.c++" 

struct command {
    private:
    std::string name;
    std::vector<std::string> args;
    
    public:
    command() : name(""), args({}) {}

    command(std::string commandName) : name(commandName), args({}) {}

    command(const std::string& cmd_name, const std::vector<std::string>& cmd_args)
        : name(cmd_name), args(cmd_args) {}

    const std::string& getName() const {
        return name;
    }

    const std::vector<std::string>& getArgs() const {
        return args;
    }

    void setName(const std::string& cmd_name) {
        name = cmd_name;
    }

    void setArgs(const std::vector<std::string>& cmd_args) {
        args = cmd_args;
    }

    void addArg(const std::string& arg) {
        args.push_back(arg);
    }

    std::string toString() const {
        std::string result = "Command: " + name + "\nArguments:";
        for (const auto& arg : args) {
            result += " " + arg;
        }
        return result;
    }
};

struct helpCommand{
    private:

        std::string helpText =
        //TODO: NEED LOGIC TO PULL COMMAND LIST FROM COMMANDS.TXT
        "Available Commands:\n"
        "1. help - Display this help message\n"
        "2. adduser - Add a new user\n"
        "3. exit - Exit the shell\n";
        command helpCmd = command("help", {});
        
        
        public:
        // Constructor
        helpCommand() {
            helpCmd.setName("help");
            helpCmd.setArgs({});
        }
        
        void execute() {
            std::cout << helpText << std::endl;
        }
        

};
struct addUserCommand{
    private:
        command userCommand = command("adduser", {});
    
    public:
    // Constructor
    addUserCommand(std::vector<std::string> args) {
        userCommand.setName("adduser");
        userCommand.setArgs(args);
    }

    void execute() {
    // 1. Get the arguments vector
    const auto& args = userCommand.getArgs();
    
    // 2. Check for the correct number of arguments (must be exactly 1: the username)
    if (args.size() != 1) {
        std::cerr << "Error: Incorrect number of arguments. Usage: adduser [username] " << std::endl;
        return;
    }
    const std::string& username = args[0];
    
    std::cout << "Executing adduser command for user: " << username << std::endl;
    
    // --- File Operations ---
    std::string filePath = "/home/simon/Documents/OSShellRoot/users.txt";

    // Check if user already exists by reading the file (if it exists)
    {
        std::ifstream usersFileRead(filePath);
        if (usersFileRead.is_open()) {
            std::string line;
            while (std::getline(usersFileRead, line)) {
                std::istringstream iss(line);
                std::string existingUser;
                if (iss >> existingUser) {
                    if (existingUser == username) {
                        std::cerr << "Error: User already exists: " << username << std::endl;
                        return;
                    }
                }
            }
        }
    }

    // Now open for append and add the new user
    std::ofstream usersFile(filePath, std::ios::app);
    if (!usersFile.is_open()) {
        std::cerr << "Error: Unable to open or create users file at: " << filePath << std::endl;
        return;
    }

    // 4. Prompt for and read the password
    std::cout << "Enter user password: ";
    std::string password;
    std::cin >> password;

    // 5. Append username and password (separated by a space, on a new line)
    usersFile << username << " " << password << std::endl;
    
    // 6. Close the file and confirm
    usersFile.close();
    std::cout << "User added successfully: " << username << std::endl; 
}

};
struct mkdirCommand {
    private:
        command dirCommand = command("mkdir", {});
    
    public:
    mkdirCommand(std::vector<std::string> args) {
        dirCommand.setName("mkdir");
        dirCommand.setArgs(args);
    }
    void execute(user& currentUser) { 
        const auto& args = dirCommand.getArgs();
        
        if (args.size() != 1) {
            std::cerr << "Error: Incorrect number of arguments. Usage: mkdir [directoryName]" << std::endl;
            return;
        }

        const std::string& dirName = args[0];
        
        // 1. Get the properties from the current session
        std::string owner = currentUser.getUsername();
        int newId = currentUser.NextDirId();
        
        // Construct the new path based on the CWD
        std::string parentPath = currentUser.getCurrentDirectory().getDirPath();
        std::string newPath = parentPath + (parentPath == "/" ? "" : "/") + dirName;
        
        // 2. Create and add the Directory object
        Directory newDir(newId, dirName, newPath, owner);
        
        // 3. Add to user's master list (which also updates CWD's subdirectory list in the user struct)
        // NOTE: This relies on user::addDirectory and user::getCurrentDirectory being correctly defined.
        currentUser.addDirectory(newDir);
        
        // 4. Output confirmation
        std::cout << "Successfully created directory: " << dirName << std::endl;
        std::cout << "New Path: " << newPath << std::endl;
        std::cout << "Total directories for user: " << currentUser.getDirectories().size() << std::endl;
    }

};

// --- UPDATED cdCommand ---
// FIX: This is now a top-level struct, resolving 'undeclared in this scope' errors.
struct cdCommand {
    private:
        command dirCommand = command("cd", {});
    
    public:
    // Constructor remains the same
    cdCommand(std::vector<std::string> args) {
        dirCommand.setName("cd");
        dirCommand.setArgs(args);
    }

    // FIX: Correct signature: execute now takes user& currentUser
    void execute(user& currentUser) { 
        const auto& args = dirCommand.getArgs();
        
        if (args.empty() || args.size() > 1) {
            std::cerr << "Error: Usage: cd [directoryName] or cd .." << std::endl;
            return;
        }

        const std::string& targetDirName = args[0];
        
        // 1. Check for '..' (parent directory)
        if (targetDirName == "..") {
            // If the CWD is the root (index 0), don't move up.
            if (currentUser.getCurrentDirectory().getDirName() != "/") {
                std::string currentPath = currentUser.getCurrentDirectory().getDirPath();
                size_t lastSlash = currentPath.find_last_of('/');
                std::string parentPathName = currentPath.substr(0, lastSlash);
                
                if (parentPathName.empty()) {
                    parentPathName = "/";
                }

                // Search for the directory index matching the parent's full path
                int targetIndex = -1;
                for (size_t i = 0; i < currentUser.getDirectories().size(); ++i) {
                    if (currentUser.getDirectories()[i].getDirPath() == parentPathName) {
                        targetIndex = i;
                        break;
                    }
                }
                
                if (targetIndex != -1) {
                    currentUser.setCurrentDirectoryIndex(targetIndex);
                    std::cout << "Changed directory to: " << parentPathName << std::endl;
                    return;
                } else if (currentUser.getCurrentDirectory().getDirName() != "/") {
                    // Fallback if parent is root "/"
                    currentUser.setCurrentDirectoryIndex(0);
                    std::cout << "Changed directory to: /" << std::endl;
                    return;
                }
            } else {
                std::cout << "Already at the root directory." << std::endl;
                return;
            }
        }
        
        // 2. Check for a subdirectory name
        
        // Check if the target is in the current directory's list of subdirectories
        const std::vector<Directory>& subdirs = currentUser.getCurrentDirectory().getSubdirectories();
        
        for (const auto& subdir : subdirs) {
            if (subdir.getDirName() == targetDirName) {
                // Found it. Now find its index in the user's master list
                int targetIndex = currentUser.findDirectoryIndexByName(targetDirName);
                if (targetIndex != -1) {
                    currentUser.setCurrentDirectoryIndex(targetIndex);
                    std::cout << "Changed directory to: " << targetDirName << std::endl;
                    return;
                }
            }
        }

        std::cerr << "Error: Directory not found: " << targetDirName << std::endl;
    }

};

// --- NEW lsCommand ---
// FIX: This is now a top-level struct, resolving 'undeclared in this scope' errors.
struct lsCommand {
    private:
        command listCommand = command("ls", {});
    
    public:
    // Constructor remains the same
    lsCommand(std::vector<std::string> args) {
        listCommand.setName("ls");
        listCommand.setArgs(args);
    }

    // FIX: Correct signature: execute now takes user& currentUser
    void execute(user& currentUser) { 
        const auto& args = listCommand.getArgs();
        
        if (!args.empty()) {
            std::cerr << "Error: ls takes no arguments in this simple implementation." << std::endl;
            return;
        }

        // 1. Get the current directory object
        const Directory& currentDir = currentUser.getCurrentDirectory();
        
        std::cout << "Contents of " << currentDir.getDirPath() << ":" << std::endl;
        
        // 2. List subdirectories
        std::cout << "Directories: ";
        if (currentDir.getSubdirectories().empty()) {
            std::cout << "(none)";
        } else {
            std::cout << currentDir.getSubdirectoryNames();
        }
        std::cout << std::endl;
        
        // 3. List files
        std::cout << "Files: ";
        if (currentDir.getFiles().empty()) {
            std::cout << "(none)";
        } else {
            std::cout << currentDir.getFileNames();
        }
        std::cout << std::endl;
    }

};
