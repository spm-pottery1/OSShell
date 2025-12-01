#pragma once

#include <iostream>
#include <string>
#include "directory.c++"
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <unordered_map>

class user {
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
    int currentDirectoryIndex = 0;

public:
    // Constructor
    user(const std::string &u, const std::string &p)
        : username(u), password(p)
    {
        directories.push_back(Directory(0, "/", "/", username));
        loadFromDisk();
    }

    // Getters
    const std::string &getUsername() const { 
        return username; 
    }
    
    const std::string &getPassword() const { 
        return password; 
    }
    
    const std::vector<Directory> &getDirectories() const { 
        return directories; 
    }
    
    const std::vector<File> &getFiles() const { 
        return files; 
    }
    
    int getUserId() const { 
        return userId; 
    }
    
    int getPermissions() const { 
        return permissions; 
    }
    
    const Directory &getCurrentDirectory() const
    {
        if (currentDirectoryIndex < 0 || currentDirectoryIndex >= directories.size())
        {
            return directories[0];
        }
        return directories[currentDirectoryIndex];
    }

    // Setters
    void setUsername(const std::string &u) { 
        username = u; 
    }
    
    void setPassword(const std::string &p) { 
        password = p; 
    }
    
    void addFile(const File &file) { 
        files.push_back(file); 
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
    
    void addDirectory(const Directory &dir)
    {
        // Append to the user's master directory list
        directories.push_back(dir);
        // Also add it to the current directory's subdirectory list so `ls` sees it.
        if (currentDirectoryIndex >= 0 && currentDirectoryIndex < static_cast<int>(directories.size()))
        {
            directories[currentDirectoryIndex].addSubdirectory(dir);
        }
    }
    
    void setCurrentDirectoryIndex(int index)
    {
        if (index >= 0 && index < directories.size())
            currentDirectoryIndex = index;
        else
            std::cerr << "Error: Invalid directory index provided to setCurrentDirectoryIndex." << std::endl;
    }

    //Cd command helper

    int findDirectoryIndexByName(const std::string &name) const
    {
        for (size_t i = 0; i < directories.size(); ++i)
        {
            if (directories[i].getDirName() == name)
                return i;
        }
        return -1;
    }

    // toString method
    std::string toString() const
    {
        return "Username: " + username + "\n" +
               "Password: " + password + "\n" +
               "Current Directory: " + getCurrentDirectory().getDirPath() + "\n" +
               "Directories: " + std::to_string(getCurrentDirectory().getSubdirectories().size()) + "\n" +
               "Files: " + std::to_string(getCurrentDirectory().getFiles().size());
    }

    void buildUser()
    {
        std::ifstream userFile(filePath);
        if (!userFile.is_open())
        {
            std::cerr << "Warning: Could not open " << filePath << " in buildUser for auxiliary data." << std::endl;
            return;
        }
        std::string line;
        while (std::getline(userFile, line))
        {
            std::istringstream iss(line);
            std::string u, p;
            if (iss >> u >> p)
            {
                if (u == username)
                {
                    password = p;
                    break;
                }
            }
        }
        userFile.close();
    }



    void loadFromDisk()
    {
        // Load user's directories and files from disk (HARDCODED PATH)
        const std::string &rootBase = "/home/simon/Documents/OSShellRoot";
        namespace fs = std::filesystem;
        fs::path base = fs::path(rootBase) / username;

        if (!fs::exists(base))
        {
            // nothing to load
            return;
        }

        // Reset in-memory state
        directories.clear();
        files.clear();
        dirIdCounter = 0;
        fileIdCounter = 0;
        currentDirectoryIndex = 0;

        // Create root directory entry
        directories.push_back(Directory(0, "/", "/", username));
        std::unordered_map<std::string, int> pathIndex;
        pathIndex["/"] = 0;

        // Walk the filesystem
        std::error_code ec;
        for (auto it = fs::recursive_directory_iterator(base, fs::directory_options::skip_permission_denied, ec);
             it != fs::recursive_directory_iterator{}; it.increment(ec))
        {
            if (ec){
                continue;
            }
            const fs::directory_entry &entry = *it;
            fs::path relPath = fs::relative(entry.path(), base, ec);
            if (ec){
                continue;
            }

            std::string relStr = relPath.string();
            // Normalize to posix-style and build virtual path
            if (relStr == ".") {
                relStr = "";
            }
            std::string vpath = "/" + relStr;
            // remove trailing slash if any
            if (!vpath.empty() && vpath.size() > 1 && vpath.back() == '/'){
                vpath.pop_back();
            }

            if (entry.is_directory(ec)) {
                std::string name = entry.path().filename().string();

                // parent virtual path
                fs::path parent = entry.path().parent_path();
                std::string parentRel = fs::relative(parent, base, ec).string();
                if (parentRel == "."){
                    parentRel = "";
                }
                std::string parentV = "/" + parentRel;
                if (!parentV.empty() && parentV.size() > 1 && parentV.back() == '/'){
                    parentV.pop_back();
                }
                // find parent index
                int parentIndex = 0;
                if (pathIndex.count(parentV)){
                    parentIndex = pathIndex[parentV];
                }

                // create Directory
                int did = NextDirId(); // assign id
                Directory newDir(did, name, vpath, username);
                directories.push_back(newDir);
                int newIndex = static_cast<int>(directories.size()) - 1;
                pathIndex[vpath] = newIndex;
                
                // add to parent's subdirectories
                directories[parentIndex].addSubdirectory(newDir);
            } else if (entry.is_regular_file(ec) && !ec) {
                std::string fname = entry.path().filename().string();
                // --- FIX 2: Skip .perms files when loading files ---  
                if (fname.size() > 6 && fname.substr(fname.size() - 6) == ".perms") {
                    continue; // Skip this file; it's just metadata
                }

                fs::path parent = entry.path().parent_path();
                std::string parentRel = fs::relative(parent, base, ec).string();
                if (parentRel == "."){
                    parentRel = "";
                }
                std::string parentV = "/" + parentRel;
                if (!parentV.empty() && parentV.size() > 1 && parentV.back() == '/'){
                    parentV.pop_back();
                }

                int parentIndex = 0;
                if (pathIndex.count(parentV)){  
                    parentIndex = pathIndex[parentV];
                }
                std::string loadedPerms = "rw"; // Default to "rw" if no .perms file is found
                fs::path permFilePath = entry.path().string() + ".perms";

                if (fs::exists(permFilePath)) {
                    std::ifstream permFile(permFilePath);
                    if (permFile.is_open()) {
                        std::getline(permFile, loadedPerms); // Read the saved permission string
                        // Ensure it's not empty, otherwise default to "rw"
                        if (loadedPerms.empty()) {
                            loadedPerms = "rw";
                        }
                        permFile.close();
                    }
                }
                // --- END FIX 2 ---

                int fid = NextFileId();
                // File constructor: File(int id, const std::string& name, const std::string& path, const std::string& owner, const std::string& perms, const std::string& content)
                std::string filePath_virt = (parentV == "/" ? std::string("/") + fname : parentV + "/" + fname);
                File f(fid, fname, filePath_virt, username, loadedPerms, ""); 
                files.push_back(f);
                directories[parentIndex].addFile(f);
            }
        }
    }
};
