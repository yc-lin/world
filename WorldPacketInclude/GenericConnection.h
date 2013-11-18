#ifndef GENERIC_CONNECTION_H
#define GENERIC_CONNECTION_H
#include <Winsock2.h>
#define RECEIVE_PACKET_SIZE 1460
#define WSAEWOULDBLOCK_SLEEP_TIME 100
class GenericConnection {
public:
	GenericConnection();
	~GenericConnection();
	CString GetLastErrorMsg();
	void    SetIPandPort(unsigned int unIP, unsigned short usPort);
	void    SetSocketAttribute(unsigned short usSocketAF, unsigned short usSocketYPE, unsigned short usSocketPROTOCAL);
	void    SetSocketOptions(unsigned int unSocketopt, unsigned int unSocketoptValue);
	void    BindRetry(unsigned short usTryBindtimes);
	void    SetServerListen(int nAcceptConnectNumber);
	UINT    GetIPAddress(CString csURL);
	void    UseBind(BOOL bBind);
	
	BOOL    CreateSocket();
	BOOL    CloseSocket();
	// for UDP
	void    SetBroadCast(BOOL bBoradCast);

	// for TCP
	BOOL   TcpConnect();
	// 
	SOCKET  m_soSocket;
	sockaddr_in m_soAddress;
protected:
	BOOL PrepareWSA();
	BOOL PrepareSocket();

private:
	UINT    m_unIP;
	unsigned short m_usPort;
	CString m_csLastErrorMsg;
	unsigned short m_usSocketAF;
	unsigned short m_usSocketTYPE;
	unsigned short m_usSocketPROTOCAL;
	unsigned int   m_unSocketopt;
	unsigned int   m_unSocketoptValue;
	unsigned short m_RetryBindTimes;
	int m_nAcceptConnectNumber;
	BOOL  m_bBroadCast;
	BOOL  m_bUseBind;

};
#endif