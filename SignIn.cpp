#ifndef DEF_SIGNIN
#define DEF_SIGNIN
#endif

#include "SignIn.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

SignIn::SignIn(void) :
	m_bSigned(false)
{
}

SignIn::SignIn(int id, int usr_id, int udpport) :
	m_bSigned(false)
{
	this->m_tSign.nId = id;
	this->m_tSign.nUsr_id = usr_id;
	this->m_tSign.nUdpPort = udpport;
}


SignIn::~SignIn(void)
{
}

/*
 * @program:
 *     judge the specific Sign_t is valid.the sign in file format is following:
 *     [id][SPACE][usr_id][SPACE][udpport][SPACE][ENTER]
 * @param:
 *     tSign: the specific Sign_t object.
 * @return:
 *     -1 for file error, 0 for success, 
 *     1 for id conflict, 2 for udp port conflict
 */
int SignIn::Sign(const Sign_t& tSign)
{
	FILE *fp = NULL;
	Sign_t tmp;

	fp = fopen(SIGNINFILEPATH, "r+");
	if(fp == NULL)
	{
		return -1;
	}

	while(!feof(fp))
	{
		fscanf(fp, SIGNINFORMAT, &tmp.nId, 
			&tmp.nUsr_id, &tmp.nUdpPort);
		if(tmp.nUsr_id == tSign.nUsr_id)
		{
			fclose(fp);
			return 1;
		}
		else if(tmp.nUdpPort == tSign.nUdpPort)
		{
			fclose(fp);
			return 2;
		}
	}
	fprintf(fp, SIGNINFORMAT, tSign.nId,
		tSign.nUsr_id, tSign.nUdpPort);
	fclose(fp);

	this->m_tSign = tSign;
	this->m_bSigned = true;
	return 0;
}

int SignIn::Sign(int id, int usr_id, int udpport)
{
	Sign_t sign;
	sign.nId = id;
	sign.nUsr_id = usr_id;
	sign.nUdpPort = udpport;

	return Sign(sign);
}

int SignIn::Quit()
{
	FILE *rFp, *wFp;
	Sign_t tmp;

	rFp = fopen(SIGNINFILEPATH, "r");
	wFp = fopen(SIGNINTMPFILEPATH, "w");
	if(rFp == NULL || wFp == NULL)
	{
		return 0;
	}

	while(!feof(rFp))
	{
		fscanf(rFp, SIGNINFORMAT, &tmp.nId, 
			&tmp.nUsr_id, &tmp.nUdpPort);
		if(tmp.nUsr_id != this->m_tSign.nUsr_id)
		{
			fprintf(wFp, SIGNINFORMAT, tmp.nId,
				tmp.nUsr_id, tmp.nUdpPort);
		}
	}
	fclose(rFp);
	fclose(wFp);

	unlink(SIGNINFILEPATH);
	rename(SIGNINTMPFILEPATH, SIGNINFILEPATH);

	return 1;
}

//int SignIn::Quit()
//{
//	FILE *fp = NULL;
//	Sign_t tmp;
//	fpos_t rPos, wPos;
//
//	if(!this->m_bSigned)
//	{
//		return 0;
//	}
//
//	fp = fopen(SIGNINFILEPATH, "r+");
//	if(fp == NULL)
//	{
//		return 0;
//	}
//	while(!feof(fp))
//	{
//		fgetpos(fp, &wPos);
//		int nRead = fscanf(fp, SIGNINFORMAT, &tmp.nId, 
//			&tmp.nUsr_id, &tmp.nUdpPort);				
//		if(tmp.nUsr_id == this->m_tSign.nUsr_id)
//		{
//			char szFormat[100];
//			int nLen;		
//
//			//fgetpos(fp, &rPos);
//			sprintf(szFormat, SIGNINFORMAT, tmp.nId,
//				tmp.nUsr_id, tmp.nUdpPort);
//			nLen = strlen(szFormat);
//			fseek(fp, -1 * (nRead + 1), SEEK_CUR);
//			//fsetpos(fp, &wPos);
//			for(int i = 0; i < nLen - 1; i++)
//			{
//				fprintf(fp, " ");
//			}
//			//fgetpos(fp, &wPos);
//			//while(!feof(fp))
//			//{
//			//	fsetpos(fp, &rPos);
//			//	fscanf(fp, SIGNINFORMAT, &tmp.nId, 
//			//		&tmp.nUsr_id, &tmp.nUdpPort);
//			//	fgetpos(fp, &rPos);
//			//	fsetpos(fp, &wPos);
//			//	fprintf(fp, SIGNINFORMAT, tmp.nId,
//			//		tmp.nUsr_id, tmp.nUdpPort);
//			//	fgetpos(fp, &wPos);
//			//}
//			break;
//		}
//	}
//	fclose(fp);
//
//	return 1;
//}