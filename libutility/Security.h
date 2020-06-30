// Security.h: interface for the CSecurity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECURITY_H__056D6010_CBFC_4456_9C19_68DBF86DDD17__INCLUDED_)
#define AFX_SECURITY_H__056D6010_CBFC_4456_9C19_68DBF86DDD17__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

/*
#include <windows.h>
#include <string>
using std::string;  // comment if your compiler doesn't use namespaces
*/

class CSecurity  
{
public:
	CSecurity();
	virtual ~CSecurity();

	static int HashData(BYTE *pbBuffer,const DWORD &dwBufferLen,string &pHashBase64);

};

#endif // !defined(AFX_SECURITY_H__056D6010_CBFC_4456_9C19_68DBF86DDD17__INCLUDED_)
