#pragma once


struct Header
{
	USHORT format;
	USHORT count;
	USHORT stringOffset;

	void Reverse()
	{
		REVERSE2( format );
		REVERSE2( count );
		REVERSE2( stringOffset );
	}
};


struct NameRecord
{
	USHORT 	platformID;
	USHORT 	encodingID;
	USHORT 	languageID;
	USHORT 	nameID;
	USHORT 	length;
	USHORT 	offset;

	void Reverse()
	{
		REVERSE2( platformID );
		REVERSE2( encodingID );
		REVERSE2( languageID );
		REVERSE2( nameID );
		REVERSE2( length );
		REVERSE2( offset );
	}
};