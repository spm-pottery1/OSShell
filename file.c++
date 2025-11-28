#include <string>
#include <iostream>

struct File {
private:
    int id;
    std::string fileName;
    std::string filePath;
    std::string owner;
    std::string permissions;
    std::string content;
    std::string EOFMarker = "<EOF>";

public:

    // Constructor
    File(int fileId, const std::string& name, const std::string& path,const std::string& fileOwner, const std::string& perms, const std::string& fileContent)
        : id(fileId), fileName(name), filePath(path), owner(fileOwner),
          permissions(perms), content(fileContent) {
    }
    // Getters
    int getId() const {
        return id;
    }

    std::string getFileName() const {
        return fileName;
    }
    
    std::string getFilePath() const {
        return filePath;
    }

    std::string getOwner() const {
        return owner;
    }

    std::string getPermissions() const {
        return permissions;
    }

    std::string getContent() const {
        return content;
    }

    std::string toString() const {
        return "File[ID=" + std::to_string(id) + ", Name=" + fileName + ",\n" +
               "Path=" + filePath + ",\n" +
               "Owner=" + owner + ",\n" +
               "Permissions=" + permissions + ",\n" +
               "Content=" + content + "]";
    }

    
    // Setters
    void setId(int fileId) {
        id = fileId;
    }

    void setFileName(const std::string& name) {
        fileName = name;
    }

    void setFilePath(const std::string& path) {
        filePath = path;
    }

    void setOwner(const std::string& fileOwner) {
        owner = fileOwner;
    }

    void setPermissions(const std::string& perms) {
        permissions = perms;
    }

    void setContent(const std::string& fileContent) {
        content = fileContent;
    }
    
    void appendContent(const std::string& additionalContent) {
        content += additionalContent;
    }


};