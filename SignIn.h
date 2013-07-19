#ifndef SIGNIN_H
#define SIGNIN_H

#define SIGNINFILEPATH "../Refference/signin"
#define SIGNINTMPFILEPATH "../Refference/signintmp"
#define SIGNINFORMAT "%d %d %d \n"

#ifdef DEF_SIGNIN
typedef struct __declspec(dllexport) Sign_t
#else
typedef struct __declspec(dllimport) Sign_t
#endif
{
	int nId;	
	int nUsr_id;
	int nUdpPort;
}Sign_t;


#ifdef DEF_SIGNIN
class __declspec(dllexport) SignIn
#else
class __declspec(dllimport) SignIn
#endif
{
protected:
	bool m_bSigned;
	Sign_t m_tSign;
public:
	SignIn(void);
	SignIn(int id, int usr_id, int udpport);
	~SignIn(void);
	int Sign(const Sign_t& tSign);
	int Sign(int id, int usr_id, int udpport);
	int Quit();
	const Sign_t& GetSign()
	{
		return this->m_tSign;
	}
};

#endif