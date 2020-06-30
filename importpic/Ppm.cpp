// Ppm.cpp: implementation of the CPpm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Ppm.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPpm::CPpm()
{
}

CPpm::~CPpm()
{

}

void CPpm::GetDimensions(const char *szFileName, UINT * width, UINT *height)
{
	*width= *height = 0;
	int fichier_source = 0;
	//int fichier_source = _open(szFileName, _O_RDONLY);

	if (_sopen_s(&fichier_source,szFileName,_O_RDONLY, _SH_DENYNO, _S_IREAD | _S_IWRITE)== -1) 
		return;

	int length = 2048;

	char * m_cdata = new char[length + 1];
	long octet_lus = _read(fichier_source, m_cdata, length);
	_close(fichier_source);

	string data = m_cdata;

	if (data.size()<1 || data[0]!='P')
		return;


	string type;
	size_t channels = 0;

	if (data[1]=='6')	// 24-bit binary RGB PPM file
	{
		channels = 3;
		type = "P6";
	}
	else
		if (data[1]=='5')	// 8-bit binary greyscale PGM file 
		{
			channels = 1;
			type = "P5";
		}
		else
			return;

	//

	const string eol("\n");
	const string digits("0123456789");

	size_t i = 0;
	
	int depth = 0;

	for (;;)
	{
		// Find end-of-line

		i = data.find_first_of(eol,i);
		if (i==string::npos)
			break;
		else
			i++;

		// If this line is a comment, try next line

		if (data[i]=='#')
			continue;
		
		// Get width

		*width = atoi(data.c_str()+i);
		i = data.find_first_not_of(digits,i); if (i==string::npos) break;
		i = data.find_first_of(digits,i);     if (i==string::npos) break;

		// Get height

		*height = atoi(data.c_str()+i);
		i = data.find_first_not_of(digits,i); if (i==string::npos) break;
		i = data.find_first_of(digits,i);     if (i==string::npos) break;

		break;
	}


	free(m_cdata);

}


int CPpm::ReadPPM(CIBitmap * m_ciBitmap, const char *szFileName)
{
	HANDLE fileh;

    //get the text of the file into a string buffer
    if ( (fileh=CreateFile(szFileName ,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
                           FILE_FLAG_SEQUENTIAL_SCAN,NULL))==INVALID_HANDLE_VALUE ) 
	{
       return(3);
    }

	
    if ( GetFileType(fileh)!=FILE_TYPE_DISK )
	{
       return(4);
    }

    long length = GetFileSize( fileh,NULL );


	CloseHandle(fileh);
	int fichier_source = 0;

	//int fichier_source = _open(szFileName, _O_RDONLY | _O_BINARY);
	if (_sopen_s(&fichier_source,szFileName,_O_RDONLY | _O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE)== -1) 
		return -1;

	if(fichier_source == -1)
		return -1;


	char * data = new char[length + 1];
	long octet_lus = _read(fichier_source, data, length);
	_close(fichier_source);


	char * entete = new char[50];

	memcpy(entete,data,50);

	decodePPM(m_ciBitmap,entete,data);

	delete[] data;
	delete[] entete;

	return 0;
}

bool CPpm::decodePPM(CIBitmap * m_ciBitmap,const string &data,char * m_cData)
{
	if (data.size()<1 || data[0]!='P')
		return false;

	//

	string type;
	size_t channels = 0;

	if (data[1]=='6')	// 24-bit binary RGB PPM file
	{
		channels = 3;
		type = "P6";
	}
	else
		if (data[1]=='5')	// 8-bit binary greyscale PGM file 
		{
			channels = 1;
			type = "P5";
		}
		else
			return false;

	//

	const string eol("\n");
	const string digits("0123456789");

	int height,width,iPos = 0;
	size_t i = 0;
	
	int depth = 0;

	for (;;)
	{
		// Find end-of-line

		i = data.find_first_of(eol,i);
		if (i==string::npos)
			break;
		else
			i++;

		// If this line is a comment, try next line

		if (data[i]=='#')
			continue;
		
		// Get width

		width = atoi(data.c_str()+i);
		i = data.find_first_not_of(digits,i); if (i==string::npos) break;
		i = data.find_first_of(digits,i);     if (i==string::npos) break;

		// Get height

		height = atoi(data.c_str()+i);
		i = data.find_first_not_of(digits,i); if (i==string::npos) break;
		i = data.find_first_of(digits,i);     if (i==string::npos) break;

		// Get depth

		depth = atoi(data.c_str()+i);
		i = data.find(eol,i); 
		if (i!=string::npos) 
			i++;

		break;
	}

	// Check that PPM seems to be valid

	const uint32 imageSize = width*height*channels;
	const uint32 rowSize   = width*channels;

	iPos = i;

	//string imageData = data.substr(i);

	m_ciBitmap->CreateBitmap(width, height);

	if(channels == 3)
	{
		//Convert 24 bits to 32 bits

		BYTE * outbuf = m_ciBitmap->GetPtBitmap();

		//long lWidthSize = ((((width * 24) + 31) & ~31) >> 3);
		long k = 0;

		for(int y = 0;y < height;y++)
		{
			//int iPos = y * lWidthSize;
			for(int x = 0;x < width;x++)
			{
				//BGR to RGB
				outbuf[k] = m_cData[iPos+2];
				outbuf[k+1] = m_cData[iPos+1];
				outbuf[k+2] = m_cData[iPos];
				outbuf[k+3] = 0;
				k+=4;
				iPos += 3;
			}
		}
	
	}
	else
	{
		//Convert 8 bits to 32 bits
		BYTE * outbuf = m_ciBitmap->GetPtBitmap();

		long lWidthSize = ((((width * 24) + 31) & ~31) >> 3);
		long k = 0;


		for(int y = 0;y < height;y++)
		{
			//int iPos = y * lWidthSize;
			for(int x = 0;x < width;x++)
			{
				
				outbuf[k] = m_cData[iPos];
				outbuf[k+1] = m_cData[iPos];
				outbuf[k+2] = m_cData[iPos];
				outbuf[k+3] = 0;
				k+=4;
				iPos++;
			}
		}
	}

	m_ciBitmap->VertFlipBuf();
	m_ciBitmap->ReCreateHBitmap(false);


	return true;
}