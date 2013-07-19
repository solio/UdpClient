#ifndef DEF_RUNTIMELOG
#define DEF_RUNTIMELOG
#endif

#include "RunTimeLog.h"
#include <windows.h>
#include <stdlib.h>

RunTimeLog::RunTimeLog()
{
	this->m_pNext = NULL;
	this->nCount = 0;
	this->nCursor = -1;
}

RunTimeLog::~RunTimeLog()
{

}

void RunTimeLog::Add(int nType, const MSG_t* tMsg, char *szMsgText)
{
	Record_t record;
	record.nType = nType;

	if(tMsg != NULL)
	{
		record.tMsgBody = *tMsg;
	}
	else
	{
		 SetDefaultMSG(record.tMsgBody);		
	}

	strcpy(record.szMsgText, szMsgText);
	
	SetRecord(this->m_tRecords[nCount], record);
	nCount++;
}

void RunTimeLog::Add(int nType, int nUdpPort, 
	const MSG_t* tMsg, char *szMsgText)
{
	Record_t record;
	record.nType = nType;
	record.nUdpPort = nUdpPort;
	if(tMsg != NULL)
	{
		record.tMsgBody = *tMsg;
	}
	else
	{
		 SetDefaultMSG(record.tMsgBody);		
	}

	strcpy(record.szMsgText, szMsgText);
	
	SetRecord(this->m_tRecords[nCount], record);
	nCount++;
}

/*
 * @program:
 *     get the current record that nCursor indicates.
 * @remark:
 *     it is encourage that call Next method before call this method.
 */
const Record_t& RunTimeLog::GetOne()
{	
	return this->m_tRecords[nCursor];
}

/*
 * @program:
 *     save records into file.
 * @param:
 *     filename : specific the name of file.
 * @return:
 *     0 for success, -1 for file error
 */
int RunTimeLog::Save(char *filename)
{
	FILE *fp = NULL;

	unlink(filename);
	fp = fopen(filename, "w");
	if(fp == NULL)
	{
		return -1;
	}

	char logText[100] = "";
	for(int i = 0; i < nCount; nCount++)
	{
		Record_t &record = m_tRecords[i];		
		
		Format(record, logText);
		fwrite(logText, strlen(logText) + 1, 1, fp);
	}
	fclose(fp);
}

/*
 * @program:
 *     make the Record_t object be a formative string.
 * @param:
 *     tRecord[in]: the object to be formated.
 *     szBuffer[out]: the result string.
 */
void RunTimeLog::Format(const Record_t& tRecord, char *szBuffer)
{
	MSG_t msgBody = tRecord.tMsgBody;
	sprintf(szBuffer, "Recordtype:%d src_id:%d  dest_id:%d usr_id:%d msgtype:%d time:%s note:%s\r\n",
		tRecord.nType, msgBody.nSrcId, msgBody.nDestId, 
		msgBody.nUserId, msgBody.nMsgType, msgBody.szData, tRecord.szMsgText);
}

/*
 * @program:
 *     check the tMsg is legal MSG_t,cause the MSG_t object is serialized by 
 *     a char buffer on a udp socket .this method guarantee the application
 *     don't be attacked by other application
 */
bool RunTimeLog::CheckMSG(const MSG_t& tMsg)
{
	//check msg type
	if(tMsg.nMsgType == MSG_BEAT_REQUEST ||
		tMsg.nMsgType == MSG_BEAT_RESPONSE || 
		tMsg.nMsgType == MSG_TIME_REQUEST)
	{
		return true;
	}

	//if it is a time response
	//check data format example: "2013-07-15 14:55:13"
	int format[20] = {	
	// 0 for digit, 1 for '-', 2 for ' ', 3 for ':', 4 for '\0'
		0, 0, 0, 0, 1, 
		0, 0, 1, 0, 0, 
		2, 0, 0, 3, 0, 
		0, 3, 0, 0, 4
	};
	for(int i = 0; i < MSGDATASIZE; i++)
	{
		switch(format[i])
		{
		case 0 :
			if(!isdigit(tMsg.szData[i]))
				return false;
			break;
		case 1:
			if(tMsg.szData[i] != '-')
				return false;
			break;
		case 2:
			if(tMsg.szData[i] != ' ')
				return false;
			break;
		case 3:
			if(tMsg.szData[i] != ':')
				return false;
			break;
		case 4:
			if(tMsg.szData[i] != '\0')
				return false;
			break;
		}
	}

	return true;
}