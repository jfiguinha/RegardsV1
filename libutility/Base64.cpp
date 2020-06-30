
//*********************************************************************
//* Base64 - a simple base64 encoder and decoder.
//*
//*     Copyright (c) 1999, Bob Withers - bwit@pobox.com
//*
//* This code may be freely used for any purpose, either personal
//* or commercial, provided the authors copyright notice remains
//* intact.
//*********************************************************************

#include "stdafx.h"
#include "Base64.h"


const char          fillchar = '=';

                        // 00000000001111111111222222
                        // 01234567890123456789012345
static string       cvt = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

                        // 22223333333333444444444455
                        // 67890123456789012345678901
                          "abcdefghijklmnopqrstuvwxyz"

                        // 555555556666
                        // 234567890123
                          "0123456789+/";


#define SKIP '\177'
#define BAD  '\277'
#define END  '\100'
#define DW_B64_STATE_0              0
#define DW_B64_STATE_1              1
#define DW_B64_STATE_2              2
#define DW_B64_STATE_3              3
#define DW_B64_STATE_OUTPUT_CRLF    4
#define DW_B64_STATE_OUTPUT_LF      5
#define DW_B64_STATE_OUTPUT_EQ_EQ   6
#define DW_B64_STATE_OUTPUT_EQ      7
#define DW_B64_STATE_DONE           8

const char base64table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

const char base64Map[] = {
  /*  00     01     02     03     04     05     06     07 */
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
  /*  08     09     0A     0B     0C     0D     0E     0F */
     BAD,  SKIP,  SKIP,   BAD,  SKIP,  SKIP,   BAD,   BAD,
  /*  10     11     12     13     14     15     16     17 */
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
  /*  18     19     1A     1B     1C     1D     1E     1F */
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
  /*  SP     !      "      #      $      %      &      '  */
    SKIP,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
  /*  (      )      *      +      ,      -      .      /  */
     BAD,   BAD,   BAD,    62,   BAD,   BAD,   BAD,    63,
  /*  0      1      2      3      4      5      6      7 */
      52,    53,    54,    55,    56,    57,    58,    59,
  /*  8      9      :      ;      <      =      >      ?  */
      60,    61,   BAD,   BAD,   BAD,   END,   BAD,   BAD,
  /*  @      A      B      C      D      E      F      G  */
     BAD,    0 ,    1 ,    2 ,    3 ,    4 ,    5 ,    6 ,
  /*  H      I      J      K      L      M      N      O  */
      7 ,    8 ,    9 ,    10,    11,    12,    13,    14,
  /*  P      Q      R      S      T      U      V      W  */
      15,    16,    17,    18,    19,    20,    21,    22,
  /*  X      Y      Z      [      \      ]      ^      _  */
      23,    24,    25,   BAD,   BAD,   BAD,   BAD,   BAD,
  /*  `      a      b      c      d      e      f      g  */
     BAD,    26,    27,    28,    29,    30,    31,    32,
  /*  h      i      j      k      l      m      n      o  */
      33,    34,    35,    36,    37,    38,    39,    40,
  /*  p      q      r      s      t      u      v      w  */
      41,    42,    43,    44,    45,    46,    47,    48,
  /*  x      y      z      {      |      }      ~     DEL */
      49,    50,    51,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,
     BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD
};

int DecodeValue(const int c)
{
   if ( (c >= 'A') && (c <= 'Z') ) 
	   return (int)(c - 'A');
   else if ( (c >= 'a') && (c <= 'z') )
        return 26 + (int)(c - 'a');
   else if ( (c >= '0') && (c <= '9' ) )
        return 52 + (int)(c - '0');
   else if ( c == '+' ) 
	   return 62;
   else if ( c == '/' ) 
	   return 63;
   else if ( c == '=' ) 
	   return -2;
   else    
	   return -1;
}

//*****************************************************************
//Fonction : string encode(string data)
//Paramètre d'entrée :  
//string : Chaine à encoder
//Paramètre de Sortie :String : Chaine Coder
//*****************************************************************

string Base64::encode(BYTE * data, const DWORD &size)
{
    string::size_type  i;
    char               c;
    string::size_type  len = size;
    string             ret;

    for (i = 0; i < len; ++i)
    {
		//if(data[i] == '\0')
		//	break;

        c = (data[i] >> 2) & 0x3f;
        ret.append(1, cvt[c]);
        c = (data[i] << 4) & 0x3f;
        if (++i < len)
            c |= (data[i] >> 4) & 0x0f;

        ret.append(1, cvt[c]);
        if (i < len)
        {
            c = (data[i] << 2) & 0x3f;
            if (++i < len)
                c |= (data[i] >> 6) & 0x03;

            ret.append(1, cvt[c]);
        }
        else
        {
            ++i;
            ret.append(1, fillchar);
        }

        if (i < len)
        {
            c = data[i] & 0x3f;
            ret.append(1, cvt[c]);
        }
        else
        {
            ret.append(1, fillchar);
        }
    }

    return(ret);
}

//*****************************************************************
//Fonction : string decode(string data)
//Paramètre d'entrée :  
//string : Chaine Codée
//Paramètre de Sortie : string : Chaine décodée
//*****************************************************************

string Base64::decode(string data)
{
    string::size_type  i;
    char               c;
    char               c1;
    string::size_type  len = data.length();
    string             ret;

    for (i = 0; i < len; ++i)
    {
        c = (char) cvt.find(data[i]);
        ++i;
        c1 = (char) cvt.find(data[i]);
        c = (c << 2) | ((c1 >> 4) & 0x3);
        ret.append(1, c);
        if (++i < len)
        {
            c = data[i];
            if (fillchar == c)
                break;

            c = (char) cvt.find(c);
            c1 = ((c1 << 4) & 0xf0) | ((c >> 2) & 0xf);
            ret.append(1, c1);
        }

        if (++i < len)
        {
            c1 = data[i];
            if (fillchar == c1)
                break;

            c1 = (char) cvt.find(c1);
            c = ((c << 6) & 0xc0) | c1;
            ret.append(1, c);
        }
    }

    return(ret);
}

//***********************************************************************************************
//Fonction : int Base64FileEnc(const char * m_chrFilenameSource, const char * m_chrFilenameCible)
//Paramètre d'entrée :  
//const char * : Fichier Source à encoder
//const char * : Fichier cible coder en base64
//Paramètre de Sortie :int :
//Valeur de sortie :
// 0 : Aucun Problème
// 1 : Erreur
//*************************************************************************************************

#define B64_ENC(Ch) (char) (base64table[(char)(Ch) & 0x3f])

int Base64::Base64FileEnc(const char * m_chrFilenameSource, const char * m_chrFilenameCible)
{

	int fichier_source, fichier_cible;
	int filesize;
	HANDLE fileh;

	char base64table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	int length;
	int i = 0;
	char data[3];
	char b[6];
	int octet_lus;
	
	//

	//fichier_cible = _open(m_chrFilenameCible,_O_WRONLY | _O_CREAT);
	if(_sopen_s(&fichier_cible,m_chrFilenameCible,_O_WRONLY | _O_CREAT, _SH_DENYNO, _S_IREAD | _S_IWRITE) == -1)
		return -1;

	if(fichier_cible == -1)
		return -1;

    //get the text of the file into a string buffer
    if ( (fileh=CreateFile(m_chrFilenameSource ,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
                           FILE_FLAG_SEQUENTIAL_SCAN,NULL))==INVALID_HANDLE_VALUE ) 
	{
       return(3);
    }

	
    if ( GetFileType(fileh)!=FILE_TYPE_DISK )
	{
       return(4);
    }

    length = GetFileSize( fileh,NULL );

	filesize = length;

	CloseHandle(fileh);

	//fichier_source = _open(m_chrFilenameSource, _O_RDONLY | _O_BINARY);
	if(_sopen_s(&fichier_source,m_chrFilenameSource,_O_RDONLY | _O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE) == -1)
		return -1;

	if(fichier_source == -1)
		return -1;
	octet_lus = _read(fichier_source, data, sizeof(data));

	for ( ; filesize > 2; filesize -= 3, octet_lus = _read(fichier_source, data, sizeof(data)))
	{
	  
		i = 0;
		b[i++] = B64_ENC(data[0] >> 2);
		b[i++] = B64_ENC(((data[0] << 4) & 060) | ((data[1] >> 4) & 017));
		b[i++] = B64_ENC(((data[1] << 2) & 074) | ((data[2] >> 6) & 03));
		b[i++] = B64_ENC(data[2] & 077);
		b[i++] = '\0';
		_write(fichier_cible, b,  strlen(b));


	}

	if ( filesize == 1 ) 
	{
	  
	  i = 0;
	  b[i++] = B64_ENC(data[0] >> 2);
	  b[i++] = B64_ENC((data[0] << 4) & 060);
	  b[i++] = '=';
	  b[i++] = '=';
	  b[i++] = '\0';
	  _write(fichier_cible, b,  strlen(b));

	} 
	else if ( filesize == 2 ) 
	{
	  i = 0;
	  b[i++] = B64_ENC(data[0] >> 2);
	  b[i++] = B64_ENC(((data[0] << 4) & 060) | ((data[1] >> 4) & 017));
	  b[i++] = B64_ENC((data[1] << 2) & 074);
	  b[i++] = '=';
	  b[i++] = '\0';

	  _write(fichier_cible, b,  strlen(b));
	  
	}

	b[0] = 0;
	b[1] = '\0';

	_write(fichier_cible, b,  strlen(b));

	_close(fichier_cible);
	_close(fichier_source);

	return 0;
}

//***********************************************************************************************
//Fonction : int Base64FileDec(const char * m_chrFilenameSource, const char * m_chrFilenameCible)
//Paramètre d'entrée :  
//const char * : Fichier Source à décoder
//const char * : Fichier cible 
//Paramètre de Sortie :int :
//Valeur de sortie :
// 0 : Aucun Problème
// 1 : Erreur
//*************************************************************************************************

int Base64::Base64FileDec(const char * m_chrFilenameSource, const char * m_chrFilenameCible)
{
	int fichier_source, fichier_cible;
	int filesize;
	HANDLE fileh;
	string ret;
	int length;
	int i = 0;
	int octet_lus;
	string m_stgValue;
	
	//

	//fichier_cible = _open(m_chrFilenameCible,_O_WRONLY | _O_CREAT | _O_BINARY);
	if(_sopen_s(&fichier_cible,m_chrFilenameCible,_O_WRONLY | _O_CREAT | _O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE) == -1)
		return -1;
	if(fichier_cible == -1)
		return -1;


    //get the text of the file into a string buffer
    if ( (fileh=CreateFile(m_chrFilenameSource ,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
                           FILE_FLAG_SEQUENTIAL_SCAN,NULL))==INVALID_HANDLE_VALUE ) 
	{
       return(3);
    }

	
    if ( GetFileType(fileh)!=FILE_TYPE_DISK )
	{
       return(4);
    }

    length = GetFileSize( fileh,NULL );

	filesize = length;

	CloseHandle(fileh);

	//fichier_source = _open(m_chrFilenameSource, _O_RDONLY);
	if(_sopen_s(&fichier_source,m_chrFilenameSource,_O_RDONLY, _SH_DENYNO, _S_IREAD | _S_IWRITE) == -1)
		return -1;

	if(fichier_source == -1)
		return -1;
	char * data = new char[filesize + 1];
	octet_lus = _read(fichier_source, data, filesize);
	_close(fichier_source);

    int ch, by, n, j=0;
	int mState = 0, mLastByte= 0, mError = 0;
	char out[1];

	while (data[j] != '\0') 
	{
		ch = data[j];
		j++;
        n = base64Map[ch];
        if (! (n & 0xc0)) 
		{
            switch (mState)
			{
            case DW_B64_STATE_0:
                mState = DW_B64_STATE_1;
                break;
            case DW_B64_STATE_1:
                mState = DW_B64_STATE_2;
                by = mLastByte << 2;
                by += n >> 4;
				out[0] = by;
				_write(fichier_cible, out,  1);
                break;
            case DW_B64_STATE_2:
                mState = DW_B64_STATE_3;
                by = mLastByte << 4;
                by += n >> 2;
				out[0] = by;
				_write(fichier_cible, out,  1);
                break;
            case DW_B64_STATE_3:
                mState = DW_B64_STATE_0;
                by = mLastByte << 6;
                by += n;
				out[0] = by;
				_write(fichier_cible, out,  1);
                break;
            }
            mLastByte = n;
        }
        else if (n == SKIP)
		{
        }
        else if (n == END) 
		{
            mState = DW_B64_STATE_DONE;
			break;
        }
        else if (n == BAD)
		{
            mError = 1;
        }
    }

	_close(fichier_cible);


	return 0;
}



int Base64::Base64FileDecLowMemory(const char * m_chrFilenameSource, const char * m_chrFilenameCible)
{
	int fichier_source, fichier_cible;
	int filesize;
	HANDLE fileh;
	string ret;
	int length;
	int i = 0;
	int octet_lus;
	string m_stgValue;
	
	//

	//fichier_cible = _open(m_chrFilenameCible,_O_WRONLY | _O_CREAT | _O_BINARY);
	if(_sopen_s(&fichier_cible,m_chrFilenameCible,_O_WRONLY | _O_CREAT | _O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE) == -1)
		return -1;
	if(fichier_cible == -1)
		return -1;
    //get the text of the file into a string buffer
    if ( (fileh=CreateFile(m_chrFilenameSource ,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
                           FILE_FLAG_SEQUENTIAL_SCAN,NULL))==INVALID_HANDLE_VALUE ) 
	{
       return(3);
    }

	
    if ( GetFileType(fileh)!=FILE_TYPE_DISK )
	{
       return(4);
    }

    length = GetFileSize( fileh,NULL );

	filesize = length;

	CloseHandle(fileh);

	//fichier_source = _open(m_chrFilenameSource, _O_RDONLY);
	if(_sopen_s(&fichier_source,m_chrFilenameSource,_O_RDONLY, _SH_DENYNO, _S_IREAD | _S_IWRITE) == -1)
		return -1;
	if(fichier_source == -1)
		return -1;

	char data[1];

    int ch, by, n, j=0;
	int mState = 0, mLastByte= 0, mError = 0;
	char out[1];

	
	while ((octet_lus = _read(fichier_source, data, 1)) > 0) 
	{
		ch = data[0];
        n = base64Map[ch];
        if (! (n & 0xc0)) 
		{
            switch (mState)
			{
            case DW_B64_STATE_0:
                mState = DW_B64_STATE_1;
                break;
            case DW_B64_STATE_1:
                mState = DW_B64_STATE_2;
                by = mLastByte << 2;
                by += n >> 4;
				out[0] = by;
				_write(fichier_cible, out,  1);
                break;
            case DW_B64_STATE_2:
                mState = DW_B64_STATE_3;
                by = mLastByte << 4;
                by += n >> 2;
				out[0] = by;
				_write(fichier_cible, out,  1);
                break;
            case DW_B64_STATE_3:
                mState = DW_B64_STATE_0;
                by = mLastByte << 6;
                by += n;
				out[0] = by;
				_write(fichier_cible, out,  1);
                break;
            }
            mLastByte = n;
        }
        else if (n == SKIP)
		{
        }
        else if (n == END) 
		{
            mState = DW_B64_STATE_DONE;
			break;
        }
        else if (n == BAD)
		{
            mError = 1;
        }
    }

	_close(fichier_cible);
	_close(fichier_source);

	return 0;
}

