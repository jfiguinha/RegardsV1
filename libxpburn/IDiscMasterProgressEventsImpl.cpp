#include "stdafx.h"
#include ".\idiscmasterprogresseventsimpl.h"



IDiscMasterProgressEventsImpl::IDiscMasterProgressEventsImpl(void)
{
}

IDiscMasterProgressEventsImpl::~IDiscMasterProgressEventsImpl(void)
{
}


IDiscMasterProgressEventsImpl::IDiscMasterProgressEventsImpl(HWND hWnd)
{
	hDlg = hWnd;
	m_lRefCount = 1;
}
