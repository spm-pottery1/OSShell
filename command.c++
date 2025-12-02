#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "directory.c++" 
#include "user.c++"      

class command
{
private:
    std::string name;
    std::vector<std::string> args;

public:
    // Base constructor
    command() : name(""), args({}) {}

    // Constructor with command name
    command(std::string commandName) : name(commandName), args({}) {}

    // Constructor with name and arguments
    command(const std::string &cmd_name, const std::vector<std::string> &cmd_args)
        : name(cmd_name), args(cmd_args) {}

    // getters
    const std::string &getName() const
    {
        return name;
    }

    const std::vector<std::string> &getArgs() const
    {
        return args;
    }

    // Mutators
    void setName(const std::string &cmd_name)
    {
        name = cmd_name;
    }

    void setArgs(const std::vector<std::string> &cmd_args)
    {
        args = cmd_args;
    }

    void addArg(const std::string &arg)
    {
        args.push_back(arg);
    }

    std::string toString() const
    {
        std::string result = "Command: " + name + "\nArguments:";
        for (const auto &arg : args)
        {
            result += " " + arg;
        }
        return result;
    }

    // Virtual destructor for proper cleanup in derived classes
    virtual ~command() = default;

    virtual void execute(user &currentUser) = 0; 
};