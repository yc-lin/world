#include "stdafx.h"
#include "GenericConnection.h"


GenericConnection::GenericConnection()
{
	m_unIP   = 0;
	m_usPort = 0;
	m_soSocket     = INVALID_SOCKET;
	m_usSocketAF   = 0;
	m_usSocketTYPE = 0;
	m_usSocketPROTOCAL = 0;
	m_unSocketopt      = 0;
	m_unSocketoptValue = 0;
	m_csLastErrorMsg = _T("");
	m_RetryBindTimes  = 0;
	m_RetryBindTimes = FALSE;
	m_nAcceptConnectNumber = 1;
	m_bUseBind = FALSE;
	PrepareWSA();
}
GenericConnection::~GenericConnection()
{
	WSACleanup();
	if(m_soSocket != INVALID_SOCKET)
	{
		closesocket( m_soSocket );
		m_soSocket = INVALID_SOCKET;
	}

}

void GenericConnection::SetIPandPort(unsigned int unIP, unsigned short usPort)
{
	m_unIP   = unIP;
	m_usPort = usPort;
}
CString GenericConnection::GetLastErrorMsg()
{
	return m_csLastErrorMsg;
}

void GenericConnection::SetSocketAttribute(unsigned short usSocketAF, unsigned short usSocketTYPE, unsigned short usSocketPROTOCAL)
{
	m_usSocketAF       = usSocketAF;
	m_usSocketTYPE     = usSocketTYPE;
	m_usSocketPROTOCAL = usSocketPROTOCAL;
}

void GenericConnection::SetSocketOptions(unsigned int unSocketopt, unsigned int unSocketoptValue)
{
	m_unSocketopt      = unSocketopt;
	m_unSocketoptValue = unSocketoptValue;
}

void GenericConnection::BindRetry(unsigned short usTryBindtimes)
{
	m_RetryBindTimes = usTryBindtimes;
}

void GenericConnection::SetBroadCast(BOOL bBoradCast)
{
	m_bBroadCast = bBoradCast;
}

BOOL GenericConnection::TcpConnect()
{
	if(m_soSocket != INVALID_SOCKET)
	{
		m_soAddress.sin_family       = m_usSocketAF; 
		m_soAddress.sin_addr.s_addr  = htonl(m_unIP);
		m_soAddress.sin_port         = htons(m_usPort);

		if(connect(m_soSocket, (sockaddr*)&m_soAddress, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
		{
			m_csLastErrorMsg.Format(_T("TCP connect Error : Error CODE %d"), WSAGetLastError());
			CloseSocket();
			return FALSE;
		}else {
			return TRUE;
		}
	}
	return FALSE;
}

void GenericConnection::UseBind(BOOL bBind)
{
	m_bUseBind = bBind;
}

void GenericConnection::SetServerListen(int nAcceptConnectNumber)
{
	m_nAcceptConnectNumber = nAcceptConnectNumber;
}

BOOL GenericConnection::CloseSocket()
{
	if(m_soSocket != INVALID_SOCKET)
	{
		if(shutdown(m_soSocket, SD_BOTH)==SOCKET_ERROR)
		{
			m_csLastErrorMsg.Format(_T("shutdown socket error : %d"), WSAGetLastError());
		}
		closesocket( m_soSocket );
		m_soSocket = INVALID_SOCKET;
		return TRUE;
	}
	return FALSE;
}
BOOL GenericConnection::CreateSocket()
{
	if(m_soSocket == INVALID_SOCKET)
		return PrepareSocket();
	else
		return TRUE;
}

UINT GenericConnection::GetIPAddress(CString csURL)
{
	LPHOSTENT lphostent;
	ULONG	  uAddr = INADDR_NONE;

	DWORD dwNum = WideCharToMultiByte(CP_ACP, NULL, csURL, -1, NULL, 0, NULL, NULL);
	char *pchHost = NULL;
	pchHost = new char[dwNum];
	if(pchHost)
	{
		WideCharToMultiByte (CP_ACP, NULL, csURL, -1, pchHost, dwNum, NULL,  NULL);

		// Check for an Internet Protocol dotted address string
		uAddr = inet_addr(pchHost);

		if ( (INADDR_NONE == uAddr) && (strcmp( pchHost, "255.255.255.255" )) )
		{
			// It's not an address, then try to resolve it as a hostname
			if ( lphostent = gethostbyname(pchHost) )
			{
				uAddr = *((ULONG *) lphostent->h_addr_list[0]);
			}
			else {
				m_csLastErrorMsg.Format(_T("Get Host Name Error : %d"), WSAGetLastError());
			}
		}
	}
	
	if(pchHost)
		delete [] pchHost;

	return ntohl(uAddr);
}

BOOL GenericConnection::PrepareWSA()
{
	WSADATA	wsaData;
	if(WSAStartup(MAKEWORD(2,2),&wsaData) != 0)
	{
		m_csLastErrorMsg.Format(_T("[UDP] Prepare WSA DATA Error return value : %d"), WSAGetLastError());
		return FALSE;
	}
	return TRUE;
}
BOOL GenericConnection::PrepareSocket()
{
	if(m_soSocket != INVALID_SOCKET)
	{
		m_csLastErrorMsg = _T("Socket is INVALID_SOCKET...");
		return FALSE;
	}

	// Check valid IP
	if(m_unIP == 0)
	{
		m_csLastErrorMsg  = _T("IP is 0");
		return FALSE;
	}

	// Check valid IP
	if(m_usPort == 0)
	{
		m_csLastErrorMsg  = _T("Port is 0");
		return FALSE;
	}

	// check Socket
	if( (m_soSocket = socket(m_usSocketAF, m_usSocketTYPE, m_usSocketPROTOCAL)) == INVALID_SOCKET)
	{
		m_csLastErrorMsg.Format( _T("Create socket error, ERROR CODE = %d"), WSAGetLastError() );
		return FALSE;
	}

	// set socket options
	if( (setsockopt(m_soSocket, SOL_SOCKET, m_unSocketopt, (char*)&m_unSocketoptValue, sizeof(m_unSocketoptValue))) == SOCKET_ERROR)
	{
		m_csLastErrorMsg.Format( _T("set sockopt, ERROR CODE = %d"), WSAGetLastError() );
		return FALSE;
	}

	// For UDP Broadcast
	if(m_usSocketTYPE == SOCK_DGRAM)
	{
		if(setsockopt(m_soSocket,SOL_SOCKET,SO_BROADCAST,(LPSTR)&m_bBroadCast,sizeof(BOOL)) == SOCKET_ERROR)
		{
			m_csLastErrorMsg.Format(_T("Set Broadcast bind port = %d fail, ERROR CODE = %d"), m_soSocket, WSAGetLastError());
			return FALSE;
		}
	}

	// Set socket Blocking options
	DWORD dwCmd = 0L;
	if(ioctlsocket(m_soSocket, FIONBIO, &dwCmd) == SOCKET_ERROR)
	{
		m_csLastErrorMsg.Format(_T("ioctlsocket, Set socket FIONBIO options fail, ERROR CODE = %d"), WSAGetLastError());
		return FALSE;
	}


	// Bind socket port and limit retry bind port times
	if(m_bUseBind)
	{
		UINT unRetry = 0;
		unsigned short usPort = m_usPort;
		do {
			m_soAddress.sin_family        =   m_usSocketAF;
			m_soAddress.sin_addr.s_addr   =   htonl(m_unIP);
			m_soAddress.sin_port          =   htons(usPort);

			if(bind(m_soSocket, (sockaddr *)&m_soAddress, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
			{
				m_csLastErrorMsg.Format(_T("bind port error, Port = %d, ERROR CODE = %d"), usPort, WSAGetLastError());
				usPort++;
				unRetry++;
			}
			else {
				break;
			}
		} while( unRetry < m_RetryBindTimes );
	}

	// For TCP
	if(m_usSocketTYPE == SOCK_STREAM && m_bUseBind)
	{
		if(listen(m_soSocket, m_nAcceptConnectNumber) != 0)
		{
			m_csLastErrorMsg = _T("Set Listen Error...");
			return FALSE;
		}
	}
	
	return TRUE;
}