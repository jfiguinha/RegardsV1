#pragma once
#include <string>
#include <vector>
using namespace std;

namespace Regards
{
	namespace Sqlite
	{
		class CSqlLib;

		class CSqlEngine
		{
		public:

			struct DataBase
			{
				CSqlLib * _singleton;
				std::string baseName;
			};

			CSqlEngine();
			virtual ~CSqlEngine();

			static CSqlLib * getInstance(const string &baseName);
			static void Initialize(const string &filename, const string &baseName, CSqlLib * sqlLib);
			static void kill(const string &baseName);

		private:
			
			static vector<DataBase> _listOfBase;
		};
	}
}

