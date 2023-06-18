#pragma once

#include <source_location>
#include <string_view>
#include <assert.h>

#include <iostream>
#include <string>
#include <vector>


enum class LogPriority
{
	InfoPriority,
	WarnPriority,
	ErrorPriority,
	CriticalPriority
};

class Logger
{
private:

public:
	static const char* logInfo;
	static const char* filepath;
	static LogPriority Priority;
	static FILE* file;
	static void SetPriority(LogPriority NewPriority) { Priority = NewPriority; }
	static std::vector<std::string> logList;

	static void EnableFileOuput()
	{		
		filepath = "log.txt";
		enable_file_output();
	}

	static void EnableFileOuput(const char* new_filepath)
	{
		filepath = new_filepath;
		enable_file_output();
	}

	static void CloseFileOutput()
	{
		free_file();
	}

	static void Info(const std::string_view msg, const std::source_location location = std::source_location::current())
	{
		Log(LogPriority::InfoPriority, "[INFO] ", msg, location);
	}

	static void Warn(const std::string_view msg, const std::source_location location = std::source_location::current())
	{
		Log(LogPriority::WarnPriority, "[WARNING] ", msg, location);
	}
	
	static void Error(const std::string_view msg, const std::source_location location = std::source_location::current())
	{
		Log(LogPriority::ErrorPriority, "[ERROR] ", msg, location);
	}

	static void Critical(const std::string_view msg, const std::source_location location = std::source_location::current())
	{
		Log(LogPriority::CriticalPriority, "[CRITICAL] ", msg, location);
	}

#pragma warning(disable : 4996)

	static void Log(LogPriority log_priority, const std::string msg_priority, const std::string_view msg, const std::source_location location = std::source_location::current())
	{
		if (Priority <= log_priority)
		{
			std::time_t current_time = std::time(0);
			std::tm* timestamp = std::localtime(&current_time);
			char buffer[80];
			strftime(buffer, 80, "%c", timestamp);

			std::cout << buffer << std::endl;

			if (log_priority == LogPriority::InfoPriority)
				std::cout << msg_priority << msg << std::endl;
			else
				std::cout << msg_priority << "file : " << location.file_name() << "(" << location.line() << ":" << location.column() << ") `" << location.function_name() << "`: " << msg << std::endl;

			int i = location.line();
			int j = location.column();

			if (file)
			{
				fprintf(file, "%s\t", buffer);
				fprintf(file, "\n");

				fprintf(file, msg_priority.c_str());
				fprintf(file, location.file_name());
				fprintf(file, "(");

				std::string str = std::to_string(i);
				fprintf(file, str.c_str());

				fprintf(file, ":");

				std::string str2 = std::to_string(j);
				fprintf(file, str2.c_str());
				
				fprintf(file, ") `");
				fprintf(file, location.function_name());
				fprintf(file, "`: ");
				fprintf(file, msg.data());
				fprintf(file, "\n");
				
			}
			
			std::string info = msg_priority + /*"file : " + location.file_name() + "(" + std::to_string(i) + ":" + std::to_string(j) + ") `" */+ location.function_name() + "`: " + msg.data();
			logList.push_back(info.c_str());
		}
	}

	static void enable_file_output()
	{
		if (file != 0)
		{
			fclose(file);
		}

		file = fopen(filepath, "a");

		if (file == 0)
		{
			Logger::Error("Failed to open file");
		}
	}

	static void free_file()
	{
		fclose(file);
		file = 0;
	}
};