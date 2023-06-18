
#include "Logger.h"

LogPriority Logger::Priority = LogPriority::InfoPriority;
const char* Logger::filepath = 0;
FILE* Logger::file = 0;
std::vector<std::string> Logger::logList;