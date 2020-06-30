// Redirector.h: interface for the CRedirector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REDIRECTOR_H__D416FF9B_7CDA_4D8E_8D61_190E5001FCA1__INCLUDED_)
#define AFX_REDIRECTOR_H__D416FF9B_7CDA_4D8E_8D61_190E5001FCA1__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

class CRedirector  
{
public:
	
	CRedirector();
	virtual ~CRedirector();

	BOOL Open(LPCTSTR pszCmdLine);
	virtual void Close();
	BOOL Printf(LPCTSTR pszFormat, ...);

	void SetWaitTime(DWORD dwWaitTime) 
	{ 
		m_dwWaitTime = dwWaitTime; 
	}

protected:
	// overrides:
	virtual void WriteStdOut(LPCSTR pszOutput);
	virtual void WriteStdError(LPCSTR pszError);
	virtual void SignalApplicationEnding();

	BOOL LaunchChild(LPCTSTR pszCmdLine,HANDLE hStdOut, HANDLE hStdIn, HANDLE hStdErr);
	int RedirectStdout();
	void DestroyHandle(HANDLE& rhObject);


	string CarriageReturnUnixToWindows(LPCSTR pszOutput)
	{
		string m_stgNewText;
		int iLastPosition = 0;
		int iLast8 = 0;
		int i = 0;
		for(;i < strlen(pszOutput);i++)
		{
			if(pszOutput[i] == 8)
			{
				iLast8 = 1;
				iLastPosition = i+1;
				continue;
			}

			if(iLast8)
			{
				int iNbCopy = i - iLastPosition;
				char * NewChar = new char[iNbCopy + 3];
				memcpy(NewChar,pszOutput+iLastPosition,iNbCopy);
				NewChar[iNbCopy] = '\r';
				NewChar[iNbCopy+1] = '\n';
				NewChar[iNbCopy+2] = '\0';
				m_stgNewText += NewChar;
				delete[] NewChar;
				iLastPosition = i+1;
				iLast8 = 0;
			}

			if(pszOutput[i] == 13)
			{
				int iNbCopy = i - iLastPosition;
				char * NewChar = new char[iNbCopy + 3];
				memcpy(NewChar,pszOutput+iLastPosition,iNbCopy);
				NewChar[iNbCopy] = '\r';
				NewChar[iNbCopy+1] = '\n';
				NewChar[iNbCopy+2] = '\0';
				m_stgNewText += NewChar;
				delete[] NewChar;
				iLastPosition = i+1;
				continue;
			}

			if(pszOutput[i] == '\n')
			{
				//Vérification si présence de \r
				if(pszOutput[i-1] != '\r')
				{
					int iNbCopy = i - iLastPosition;
					char * NewChar = new char[iNbCopy + 3];
					memcpy(NewChar,pszOutput+iLastPosition,iNbCopy);
					NewChar[iNbCopy] = '\r';
					NewChar[iNbCopy+1] = '\n';
					NewChar[iNbCopy+2] = '\0';
					m_stgNewText += NewChar;
					delete[] NewChar;
					iLastPosition = i+1;
				}
			}
		}

		int iNbCopy = i - iLastPosition;
		if(iNbCopy > 0)
		{
			char * NewChar = new char[iNbCopy + 3];
			memcpy(NewChar,pszOutput+iLastPosition,iNbCopy);
			NewChar[iNbCopy] = '\0';
			m_stgNewText += NewChar;
			delete[] NewChar;
		}
		return m_stgNewText;
	}

	static DWORD WINAPI OutputThread(LPVOID lpvThreadParam);

	HANDLE m_hStdinWrite;	// write end of child's stdin pipe
	HANDLE m_hStdoutRead;	// read end of child's stdout pipe
	HANDLE m_hChildProcess;

private:

	HANDLE m_hThread;		// thread to receive the output of the child process
	HANDLE m_hEvtStop;		// event to notify the redir thread to exit
	DWORD m_dwThreadId;		// id of the redir thread
	DWORD m_dwWaitTime;		// wait time to check the status of the child process

};

#endif // !defined(AFX_REDIRECTOR_H__D416FF9B_7CDA_4D8E_8D61_190E5001FCA1__INCLUDED_)
