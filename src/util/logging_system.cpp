#include <util/logging_system.h>
#include <util/directory_system.h>
#include <util/timestamp.h>

#include <Windows.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

LogSystem::LogSystem()
{
#ifndef _DEBUG
	// Get the APPDATA directory, then open up an empty runtime log file
	this->directory = Util::GetGameRequisitesDirectory();
	std::ofstream logFile(this->directory + "runtime_log.txt", std::ios::trunc);
#endif
}

void LogSystem::OutputToConsole(const std::string_view& msg, Severity severity) const
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	switch (severity)
	{
	case Severity::INFO:
		SetConsoleTextAttribute(consoleHandle, 15); // Change console text color to white
		std::cout << "[" << Util::GetTimestampStr() << "] Info: " << msg << std::endl;
		break;
	case Severity::WARNING:
		SetConsoleTextAttribute(consoleHandle, 14); // Change console text color to yellow
		std::cout << "[" << Util::GetTimestampStr() << "] Warning: " << msg << std::endl;
		break;
	case Severity::FATAL:
		SetConsoleTextAttribute(consoleHandle, 12); // Change console text color to red
		std::cout << "[" << Util::GetTimestampStr() << "] Error: " << msg << std::endl;
		
		std::this_thread::sleep_for(std::chrono::minutes(1)); // Pause program to allow time for fatal error to be read before exiting
		std::exit(-1);
		break;
	}
}

void LogSystem::OutputToFile(const std::string_view& msg, Severity severity) const
{
	std::ofstream logFile(this->directory + "runtime_log.txt", std::ios::app); // Open the log file in append mode
	
	switch (severity)
	{
	case Severity::INFO:
		logFile << "[" << Util::GetTimestampStr() << "] Info: " << msg << std::endl;
		break;
	case Severity::WARNING:
		logFile << "[" << Util::GetTimestampStr() << "] Warning: " << msg << std::endl;
		break;
	case Severity::FATAL:
		logFile << "[" << Util::GetTimestampStr() << "] Error: " << msg << std::endl;
		std::exit(-1);
		break;
	}
}

void LogSystem::OutputLog(const std::string_view& msg, Severity severity) const
{
#ifdef _DEBUG
	this->OutputToConsole(msg, severity);
#else
	this->OutputToFile(msg, severity);
#endif
}

LogSystem& LogSystem::GetInstance()
{
	static LogSystem instance;
	return instance;
}
