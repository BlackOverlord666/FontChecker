#pragma once

#include <Windows.h>
#include <dwrite.h>
#include <sstream>

#define RELEASE( p ) { if( p ) { p->Release(); p = nullptr; }}

#define REVERSE2( b ) b = ( ( ( b ) & 0xff ) << 8 ) | ( ( b ) & 0xff00 ) >> 8

#define PRINT( msg ) WriteConsole( GetStdHandle( STD_OUTPUT_HANDLE ), msg, wcslen( msg ), 0, 0 )
#define PRINTLN() PRINT( L"\n" )