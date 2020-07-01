#pragma once

namespace Regards
{
	namespace Sqlite
	{
		class CSqlResult;
		class CSqlLib;

		class CSqlExecuteRequest
		{
		public:
			CSqlExecuteRequest(const string & databaseName);
			~CSqlExecuteRequest();
			int ExecuteRequestWithNoResult(const string &requestSQL);
			int ExecuteRequest(const string &requestSQL);
			bool ExecuteInsertBlobData(const string &requestSQL, const int &numCol, const void * zBlob, const int &nBlob);
			int64_t GetLastId();

		protected:

			void BeginTransaction();
			void CommitTransection();

			bool useTransaction;
			CSqlLib * _sqlLibTransaction;
			string databaseName;
		private:

			virtual int TraitementResult(CSqlResult * sqlResult) = 0;
		};
	}
}

