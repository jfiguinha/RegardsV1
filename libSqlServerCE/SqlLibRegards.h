#pragma once
#include <SqlLib.h>


namespace Regards
{
	namespace Sqlite
	{
		class CSqlLibRegards : public CSqlLib
		{
		public:
			CSqlLibRegards(const bool &readOnly, const bool &load_inmemory);
			~CSqlLibRegards();

			bool InitDatabase(const string &lpFilename);
            bool CheckVersion(const string &lpFilename);
		private:
			
			bool CreateDatabase(const string &databasePath, const bool &load_inmemory);
		};
	}
}

