#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <string>
#include <vector>
using namespace std;

#define MSGSIZE	sizeof(MSG_t)
#define MSGDATASIZE 20

// nMsgType defination
#define MSG_TIME_REQUEST 0		// time request
#define MSG_TIME_RESPONSE 1		// time response
#define MSG_BEAT_REQUEST 2		// beat request
#define MSG_BEAT_RESPONSE 3		// beat response

#define INVALID_ID -1

#ifdef DEF_DATAMESSAGE
typedef struct _declspec(dllexport) DataMessage
#else
typedef struct _declspec(dllimport) DataMessage
#endif
{
	unsigned int nUserId;
	unsigned int nDestId;
	unsigned int nSrcId;
	unsigned int nMsgType;
	char szData[MSGDATASIZE];
}MSG_t;

#ifdef DEF_DATASTREAM
class _declspec(dllexport) DataStream
#else
class _declspec(dllimport) DataStream
#endif
{
public:
	char *szBuffer;
	int nCount;
	int nSize;
public:
	DataStream()
	{
		szBuffer = new char[MSGSIZE];
		nSize = MSGSIZE;
		nCount = 0;
	}
	DataStream(int size)
	{
		szBuffer = new char[size];
		nCount = 0;
		nSize = 0;
	}
	DataStream(DataStream& stream)
	{
		this->nCount = stream.nCount;
		this->nSize = stream.nSize;
		strncpy(this->szBuffer, stream.szBuffer, nCount);		
	}
};

#define REC_ERR 0	// record error
#define REC_RUN 1   // record run action
#define REC_RCV	2	// record recieved action
#define REC_SND 3	// record send action

#ifdef DEF_LOGRECORD
typedef struct _declspec(dllexport) Record_t
#else
typedef struct _declspec(dllimport) Record_t
#endif
{
public:
	unsigned int nType;
	unsigned int nUdpPort;	
	char szMsgText[100];
	MSG_t tMsgBody;
}Record_t;

void Serialize(const MSG_t& tMsg, char* szBuffer);
MSG_t DeSerialize(char* szBuffer);
void SetDefaultMSG(MSG_t& msg);
bool IsDefaultMSG(const MSG_t& msg);
void SetMSG(MSG_t &tDest, const MSG_t &tSrc);
void SetRecord(Record_t &tDest, const Record_t &tSrc);

#endif