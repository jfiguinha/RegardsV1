#pragma once
#include <sqlite3.h>

namespace Regards
{
	namespace Sqlite
	{
		class CSqlResult
		{
		public:
			CSqlResult();
			~CSqlResult();
			
			void SetStatement(sqlite3_stmt * pRes);
			/*Result Set Definations*/
			int	GetColumnCount();
			bool Next();

			string NextColumnName(const int &iClmnCount);
			string ColumnDataText(const int & clmNum);
			int ColumnDataInt(const int & clmNum);
			int ColumnDataBlob(const int & clmNum, void * & pzBlob, const int & pnBlob);
			int ColumnDataBlobSize(const int & clmNum);
			const void * ColumnDataBlob(const int & clmNum);
		private:
			sqlite3_stmt * pRes;
			int m_iColumnCount;
		};
	}
}

