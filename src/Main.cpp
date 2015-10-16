#include "Precompiled.h"
#include "Datatypes.h"

using namespace std;

wchar_t* GetUnicodeString( wchar_t* dest, void* src, int symbolsCount )
{
	int bytesCounts = symbolsCount << 1;
	memcpy( dest, src, bytesCounts );
	dest[ symbolsCount ] = CRLF;
	for( int i = 0; i < symbolsCount; ++i )
		REVERSE2( dest[ i ] );

	return dest;
}


int main( int argc, const char** argv )
{
	IDWriteFactory* factory = nullptr;
	IDWriteFontFile* file = nullptr;
	IDWriteFontFace* face = nullptr;
	HRESULT res;

	res = DWriteCreateFactory( DWRITE_FACTORY_TYPE_SHARED, __uuidof( IDWriteFactory ), (IUnknown**)&factory );
	res = factory->CreateFontFileReference( L"arial.ttf", nullptr, &file );
	res = factory->CreateFontFace( DWRITE_FONT_FACE_TYPE_TRUETYPE, 1, &file, 0, DWRITE_FONT_SIMULATIONS_NONE, &face );

	char* data = nullptr;
	unsigned int size = 0;
	void* context = nullptr;
	BOOL exists = false;

	res = face->TryGetFontTable( DWRITE_MAKE_OPENTYPE_TAG( 'n', 'a', 'm', 'e' ), (const void**) &data, &size, &context, &exists );
	cout << exists << endl;
	
	char* p = data;
	
	Header head = *(Header*)p;
	head.Reverse();
	p += sizeof( Header );

	HANDLE f = CreateFile( L"arial.txt", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
	DWORD dummy = BOM;
	WriteFile( f, &dummy, 3, 0, 0 );

	NameRecord* names = new NameRecord[ head.count ];
	for( int i = 0; i < head.count; ++i )
	{
		names[ i ] = *(NameRecord*) p;
		names[ i ].Reverse();
		p += sizeof( NameRecord );

		stringstream ss;
		ss << "(" << names[ i ].platformID << "," << names[ i ].encodingID << "," << names[ i ].languageID << "): " << names[ i ].nameID << " - ";
		
		WriteFile( f, ss.str().c_str(), ss.str().size(), 0, 0 );

		if( names[ i ].platformID == 1 && names[ i ].encodingID == 0 && names[ i ].languageID == 0 )
		{
			WriteFile( f, data + head.stringOffset + names[ i ].offset, names[ i ].length, 0, 0 );
			dummy = CRLF;
			WriteFile( f, &dummy, 2, 0, 0 );
		}
		else
		{
			int size = names[ i ].length / 2;
			wchar_t* value = new wchar_t[ size + 1 ];
			GetUnicodeString( value, data + head.stringOffset + names[ i ].offset, size );
			
			int utf8size = WideCharToMultiByte( CP_UTF8, 0, value, size, nullptr, 0, nullptr, nullptr );
			char* utf8 = new char[ utf8size ];
			WideCharToMultiByte( CP_UTF8, 0, value, size, utf8, utf8size, nullptr, nullptr );
			
			WriteFile( f, utf8, utf8size, 0, 0 );
			dummy = CRLF;
			WriteFile( f, &dummy, 2, 0, 0 );
			delete[] utf8;
			delete[] value;
		}
	}

	CloseHandle( f );

	face->ReleaseFontTable( context );
	RELEASE( face );
	RELEASE( file );
	RELEASE( factory );

	return 0;
}