
#include <iostream>
#include <vector>
#include <string>
#include "file.c++"

struct Directory {
private:
    int id;
    int fileIdCounter = 0;
    std::string dirName;
    std::string dirPath;
    std::string owner; 
    std::vector<Directory> subdirectories;
    std::vector<File> files;

    public:

    // Constructor
    Directory(int dirId, const std::string& name, const std::string& path,const std::string& dirOwner)
        : id(dirId), dirName(name), dirPath(path), owner(dirOwner) {
    }

    //Getters
    int getId() const {
        return id;
    }

    int getNextFileId() {
        return ++fileIdCounter;
    }

    std::string getDirName() const {
        return dirName;
    }

    std::string getDirPath() const {
        return dirPath;
    }

    std::string getOwner() const {
        return owner;
    }

    std::vector<Directory> getSubdirectories() const {
        return subdirectories;
    }

    std::vector<File> getFiles() const {
        return files;
    }

    std::vector<File> getFile(int n) const {
        if (n < 0 || n >= files.size()) {
            return std::vector<File>();  // Return empty vector if index is out of bounds
        }
        return std::vector<File>{files[n]};
    }

    std::string getSubdirectoryNames() const {
        std::string names;
        for (const auto& subdir : subdirectories) {
            if (!names.empty()) names += ", ";
            names += subdir.getDirName();
        }
        return names;
    }

    std::string getFileNames() const {
        std::string names;
        for (const auto& file : files) {
            if (!names.empty()) names += ", ";
            names += file.getFileName();
        }
        return names;
    }   

    

    std::string toString() const {
        return "Directory[ID=" + std::to_string(id) + ", Name=" + dirName + ",\n" +
               "Path=" + dirPath + ",\n" +
               "Owner=" + owner + ",\n" +
               "Subdirectories=[" + getSubdirectoryNames() + "],\n" +
               "Files=[" + getFileNames() + "]]";
    }

    //Setters

    void setId(int dirId) {
        id = dirId;
    }

    void setDirName(const std::string& name) {
        dirName = name;
    }

    void setDirPath(const std::string& path) {
        dirPath = path;
    }

    void setOwner(const std::string& dirOwner) {
        owner = dirOwner;
    }

    void addSubdirectory(const Directory& subdir) {
        subdirectories.push_back(subdir);
    }

    void addFile(const File& file) {
        files.push_back(file);
    }
};