#include "Precompiled.h"
#include "Datatypes.h"

using namespace std;


bool IsValidFont( IDWriteFactory* factory, wchar_t* filename )
{
	HRESULT res;
	IDWriteFontFile* file = nullptr;
	IDWriteFontFace* face = nullptr;

	res = factory->CreateFontFileReference( filename, nullptr, &file );
	if( res != S_OK )
	{
		PRINT( L"CreateFontFileReference failed\n" );
		return false;
	}

	res = factory->CreateFontFace( DWRITE_FONT_FACE_TYPE_TRUETYPE, 1, &file, 0, DWRITE_FONT_SIMULATIONS_NONE, &face );
	if( res != S_OK )
	{
		PRINT( L"CreateFontFace failed\n" );
		return false;
	}

	char* data = nullptr;
	unsigned int size = 0;
	void* context = nullptr;
	BOOL exists = false;

	res = face->TryGetFontTable( DWRITE_MAKE_OPENTYPE_TAG( 'n', 'a', 'm', 'e' ), (const void**) &data, &size, &context, &exists );
	if( res != S_OK || !exists )
	{
		PRINT( L"TryGetFontTable failed\n" );
		return false;
	}

	char* p = data;

	Header head = *(Header*) p;
	head.Reverse();
	p += sizeof( Header );

	const wchar_t* familyName = nullptr;
	const wchar_t* ufid = nullptr;

	bool ok = true;

	NameRecord* names = new NameRecord[ head.count ];
	for( int i = 0; i < head.count; ++i )
	{
		NameRecord name = *(NameRecord*) p;
		name.Reverse();
		p += sizeof( NameRecord );

		const wchar_t** base;
		if( name.nameID == 1 )
			base = &familyName;
		else if( name.nameID == 3 )
			base = &ufid;
		else
			continue;

		wchar_t* buffer = nullptr;

		// mac roman, code page - https://msdn.microsoft.com/en-us/library/windows/desktop/dd317756%28v=vs.85%29.aspx
		if( name.platformID == 1 && name.encodingID == 0 && name.languageID == 0 )
		{
			int size = MultiByteToWideChar( 10000, MB_PRECOMPOSED, data + head.stringOffset + name.offset, name.length, nullptr, 0 );
			buffer = new wchar_t[ size + 1 ];
			MultiByteToWideChar( 10000, MB_PRECOMPOSED, data + head.stringOffset + name.offset, name.length, buffer, size );
			buffer[ size ] = 0;
		}
		// unicode english
		else if( name.platformID == 0 && name.encodingID == 3 && name.languageID == 0
			|| name.platformID == 3 && name.encodingID == 1 && name.languageID == 1033 )
		{
			int size = name.length / 2;
			buffer = new wchar_t[ size + 1 ];
			memcpy( buffer, data + head.stringOffset + name.offset, name.length );
			buffer[ size ] = 0;
			for( int i = 0; i < size; ++i )
				REVERSE2( buffer[ i ] );
		}
		else
		{
			continue;
		}

		if( !*base )
		{
			*base = buffer;
			continue;
		}
		else if( wcscmp( *base, buffer ) != 0 )
		{
			PRINT( L"Font " );
			PRINT( filename );
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
	RELEASE( file );

	return ok;
}


int wmain( int argc, const wchar_t** argv )
{
	HRESULT res;
	IDWriteFactory* factory = nullptr;
	
	res = DWriteCreateFactory( DWRITE_FACTORY_TYPE_SHARED, __uuidof( IDWriteFactory ), (IUnknown**)&factory );
	
	wchar_t dir[ MAX_PATH ];
	if( argc > 1 )
	{
		wcsncpy( dir, argv[ 1 ], MAX_PATH );
		if( !SetCurrentDirectory( dir ) )
		{
			PRINT( L"Wrong directory!" );
			return 1;
		}
	}
	else
	{
		GetCurrentDirectory( MAX_PATH, dir );
	}

	int len = wcslen( dir );
	if( len > MAX_PATH - 3 )
		return 1;

	if( dir[ len ] != '\\' )
		dir[ len++ ] = '\\';

	dir[ len++ ] = '*';


	WIN32_FIND_DATA ffd;
	HANDLE find = FindFirstFile( dir, &ffd );

	if( find == INVALID_HANDLE_VALUE )
		return 1;

	do
	{
		wchar_t* name = ffd.cFileName;
		int len = wcslen( name );

		if( wcscmp( name + len - 4, L".ttf" ) == 0 || wcscmp( name + len - 4, L".otf" ) || wcscmp( name + len - 4, L".ttc" ) )
			IsValidFont( factory, name );

	} while( FindNextFile( find, &ffd ) != 0 );

	
	FindClose( find );
	RELEASE( factory );

	return 0;
}