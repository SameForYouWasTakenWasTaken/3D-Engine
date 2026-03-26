#include "vendor/Logger.hpp"

namespace fs = std::filesystem;

void removeSubstrs(std::string& string, const std::string& pattern)
{
    std::string::size_type pos = 0;
    while ((pos = string.find(pattern, pos)) != std::string::npos) {
        string.erase(pos, pattern.length());
    }
}

std::string time_to_string()
{
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm local_time{};

#ifdef _WIN32
    localtime_s(&local_time, &t);
#else
    localtime_r(&t, &local_time);
#endif

    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(2) << local_time.tm_hour << ":"
        << std::setw(2) << local_time.tm_min << ":"
        << std::setw(2) << local_time.tm_sec;

    return oss.str();
}

std::string ColorToAscii(LogColors color)
{

    switch (color)
    {
        case LogColors::RESET:
            return "\033[0m";
        case LogColors::RED:
            return "\033[31m";
        case LogColors::BLUE:
            return "\033[94m";
        case LogColors::YELLOW:
            return "\033[33m";
        case LogColors::GRAY:
            return "\033[37m";
        case LogColors::GREEN:
            return "\033[32m";
        case LogColors::CYAN:
            return "\033[36m";
        case LogColors::MAGENTA:
            return "\033[35m";
        case LogColors::WHITE:
            return "\033[97m";
        case LogColors::BLACK:
            return "\033[30m";
        default:
            return "";
    }
}
std::string FormatLogMessage(Log& log)
{
    std::stringstream sstream;

    // If it has tags, assign all of them
    std::vector<LogTag> tags = log.tags;
    if (!tags.empty())
        std::for_each(tags.begin(), tags.end(), [&sstream](LogTag& tag){
            sstream 
            << ColorToAscii(tag.color) 
            << "[" << tag.msg << "] "  
            << ColorToAscii(LogColors::RESET); // Reset back to default color
        });

    // Standard warnings and errors
    if (log.type == LogTypes::WARNING)
    {
        sstream 
        << ColorToAscii(LogColors::YELLOW) 
        << "[SEVERITY: WARNING] " 
        << ColorToAscii(LogColors::RESET);
    }
    else if (log.type == LogTypes::ERROR)
    {
        sstream 
        << ColorToAscii(LogColors::RED) 
        << "[SEVERITY: ERROR] " 
        << ColorToAscii(LogColors::RESET);
    }

    sstream 
    << ColorToAscii(LogColors::GRAY) 
    << "[" << log.time << "]: " 
    << ColorToAscii(LogColors::RESET) << 
    log.msg << "\n";

    return sstream.str();
}

void Logger::AppendLogTag(const std::string& TagName, LogColors color)
{
    LogTag tag;
    tag.color = color;
    tag.msg = TagName;
    // No time needed, since it is useless for tags.

    PendingTags.push_back(tag);
}

void Logger::AssignPendingTags(Log& log)
{
    if (!PendingTags.empty())
        std::for_each(PendingTags.begin(), PendingTags.end(), [&log](LogTag& tag){
            log.tags.push_back(tag);
        });
}

void Logger::LogError(const std::string& msg)
{
    std::string time = time_to_string();
    
    Log log;
    log.msg = msg;
    log.time = time;
    log.type = LogTypes::ERROR;
    AssignPendingTags(log);
    PendingTags.clear();

    Logs.push_back(log);
}

void Logger::LogWarning(const std::string& msg)
{
    std::string time = time_to_string();

    Log log;
    log.msg = msg;
    log.time = time;
    log.type = LogTypes::WARNING;
    AssignPendingTags(log);
    PendingTags.clear();

    Logs.push_back(log);
}

void Logger::DumpLogs()
{
    if (Logs.empty())
        return;
    
    std::for_each(Logs.begin(), Logs.end(), [](Log& log){
        std::string log_msg = FormatLogMessage(log);
        std::cout << log_msg;
    });

    std::cout << std::endl; // Clear buffer
    PendingTags.clear(); // Clear any pending tags
    Logs.clear(); // Clear logs
}

std::string Logger::GetValidFileName(std::string& FileTitle)
{
    
    std::string file_name = LOGFILES_PATH + m_Name + "_" + FileTitle;
    std::string file_path = file_name + LOGFILES_DATATYPE;
    int suffix = 1; // Always changes to _1, _2, _3, ... if more similar files exist

    if (!fs::exists(file_path))
        return file_path;
    

    // Retry until suffix works, because all have already been taken
    while (true)
    {
        std::string new_file_name = file_name + "_" + std::to_string(suffix) + LOGFILES_DATATYPE;
        if(fs::exists(new_file_name))
        {
            suffix++;
            continue;
        }
        
        return new_file_name;
    }
}

void Logger::CreateLogFile()
{
    time_t now = time(0);
    tm* localTime = localtime(&now);
    std::stringstream Date;

    Date << 
    (localTime->tm_year + 1900) << "-" // year
    << (localTime->tm_mon + 1) << "-" // month
    << localTime->tm_mday; // day

    std::string FileName = Date.str();
    FileName = GetValidFileName(FileName); // Adds the suffix and stuff, if files are the same name

    //?   structure of file name (1 file):                NameOfLogger_DATE.txt
    //?structure of file name (multiple and same date?):  NameOfLogger_DATE_suffix.txt
    std::ofstream file(FileName);
   
    if (!file.is_open())
    {
        AppendLogTag("FILE", LogColors::YELLOW);
        LogError("Couldn't open log file!");
        return;
    }

    std::for_each(Logs.begin(), Logs.end(), [&file](Log& log){
        std::string logMsg = FormatLogMessage(log);
        // Look, I know this is a terrible approach, but pls dont hate me..
        for (int i{}; i < 20; i++)
        {
            LogColors color = static_cast<LogColors>(i);
            std::string ascii = ColorToAscii(color);
            if (ascii == "") break;
            removeSubstrs(logMsg, ascii); // Remove the colors
        }
        file << logMsg;
    });
    file.close();
    DumpLogs(); // If not done yet, will dump into terminal and clear everything.
}

void Logger::RenameLogger(const std::string& NewName)
{
    m_Name = NewName;
}