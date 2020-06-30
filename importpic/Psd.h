// Psd.h: interface for the CPsd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PSD_H__93C5C2F6_D636_420A_8FAF_4748C8CF94CD__INCLUDED_)
#define AFX_PSD_H__93C5C2F6_D636_420A_8FAF_4748C8CF94CD__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

class CIBitmap;

class CPsd  
{

typedef struct PSDHEAD
{
	char	Signature[4];
	short	Version;
	char	Reserved[6];
	short	Channels;
	UINT	Height;
	UINT	Width;
	short	Depth;
	short	Mode;
	BYTE	Bpp;
	BYTE	Bpc;
	UINT	Bps;
	UINT	SizeOfPlane;
	UINT	SizeOfData;
	BYTE *	Data;
}PSDHEAD;

typedef struct PSDHEADREAD
{
	BYTE	Signature[4];
	BYTE	Version[2];
	BYTE	Reserved[6];
	BYTE	Channels[2];
	BYTE	Height[4];
	BYTE	Width[4];
	BYTE	Depth[2];
	BYTE	Mode[2];
} PSDHEADREAD;

public:

	int ReadPsd(CIBitmap * m_ciBitmap,const char * szFileName);
	void GetDimensions(const char *fileName, UINT *width, UINT *height);


	CPsd();

	virtual ~CPsd();

private:
	
	short ChannelNum;
	int	PsdFile;
	PSDHEAD	Head;

	bool IsValidPsd(const char *szFileName);
	bool GetPsdHead(PSDHEAD *Header);
	bool CheckPsd(PSDHEAD *Header);
	bool LoadPsdInternal(void);
	bool ReadPsd(PSDHEAD *Head);
	bool ReadGrey(PSDHEAD *Head);
	bool ReadIndexed(PSDHEAD *Head);
	bool ReadRGB(PSDHEAD *Head);
	bool ReadCMYK(PSDHEAD *Head);
	UINT * GetCompChanLen(PSDHEAD *Head);
	bool PsdGetData(PSDHEAD *Head, BYTE *Buffer, bool Compressed);
	bool ParseResources(UINT ResourceSize, byte *Resources);
	bool GetSingleChannel(PSDHEAD *Head, byte *Buffer, bool Compressed);
	bool SavePsdInternal(void);

};

#endif // !defined(AFX_PSD_H__93C5C2F6_D636_420A_8FAF_4748C8CF94CD__INCLUDED_)
