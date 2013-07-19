#ifndef DEF_DATASTREAM
#define DEF_DATASTREAM
#define DEF_DATAMESSAGE
#define DEF_LOGRECORD
#endif

#include "stdlib.h"
#include "DataModel.h"

/*/
 * @program: 
 *     serialize the MSG_t to an series of bytes;
 *     the format of the buffer is [nUserId][nDestId][nSrcId][nMsgType][szData]
 * @param:
 *     tMsg: the object for serialize; buffer: the bytes whose MSG_t's memory.
 * @remark:
 *     the size of buffer must larger than or equal with sizeof(MSG_t)
 * @return:
 *     an MSG_t value.
 */
void Serialize(const MSG_t& tMsg, char* szBuffer)
{
	char *ref = NULL;
	int index = 0;

	// nUserId
	ref = &szBuffer[index];
	strncpy(ref, (char *)&tMsg.nUserId, sizeof(tMsg.nUserId));
	index += sizeof(tMsg.nUserId);
	// nDestId
	ref = &szBuffer[index];
	strncpy(ref, (char *)&tMsg.nDestId, sizeof(tMsg.nDestId));
	index += sizeof(tMsg.nDestId);
	// nSrcId
	ref = &szBuffer[index];
	strncpy(ref, (char *)&tMsg.nSrcId, sizeof(tMsg.nSrcId));
	index += sizeof(tMsg.nSrcId);
	// nMsgType
	ref = &szBuffer[index];
	strncpy(ref, (char *)&tMsg.nMsgType, sizeof(tMsg.nMsgType));
	index += sizeof(tMsg.nMsgType);
	// szData
	ref = &szBuffer[index];
	strncpy(ref, tMsg.szData, MSGDATASIZE);	
}

/*
 * @program: 
 *     deserialize the buffer and get a info of an MSG_t.
 *     the format of the buffer is [nUserId][nDestId][nSrcId][nMsgType][szData]
 * @param:
 *     szBuffer: the bytes whose MSG_t's memory.
 * @remark:
 *     the size of buffer must larger than or equal with sizeof(MSG_t)
 * @return:
 *     an MSG_t value.
 */
MSG_t DeSerialize(char* szBuffer)
{
	int index = 0;
	char *dataref = NULL;
	unsigned int *value = NULL;
	MSG_t msg;

	// nUserId
	value = (unsigned int *)&szBuffer[index];
	msg.nUserId = *value;
	index += sizeof(msg.nUserId);
	// nDestId
	value = (unsigned int *)&szBuffer[index];
	msg.nDestId = *value;
	index += sizeof(msg.nDestId);
	// nSrcId
	value = (unsigned int*)&szBuffer[index];
	msg.nSrcId = *value;
	index += sizeof(msg.nSrcId);
	// nMsgType
	value = (unsigned int *)&szBuffer[index];
	msg.nMsgType = *value;
	index += sizeof(msg.nMsgType);

	// szData
	dataref = &szBuffer[index];
	strncpy(msg.szData, dataref, 20);

	return msg;
}

void SetDefaultMSG(MSG_t& msg)
{
	memset(msg.szData, 0, MSGDATASIZE);
	msg.nDestId = 0;
	msg.nUserId = 0;		
	msg.nSrcId = 0;
	msg.nMsgType = 0;
}

void SetMSG(MSG_t &tDest, const MSG_t& tSrc)
{
	tDest.nDestId = tSrc.nDestId;
	tDest.nMsgType = tSrc.nMsgType;
	tDest.nSrcId = tSrc.nSrcId;
	tDest.nUserId = tSrc.nUserId;

	strcpy(tDest.szData, tSrc.szData);
}

void SetRecord(Record_t &tDest, const Record_t &tSrc)
{
	tDest.nType = tSrc.nType;
	tDest.nUdpPort = tSrc.nUdpPort;
	strcpy(tDest.szMsgText, tSrc.szMsgText);
	SetMSG(tDest.tMsgBody, tSrc.tMsgBody);
}

bool IsDefaultMSG(const MSG_t& msg)
{
	return (msg.nUserId == 0);
}