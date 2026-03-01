#pragma once
#include <filesystem>
#include <fstream>
#include <deque>
#include <vector>
#include <string>
#include <chrono>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <iostream>

#define LOGFILES_PATH "Resources/LogFiles/"
#define LOGFILES_DATATYPE ".txt"

enum LogTypes 
{
    ERROR,
    WARNING,
};

enum LogColors
{
    RED,
    BLUE,
    YELLOW,
    GRAY,
    GREEN,
    CYAN,
    MAGENTA,
    WHITE,
    BLACK,
    RESET
};

struct LogTag
{
    std::string msg;
    LogColors color;
};

struct Log
{
    LogTypes type;
    std::string msg;
    std::string time;
    std::vector<LogTag> tags;
};

void removeSubstrs(std::string& s, const std::string& pattern);
std::string time_to_string();
std::string FormatLogMessage(Log& log);
std::string ColorToAscii(LogColors color); // Converts the color enum to an ASCII escape code

class Logger {
    std::string m_Name; // Name of the logger
public:
    Logger(const std::string& Name) : m_Name(Name) {}
    
    std::deque<Log> Logs; // Each error/warning message that should be logged goes here.
    std::vector<LogTag> PendingTags; // Pending tags, ready to be added to the next log message
    std::string GetValidFileName(std::string& FileName);
    void AppendLogTag(const std::string& TagMessage, LogColors color);
    void AssignPendingTags(Log& log);
    void LogError(const std::string& ErrorMessage);
    void LogWarning(const std::string& WarningMessage);
    void DumpLogs();
    void CreateLogFile();
    void RenameLogger(const std::string& NewName);
};