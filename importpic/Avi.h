

#ifndef __CAVI__H__
#define __CAVI__H__

typedef struct
{
	FOURCC riff;
	DWORD fileSize;
	DWORD fileType;
} RIFFHEADER;

class CHUNKHEADER
{
public:
	FOURCC ckID;
	DWORD ckSize;
	DWORD ckDataSize;
	void *ckData;
	CHUNKHEADER() : ckID(0), ckSize(0), ckData(0), ckDataSize(0) {};
	~CHUNKHEADER() { if (ckData) free(ckData); }
};

class LISTHEADER
{
public:
	FOURCC lstID;
	DWORD lstSize;
	FOURCC lstType;
	CHUNKHEADER *chkHd;
	LISTHEADER() : lstID(0), lstSize(0), lstType(0), chkHd(0) {};
	~LISTHEADER() { if (chkHd) delete chkHd; }
	LISTHEADER& operator=(LISTHEADER& ob2)
	{
		lstID = ob2.lstID;
		lstSize = ob2.lstSize;
		lstType = ob2.lstType;
		if (chkHd) delete chkHd;
		chkHd = new CHUNKHEADER();
		chkHd->ckID = ob2.chkHd->ckID;
		chkHd->ckSize = ob2.chkHd->ckSize;
		chkHd->ckDataSize = ob2.chkHd->ckDataSize;
		if (chkHd->ckData) free(chkHd->ckData);
		chkHd->ckData = malloc(chkHd->ckDataSize);
		memcpy(chkHd->ckData,ob2.chkHd->ckData,chkHd->ckDataSize);
		return *this;
	}
};

class CAvi
{
public:
	CAvi(void);
	~CAvi(void);
	bool GetDimensionAvi(const char * szFilename, int &iWidth, int &iHeight);

private:

	DWORD MAKE_FOURCC(char* fourCC);
	bool checkRiffHeader(FILE *f, FILE *o, RIFFHEADER *rh, bool write);
	bool readLHChunk(FILE *f, LISTHEADER *lh, char *fourCC, DWORD size);
	bool scanForList(FILE *f, FILE *o, LISTHEADER *lh, char *fourCC, char *fourCCStop,
					 bool force, bool write, bool &stop);
	bool readLH(FILE *f, LISTHEADER *lh);


	DWORD wpos;
};



#endif