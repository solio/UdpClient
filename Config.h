#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <Windows.h>
#define CONFIGFILEPATH "../Refference/config"

#ifdef DEF_SRVCONFIG_T
typedef struct __declspec(dllexport) SrvConfig_t
#else
typedef struct __declspec(dllimport) SrvConfig_t
#endif
{
	int WorkID;
	int UdpPort;
}SrvConfig_t, CltConfig_t;

#ifdef LOCAL_INCLUDE_CONFIG
class Config
#else 
#ifdef DEF_CONFIG
class __declspec(dllexport) Config
#else
class __declspec(dllimport) Config
#endif
#endif
{
private:
	/* config content*/
	int m_nWorkID;
	int m_nServerUdpPort;
	int m_nClientUdpPort;
	int m_nMaxConnSize;	
	SrvConfig_t *m_tServerList;
	/* end config content*/
private:
	int m_cServerCount;
public:
	Config(void);
	~Config(void);
	void ReadConfig();
	void WriteConfig();
	bool IsExist(const SrvConfig_t& tConfig);
	void ResetSize(int nSize);
	void Add(const SrvConfig_t& tConfig);
	void Set(const int nWorkID, const int nSrvPort,
		const int nClientPort, const int nMaxSize);
	SrvConfig_t* GetServerList(int *pCount = NULL);
public:
	static bool HasConfig()
	{
		return _waccess(TEXT(CONFIGFILEPATH), 0) == 0;
	}	
	bool IsEmpty()
	{
		return (m_cServerCount < this->m_nMaxConnSize);
	}
	int GetWorkID() 
	{ 
		return this->m_nWorkID; 
	}
	int GetServerUdpPort() 
	{ 
		return this->m_nServerUdpPort; 
	}
	int GetClientUdpPort() 
	{ 
		return this->m_nClientUdpPort; 
	}
	int GetMaxConnSize() 
	{ 
		return this->m_nMaxConnSize; 
	}	
	int GetSrvCount()
	{		
		return this->m_cServerCount;
	}
};

#endif