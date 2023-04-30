#include "stdafx.h"
#include "Log.h"
#include <iostream>
#include <windows.h>

void Log::LogMessage( const std::string& Message, ELog Type )
{
	Log::LogMessage(Message.c_str(), Type);
}

void Log::LogMessage(const char* const Message, ELog Type)
{
	static HANDLE hstdout = GetStdHandle( STD_OUTPUT_HANDLE );
	
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo( hstdout, &csbi );

	switch (Type)
	{
	case ELog::Log:
		SetConsoleTextAttribute( hstdout, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
		break;
	case ELog::Warning:
		SetConsoleTextAttribute( hstdout, FOREGROUND_GREEN | FOREGROUND_RED );
		std::cout <<"Warning: ";
		break;
	case ELog::Error:
		SetConsoleTextAttribute( hstdout, FOREGROUND_RED );
		std::cout <<"Error: ";
		break;
	}

	std::cout<< Message <<std::endl;

	SetConsoleTextAttribute( hstdout, csbi.wAttributes );
}