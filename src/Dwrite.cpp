#include "Precompiled.h"
#include "Dwrite.h"


FontCollectionLoader::FontCollectionLoader( WCHAR* filename )
	: m_filename( filename )
{
}

void FontCollectionLoader::Init( WCHAR* filename )
{
	m_filename = filename;
}


HRESULT FontCollectionLoader::CreateEnumeratorFromKey( IDWriteFactory* factory, const void* collectionKey, UINT32 collectionKeySize, IDWriteFontFileEnumerator** fontFileEnumerator )
{
	IDWriteFontFile* file = nullptr;
	HRESULT res;
	res = factory->CreateFontFileReference( m_filename, nullptr, &file );
	if( res != S_OK )
		return res;

	m_enum.SetFile( file );

	if( fontFileEnumerator )
		*fontFileEnumerator = &m_enum;

	return S_OK;
}

HRESULT FontFileEnumerator::GetCurrentFontFile( IDWriteFontFile** fontFile )
{
	if( fontFile && m_idx == 0 && m_file )
	{
		*fontFile = m_file;
		return S_OK;
	}

	return E_FAIL;
}

HRESULT FontFileEnumerator::MoveNext( BOOL* hasCurrentFile )
{
	++m_idx;
	*hasCurrentFile = m_idx == 0;
	return S_OK;
}
