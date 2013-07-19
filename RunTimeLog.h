#ifndef RUNTIMELOG_H
#define RUNTIMELOG_H

#include "DataModel.h"

#define MAXRECORDSIZE 100

#ifdef DEF_RUNTIMELOG
class _declspec(dllexport) RunTimeLog
#else
class _declspec(dllimport) RunTimeLog
#endif
{
private:
	Record_t m_tRecords[MAXRECORDSIZE];
	int nCount;
	int nCursor;
	RunTimeLog *m_pNext;
public:
	RunTimeLog();
	~RunTimeLog();
	void Add(int nType, const MSG_t* tMsg, char *szMsgText);	
	void Add(int nType, int nUdpPort, const MSG_t* tMsg, char *szMsgText);	
	const Record_t& GetOne();
	int Save(char *filename);
public:	
	static void Format(const Record_t& tRecord, char *szBuffer);
	static bool CheckMSG(const MSG_t& tMsg);
	bool Next()
	{
		bool result = (nCursor + 1 < nCount);
		if(result)
		{
			nCursor = (nCursor + 1) % MAXRECORDSIZE;
		}
		return result;
	}
};

#endif