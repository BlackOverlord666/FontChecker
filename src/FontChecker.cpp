#include "Precompiled.h"
#include "Datatypes.h"
#include <map>

using namespace std;

map< int, map< int, map< int, int > > > g_encodingMap;


wchar_t* RemoveSpaces( wchar_t* str )
{
	static wchar_t symbols[] = L" \t\n\r-,.\\/";

	int len = wcslen( str );
	for( int i = 0; i < len; ++i )
	{
		if( wcschr( symbols, str[ i ] ) )
			wcscpy( str + i, str + i + 1 );
	}

	return str;
}


bool CheckFontFile( IDWriteFactory* factory, wchar_t* filename )
{
	HRESULT res;
	IDWriteFontFile* file = nullptr;
	IDWriteFontFace* face = nullptr;

	res = factory->CreateFontFileReference( filename, nullptr, &file );
	if( res != S_OK )
	{
		CONSOLE_WHITE;
		PRINT( filename );
		PRINT( L" CreateFontFileReference failed\n" );
		return false;
	}

	BOOL supported;
	DWRITE_FONT_FILE_TYPE fileType;
	DWRITE_FONT_FACE_TYPE faceType;
	UINT32 numFaces;

	res = file->Analyze( &supported, &fileType, &faceType, &numFaces );
	if( res != S_OK )
	{
		CONSOLE_WHITE;
		PRINT( filename );
		PRINT( L" Analyze failed\n" );
		return false;
	}

	if( !supported )
	{
		CONSOLE_WHITE;
		PRINT( filename );
		PRINT( L" not supported\n" );
		return false;
	}

	int validFaces = 0;

	for( int faceIdx = 0; faceIdx < numFaces; ++faceIdx )
	{
		res = factory->CreateFontFace( faceType, 1, &file, faceIdx, DWRITE_FONT_SIMULATIONS_NONE, &face );
		if( res != S_OK )
		{
			CONSOLE_WHITE;
			PRINT( filename );
			if( numFaces > 1 )
				PRINTIDX( faceIdx );
			PRINT( L" CreateFontFace failed\n" );
			return false;
		}

		char* data = nullptr;
		unsigned int size = 0;
		void* context = nullptr;
		BOOL exists = false;

		res = face->TryGetFontTable( DWRITE_MAKE_OPENTYPE_TAG( 'n', 'a', 'm', 'e' ), (const void**) &data, &size, &context, &exists );
		if( res != S_OK || !exists )
		{
			CONSOLE_WHITE;
			PRINT( filename );
			if( numFaces > 1 )
				PRINTIDX( faceIdx );
			PRINT( L" TryGetFontTable failed\n" );
			return false;
		}

		char* p = data;

		Header head = *(Header*) p;
		head.FixByteOrder();
		p += sizeof( Header );

		const wchar_t* familyName = nullptr;
		const wchar_t* ufid = nullptr;

		bool ok = true;

		for( int i = 0; i < head.count; ++i )
		{
			NameRecord name = *(NameRecord*) p;
			name.FixByteOrder();
			p += sizeof( NameRecord );

			const wchar_t** base;
			if( name.nameID == 1 )
				base = &familyName;
			else if( name.nameID == 3 )
				base = &ufid;
			else
				continue;

			// filter out non-english tags
			if( name.platformID == 1 && name.encodingID != 0 )
				continue;

			if( name.platformID == 3 && name.languageID != 1033 )
				continue;

			wchar_t* buffer;
			int size;
			if( name.platformID == 0 || name.platformID == 3 )
			{
				size = name.length / 2;
				buffer = new wchar_t[ size + 1 ];
				memcpy( buffer, data + head.stringOffset + name.offset, name.length );
				for( int j = 0; j < size; ++j )
					FIXBYTEORDER( buffer[ j ] );
			}
			else if( int encoding = g_encodingMap[ name.platformID ][ name.encodingID ][ name.languageID ] )
			{
				size = MultiByteToWideChar( encoding, MB_PRECOMPOSED, data + head.stringOffset + name.offset, name.length, nullptr, 0 );
				buffer = new wchar_t[ size +1 ];
				MultiByteToWideChar( encoding, MB_PRECOMPOSED, data + head.stringOffset + name.offset, name.length, buffer, size );
			}
			else
			{
				CONSOLE_WHITE;
				PRINT( filename );
				if( numFaces > 1 )
					PRINTIDX( faceIdx );
				PRINT( L" Unsupported encoding\n" );
				continue;
			}

			buffer[ size ] = 0;
			_wcslwr_s( buffer, size + 1);
			RemoveSpaces( buffer );

			if( !*base )
			{
				*base = buffer;
				continue;
			}
			else if( wcsncmp( *base, buffer, min( wcslen( *base ), wcslen( buffer ) ) ) != 0 )
			{
				CONSOLE_RED;
				PRINT( filename );
				if( numFaces > 1 )
					PRINTIDX( faceIdx );
				PRINT( L" has suspicious tags\n" );
				delete[] buffer;
				ok = false;
				break;
			}
		}
	

		if( familyName )
		{
			delete[] familyName;
			familyName = nullptr;
		}

		if( ufid )
		{
			delete[] ufid;
			ufid = nullptr;
		}

		face->ReleaseFontTable( context );
		RELEASE( face );

		if( ok )
		{
			CONSOLE_GREEN;
			PRINT( filename );
			if( numFaces > 1 )
				PRINTIDX( faceIdx );
			PRINT( L" seems like valid\n" );
			++validFaces;
		}
	}
	
	RELEASE( file );

	return validFaces == numFaces;
}


int wmain( int argc, const wchar_t** argv )
{
	wchar_t dir[ MAX_PATH ];
	if( argc > 1 )
	{
		wcscpy_s( dir, MAX_PATH, argv[ 1 ] );
		if( !SetCurrentDirectory( dir ) )
		{
			CONSOLE_WHITE;
			PRINT( L"Wrong directory!" );
			system( "pause" );
			return 1;
		}
	}
	
	GetCurrentDirectory( MAX_PATH, dir );	

	int len = wcslen( dir );
	if( len > MAX_PATH - 3 )
		return 1;

	if( dir[ len - 1 ] != '\\' )
		dir[ len++ ] = '\\';

	dir[ len++ ] = '*';
	dir[ len++ ] = '\0';

	g_encodingMap[ 1 ][ 0 ][ 0 ] = 10000; // Mac Roman
	g_encodingMap[ 1 ][ 0 ][ 32 ] = 10000; // Mac Roman
	g_encodingMap[ 1 ][ 1 ][ 11 ] = 10001; // Mac Japanese
	g_encodingMap[ 1 ][ 1 ][ 65535 ] = 10001; // Mac Japanese
	g_encodingMap[ 2 ][ 0 ][ 0 ] = 20127; // 7-bit ascii

	HRESULT res;
	IDWriteFactory* factory = nullptr;

	res = DWriteCreateFactory( DWRITE_FACTORY_TYPE_SHARED, __uuidof( IDWriteFactory ), (IUnknown**) &factory );


	WIN32_FIND_DATA ffd;
	HANDLE find = FindFirstFile( dir, &ffd );

	if( find != INVALID_HANDLE_VALUE )
	{
		int counter = 0;
		do
		{
			if( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				continue;

			wchar_t* name = ffd.cFileName;
			int len = wcslen( name );
			_wcslwr_s( name + len - 4, 5 );

			if( wcscmp( name + len - 4, L".ttf" ) == 0 || wcscmp( name + len - 4, L".otf" ) == 0 || wcscmp( name + len - 4, L".ttc" ) == 0 )
			{
				CheckFontFile( factory, name );
				++counter;
			}

		} while( FindNextFile( find, &ffd ) != 0 );

		PRINTIDX( counter );
		PRINT( L" files checked" );
		PRINTLN();
	}
	
	FindClose( find );
	RELEASE( factory );
	CONSOLE_WHITE;
	system( "pause" );
	return 0;
}