#ifndef UDPCLIENT_H
#define UDPCLIENT_H


#include "DataModel.h"
#include "RunTimeLog.h"
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX_LISTEN 100

#ifdef DEF_UDPCLIENT
class _declspec(dllexport) UdpClient
#else
class _declspec(dllimport) UdpClient
#endif
{
private:
	SOCKET m_tWorkSocket;
	SOCKADDR_IN m_tLocalAddress;
	DataStream m_tBuffer;
	RunTimeLog m_tRecorder;
	HANDLE m_hCLoseMutex;
	
	char m_szLogText[100];
	bool m_bIsValid;
	bool m_bWork;
protected:
	static WSADATA *WSALoader;
public:
	void (*RecievedHandler)(const MSG_t& msg);
public: 
	UdpClient(int port);
	~UdpClient();			
	void StartWork();	
	int SendTo(char *ipaddress, int port, const MSG_t& msg);

	HANDLE& GetCloseMutex()
	{
		return this->m_hCLoseMutex;
	}
	char *GetLogText()
	{
		return this->m_szLogText;
	}
	RunTimeLog& GetRecorder()
	{
		return this->m_tRecorder;
	}
	const SOCKET GetSocket()
	{
		return this->m_tWorkSocket;
	}	
	const bool IsWork()
	{
		return (this->m_bWork || this->m_tWorkSocket == INVALID_SOCKET);
	}
	const bool IsValid()
	{
		return this->m_bIsValid;
	}
	const bool HasHandler()
	{
		return (this->RecievedHandler == NULL);
	}
private:		
	bool CheckWSALoaded()
	{
		return (WSALoader != NULL);
	}
	void WSALoad()
	{
		if(!CheckWSALoaded())
		{
			WSALoader = new WSADATA;
			if(WSAStartup(MAKEWORD(2, 2), WSALoader))
			{
				sprintf(m_szLogText, "Windows socket startup error:%d!", 			
					WSAGetLastError());		
				this->m_tRecorder.Add(REC_ERR, NULL, m_szLogText);
				exit(0);
			}
		}

	}	
	void InitDefault()
	{
		m_bWork = true;
	}
	void InitUdpSocket(int port);	
public:	
	static void StartClose(UdpClient *pWorker);	
	static DWORD WINAPI Close(LPVOID param);
	static DWORD WINAPI SetStartWork(LPVOID param);
	static DWORD WINAPI WaitForRecieved(LPVOID param);
	static DWORD WINAPI RecievedCallBack(LPVOID param);	
};

typedef struct RecievedParam
{
	UdpClient *pWorker;
	DataStream *pStream;
	int nUdpPort;
}RecievedParam;

#endif