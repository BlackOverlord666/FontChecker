#include "Precompiled.h"
#include "Datatypes.h"
#include <map>
#include <sstream>

using namespace std;


map< int, map< int, map< int, int > > > g_encodingMap;


int wmain( int argc, const wchar_t** argv )
{
	if( argc < 2 )
		return 1;

	g_encodingMap[ 1 ][ 0 ][ 0 ] = 10000; // Mac Roman
	g_encodingMap[ 1 ][ 0 ][ 32 ] = 10000; // Mac Roman
	g_encodingMap[ 1 ][ 1 ][ 11 ] = 10001; // Mac Japanese
	g_encodingMap[ 1 ][ 1 ][ 65535 ] = 10001; // Mac Japanese
	g_encodingMap[ 2 ][ 0 ][ 0 ] = 20127; // 7-bit ascii

	HRESULT res;
	IDWriteFactory* factory = nullptr;
	res = DWriteCreateFactory( DWRITE_FACTORY_TYPE_SHARED, __uuidof( IDWriteFactory ), (IUnknown**) &factory );

	for( int argIdx = 1; argIdx < argc; ++argIdx )
	{
		IDWriteFontFile* file = nullptr;

		wchar_t fontfile[ MAX_PATH ] = { 0 };
		wchar_t txtfile[ MAX_PATH ] = { 0 };

		wcsncpy_s( fontfile, argv[ argIdx ], MAX_PATH );
		wcscpy_s( txtfile, fontfile );
		int len = wcslen( txtfile );
		len -= 4;
		txtfile[ len ] = L'\0';


		res = factory->CreateFontFileReference( fontfile, nullptr, &file );
		if( res != S_OK )
		{
			CONSOLE_WHITE;
			PRINT( fontfile );
			PRINT( L" CreateFontFileReference failed\n" );
			continue;
		}

		BOOL supported;
		DWRITE_FONT_FILE_TYPE fileType;
		DWRITE_FONT_FACE_TYPE faceType;
		UINT32 numFaces;

		res = file->Analyze( &supported, &fileType, &faceType, &numFaces );
		if( res != S_OK )
		{
			CONSOLE_WHITE;
			PRINT( fontfile );
			PRINT( L" Analyze failed\n" );
			continue;
		}

		if( !supported )
		{
			CONSOLE_WHITE;
			PRINT( fontfile );
			PRINT( L" not supported\n" );
			continue;
		}

		for( int faceIdx = 0; faceIdx < numFaces; ++faceIdx )
		{
			IDWriteFontFace* face = nullptr;

			res = factory->CreateFontFace( faceType, 1, &file, faceIdx, DWRITE_FONT_SIMULATIONS_NONE, &face );
			if( res != S_OK )
			{
				CONSOLE_WHITE;
				PRINT( fontfile );
				if( numFaces > 1 )
					PRINTIDX( faceIdx );
				PRINT( L" CreateFontFace failed\n" );
				continue;
			}

			char* data = nullptr;
			unsigned int size = 0;
			void* context = nullptr;
			BOOL exists = false;

			res = face->TryGetFontTable( DWRITE_MAKE_OPENTYPE_TAG( 'n', 'a', 'm', 'e' ), (const void**) &data, &size, &context, &exists );
			if( res != S_OK )
			{
				CONSOLE_WHITE;
				PRINT( fontfile );
				if( numFaces > 1 )
					PRINTIDX( faceIdx );
				PRINT( L" TryGetFontTable failed\n" );
				continue;
			}

			char* p = data;

			Header head = *(Header*) p;
			head.FixByteOrder();
			p += sizeof( Header );

			int offset = 0;
			if( numFaces > 1 )
			{
				txtfile[ len + offset++ ] = L'[';
				txtfile[ len + offset++ ] = L'0' + faceIdx;
				txtfile[ len + offset++ ] = L']';
			}

			txtfile[ len + offset++ ] = L'.';
			txtfile[ len + offset++ ] = L't';
			txtfile[ len + offset++ ] = L'x';
			txtfile[ len + offset++ ] = L't';
			txtfile[ len + offset++ ] = L'\0';

			HANDLE f = CreateFile( txtfile, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
			DWORD dummy = BOM;
			WriteFile( f, &dummy, 3, 0, 0 );

			for( int i = 0; i < head.count; ++i )
			{
				NameRecord name = *(NameRecord*) p;
				name.FixByteOrder();
				p += sizeof( NameRecord );

				stringstream ss;
				ss << "(" << name.platformID << "," << name.encodingID << "," << name.languageID << "): " << name.nameID << " - ";

				WriteFile( f, ss.str().c_str(), ss.str().size(), 0, 0 );

				wchar_t* buffer;
				int size;
				if( name.platformID == 0 || name.platformID == 3 )
				{
					size = name.length / 2;
					buffer = new wchar_t[ size ];
					memcpy( buffer, data + head.stringOffset + name.offset, name.length );
					for( int j = 0; j < size; ++j )
						FIXBYTEORDER( buffer[ j ] );
				}
				else if( int encoding = g_encodingMap[ name.platformID ][ name.encodingID ][ name.languageID ] )
				{
					size = MultiByteToWideChar( encoding, MB_PRECOMPOSED, data + head.stringOffset + name.offset, name.length, nullptr, 0 );
					buffer = new wchar_t[ size ];
					MultiByteToWideChar( encoding, MB_PRECOMPOSED, data + head.stringOffset + name.offset, name.length, buffer, size );
				}
				else
				{
					CONSOLE_WHITE;
					PRINT( fontfile );
					if( numFaces > 1 )
						PRINTIDX( faceIdx );
					PRINT( L" Unsupported encoding\n" );
					continue;
				}

				int utf8size = WideCharToMultiByte( CP_UTF8, 0, buffer, size, nullptr, 0, nullptr, nullptr );
				char* utf8 = new char[ utf8size ];
				WideCharToMultiByte( CP_UTF8, 0, buffer, size, utf8, utf8size, nullptr, nullptr );

				WriteFile( f, utf8, utf8size, 0, 0 );
				dummy = CRLF;
				WriteFile( f, &dummy, 2, 0, 0 );
				delete[] utf8;
				delete[] buffer;
			}

			CloseHandle( f );

			face->ReleaseFontTable( context );
			RELEASE( face );
		}

		RELEASE( file );
	}

	RELEASE( factory );

	system( "pause" );

	return 0;
}