#ifndef LOGGING_SYSTEM_H
#define LOGGING_SYSTEM_H

#include <string>
#include <string_view>

enum class Severity
{
	INFO,
	WARNING,
	FATAL
};

class LogSystem
{
private:
	std::string directory;
private:
	// Outputs the log message to the console.
	void OutputToConsole(const std::string_view& msg, Severity severity) const;

	// Outputs the log message to the game runtime log file.
	void OutputToFile(const std::string_view& msg, Severity severity) const;
private:
	LogSystem();
public:
	LogSystem(const LogSystem& other) = delete;
	LogSystem(LogSystem&& temp) noexcept = delete;
	~LogSystem() = default;

	LogSystem& operator=(const LogSystem& other) = delete;
	LogSystem& operator=(LogSystem&& temp) noexcept = delete;

	// Outputs log to console if in DEBUG mode, else logs are outputted to the game runtime file.
	void OutputLog(const std::string_view& msg, Severity severity) const;

	// Returns singleton instance object of this class.
	static LogSystem& GetInstance();
};

#endif
