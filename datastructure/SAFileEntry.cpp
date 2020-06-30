#include "stdafx.h"
#include ".\safileentry.h"

CSAFileEntry::CSAFileEntry(void)
{
}

CSAFileEntry::~CSAFileEntry(void)
{
}

void CSAFileEntry::MakeFiletime(unsigned __int64 inTime, _FILETIME &outTime)
{
	memcpy(&outTime, &inTime, sizeof(_FILETIME));
}