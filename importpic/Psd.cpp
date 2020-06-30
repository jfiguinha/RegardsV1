// Psd.cpp: implementation of the CPsd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Psd.h"
#include "Image.h"

// Matches OpenGL's right now.
#define IL_COLOUR_INDEX						0x1900
#define IL_COLOR_INDEX						0x1900
#define IL_RGB								0x1907
#define IL_RGBA								0x1908
#define IL_BGR								0x80E0
#define IL_BGRA								0x80E1
#define IL_LUMINANCE						0x1909
#define IL_LUMINANCE_ALPHA					0x190A


#define IL_BYTE								0x1400
#define IL_UNSIGNED_BYTE					0x1401
#define IL_SHORT							0x1402
#define IL_UNSIGNED_SHORT					0x1403
#define IL_INT								0x1404
#define IL_UNSIGNED_INT						0x1405
#define IL_FLOAT							0x1406
#define IL_DOUBLE							0x140A

inline int ByteToInt(BYTE * m_bValue)
{
	int m_iValue;
	m_iValue=m_bValue[0] << 24;
	m_iValue|=m_bValue[1] << 16;
	m_iValue|=m_bValue[2] << 8;
	m_iValue|=m_bValue[3];

	return m_iValue;
}

inline short ByteToShort(BYTE * m_bValue)
{
	short m_sValue;
	m_sValue=m_bValue[0] << 8;
	m_sValue|=m_bValue[1];
	return m_sValue;
}

// Returns the bpp of any Type
inline BYTE GetBppType(int Type)
{
	switch (Type)
	{
		case IL_BYTE:
			return 1;
		case IL_UNSIGNED_BYTE:
			return 1;
		case IL_SHORT:
			return 2;
		case IL_UNSIGNED_SHORT:
			return 2;
		case IL_INT:
			return 4;
		case IL_UNSIGNED_INT:
			return 4;
		case IL_FLOAT:
			return 4;
		case IL_DOUBLE:
			return 8;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPsd::CPsd()
{
	ZeroMemory(&Head,sizeof(PSDHEAD));
}

CPsd::~CPsd()
{

}

int CPsd::ReadPsd(CIBitmap * m_ciBitmap, const char *szFileName)
{
	IsValidPsd(szFileName);

	BITMAPINFO bmiInfos = {0};

	HBITMAP hBitmap;



	bmiInfos.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmiInfos.bmiHeader.biWidth = Head.Width;
	bmiInfos.bmiHeader.biHeight = Head.Height;
	bmiInfos.bmiHeader.biPlanes = 1;
	bmiInfos.bmiHeader.biBitCount = Head.Channels * 8;
	bmiInfos.bmiHeader.biCompression = BI_RGB;
	bmiInfos.bmiHeader.biSizeImage = ((Head.Channels * Head.Width + Head.Channels) & ~Head.Channels) * Head.Height;

	// Create a DIBSection. Note that this returns us two
	// things - an HBITMAP handle and a memory pointer, in
	// surface_bits.
		
	if(Head.Data != NULL)
	{
		hBitmap = CreateDIBitmap (GetDC(NULL),              
						   &bmiInfos.bmiHeader,
						   CBM_INIT,
						   Head.Data,
						   (LPBITMAPINFO)&bmiInfos,
						   DIB_RGB_COLORS) ;


		free(Head.Data);

		Head.Data = NULL;

		m_ciBitmap->SetBitmap(hBitmap,false);

		DeleteObject(hBitmap);

		// Error Check
		if(!m_ciBitmap->hBitmap)
			return -1;
	}
	else
		return -1;



	return 0;
}

void CPsd::GetDimensions(const char *fileName, UINT *width, UINT *height)
{
	bool	bPsd = FALSE;

	UINT m_iValue = 0;
	char drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];

	_splitpath_s(fileName, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

	_strlwr_s(ext,_MAX_EXT);

	if (strcmp(ext,".psd") != 0 &&
		strcmp(ext,".pdd") != 0)
	{
		return;
	}

	/*
	PsdFile = open(fileName,_O_RDONLY | _O_BINARY);
	if (PsdFile == NULL) 
	{
		return;
	}
	*/

	if (_sopen_s(&PsdFile,fileName,_O_RDONLY | _O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE)== -1) 
		return;

	if(PsdFile == -1)
		return;

	UINT FirstPos;

	FirstPos = _tell(PsdFile);

	if(!GetPsdHead(&Head))
		goto FIN;

	_lseek(PsdFile,-(int)sizeof(PSDHEAD), SEEK_CUR);

	bPsd = CheckPsd(&Head);

	*width = Head.Width;
	*height = Head.Height;

	_lseek(PsdFile,FirstPos, SEEK_SET);


FIN:

	_close(PsdFile);

}

bool CPsd::IsValidPsd(const char *szFileName)
{
	
	bool	bPsd = FALSE;

	UINT m_iValue = 0;
	char drive[_MAX_DRIVE], fname[_MAX_FNAME], ext[_MAX_EXT], dir[_MAX_DIR];

	//_splitpath(szFileName, drive, dir, fname, ext);
	_splitpath_s(szFileName, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

	_strlwr_s(ext,_MAX_EXT);

	if (strcmp(ext,".psd") != 0 &&
		strcmp(ext,".pdd") != 0)
	{
		return bPsd;
	}

	/*
	PsdFile = open(szFileName,_O_RDONLY | _O_BINARY);
	if (PsdFile == NULL) 
	{
		return bPsd;
	}
	*/
	if (_sopen_s(&PsdFile,szFileName,_O_RDONLY | _O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE)== -1) 
		return bPsd;

	if(PsdFile == -1)
		return -1;

	UINT FirstPos;

	FirstPos = _tell(PsdFile);

	if(!GetPsdHead(&Head))
		goto FIN;

	bPsd = CheckPsd(&Head);

	if (!ReadPsd(&Head))
		goto FIN;

	//iCurImage->Origin = IL_ORIGIN_UPPER_LEFT;

	//ilFixImage();


FIN:

	_close(PsdFile);

	return bPsd;
}

// Internal function used to get the Psd header from the current file.
bool CPsd::GetPsdHead(PSDHEAD *Header)
{
	PSDHEADREAD m_HeaderRead;

	if(_read(PsdFile, &Header->Signature, 4) == 0)
		return false;

	if(_read(PsdFile, &m_HeaderRead.Version, 2) == 0)
		return false;

	Header->Version= ByteToShort(m_HeaderRead.Version);

	if(_read(PsdFile, &Header->Reserved, 6) == 0)
		return false;

	if(_read(PsdFile, &m_HeaderRead.Channels, 2) == 0)
		return false;
	
	Header->Channels= ByteToShort(m_HeaderRead.Channels);

	if(_read(PsdFile, &m_HeaderRead.Height, 4) == 0)
		return false;

	Header->Height = ByteToInt(m_HeaderRead.Height);

	if(_read(PsdFile, &m_HeaderRead.Width, 4) == 0)
		return false;

	Header->Width = ByteToInt(m_HeaderRead.Width);

	if(_read(PsdFile, &m_HeaderRead.Depth, 2) == 0)
		return false;

	Header->Depth= ByteToShort(m_HeaderRead.Depth);

	if(_read(PsdFile, &m_HeaderRead.Mode, 2) == 0)
		return false;

	Header->Mode= ByteToShort(m_HeaderRead.Mode);

	return true;
}

bool CPsd::CheckPsd(PSDHEAD *Header)
{
// Internal function used to check if the HEADER is a valid Psd header.

	UINT i;

	if (strncmp(Header->Signature, "8BPS", 4))
		return false;
	
	if (Header->Version != 1)
		return false;
	for (i = 0; i < 6; i++) {
		if (Header->Reserved[i] != 0)
			return false;
	}
	if (Header->Channels < 1 || Header->Channels > 24)
		return false;
	if (Header->Height < 1 || Header->Width < 1)
		return false;
	if (Header->Depth != 1 && Header->Depth != 8 && Header->Depth != 16)
		return false;

	return true;

}

bool CPsd::LoadPsdInternal(void)
{
	return true;
}

bool CPsd::ReadPsd(PSDHEAD *Head)
{
	switch (Head->Mode)
	{
		case 1:  // Greyscale
			return ReadGrey(Head);
		case 2:  // Indexed
			return ReadIndexed(Head);
		case 3:  // RGB
			return ReadRGB(Head);
		case 4:  // CMYK
			return ReadCMYK(Head);
	}
	return true;
}

bool CPsd::ReadGrey(PSDHEAD *Head)
{
	return true;
}

bool CPsd::ReadIndexed(PSDHEAD *Head)
{
	return true;
}

bool CPsd::ReadRGB(PSDHEAD *Head)
{
	int		ColorMode, ResourceSize, MiscInfo;
	int		iRead;
	short	Compressed;
	int		Format, Type;
	BYTE	* Resources = NULL;
	BYTE	m_bColorMode[4];
	
	_read(PsdFile,&m_bColorMode,4);

	ColorMode = ByteToInt(m_bColorMode);

	_lseek(PsdFile,ColorMode, SEEK_CUR);

	_read(PsdFile,&m_bColorMode,4);  // Read the 'image resources section'

	ResourceSize = ByteToInt(m_bColorMode);

	Resources = (BYTE*)malloc(ResourceSize * sizeof(BYTE));

	if (Resources == NULL)
		return false;

	iRead = _read(PsdFile,Resources, ResourceSize);

	//iRead = ResourceSize - iRead;

	//if (read(PsdFile,Resources, ResourceSize) != ResourceSize)
	//	goto cleanup_error;

	//_lseek(PsdFile,iRead, SEEK_CUR);

	_read(PsdFile,&m_bColorMode,4);

	MiscInfo = ByteToInt(m_bColorMode);

	_lseek(PsdFile,MiscInfo, SEEK_CUR);

	_read(PsdFile,&m_bColorMode,2);  // Read the 'image resources section'

	Compressed = ByteToShort(m_bColorMode);

	ChannelNum = Head->Channels;
	switch (Head->Channels)
	{
		case 3:
			Format = IL_RGB;
			break;
		case 4:
			Format = IL_RGBA;
			break;
		default:
			return false;
	}
	switch (Head->Depth)
	{
		case 8:
			Type = IL_UNSIGNED_BYTE;
			break;
		case 16:
			Type = IL_UNSIGNED_SHORT;
			break;
		default:
			return false;
	}
	/*
	if (!ilTexImage(Head->Width, Head->Height, 1, (ILubyte)Head->Channels, Format, Type, NULL))
		goto cleanup_error;
		*/
	Head->Bpp = Head->Channels;

	Head->Bpc = GetBppType(Type);

	Head->Bps = Head->Width * Head->Channels * Head->Bpc;

	Head->Data = (BYTE*)malloc(Head->Width * Head->Bpc * Head->Channels * Head->Height * Head->Depth);

	
	if (!PsdGetData(Head, Head->Data, (bool)Compressed))
		goto cleanup_error;

	free(Resources);

	return true;

cleanup_error:
	free(Resources);
	return true;
}

bool CPsd::ReadCMYK(PSDHEAD *Head)
{
	return true;
}

UINT * CPsd::GetCompChanLen(PSDHEAD *Head)
{
	USHORT	*RleTable;
	UINT	*ChanLen, c, i, j;

	RleTable = (USHORT*)malloc(Head->Height * Head->Channels * sizeof(USHORT));
	
	ChanLen = (UINT*)malloc(Head->Channels * sizeof(UINT));

	if (RleTable == NULL || ChanLen == NULL) 
	{
		return NULL;
	}

	if (_read(PsdFile,RleTable, Head->Height * Head->Channels * sizeof(USHORT))== 0) 
	{
		free(RleTable);
		free(ChanLen);
		return NULL;
	}

	memset(ChanLen,0, Head->Channels * sizeof(UINT));
	for (c = 0; c < Head->Channels; c++) 
	{
		j = c * Head->Height;
		for (i = 0; i < Head->Height; i++) 
		{
			ChanLen[c] += RleTable[i + j];
		}
	}

	free(RleTable);

	return ChanLen;
}

bool CPsd::PsdGetData(PSDHEAD *Head, BYTE *Buffer, bool Compressed)
{

	int		c, x, y, i, Size;
	BYTE		*Channel = NULL;
	short	*ShortPtr;
	char 	HeadByte;
	BYTE Run;
	UINT		*ChanLen = NULL;
	bool	PreCache = false;

	Channel = (BYTE*)malloc(Head->Width * Head->Height * Head->Bpc);
	if (Channel == NULL) 
	{
		return false;
	}

	ShortPtr = (short*)Channel;

	UINT lWidthSize = ((((Head->Width * Head->Channels * 8) + 31) & ~31) >> 3);

	
	// @TODO: Add support for this in, though I have yet to run across a .psd
	//	file that uses this.
	if (Compressed && Head->Bpc == 2) 
	{
		return false;
	}

	if (!Compressed)
	{
		if (Head->Bpc == 1) 
		{
			for (c = 0; c < Head->Channels; c++) 
			{
				i = 0;
				if (_read(PsdFile,Channel, 1) == 0) 
				{
					free(Channel);
					return false;
				}
				for (y = 0; y < Head->Height * Head->Bps; y += Head->Bps) 
				{
					for (x = 0; x < Head->Bps; x += Head->Bpp, i++) 
					{
						Buffer[y + x + c] = Channel[i];
					}
				}
			}
		}
		else 
		{  // iCurImage->Bpc == 2
			for (c = 0; c < Head->Channels; c++) 
			{
				i = 0;
				if (_read(PsdFile,Channel, Head->Width * Head->Height * 2) == 0) 
				{
					free(Channel);
					return false;
				}
				
				Head->Bps /= 2;
				for (y = 0; y < Head->Height * Head->Bps; y += Head->Bps) 
				{
					for (x = 0; x < Head->Bps; x += Head->Bpp, i++) 
					{
						((short*)Buffer)[y + x + c] = ShortPtr[i];
					}
				}
				Head->Bps *= 2;
			}
		}
	}
	else 
	{

		ChanLen = GetCompChanLen(Head);

		Size = Head->Width * Head->Height;
		for (c = Head->Channels - 1; c >= 0; c--) 
		{
			//iPreCache(ChanLen[c]);

			for (i = 0; i < Size; ) 
			{
				_read(PsdFile, &HeadByte, 1);

				if (HeadByte >= 0)
				{  //  && HeadByte <= 127
					if (i + HeadByte > Size)
						goto file_corrupt;

					if (_read(PsdFile, Channel + i, HeadByte + 1) == 0)
						goto file_read_error;

					i += HeadByte + 1;
				}
				if (HeadByte >= -127 && HeadByte <= -1) 
				{
					_read(PsdFile, &Run, 1);
					if (Run == EOF)
						goto file_read_error;
					if (i + (-HeadByte + 1) > Size)
						goto file_corrupt;

					memset(Channel + i, Run, -HeadByte + 1);
					i += -HeadByte + 1;
				}
				if (HeadByte == -128)
				{ }  // Noop
			}


			i = 0;
			for (y = (Head->Height - 1); y >= 0; y--) 
			{
				for (x = 0; x < Head->Width; x++, i++) 
				{
					UINT pos = (y * lWidthSize) + (x * Head->Bpp);
					Head->Data[pos + c] = Channel[i];
				}
				if(y == 0)
					break;

			}

			if(c == 0)
				break;
		}

		free(ChanLen);
	}

	free(Channel);

	return true;

file_corrupt:
	free(ChanLen);
	free(Channel);
	return false;

file_read_error:
	free(ChanLen);
	free(Channel);
	return false;
}

bool CPsd::ParseResources(UINT ResourceSize, byte *Resources)
{
	return true;
}

bool CPsd::GetSingleChannel(PSDHEAD *Head, byte *Buffer, bool Compressed)
{
	return true;
}

bool CPsd::SavePsdInternal(void)
{
	return true;
}