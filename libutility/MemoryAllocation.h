// MemoryAllocation.h: interface for the CMemoryAllocation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMORYALLOCATION_H__30FBE431_BC74_42AE_B7BF_BC50B86748A7__INCLUDED_)
#define AFX_MEMORYALLOCATION_H__30FBE431_BC74_42AE_B7BF_BC50B86748A7__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000


class CMemoryAllocation  
{
public:

	virtual ~CMemoryAllocation();

	CMemoryAllocation(const long &m_lSize)
	{

		if(X != NULL)
			DesAllocationMemoire();

		iAllocationType = 0;
		X = malloc(m_lSize);

		if(X == NULL)
		{
			//Probléme d'allocation mémoire passage par un heap
			iAllocationType = 1;
			X = (void *) HeapAlloc (GetProcessHeap (),
					HEAP_ZERO_MEMORY, m_lSize) ;
		}
	}

	void DesAllocationMemoire()
	{
		if(X != NULL)
		{
			switch(iAllocationType)
			{
				case 0:
					free(X);
					break;

				case 1:
					HeapFree (GetProcessHeap(), 0, X) ;
					break;
			}

			X = NULL;
		}
	}

	void * GetDataPt()
	{
		return X;
	}

private:

	int iAllocationType;
	void * X;
};

#endif // !defined(AFX_MEMORYALLOCATION_H__30FBE431_BC74_42AE_B7BF_BC50B86748A7__INCLUDED_)
