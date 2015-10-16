#pragma once

//
class FontFileEnumerator : public IDWriteFontFileEnumerator
{
public:
							FontFileEnumerator() : m_file( nullptr ), m_idx( -1 ) {}

	void					SetFile( IDWriteFontFile* file )							{	m_file = file;	}
	IDWriteFontFile*		GetFile()													{	return m_file;	}

	virtual HRESULT WINAPI	GetCurrentFontFile( IDWriteFontFile** fontFile );
	virtual HRESULT WINAPI	MoveNext( BOOL* hasCurrentFile );

	virtual HRESULT WINAPI	QueryInterface( const IID& riid, void** ppvObject )			{	return S_OK;	}
	virtual ULONG WINAPI	AddRef()													{	return 0;		}
	virtual ULONG WINAPI	Release()													{	return 0;	}

private:

	IDWriteFontFile*		m_file;
	int						m_idx;
};


//
class FontCollectionLoader : public IDWriteFontCollectionLoader
{
public:

							FontCollectionLoader( WCHAR* filename = L"arial.ttf" );
	void					Init( WCHAR* filename );
	IDWriteFontFile*		GetFile()													{	return m_enum.GetFile();		}

	virtual HRESULT WINAPI	CreateEnumeratorFromKey(
		IDWriteFactory* factory, 
		const void* collectionKey, 
		UINT32 collectionKeySize, 
		IDWriteFontFileEnumerator** fontFileEnumerator 
	);


	virtual HRESULT WINAPI	QueryInterface( const IID& riid, void** ppvObject )			{	return S_OK;	}
	virtual ULONG WINAPI	AddRef()													{	return 0;		}
	virtual ULONG WINAPI	Release()													{	return 0;		}

private:

	WCHAR*					m_filename;
	FontFileEnumerator		m_enum;
};

