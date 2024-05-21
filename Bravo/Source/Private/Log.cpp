#include "stdafx.h"
#include "Log.h"
#include <iostream>
#include <windows.h>

void Log::Print( const std::string& Message, ELog Type )
{
	Log::Print(Message.c_str(), Type);
}

void Log::Print(const char* const Message, ELog Type)
{
	static HANDLE hstdout = GetStdHandle( STD_OUTPUT_HANDLE );
	
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo( hstdout, &csbi );

	char debugMessage[512];

	switch (Type)
	{
	case ELog::Success:
		SetConsoleTextAttribute( hstdout, FOREGROUND_GREEN );
		snprintf(debugMessage, sizeof(debugMessage), "Log: %s\n", Message);
		break;
	case ELog::Log:
		SetConsoleTextAttribute( hstdout, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
		snprintf(debugMessage, sizeof(debugMessage), "Log: %s\n", Message);
		break;
	case ELog::Warning:
		SetConsoleTextAttribute( hstdout, FOREGROUND_GREEN | FOREGROUND_RED );
		snprintf(debugMessage, sizeof(debugMessage), "Warning: %s\n", Message);
		std::cout <<"Warning: ";
		break;
	case ELog::Error:
		SetConsoleTextAttribute( hstdout, FOREGROUND_RED );
		snprintf(debugMessage, sizeof(debugMessage), "Error: %s\n", Message);
		std::cout <<"Error: ";
		break;
	}

	std::cout<< Message <<std::endl;
	OutputDebugString(debugMessage);
	
	SetConsoleTextAttribute( hstdout, csbi.wAttributes );
}