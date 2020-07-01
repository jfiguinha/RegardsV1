#pragma once
#include <mutex>
#include "SqlResult.h"
using namespace std;

namespace Regards
{
	namespace Sqlite
	{
		class CSqlLib
		{
		public:
			CSqlLib();
			virtual ~CSqlLib();

			bool OpenConnection(const string &databasePath, const bool &readonly, const bool &load_inmemory);
			void CloseConnection();

			virtual bool InitDatabase(const string &lpFilename) = 0;
            virtual bool CheckVersion(const string &lpFilename)
            {
                return false;
            }
            int ExecuteSQLWithNoResult(const string &query);
			bool ExecuteSQLSelect(const string &query, CSqlResult * sqlResult);
			bool ExecuteSQLBlobInsert(const string &query, const int &numCol, const void * zBlob, const int &nBlob, CSqlResult * sqlResult);
			void BeginTransaction();
			void CommitTransection();
			void lock();
			void unlock();
			void Release();

			int64_t GetLastId();
			sqlite3 * GetDB()
			{
				return pCon;
			}

		protected:

			int LoadOrSaveDb(sqlite3 *pInMemory, const char *zFilename, int isSave);
			bool isConnected();
	
			mutex sync;
			string		 sqLiteDBPath;		   //Databse File Dir
			sqlite3		 *pCon;				   //SQLite Connection Object
			sqlite3_stmt * pRes;
			bool m_bConnected;
			bool readonly;
			bool load_inmemory;
			string  m_strLastError;    /*Last Error String*/
		};
	}
}

