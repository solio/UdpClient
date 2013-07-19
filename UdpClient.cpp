#ifndef DEF_UDPCLIENT
#define DEF_UDPCLIENT
#endif

#include "UdpClient.h"
#include <windows.h>

HANDLE hMutex;
WSADATA *UdpClient::WSALoader = NULL;

UdpClient::UdpClient(int port) :
	m_bWork(true), m_bIsValid(false)
{
	WSALoad();
	InitUdpSocket(port);
}

UdpClient::~UdpClient()
{	
}

void UdpClient::StartClose(UdpClient *pWorker)
{
	DWORD threadid = 0;
	HANDLE hThread = CreateThread(NULL, 0, pWorker->Close, pWorker, 0, &threadid);
	CloseHandle(hThread);
}

/*
 * @program:
 *     a subthread in order to close the socket on udp and release the resource relative.
 */
DWORD WINAPI UdpClient::Close(LPVOID param)
{
	UdpClient *pWorker = (UdpClient *)param;
	//WaitForSingleObject(this->m_hCLoseMutex, INFINITE);
	WaitForSingleObject(hMutex, INFINITE);
	pWorker->m_bWork = false;
	if(pWorker->m_tWorkSocket != INVALID_SOCKET)
	{	// close socket		
		closesocket(pWorker->m_tWorkSocket);
		pWorker->m_tWorkSocket = INVALID_SOCKET;
	}
	ReleaseMutex(hMutex);	
	return 0;
}

/*
 * @function:
 *     send a message on the udp socket.
 * @param:
 *     ipaddress: the remote machine ip you want to send to.
 *	   port: the port that remote machine listen.
 *     msg: the msg you want to send
 * @return:
 *     0 for sended success, SOCKET_ERROR otherwise. 
 */
int UdpClient::SendTo(char *ipaddress, int port, const MSG_t& msg)
{
	SOCKADDR_IN dest;
	DataStream stream;

	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = inet_addr(ipaddress);
	dest.sin_port = htons(port);

	Serialize(msg, stream.szBuffer);

	if(sendto(this->m_tWorkSocket, stream.szBuffer, 
		stream.nSize, 0, (SOCKADDR*)&dest, sizeof(dest)) == SOCKET_ERROR)
	{	// log this error
		this->m_tRecorder.Add(REC_ERR, NULL, "sended message error");
		return SOCKET_ERROR;
	}

	this->m_tRecorder.Add(REC_SND, &msg, "sended a message");

	return 0;
}

void UdpClient::InitUdpSocket(int port)
{
	this->m_tWorkSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(this->m_tWorkSocket == INVALID_SOCKET)
	{	// create socket error
		sprintf(m_szLogText, "initial socket error:%d!", 			
			WSAGetLastError());		
		this->m_tRecorder.Add(REC_ERR, NULL, m_szLogText);
	}
	else
	{	
		this->m_tLocalAddress.sin_family = AF_INET;
		this->m_tLocalAddress.sin_port = htons(port);
		this->m_tLocalAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		this->m_bIsValid = true;
		this->m_hCLoseMutex = CreateMutex(NULL, FALSE, NULL);
		hMutex = CreateMutex(NULL, FALSE, NULL);
	}
}

void UdpClient::StartWork()
{	
	if(!this->m_bIsValid)
		return;
	if(bind(this->m_tWorkSocket, (SOCKADDR*)&this->m_tLocalAddress, 
		sizeof(this->m_tLocalAddress)) == SOCKET_ERROR)
	{	// log this error			
		sprintf(m_szLogText, "bind local port(%d) error:%d", 
			ntohs(this->m_tLocalAddress.sin_port),
			WSAGetLastError());		
		this->m_tRecorder.Add(REC_ERR, NULL, m_szLogText);		
	}
	else
	{
		sprintf(m_szLogText, "bind local port(%d) success!", 
			ntohs(this->m_tLocalAddress.sin_port));
		this->m_tRecorder.Add(REC_RUN, NULL, m_szLogText);

		listen(this->m_tWorkSocket, MAX_LISTEN);

		SetStartWork(this);
	}
}

/*
 * @program
 */
DWORD WINAPI UdpClient::SetStartWork(LPVOID param)
{
	UdpClient *pWorker = (UdpClient *)param;
	HANDLE hcallback;
	DWORD threadid;
	hcallback = CreateThread(NULL, 0, pWorker->WaitForRecieved, pWorker, 0, &threadid);
	CloseHandle(hcallback);			

	return 0;
}

/*
 * @program:
 *     wait for message come in.
 */
DWORD WINAPI UdpClient::WaitForRecieved(LPVOID param)
{
	UdpClient *pWorker = (UdpClient *)param;
	SOCKADDR_IN remoteaddress;
	int addrlen = sizeof(remoteaddress);

	while(pWorker->IsWork())
	{
		DataStream *pStream = NULL;
		pStream = new DataStream();	//recycle by its handler.
		if((pStream->nCount = recvfrom(pWorker->GetSocket(), pStream->szBuffer, pStream->nSize,
			0, (SOCKADDR*)&remoteaddress, &addrlen)) == SOCKET_ERROR)
		{	// log this action
			int errcode = WSAGetLastError();
			sprintf(pWorker->GetLogText(), "recv message error:%d", errno);
			//WaitForSingleObject(pWorker->GetCloseMutex(), INFINITE);
			Sleep(100);
			WaitForSingleObject(hMutex, INFINITE);
			if(pWorker->IsWork() && errcode != 10004)
			{
				pWorker->GetRecorder().Add(REC_ERR, NULL, pWorker->GetLogText());
				delete pStream;	
			}
			else
			{
				break;
			}
			//ReleaseMutex(pWorker->GetCloseMutex());
			ReleaseMutex(hMutex);
		}
		else
		{
			HANDLE hcallback;
			DWORD threadid;
			RecievedParam *lparam = new RecievedParam;	//recycle by its handler
			lparam->pStream = pStream;
			lparam->pWorker = pWorker;
			lparam->nUdpPort = ntohs(remoteaddress.sin_port);

			hcallback = CreateThread(NULL, 0, pWorker->RecievedCallBack, lparam, 0, &threadid);
			CloseHandle(hcallback);
		}
		Sleep(100);
	}	
	
	return 0;
}

DWORD WINAPI UdpClient::RecievedCallBack(LPVOID param)
{
	RecievedParam *rparam = (RecievedParam*)param;
	MSG_t msg;

	msg = DeSerialize(rparam->pStream->szBuffer);
	if(RunTimeLog::CheckMSG(msg))
	{
		rparam->pWorker->GetRecorder().Add(REC_RCV, 
			rparam->nUdpPort, &msg, "recieved a message");
	}
	else
	{
		rparam->pWorker->GetRecorder().Add(REC_ERR, 
			&msg, "recieved a error message");
	}

	delete rparam->pStream->szBuffer;
	delete rparam->pStream;
	delete rparam;

	return 0;
}