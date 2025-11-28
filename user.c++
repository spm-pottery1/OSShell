#include <iostream>
#include <string>
#include "directory.c++"
#include <fstream>
#include <sstream>
#include <vector>

struct user {
    private:
        std::string username;
        std::string password;
        std::vector<Directory> directories;
        std::vector<File> files;
        std::string filePath = "/home/simon/Documents/OSShellRoot/users.txt";
        
        int userId;
        int dirIdCounter = 0;
        int fileIdCounter = 0;
        int permissions = 0; // Placeholder for future permission management
        // FIX: Added member to track current directory
        int currentDirectoryIndex = 0; 
        


    public:

    // Constructor
    user(const std::string& u, const std::string& p)
        : username(u), password(p) {
            // FIX: Initialize the root directory upon user creation
            // Assuming 0 is the ID for root
            directories.push_back(Directory(0, "/", "/", username)); 
        }

    // Getters
    const std::string& getUsername() const { 
        return username; 
    }
    const std::string& getPassword() const { 
        return password; 
    }
    const std::vector<Directory>& getDirectories() const { 
        return directories; 
    }
    const std::vector<File>& getFiles() const { 
        return files; 
    }
    
    // FIX: Add getter for the current directory
    const Directory& getCurrentDirectory() const {
        // Handle out-of-bounds access defensively
        if (currentDirectoryIndex < 0 || currentDirectoryIndex >= directories.size()) {
            return directories[0]; // Default to root
        }
        return directories[currentDirectoryIndex];
    }

    int getUserId() const { 
        return userId; 
    }

    int getPermissions() const { 
        return permissions; 
    }

    // Setters
    void setUsername(const std::string& u) { 
        username = u; 
    }

    void setPassword(const std::string& p) { 
        password = p; 
    }

    void addDirectory(const Directory& dir) { 
        directories.push_back(dir); 
        // OPTIONAL: You may also need logic here to update the parent directory's 'subdirectories' list
        // For simplicity, we assume this is handled elsewhere or implicitly via currentDirectoryIndex.
    }

    void addFile(const File& file) { 
        files.push_back(file); 
    }
    
    // FIX: Add setter for the current directory index
    void setCurrentDirectoryIndex(int index) {
        if (index >= 0 && index < directories.size()) {
            currentDirectoryIndex = index;
        } else {
            std::cerr << "Error: Invalid directory index provided to setCurrentDirectoryIndex." << std::endl;
        }
    }

    int NextDirId() { 
        return ++dirIdCounter; 
    }

    int NextFileId() { 
        return ++fileIdCounter; 
    }

    void setUserId(int id) { 
        userId = id; 
    }

    void setPermissions(int perms) { 
        permissions = perms; 
    }

    // FIX: Add search function required by cdCommand to find index by directory name
    int findDirectoryIndexByName(const std::string& name) const {
        for (size_t i = 0; i < directories.size(); ++i) {
            // Check if the directory name matches AND if its path indicates it's a child of the current directory (optional complexity)
            // For now, only matching by name:
            if (directories[i].getDirName() == name) {
                return i;
            }
        }
        return -1;
    }
    
    // toString method
    std::string toString() const {
        return "Username: " + username + "\n" +
               "Password: " + password + "\n" +
               "Current Directory: " + getCurrentDirectory().getDirPath() + "\n" + // Show CWD
               "Directories: " + std::to_string(directories.size()) + "\n" +
               "Files: " + std::to_string(files.size());
    }

    void buildUser() {
    // Note: The file stream object should be userFile, not users.txt, 
    // but assuming this is the definition within user.c++:
    std::ifstream userFile(filePath); 
    if (!userFile.is_open()) {
        // Log an error or handle the failure gracefully
        std::cerr << "Warning: Could not open " << filePath << " in buildUser for auxiliary data." << std::endl;
        return; 
    }
    std::string line;
    while (std::getline(userFile, line)) {
        std::istringstream iss(line);
        std::string u, p; 
        // Temporarily read username and password to find the correct line
        if (iss >> u >> p) {
            // Found the line for the current user
            if (u == username) {
                // Read additional data as needed
                password = p;
                break;
            }
        }
    }
    userFile.close();
}

};