#pragma once

#include <cstdlib>
#include <filesystem>
#include <functional>
#include <iostream>
#include <optional>
#include <sstream>
#include <utility>
#include <unordered_map>

namespace fs = std::filesystem;

class Shell {
public:
    Shell(const std::string prompt = "$ ");

private:
    void Run();
    void AddPaths(const std::string &str, const char delim);
    bool CheckCommand(const std::string &command);
    bool ExecCommand(const std::string &command);
    std::vector<std::string> TokenizeStr(const std::string &str);
    std::pair<std::string, size_t> getNextWord(const std::string &str, const size_t start);

    void CommandCd();
    void CommandEcho();
    void CommandExit();
    void CommandPwd();
    void CommandType();

    const std::unordered_map<std::string, std::function<void()> > m_Commands{
        { "cd", std::bind(&Shell::CommandCd, this) },
        { "echo", std::bind(&Shell::CommandEcho, this) },
        { "exit", std::bind(&Shell::CommandExit, this) },
        { "pwd", std::bind(&Shell::CommandPwd, this) },
        { "type", std::bind(&Shell::CommandType, this) },
    };

    std::vector<std::string> m_TokenizedCommand;
    std::string m_Prompt;
    std::vector<fs::path> m_PathEnv;
};
