// Pidl.cpp: implementation of the CPidl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Pidl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPidl::CPidl()
{
	SHGetMalloc(&g_pMalloc);
}

CPidl::~CPidl()
{
  g_pMalloc->Release();
  g_pMalloc = NULL;
}

/**************************************************************************

   Pidl_GetNextItem()

**************************************************************************/

LPITEMIDLIST CPidl::Pidl_GetNextItem(LPCITEMIDLIST pidl)
{
	if(pidl)
	   return (LPITEMIDLIST)(LPBYTE)(((LPBYTE)pidl) + pidl->mkid.cb);
	else
	   return NULL;
}

/**************************************************************************

   Pidl_Create()

**************************************************************************/

LPITEMIDLIST CPidl::Pidl_Create(UINT cbSize)
{
	LPITEMIDLIST pidl = NULL;

	pidl = (LPITEMIDLIST) g_pMalloc->Alloc(cbSize);
	if(pidl)
	   ZeroMemory(pidl, cbSize);

	return pidl;
}

///////////////////////////////////////////////////
///////////////////////////////////////////////////

BOOL CPidl::GetParentID(LPITEMIDLIST pidl)
{
    BOOL fRemoved = FALSE;

// Make sure it's a valid PIDL.
    if (pidl == NULL)
        return(FALSE);

    if (pidl->mkid.cb)
    {
		LPITEMIDLIST pidlNext = pidl;
        while (pidlNext)
        {
           pidl = pidlNext;
		   pidlNext = GetNextItemID(pidl);
        }
// Remove the last one, insert null-terminator.
        pidl->mkid.cb = 0; 
        fRemoved = TRUE;
    }

    return fRemoved;
}

// GetNextItemID - points to the next element in an item
// identifier list. 
// Returns a PIDL if successful, or NULL if at the end of the 
// list.
// pidl - previous element

LPITEMIDLIST CPidl::GetNextItemID(LPCITEMIDLIST pidl) 
   { 
   // Check for valid pidl.
   if(pidl == NULL)
      return NULL;

   // Get the size of the specified item identifier. 
   int cb = pidl->mkid.cb; 

   // If the size is zero, it is the end of the list. 
   if (cb == 0) 
      return NULL; 

   // Add cb to pidl (casting to increment by bytes). 
   pidl = (LPITEMIDLIST) (((LPBYTE) pidl) + cb); 

   // Return NULL if it is null-terminating, or a pidl otherwise. 
   return (pidl->mkid.cb == 0) ? NULL : (LPITEMIDLIST) pidl; 
   } 

/**************************************************************************

   Pidl_GetSize()

**************************************************************************/

UINT CPidl::Pidl_GetSize(LPCITEMIDLIST pidl)
{
	UINT           cbTotal = 0;
	LPITEMIDLIST   pidlTemp = (LPITEMIDLIST) pidl;

	if(pidlTemp)
	   {
	   while(pidlTemp->mkid.cb)
		  {
		  cbTotal += pidlTemp->mkid.cb;
		  pidlTemp = Pidl_GetNextItem(pidlTemp);
		  }  

	   // Requires a 16 bit zero value for the NULL terminator
	   cbTotal += 2 * sizeof(BYTE);
	   }

	return cbTotal;
}

/**************************************************************************

   Pidl_Copy()

**************************************************************************/

LPITEMIDLIST CPidl::Pidl_Copy(LPCITEMIDLIST pidlSource)
{
	LPITEMIDLIST   pidlTarget = NULL;
	UINT           cbSource = 0;

	if(NULL == pidlSource)
	   return NULL;

	//
	// Allocate the new ITEMIDLIST
	//
	cbSource = Pidl_GetSize(pidlSource);
	pidlTarget = (LPITEMIDLIST) g_pMalloc->Alloc(cbSource);
	if(!pidlTarget)
	   return NULL;

	//
	// Copy the source to the target
	//
	CopyMemory(pidlTarget, pidlSource, cbSource);

	return pidlTarget;
}

/**************************************************************************

   Pidl_Concatenate()

**************************************************************************/

LPITEMIDLIST CPidl::Pidl_Concatenate(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
{
	LPITEMIDLIST   pidlNew;
	UINT           cb1 = 0, 
				   cb2 = 0;

	//
	// Pidl1 can possibly be NULL if it points to the desktop.  Since we only
	// need a single NULL terminator, we remove the extra 2 bytes from the
	// size of the first ITEMIDLIST.
	//
	if(pidl1)
	   cb1 = Pidl_GetSize(pidl1) - (2 * sizeof(BYTE));

	cb2 = Pidl_GetSize(pidl2);

	//
	// Create a new ITEMIDLIST that is the size of both pidl1 and pidl2, then
	// copy pidl1 and pidl2 to the new list.
	//
	pidlNew = Pidl_Create(cb1 + cb2);
	if(pidlNew)
	   {
	   if(pidl1)   
		  CopyMemory(pidlNew, pidl1, cb1);

	   CopyMemory(((LPBYTE)pidlNew) + cb1, pidl2, cb2);
	   }

	return pidlNew;
}

/**************************************************************************

   Pidl_Free()

**************************************************************************/

void CPidl::Pidl_Free(LPITEMIDLIST pidl)
{
	if(pidl)
	{
		try
		{
			g_pMalloc->Free(pidl);
		}
		catch(...){}
	}
}