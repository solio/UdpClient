#ifndef DEF_CONFIG
#define DEF_CONFIG
#define DEF_SRVCONFIG_T
#endif

#include "Config.h"


Config::Config(void) : 
	m_nWorkID(0), m_nMaxConnSize(0),
	m_nServerUdpPort(0), m_nClientUdpPort(0),
	m_tServerList(NULL), m_cServerCount(0)
{
	
}


Config::~Config(void)
{

}

/*
 * @program:
 *     read the config file.the file format is following.
 *     [m_nWorkID][SPACE][m_nServerUdpPort][SPACE][m_nClientUdpPort][SPACE][m_nMaxConnSize][SPACE].
 *	   [(here is list of ServerList like [WorkID][SPACE][UdpPort])]
 */
void Config::ReadConfig()
{
	if(HasConfig())
	{
		FILE *fp = NULL;
		fp = fopen(CONFIGFILEPATH, "r");
		if(fp == NULL)
		{
			return;
		}

		fscanf(fp, "%d %d %d %d", 
			&this->m_nWorkID, &this->m_nServerUdpPort,
			&this->m_nClientUdpPort, &this->m_nMaxConnSize);

		m_tServerList = new SrvConfig_t[this->m_nMaxConnSize];
		while(!feof(fp) && this->IsEmpty())
		{
			fscanf(fp, "%d %d", &m_tServerList[m_cServerCount].WorkID, 
				&m_tServerList[m_cServerCount].UdpPort);
			m_cServerCount++;
		}
		fclose(fp);
	}
}

/*
 * @program:
 *	   write the config file. the file format please see
 *     the @program over the method ReadConfig.
 */
void Config::WriteConfig()
{
	FILE *fp = NULL;

	fp = fopen(CONFIGFILEPATH, "w");
	if(fp == NULL)
	{
		return;
	}

	fprintf(fp, "%d %d %d %d ",
		this->m_nWorkID, this->m_nServerUdpPort,
		this->m_nClientUdpPort, this->m_nMaxConnSize);

	for(int i = 0; i < m_cServerCount; i++)
	{
		fprintf(fp, "%d %d ", 
			m_tServerList[i].WorkID,
			m_tServerList[i].UdpPort);
	}
	fclose(fp);
}

bool Config::IsExist(const SrvConfig_t& tConfig)
{	
	bool bResult = false;
	for(int i = 0; i < m_cServerCount; i++)
	{
		if(tConfig.WorkID == m_tServerList[i].WorkID
			|| tConfig.UdpPort == m_tServerList[i].UdpPort)
		{
			bResult = true;
			break;
		}
	}
	return bResult;
}

void Config::ResetSize(int nSize)
{
	SrvConfig_t *newList = NULL;
	newList = new SrvConfig_t[nSize];
	memcpy(newList, m_tServerList, nSize * sizeof(SrvConfig_t));
	delete m_tServerList;
	m_tServerList = newList;
	m_nMaxConnSize = nSize;
	
}

void Config::Add(const SrvConfig_t& tConfig)
{	
	m_tServerList[m_cServerCount] = tConfig;
	m_cServerCount++;	
}

void Config::Set(const int nWorkID, const int nSrvPort,
	const int nClientPort, const int nMaxSize)
{
	this->m_nWorkID = nWorkID;
	this->m_nMaxConnSize = nMaxSize;
	this->m_nServerUdpPort = nSrvPort;
	this->m_nClientUdpPort = nClientPort;
}

SrvConfig_t* Config::GetServerList(int *pCount)
{
	if(pCount != NULL)
		*pCount = this->m_cServerCount;
	return this->m_tServerList;
}