#include "StdAfx.h"
#include "Avi.h"
#include <Aviriff.h>

DWORD CAvi::MAKE_FOURCC(char* fourCC)
{
	return (fourCC[0]|(fourCC[1]<<8)|(fourCC[2]<<16)|(fourCC[3]<<24));
}

bool CAvi::checkRiffHeader(FILE *f, FILE *o, RIFFHEADER *rh, bool write)
{
	if (!fread(rh,1,12,f)) return false;
	if (rh->riff != MAKE_FOURCC((char *)"RIFF")) return false;
	if (rh->fileType != MAKE_FOURCC((char*)"AVI ")) return false;
	if (write) { wpos += 12; if (!fwrite(rh,1,12,o)) return false; }
	return true;
}

bool CAvi::readLHChunk(FILE *f, LISTHEADER *lh, char *fourCC, DWORD size)
{
	if (lh->chkHd) delete lh->chkHd;
	lh->chkHd = new CHUNKHEADER();
	if (!lh->chkHd) return false;
	if (!fread(&lh->chkHd->ckID,4,1,f)) return false;
	if (lh->chkHd->ckID != MAKE_FOURCC(fourCC)) return false;
	if (!fread(&lh->chkHd->ckSize,4,1,f)) return false;
	if (lh->chkHd->ckSize != size) return false;
	if (lh->chkHd->ckData) free(lh->chkHd->ckData);
	lh->chkHd->ckDataSize = lh->chkHd->ckSize+(lh->chkHd->ckSize&1);
	lh->chkHd->ckData = malloc(lh->chkHd->ckDataSize);
	if (!lh->chkHd->ckData) return false;
	if (!fread(lh->chkHd->ckData,lh->chkHd->ckSize,1,f)) return false;
	return true;
}

bool CAvi::scanForList(FILE *f, FILE *o, LISTHEADER *lh, char *fourCC, char *fourCCStop,
				 bool force, bool write, bool &stop)
{
	lh->lstID = 0;
	stop = false;
repeat:
	if (!fread(&lh->lstID,1,2,f)) return false;
	while(1)
	{
		if (!fread(((unsigned char*)&lh->lstID)+2,1,2,f)) return false;
		if (lh->lstID == 0x5453494C) break;
		if (write) { wpos += 2; if (!fwrite(&lh->lstID,1,2,o)) return false; }
		lh->lstID = lh->lstID>>16;
	}
	if (!fread(&lh->lstSize,4,1,f)) return false;
	if (!fread(&lh->lstType,4,1,f)) return false;
	if (lh->lstType != MAKE_FOURCC(fourCC)) 
	{
		if (force) return false;
		if (lh->lstType == MAKE_FOURCC(fourCCStop))
		{
			if (fseek(f,-12,SEEK_CUR)) return false;
			stop = true;
			return false;
		}
		DWORD fsize = lh->lstSize-4-(lh->lstSize&1);
		if (fseek(f,fsize,SEEK_CUR)) 
			return false;
		goto repeat;
	}
	if (fseek(f,-12,SEEK_CUR)) return false;
	return true;
}

bool CAvi::readLH(FILE *f, LISTHEADER *lh)
{
	if (!fread(&lh->lstID,4,1,f)) return false;
	if (!fread(&lh->lstSize,4,1,f)) return false;
	if (!fread(&lh->lstType,4,1,f)) return false;
	return true;
}

CAvi::CAvi(void)
{
	wpos = 0;
}

CAvi::~CAvi(void)
{
}

bool CAvi::GetDimensionAvi(const char * szFilename, int &iWidth, int &iHeight)
{
	AVIMAINHEADER aviHeader;
	FILE * file_pointer = NULL;

	ZeroMemory(&aviHeader,sizeof(AVIMAINHEADER));

	errno_t err;
	err  = fopen_s(&file_pointer, szFilename, "r");
	if(err != 0)
	{
		return false;
	}

	/* Check for valid RIFF header. */
	RIFFHEADER rh;
	if (!checkRiffHeader(file_pointer, NULL, &rh, false))
	{
		return false;
	}

	/* Scan until first "LIST" fourcc.  The first one should be "hdrl" type. */
	LISTHEADER hdrl;
	bool stop;
	if (!scanForList(file_pointer, NULL, &hdrl, (char*)"hdrl", (char*)"FFFF", true, false, stop))
	{
		return false;
	}
	if (!readLH(file_pointer, &hdrl)) 
		return false;

	DWORD nhdrl_size = hdrl.lstSize;

	/* Next should be the "avih" chunk with the main AVI Header. */
	fread(&aviHeader,sizeof(AVIMAINHEADER),1,file_pointer);

	iWidth = aviHeader.dwWidth;
	iHeight = aviHeader.dwHeight;

	fclose(file_pointer);

	return true;
}