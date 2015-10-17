#pragma once

struct Header
{
	USHORT format;
	USHORT count;
	USHORT stringOffset;

	void FixByteOrder()
	{
		FIXBYTEORDER( format );
		FIXBYTEORDER( count );
		FIXBYTEORDER( stringOffset );
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

	void FixByteOrder()
	{
		FIXBYTEORDER( platformID );
		FIXBYTEORDER( encodingID );
		FIXBYTEORDER( languageID );
		FIXBYTEORDER( nameID );
		FIXBYTEORDER( length );
		FIXBYTEORDER( offset );
	}
};