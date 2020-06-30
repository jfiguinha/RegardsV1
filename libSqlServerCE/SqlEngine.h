

#ifndef __CSQLENGINE__
#define __CSQLENGINE__

namespace LIBSQLSERVERCE
{
	class CSqlLib;

	class CSqlEngine
	{
	public:
		CSqlEngine(void);
		~CSqlEngine(void);

		static CSqlLib *getInstance();
		static void Initialize(LPCWSTR lpFilename);
		static void kill();
		static LPSTR UnicodeToAnsi(LPCWSTR s);
		static HRESULT AnsiToUnicode(LPCSTR pszA, LPOLESTR* ppszW);

	private:

		static CSqlLib *_singleton;

	};
}

#endif