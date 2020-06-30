#define DllImport  __declspec(dllimport) 

int DllImport GetDimensionsRaw(const char * fileName,UINT * extWidth,UINT * extHeight);

int DllImport WriteIBitmap(const char * szFileName,HWND hWnd, BYTE * * bValue);

int DllImport GetRawInfos(const char * szFileName, char * * cInfos);