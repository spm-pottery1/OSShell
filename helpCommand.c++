#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "directory.c++"
#include "user.c++"
#include "command.c++"

// Redefining the struct for abstract base class 'command' to inherit from
// This structure is necessary because 'command.c++' is not a header.
struct helpCommand : public command
{
private:
    std::string helpText =
        R"(Available Commands:
  help                Display this help message
  adduser [username]  Add a new user (prompts for password)
  mkdir [name]        Create a new subdirectory in the current directory
  cd <dir>            Change directory (use .. to go up)
  ls                  List subdirectories and files in the current directory
  touch <filename>    Create an empty file in the current directory (disk-backed)
  echo <text> [> | >>] <file>
                      Write <text> to <file>. Use '>' to overwrite, '>>' to append. Supports quoted text.
  cat <filename>      Display the contents of a file
  rm <filename>       Remove a file from the current directory (deletes from disk)
  chmod <perms> <file>
                      Change file permissions. Use +r/+w/+x (add), -r/-w/-x (remove), or rwx (set).
                      Examples: chmod +rw file.txt, chmod -w file.txt, chmod rwx file.txt
  exec <scriptfile>
                      Execute a script file containing OSShell commands line by line
  exit                Exit the shell / logout

Usage notes:
  - adduser appends "username password" to /home/simon/Documents/OSShellRoot/users.txt
  - mkdir and cd operate on the user's in-memory directory list; mkdir also creates the physical directory.
  - ls prints names for subdirectories and files in the current directory (direct children only).
  - touch, echo, cat, rm, and chmod operate on the mapped physical path: /home/simon/Documents/OSShellRoot/<username>/...
  - echo supports simple quoting (single token or quoted string).
  - chmod stores permissions in a .perms metadata file (e.g., file.txt.perms contains "rw").
  - exec reads and executes commands from a script file line by line, scripting hasn't been tested extensively.
)";

public:
    // Constructor initializes the base class with the command name
    helpCommand(std::vector<std::string> args = {})
        : command("help", args) {}

    // The required execute implementation
    void execute(user &currentUser) override
    {
        std::cout << helpText << std::endl;
    }
};