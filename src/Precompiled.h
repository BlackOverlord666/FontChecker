#pragma once

#include <Windows.h>
#include <dwrite.h>

long long num = 1;
bool littleEndian = *(char*) &num == 1;

#define RELEASE( p ) { if( p ) { p->Release(); p = nullptr; }}

#define REVERSE2( b ) b = ( ( ( b ) & 0xff ) << 8 ) | ( ( b ) & 0xff00 ) >> 8
#define FIXBYTEORDER( b ) { if( littleEndian ) REVERSE2( b ); }

#define CRLF ( ( 10 << 8 ) | 13 )
#define BOM ( 0xBFBBEF )

#define PRINT( msg ) WriteConsole( GetStdHandle( STD_OUTPUT_HANDLE ), msg, wcslen( msg ), 0, 0 )
#define PRINTLN() PRINT( L"\n" )
#define PRINTIDX( idx ) { PRINT( L"[" ); wchar_t buf[ 5 ] = { 0 }; _itow_s( idx, buf, sizeof( buf ) / sizeof( *buf ), 10 ); PRINT( buf ); PRINT( L"]" ); }

#define CONSOLE_RED SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_RED )
#define CONSOLE_GREEN SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_GREEN )
#define CONSOLE_WHITE SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE )