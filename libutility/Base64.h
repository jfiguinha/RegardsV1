
//*********************************************************************
//* C_Base64 - a simple base64 encoder and decoder.
//*
//*     Copyright (c) 1999, Bob Withers - bwit@pobox.com
//*
//* This code may be freely used for any purpose, either personal
//* or commercial, provided the authors copyright notice remains
//* intact.
//*********************************************************************

#ifndef Base64_H
#define Base64_H

class Base64
{
public:
	int Base64FileDecLowMemory(const char * m_chrFilenameSource, const char * m_chrFilenameCible);
    string encode(BYTE * data, const DWORD &size);
    string decode(string data);
	int Base64FileEnc(const char * m_chrFilenameSource, const char * m_chrFilenameCible);
	int Base64FileDec(const char * m_chrFilenameSource, const char * m_chrFilenameCible);

};

#endif
