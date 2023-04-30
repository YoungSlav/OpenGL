#pragma once

#include <iostream>

enum ELog
{
	Log,
	Warning,
	Error
};

class Log
{
public:
	static void LogMessage(const std::string& Message, ELog Type = ELog::Log);
	static void LogMessage(const char* const Message, ELog Type = ELog::Log);
};
