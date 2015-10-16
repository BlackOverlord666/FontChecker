#pragma once

#include <Windows.h>
#include <dwrite.h>
#include <iostream>
#include <sstream>

#define RELEASE( p ) { if( p ) { p->Release(); p = nullptr; }}

#define ZeroStruct( s ) ZeroMemory( s, sizeof( s ) )

#define REVERSE2( b ) b = ( ( ( b ) & 0xff ) << 8 ) | ( ( b ) & 0xff00 ) >> 8

#define CRLF ( ( 10 << 8 ) | 13 )
#define BOM ( 0xBFBBEF)