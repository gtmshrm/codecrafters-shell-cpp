#pragma once

#include <cstdlib>
#include <filesystem>
#include <functional>
#include <iostream>
#include <optional>
#include <sstream>
#include <unordered_map>

namespace fs = std::filesystem;

class Shell {
public:
    Shell(const std::string prompt = "$ ");

private:
    void Run();
    void TokenizeString(const std::string& str, const char delim);
    bool CheckCmd(const std::string& cmd);
    bool ExecCmd(const std::string& cmd);

    void CmdType();
    void CmdExit();
    void CmdEcho();
    void CmdPwd();
    void CmdCd();

    const std::unordered_map<std::string, std::function<void()>> m_Cmds {
        {"type", std::bind(&Shell::CmdType, this)},
        {"exit", std::bind(&Shell::CmdExit, this)},
        {"echo", std::bind(&Shell::CmdEcho, this)},
        {"pwd", std::bind(&Shell::CmdPwd, this)},
        {"cd", std::bind(&Shell::CmdCd, this)},
    };

    std::string m_Prompt;
    std::string m_CurrentCmd;
    std::istringstream m_ISStream;
    std::vector<fs::path> m_PathEnv;
};
