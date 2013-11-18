// WorldServerDlg.cpp : implementation file
//

#include "stdafx.h"

#include "WorldServerDlg.h"
#include "IniFile.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWorldServerDlg dialog




CWorldServerDlg::CWorldServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWorldServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
}

CWorldServerDlg::~CWorldServerDlg()
{
	DeleteCriticalSection(&m_csPlayerList);
	DeleteCriticalSection(&m_csGroupList);
	DeleteCriticalSection(&m_csAcceptConnection);
	DeleteCriticalSection(&m_csParsePacket);
}

void CWorldServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWorldServerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_CB_IPLIST, &CWorldServerDlg::OnCbnSelchangeCbIplist)
	ON_BN_CLICKED(ID_BTN_STARTSERVER, &CWorldServerDlg::OnBnClickedBtnStartserver)
	ON_BN_CLICKED(ID_BTN_STOPSERVER, &CWorldServerDlg::OnBnClickedBtnStopserver)
	ON_WM_CLOSE()
	ON_BN_CLICKED(ID_BTN_ADDUSER, &CWorldServerDlg::OnBnClickedBtnAdduser)
	ON_BN_CLICKED(ID_BTN_DELUSER, &CWorldServerDlg::OnBnClickedBtnDeluser)
END_MESSAGE_MAP()


// CWorldServerDlg message handlers

BOOL CWorldServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	Initialize();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWorldServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWorldServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CWorldServerDlg::Initialize()
{
	srand( (unsigned)time( NULL ) );

	m_unPort = DEFAULT_SERVER_PORT;
	m_csIP   = _T("");
	m_stServerMsg = _T("");
	m_bStopAcceptConnectionThread = FALSE;
	m_hAcceptConnectionThread     = NULL;
	m_hAcceptConnectionCompleteEvent = NULL;
	m_hSendPacketThread = NULL;
	m_bStopSendPacketThread = FALSE;
	InitializeCriticalSection(&m_csParsePacket);
	InitializeCriticalSection(&m_csPlayerList);
	InitializeCriticalSection(&m_csGroupList);
	InitializeCriticalSection(&m_csAcceptConnection);
	m_pstServerMsg    = static_cast<CStatic*>(GetDlgItem(IDC_STA_SERVER_MSG));
	m_pcbServerIPList = static_cast<CComboBox*>(GetDlgItem(IDC_CB_IPLIST));
	m_pedUserList     = static_cast<CEdit*>(GetDlgItem(IDC_EDT_USER_LIST));
	m_pbuServerStart  = static_cast<CButton*>(GetDlgItem(ID_BTN_STARTSERVER));
	m_pbuServerStop   = static_cast<CButton*>(GetDlgItem(ID_BTN_STOPSERVER));
	m_pliUserList     = static_cast<CListBox*>(GetDlgItem(IDC_LIST_USERLIST));
	m_pedUser		  = static_cast<CEdit*>(GetDlgItem(IDC_EDT_USER));
	m_pbnAddUser      = static_cast<CButton*>(GetDlgItem(ID_BTN_ADDUSER));
	m_pbnDelUser      = static_cast<CButton*>(GetDlgItem(ID_BTN_DELUSER));
	m_pstServerMsg->SetWindowText(_T("訊息"));
	m_pbuServerStart->SetWindowText(_T("啟動"));
	m_pbuServerStop->SetWindowText(_T("停止"));
	m_pbnAddUser->SetWindowText(_T("增加使用者"));
	m_pbnDelUser->SetWindowText(_T("刪除使用者"));
	(static_cast<CButton*>(GetDlgItem(IDC_STA_ONLINE_USER)))->SetWindowText(_T("在線上的使用者"));
	(static_cast<CButton*>(GetDlgItem(IDC_STA_ALLOW_USER_LIST)))->SetWindowText(_T("允許連線的使用者清單"));
	(static_cast<CButton*>(GetDlgItem(IDC_STA_SERVER_IPLIST)))->SetWindowText(_T("伺服器位置 :"));

	m_pbuServerStop->EnableWindow(FALSE);

	//// Detected Server IP List;
	DetectIPlist();

	// Check INI File
	if(CheckINIFile())
		if(GetUserListFormINI())
			SetUserListToListUI();

	// Refresh User List in Edit control
	PostMessage(WM_UPDATE_USER_LIST_MESSAGE);

	// Update UI Status
	UpdateUIStatus(UI_STATUS_NORMAL);
}
void CWorldServerDlg::DetectIPlist()
{
	ULONG	          ulOutBufLen	     = sizeof(IP_ADAPTER_INFO);
	PIP_ADAPTER_INFO  PIPAdapterInfo     = NULL;
	PIP_ADAPTER_INFO  PIPAdapterInfoHead = NULL;
	std::set<CString> NICsIPList;

	PIPAdapterInfo = (IP_ADAPTER_INFO *) malloc( sizeof(IP_ADAPTER_INFO));

	// First time, we must got ERROR_BUFFER_OVERFLOW, so we need get right buffer size
	if(GetAdaptersInfo(PIPAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(PIPAdapterInfo);
		// Get right PIP_ADAPTER_INFO buffer size...
		PIPAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen);	
	}
	
	DWORD dwRet = GetAdaptersInfo(PIPAdapterInfo, &ulOutBufLen);
	if(dwRet == ERROR_SUCCESS)
	{
		PIPAdapterInfoHead = PIPAdapterInfo; 
		while(PIPAdapterInfo)
		{
			UINT unIP = 0;
			IP_ADDR_STRING* pIPADDRSTR = NULL;

			pIPADDRSTR = &(PIPAdapterInfo->IpAddressList);
			while(pIPADDRSTR) {
				CString csIPtmp(reinterpret_cast<char*>(&pIPADDRSTR->IpAddress));
				//unIP = htonl(inet_addr((char*)&(pIPADDRSTR->IpAddress)));
				// Check IP is not 0 and IP location doesn't exist in IP list
				if(!csIPtmp.IsEmpty()){
					if(NICsIPList.find(csIPtmp) == NICsIPList.end())
					{
						// Create IP list				
						NICsIPList.insert(csIPtmp);
					}
				}
				// Find next IP information
				pIPADDRSTR  =   pIPADDRSTR->Next;
			}	
			// Get next NIC information
			PIPAdapterInfo = PIPAdapterInfo->Next;
		}
	} else {
		
		m_stServerMsg.Format(_T("[UDP] DetectNICnumber:GetAdaptersInfo(), ERROR CODE %d"), dwRet);
		PostMessage(WM_UPDATE_SERVER_MSG_MESSAGE);
	}
	// delete NIC info
	if(PIPAdapterInfoHead)
	{
		free(PIPAdapterInfoHead);
		PIPAdapterInfoHead = NULL;
	}

	if(!NICsIPList.empty())
	{
		for(std::set<CString>::iterator it = NICsIPList.begin();
			it != NICsIPList.end(); ++it)
		{
			m_pcbServerIPList->AddString(*it);
		}
		
		m_pcbServerIPList->SetCurSel(0);
		m_pcbServerIPList->EnableWindow(TRUE);
	}
	OnCbnSelchangeCbIplist();
}

void CWorldServerDlg::SetServerMsg()
{
	m_pstServerMsg->SetWindowText(m_stServerMsg);
}

void CWorldServerDlg::UpdateUIStatus(unsigned short usUIStatus)
{

	if( usUIStatus == UI_STATUS_NORMAL )
	{
		if(!m_csIP.IsEmpty() && m_unPort)
		{
			m_pbuServerStart->EnableWindow(TRUE);
			m_stServerMsg = _T("伺服器準備完畢...");
		} else {
			m_pbuServerStart->EnableWindow(FALSE);
			m_stServerMsg = _T("請輸入伺服器參數...");
		}
	} else if( usUIStatus == UI_STATUS_START_SERVER )
	{
		m_pcbServerIPList->EnableWindow(FALSE);
		m_pbuServerStart->EnableWindow(FALSE);
		m_pbuServerStop->EnableWindow(TRUE);
		m_stServerMsg = _T("啟動伺服器...");
		
	} else if ( usUIStatus == UI_STATUS_STOP_SERVER )
	{
		m_pcbServerIPList->EnableWindow(TRUE);
		m_pbuServerStart->EnableWindow(TRUE);
		m_pbuServerStop->EnableWindow(FALSE);
		m_stServerMsg = _T("停止伺服器...");
	}
	PostMessage(WM_UPDATE_SERVER_MSG_MESSAGE);
}

void CWorldServerDlg::OnCbnSelchangeCbIplist()
{
	int nSelectIP = m_pcbServerIPList->GetCurSel();
	if(nSelectIP != -1)
		m_pcbServerIPList->GetLBText(nSelectIP , m_csIP);

	
}

void CWorldServerDlg::OnBnClickedBtnStopserver()
{
	m_stServerMsg = _T("伺服器停止中...");
	SetServerMsg();

	SendSerevrOffLinePacket();
	StopServer();
	UpdateUIStatus(UI_STATUS_STOP_SERVER);
	m_pbuServerStart->SetFocus();

	m_stServerMsg = _T("伺服器已停止...");
	SetServerMsg();
}

void CWorldServerDlg::SendSerevrOffLinePacket()
{
	world_header wtmp;
	wtmp.uhCommand = SERVER_OFFLINE;
	m_SendPacketList.push(wtmp);
}

void CWorldServerDlg::StopAcceptConnection()
{
	// Close Accept Connection Thread
	m_bStopAcceptConnectionThread = TRUE;
	
	if(m_hAcceptConnectionThread != NULL)
	{
		if( WaitForSingleObject(m_hAcceptConnectionThread, 1000) != WAIT_OBJECT_0)
		{
			TerminateThread(m_hAcceptConnectionThread, 0xffffffff);	
		}
		CloseHandle(m_hAcceptConnectionThread);
		m_hAcceptConnectionThread = NULL;
	}

	// Close Send Packet Thread
	m_bStopSendPacketThread = TRUE;
	if(m_hSendPacketThread != NULL)
	{	
		if( WaitForSingleObject(m_hSendPacketThread, 1000) != WAIT_OBJECT_0)
		{
			TerminateThread(m_hSendPacketThread, 0xffffffff);	
		}
		CloseHandle(m_hSendPacketThread);
		m_hSendPacketThread = NULL;
	}


	// close Server socket
	if(m_Connection.m_soSocket != INVALID_SOCKET)
	{
		if(shutdown(m_Connection.m_soSocket , SD_BOTH) == SOCKET_ERROR )
		{
			//m_csServerMsg.Format(_T("shutdown Server socket, ERROR CODE = %d"), WSAGetLastError());
		}
		closesocket(m_Connection.m_soSocket );
		m_Connection.m_soSocket  = INVALID_SOCKET;
	}
}

void CWorldServerDlg::StopServer()
{

	StopAcceptConnection();

	// Close client connect
	// Send OFF line msg to all cient...
	std::vector<AcceptClient> tmpList;
	AccessAcceptClientList(GET_CONNECTION_LIST, tmpList);
	for(AcceptClientit it = tmpList.begin(); it != tmpList.end(); it++)
	{
		it->m_bStopReceivePacketThread = TRUE;

		if( WaitForSingleObject(it->m_hReceivePacketThread, 1000) != WAIT_OBJECT_0)
		{
			TerminateThread(it->m_hReceivePacketThread, 0xffffffff);	
		}
		CloseHandle(it->m_hReceivePacketThread);
		it->m_hReceivePacketThread = NULL;

		if(it->m_soClientSocket != INVALID_SOCKET)
		{
			if(shutdown(it->m_soClientSocket, SD_BOTH)==SOCKET_ERROR)
			{
				//m_csServerMsg.Format(_T("shutdown Client socket, ERROR CODE = %d"), WSAGetLastError());
			}
			closesocket(it->m_soClientSocket);
			it->m_soClientSocket = INVALID_SOCKET;
		}
	}
	// Clear All data
	Player tmpP;
	AccessUserList(CLEAR_ALL_USER, tmpP);
	
	std::vector<AcceptClient> tmpvAc;
	AccessAcceptClientList(CLEAR_ALL_CONNECTION, tmpvAc);

	std::vector<UINT> tmpU;
	AccessGroupList(CLEAR_ALL_GROUP,0,tmpU);

	m_UserGroup.clear();
	if(!m_SendPacketList.empty())
	{
		m_SendPacketList.pop();
	}
}

void CWorldServerDlg::OnBnClickedBtnStartserver()
{
	m_stServerMsg = _T("伺服器啟動中...");
	SetServerMsg();

	if(!m_csIP.IsEmpty() && m_unPort)
	{ 
		UpdateUIStatus(UI_STATUS_START_SERVER);

		char pchIP[64] = "";
		W2MBChar(m_csIP, pchIP, PACKET_USERNAME_CHARSIZE);
		m_Connection.SetIPandPort( htonl(inet_addr(pchIP)), m_unPort);
		m_Connection.SetSocketAttribute(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		m_Connection.SetSocketOptions(TCP_NODELAY, 1);
		m_Connection.UseBind(TRUE);
		m_Connection.SetServerListen(SERVER_ACCEPT_CONNECTION_NUMBER);

		if(!m_Connection.CreateSocket())
		{
			m_stServerMsg = m_Connection.GetLastErrorMsg() + _T("伺服器啟動失敗...");
			SetServerMsg();
		} else {
			m_stServerMsg = _T("伺服器啟動完畢...");
			SetServerMsg();

			m_bStopAcceptConnectionThread = FALSE;
			m_bStopSendPacketThread       = FALSE;
			if(m_hAcceptConnectionThread ==  NULL)
			{
				if( !(m_hAcceptConnectionThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AcceptConnection, this, 0, NULL )) )
				{
					//m_csServerMsg = _T("Create Accept Client Connect Thread fail...");				
				}
			}

			if(m_hAcceptConnectionThread != NULL && m_hSendPacketThread == NULL)
			{
				if( !(m_hSendPacketThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SendPacket, this, 0, NULL )) )
				{
					//m_csServerMsg = _T("Create Send Packet Thread fail...");
				}
			}

			if(m_hAcceptConnectionThread != NULL && m_hSendPacketThread != NULL)
			{
				m_pbuServerStop->SetFocus();
				UpdateUIStatus(UI_STATUS_START_SERVER);
			}
			else
				UpdateUIStatus(UI_STATUS_STOP_SERVER);
			
		}
	} else {
		m_stServerMsg = _T("伺服器啟動失敗...");
		SetServerMsg();
	}
}

DWORD CWorldServerDlg::AcceptConnection(LPVOID pObject)
{
	CWorldServerDlg* pWSDlg = static_cast<CWorldServerDlg*>(pObject);
	pWSDlg->m_hAcceptConnectionCompleteEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL);
	int size_from = sizeof(sockaddr);
	
	while(!pWSDlg->m_bStopAcceptConnectionThread)
	{
		if(pWSDlg->m_AcceptClientList.size() < SERVER_ACCEPT_CONNECTION_NUMBER )
		{
			// Create New Client strut
			AcceptClient tmpClient;
			// Accept Client connect	
			sockaddr_in sotmpAddr;
			pWSDlg->m_soTmpClientSocket = accept( pWSDlg->m_Connection.m_soSocket, (sockaddr*)&(sotmpAddr), &size_from);

			if( pWSDlg->m_soTmpClientSocket != INVALID_SOCKET)
			{
				if(!tmpClient.m_hReceivePacketThread != NULL)
				{
					if( !(tmpClient.m_hReceivePacketThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)pWSDlg->ReceivePacket, pWSDlg, 0, NULL )) )
					{
						//pWSDlg->m_csServerMsg = _T("Create Receive Packet Thread fail...");
						pWSDlg->SetServerMsg();
						
						// Create Thread Fail, so Close Socket ..
						if(pWSDlg->m_soTmpClientSocket != INVALID_SOCKET)
						{
							if(shutdown(pWSDlg->m_soTmpClientSocket, SD_BOTH) == SOCKET_ERROR )
							{
								//pWSDlg->m_csServerMsg.Format(_T("shutdown Server socket, ERROR CODE = %d"), WSAGetLastError());
								pWSDlg->SetServerMsg();
							}
							closesocket(tmpClient.m_soClientSocket);
							tmpClient.m_soClientSocket = INVALID_SOCKET;
						}
					} else {
						// Push in vector

						std::vector<AcceptClient> tmpList;
						tmpClient.m_soClientSocket =  pWSDlg->m_soTmpClientSocket;
						tmpList.push_back(tmpClient);
						pWSDlg->AccessAcceptClientList(ADD_CONNECTION, tmpList);

						// wait Recv Thread get client info complete...
						WaitForSingleObject(pWSDlg->m_hAcceptConnectionCompleteEvent, 500);
					}
				}
			}

		} else {
			// Close Receive Packet Event
			if(pWSDlg->m_hAcceptConnectionCompleteEvent != NULL)
			{
				CloseHandle(pWSDlg->m_hAcceptConnectionCompleteEvent);
				pWSDlg->m_hAcceptConnectionThread = NULL;
			}

			pWSDlg->m_bStopAcceptConnectionThread = TRUE;
		}
	}
	return 0;
}
DWORD CWorldServerDlg::ReceivePacket(LPVOID pObject)
{
	CWorldServerDlg* pWSDlg = static_cast<CWorldServerDlg*>(pObject);

	// Get specify client connect data
	AcceptClient tmpClient;
	std::vector<AcceptClient> tmpList;
	tmpClient.m_soClientSocket =  pWSDlg->m_soTmpClientSocket;
	tmpList.push_back(tmpClient);

	// notify Acception connection function can Accept next connection
	if(pWSDlg->m_hAcceptConnectionCompleteEvent != NULL)
		::SetEvent(pWSDlg->m_hAcceptConnectionCompleteEvent);

	int  nSOCKADDRSIZE = sizeof(SOCKADDR);
	char chRecvPacket[RECEIVE_PACKET_SIZE] = "";
	int  nRecvPacketLen = 0;

	while(!tmpList[0].m_bStopReceivePacketThread)
	{
		// Initialize receive char array
		memset(chRecvPacket,0,sizeof(chRecvPacket));
		if(pWSDlg->m_Connection.m_soSocket != INVALID_SOCKET)
		{
			// Receive Packet Error
			if( (nRecvPacketLen = recv(tmpList[0].m_soClientSocket, chRecvPacket, RECEIVE_PACKET_SIZE, 0) == SOCKET_ERROR))
			{
				DWORD dwRet = WSAGetLastError();
				
				if(dwRet == WSAEWOULDBLOCK)
					Sleep(WSAEWOULDBLOCK_SLEEP_TIME);
				else {
					tmpList[0].m_bStopReceivePacketThread = TRUE;
					pWSDlg->AccessAcceptClientList(SET_STOPRECEIVEPACKET, tmpList);
				}
			} else {
				EnterCriticalSection(&pWSDlg->m_csParsePacket);
				pWSDlg->ParseRecivePacket(chRecvPacket, nRecvPacketLen, static_cast<UINT>(tmpList[0].m_soClientSocket));
				LeaveCriticalSection(&pWSDlg->m_csParsePacket);

				pWSDlg->AccessAcceptClientList(GET_STOPRECEIVEPACKET, tmpList);
			}
		} else {
			tmpList[0].m_bStopReceivePacketThread = TRUE;
			pWSDlg->AccessAcceptClientList(SET_STOPRECEIVEPACKET, tmpList);
		}	
	}

	return 0;
}

void CWorldServerDlg::ParseRecivePacket(char *chReceivePacket, int nReceivePacketLength, UINT unSessionID)
{
	
	world_header *pPacket = reinterpret_cast<world_header*>(chReceivePacket);
	
	if(pPacket->uhCommand == REGISTER_USERNAME)
	{
		// Response to User
		world_header wtmp;
		wtmp.Receiver.unSessionID = unSessionID;
		
		// Check User List form INI file
		INI_USER_LIST iniUser;
		POSITION      pos =	NULL;
		iniUser.m_csUsername = MB2WCString(reinterpret_cast<char *>(pPacket->Receiver.uhUserName));
		pos = m_IniUserList.Find(iniUser);
		if(pos != NULL)
		{
			// Find User 
			Player tmp;
			memcpy(tmp.uhUserName, pPacket->Receiver.uhUserName, PACKET_USERNAME_CHARSIZE);
			tmp.unChannelID = pPacket->Receiver.unChannelID;
			AccessUserList(SEARCH_USER_BYNAME, tmp);

			if(tmp.unSessionID == 0) // New User
			{
				// Add User
				memcpy(tmp.uhUserName, pPacket->Receiver.uhUserName, PACKET_USERNAME_CHARSIZE);
				tmp.unSessionID = unSessionID;
				tmp.unChannelID = pPacket->Receiver.unChannelID;
				AccessUserList(ADD_USER, tmp);

				// Response to Register Success User
				memcpy(wtmp.Receiver.uhUserName, pPacket->Receiver.uhUserName, PACKET_USERNAME_CHARSIZE);		
				wtmp.uhCommand = REGISTER_USERNAME_RESPONSE;
				wtmp.Receiver.unSessionID = unSessionID;
			} else {
				wtmp.uhCommand = REGISTER_USERNAME_DENY_RESPONSE;
			}
		} else {
			wtmp.uhCommand = REGISTER_USERNAME_DENY_RESPONSE;
		}
		m_SendPacketList.push(wtmp);

	} else if(pPacket->uhCommand == UPDATE_USER_LIST)
	{
		if(pPacket->Receiver.unSessionID)
		{
			Player tmp;
			UINT unPlarer1ID = 0;
			UINT unPlarer2ID = 0;
			UINT unSelfGroupID = 0;
			UINT unPlarer1GroupID = 0;
			UINT unPlarer2GroupID = 0;

			// Find Self Group
			tmp.clear();
			tmp.unSessionID = pPacket->Receiver.unSessionID;
			AccessUserList(GET_USER_GROUPID, tmp);
			unSelfGroupID  = tmp.unGroupID;

			if(unSelfGroupID == 0)
			{
				unSelfGroupID = unSessionID;

				// Add new Group Map
				std::vector<UINT> tmpIDList;
				tmpIDList.push_back(unSelfGroupID);
				AccessGroupList(ADD_ID_TO_GROUP, unSelfGroupID , tmpIDList);

				// Update Player2 Group ID
				tmp.unGroupID = unSelfGroupID;
				AccessUserList(UPDATE_USER_GROUPID, tmp);
			}

			// Find Player 1
			// Need Same Name and Same Channel ID
			tmp.clear();
			memcpy(tmp.uhUserName, pPacket->Player1.uhUserName, PACKET_USERNAME_CHARSIZE);
			tmp.unChannelID = pPacket->Receiver.unChannelID;
			AccessUserList(SEARCH_USER_BYNAME, tmp);
			unPlarer1ID = tmp.unSessionID;
			if( unPlarer1ID != 0 )
			{			
				AccessUserList(GET_USER_GROUPID, tmp);
				unPlarer1GroupID  = tmp.unGroupID;

				if( unPlarer1GroupID != 0)
				{
					if(unPlarer1GroupID != unSelfGroupID)
					{
						// Remove self old group
						std::vector<UINT> tmpIDList;
						tmpIDList.push_back(unPlarer1ID);
						AccessGroupList(REMOVE_ID_FORM_GROUP, unPlarer1GroupID , tmpIDList);
						
						// Add new Group Map
						AccessGroupList(ADD_ID_TO_GROUP, unSelfGroupID , tmpIDList);

						// Update Player2 Group ID
						tmp.unGroupID = unSelfGroupID;
						AccessUserList(UPDATE_USER_GROUPID, tmp);			
					}
				} else {
					// Update Player2 Group ID
					tmp.unGroupID = unSelfGroupID;
					AccessUserList(UPDATE_USER_GROUPID, tmp);
				}	
			}

			// Find Player 2
			// Need Same Name and Same Channel ID
			tmp.clear();
			memcpy(tmp.uhUserName, pPacket->Player2.uhUserName, PACKET_USERNAME_CHARSIZE);
			tmp.unChannelID = pPacket->Receiver.unChannelID;
			AccessUserList(SEARCH_USER_BYNAME, tmp);
			unPlarer2ID = tmp.unSessionID;
			if( unPlarer2ID != 0 )
			{			
				AccessUserList(GET_USER_GROUPID, tmp);
				unPlarer2GroupID  = tmp.unGroupID;

				if( unPlarer2GroupID != 0)
				{
					if(unPlarer2GroupID != unSelfGroupID)
					{
						// Remove Player2 old group List
						std::vector<UINT> tmpIDList;
						tmpIDList.push_back(unPlarer2ID);
						AccessGroupList(REMOVE_ID_FORM_GROUP, unPlarer2GroupID , tmpIDList);

						// Add new Group Map
						AccessGroupList(ADD_ID_TO_GROUP, unSelfGroupID , tmpIDList);
						
						// Update Player2 Group ID
						tmp.unGroupID = unSelfGroupID;
						AccessUserList(UPDATE_USER_GROUPID, tmp);
					}
				} else {
					// Update Player2 Group ID
					tmp.unGroupID = unSelfGroupID;
					AccessUserList(UPDATE_USER_GROUPID, tmp);
				}	
			}
			

			// Response to User
			world_header wtmp;
			wtmp.uhCommand = UPDATE_USER_LIST_RESPONSE;
			wtmp.Receiver.unSessionID = pPacket->Receiver.unSessionID;
			wtmp.Receiver.unGroupID   = unSelfGroupID;
			wtmp.Player1.unSessionID  = unPlarer1ID;
			wtmp.Player2.unSessionID  = unPlarer2ID;
			m_SendPacketList.push(wtmp);
		}
	} else if(pPacket->uhCommand == CLIENT_OFFLINE)
	{
		if(pPacket->Receiver.unSessionID)
		{
			// Update Player
			Player tmp;
			tmp.unSessionID = pPacket->Receiver.unSessionID;
			AccessUserList(REMOVE_USER, tmp);

			// Remove Group
			std::vector<UINT> tmpIDList;
			tmpIDList.push_back(pPacket->Receiver.unSessionID);
			AccessGroupList(REMOVE_ID_FORM_GROUP, pPacket->Receiver.unGroupID, tmpIDList);
			
			// Stop Receive Packet
			AcceptClient tmpClient;
			std::vector<AcceptClient> tmpList;
			tmpClient.m_soClientSocket =  unSessionID;
			tmpClient.m_bStopReceivePacketThread = TRUE;
			tmpList.push_back(tmpClient);
			AccessAcceptClientList(SET_STOPRECEIVEPACKET, tmpList);

			PostMessage(WM_UPDATE_USER_LIST_MESSAGE);

		}
	} else if(pPacket->uhCommand == UPDATE_SELF_CARD)
	{
		if(pPacket->Receiver.unSessionID)
		{
			// Update Self Card
			Player tmp;
			tmp.unSessionID = pPacket->Receiver.unSessionID;
			AccessUserList(SEARCH_USER_BYID, tmp);
			if(tmp.unSessionID != 0)
			{
				// Update Receiver Card List
				tmp.unCard = pPacket->Receiver.unCard;
				AccessUserList(UPDATE_USER_CARD, tmp);
			}


			// Send Card To Other User
			UINT   unPlayer1ID = 0;
			UINT   unPlayer2ID = 0;
			UINT64 unPlayer1Card = 0;
			UINT64 unPlayer2Card = 0;

			// Find Player 1 ID
			tmp.clear();
			memcpy(tmp.uhUserName, pPacket->Player1.uhUserName, PACKET_USERNAME_CHARSIZE);
			tmp.unChannelID = pPacket->Receiver.unChannelID;
			AccessUserList(SEARCH_USER_BYNAME, tmp);
			unPlayer1ID = tmp.unSessionID;

			// Find Player 2 ID
			tmp.clear();
			memcpy(tmp.uhUserName, pPacket->Player2.uhUserName, PACKET_USERNAME_CHARSIZE);
			tmp.unChannelID = pPacket->Receiver.unChannelID;
			AccessUserList(SEARCH_USER_BYNAME, tmp);
			unPlayer2ID = tmp.unSessionID;

			// Get Card
			if(pPacket->Receiver.unGroupID != 0) // Has Group
			{
				// Find same Group User
				std::vector<UINT> tmpIDList;
				AccessGroupList(GET_USERID_LIST, pPacket->Receiver.unGroupID, tmpIDList);

				// Get Other User Card
				for(std::vector<UINT>::iterator beg = tmpIDList.begin();
					beg != tmpIDList.end(); ++beg)
				{
					// Update Player Card List
					if(*beg != 0)
					{
						if( *beg == unPlayer1ID )
						{
							tmp.clear();
							tmp.unSessionID = *beg;
							AccessUserList(GET_USER_CARD, tmp);

							if(tmp.unSessionID == unPlayer1ID)
								unPlayer1Card = tmp.unCard;
						}

						if( *beg == unPlayer2ID )
						{
							tmp.clear();
							tmp.unSessionID = *beg;
							AccessUserList(GET_USER_CARD, tmp);

							if(tmp.unSessionID == unPlayer2ID)
								unPlayer2Card = tmp.unCard;
						}
					}
				}
			}

			// Response to User1
			if(unPlayer1ID != 0)
			{
				world_header wtmp;
				wtmp.uhCommand = GET_OTHER_USER_CARD_RESPONSE;
				wtmp.Receiver.unSessionID = pPacket->Receiver.unSessionID;
				wtmp.Player1.unSessionID  = unPlayer1ID;;
				wtmp.Player1.unCard       = unPlayer1Card;
				wtmp.Player2.unSessionID  = unPlayer2ID;
				wtmp.Player2.unCard       = unPlayer2Card;
				m_SendPacketList.push(wtmp);
			}
			
			// Response to User1
			if(unPlayer1ID != 0)
			{
				world_header wtmp;
				wtmp.uhCommand = GET_OTHER_USER_CARD_RESPONSE;
				wtmp.Receiver.unSessionID = unPlayer1ID;
				wtmp.Player1.unSessionID  = pPacket->Receiver.unSessionID;
				wtmp.Player1.unCard       = pPacket->Receiver.unCard;
				wtmp.Player2.unSessionID  = unPlayer2ID;
				wtmp.Player2.unCard       = unPlayer2Card;
				m_SendPacketList.push(wtmp);
			}

			// Response to User2
			if(unPlayer2ID != 0)
			{
				world_header wtmp;
				wtmp.uhCommand = GET_OTHER_USER_CARD_RESPONSE;
				wtmp.Receiver.unSessionID = unPlayer2ID;
				wtmp.Player1.unSessionID  = pPacket->Receiver.unSessionID;
				wtmp.Player1.unCard       = pPacket->Receiver.unCard;
				wtmp.Player2.unSessionID  = unPlayer1ID;
				wtmp.Player2.unCard       = unPlayer1Card;
				m_SendPacketList.push(wtmp);
			}	
		}
	} else if(pPacket->uhCommand == GET_OTHER_USER_CARD)
	{

	}
}

DWORD CWorldServerDlg::SendPacket(LPVOID pObject)
{
	CWorldServerDlg* pWSDlg = static_cast<CWorldServerDlg*>(pObject);
	int nPacketSize = sizeof(world_header);

	while(!pWSDlg->m_bStopSendPacketThread)
	{
		if(!pWSDlg->m_SendPacketList.empty())
		{
			world_header tmp = pWSDlg->m_SendPacketList.front();
			if(tmp.uhCommand == SERVER_OFFLINE)
			{
				// Send OFF line msg to all cient...
				std::vector<AcceptClient> tmpList;
				pWSDlg->AccessAcceptClientList(GET_CONNECTION_LIST, tmpList);
				for(AcceptClientit it = tmpList.begin();
					it != tmpList.end(); ++it)
				{
					if( send(it->m_soClientSocket, (char *)&tmp, nPacketSize, 0) == SOCKET_ERROR)
					{
						pWSDlg->m_bStopSendPacketThread = TRUE;
					}
				}
			} else {
				// Send msg to specific client
				AcceptClient tmpAccept;
				tmpAccept.m_soClientSocket = tmp.Receiver.unSessionID;
				std::vector<AcceptClient> tmpList;
				tmpList.push_back(tmpAccept);
				pWSDlg->AccessAcceptClientList(SEARCH_CONNECTION_BYUSERID, tmpList);
				if(tmpList[0].m_soClientSocket != 0)
				{
					// Send Message
					if( send(tmpList[0].m_soClientSocket, (char *)&tmp, nPacketSize, 0) == SOCKET_ERROR)
					{
						pWSDlg->m_bStopSendPacketThread = TRUE;
					}

					// Delete Connection
					if(tmp.uhCommand == REGISTER_USERNAME_DENY_RESPONSE)
					{
						tmpList[0].m_bStopReceivePacketThread = TRUE;
						pWSDlg->AccessAcceptClientList(SET_STOPRECEIVEPACKET, tmpList);
						pWSDlg->AccessAcceptClientList(REMOVE_CONNECTION, tmpList);	
					}
				}
			}
			pWSDlg->m_SendPacketList.pop();
		}
	}
	return 0;
}

void CWorldServerDlg::AccessUserList(USHORT usCommand, Player& tmpPlayer)
{
	EnterCriticalSection(&m_csPlayerList);
	if(usCommand == ADD_USER)
	{
		Playerit it=  std::find_if(m_PlayerList.begin(), m_PlayerList.end(), CFindeUserByName(tmpPlayer.uhUserName));
		if( it == m_PlayerList.end())
		{
			// Get wide character User Name
			CString csUserName = MB2WCString(reinterpret_cast<char *>(tmpPlayer.uhUserName));

			// Add User to GUI User List
			m_UserList.insert(csUserName);
			
			// Add User
			m_PlayerList.push_back(tmpPlayer);
		}
		PostMessage(WM_UPDATE_USER_LIST_MESSAGE);

	}
	else if(usCommand == REMOVE_USER)
	{
		Playerit it = std::find_if(m_PlayerList.begin(), m_PlayerList.end(), CFindeUserByID(tmpPlayer.unSessionID));
		if( it != m_PlayerList.end())
		{
			// Remove GUI User List
			CString stTmp(it->uhUserName);
			if(!stTmp.IsEmpty())
			{
				std::set<CString>::iterator beg  = m_UserList.find(stTmp);
				if(beg != m_UserList.end())
					m_UserList.erase(beg);
			}
			// Remove User
			m_PlayerList.erase(it);
		}
		PostMessage(WM_UPDATE_USER_LIST_MESSAGE);


	}else if(usCommand == UPDATE_USER_SESSIONID)
	{
		Playerit it = std::find_if(m_PlayerList.begin(), m_PlayerList.end(), CFindeUserByID(tmpPlayer.unSessionID));
		if( it != m_PlayerList.end())
			it->unSessionID = tmpPlayer.unSessionID;
	}else if(usCommand == UPDATE_USER_GROUPID)
	{
		Playerit it = std::find_if(m_PlayerList.begin(), m_PlayerList.end(), CFindeUserByID(tmpPlayer.unSessionID));
		if( it != m_PlayerList.end())
			it->unGroupID = tmpPlayer.unGroupID;
	}else if(usCommand == UPDATE_USER_CARD)
	{
		Playerit it = std::find_if(m_PlayerList.begin(), m_PlayerList.end(), CFindeUserByID(tmpPlayer.unSessionID));
		if( it != m_PlayerList.end())
			it->unCard = tmpPlayer.unCard;
	}else if(usCommand == GET_USER_NAME)
	{
		Playerit it = std::find_if(m_PlayerList.begin(), m_PlayerList.end(), CFindeUserByID(tmpPlayer.unSessionID));
		if( it != m_PlayerList.end())
			memcpy(tmpPlayer.uhUserName, it->uhUserName, PACKET_USERNAME_CHARSIZE);
		else
			memcpy(tmpPlayer.uhUserName, 0x0, PACKET_USERNAME_CHARSIZE);
	}else if(usCommand == GET_USER_GROUPID)
	{
		Playerit it = std::find_if(m_PlayerList.begin(), m_PlayerList.end(), CFindeUserByID(tmpPlayer.unSessionID));
		if( it != m_PlayerList.end())
			tmpPlayer.unGroupID = it->unGroupID;
		else
			tmpPlayer.unGroupID = 0;
	}else if(usCommand == GET_USER_CARD)
	{
		Playerit it = std::find_if(m_PlayerList.begin(), m_PlayerList.end(), CFindeUserByID(tmpPlayer.unSessionID));
		if( it != m_PlayerList.end())
			tmpPlayer.unCard = it->unCard;
		else
			tmpPlayer.unCard = 0;
	}else if(usCommand == SEARCH_USER_BYID)
	{
		Playerit it = std::find_if(m_PlayerList.begin(), m_PlayerList.end(), CFindeUserByID(tmpPlayer.unSessionID));
		if( it != m_PlayerList.end())
			tmpPlayer.unSessionID = it->unSessionID;
		else
			tmpPlayer.unSessionID = 0;
	}else if(usCommand == SEARCH_USER_BYNAME)
	{
		Playerit it=  std::find_if(m_PlayerList.begin(), m_PlayerList.end(), CFindeUserByName(tmpPlayer.uhUserName));
		if( it != m_PlayerList.end() && tmpPlayer.unChannelID == it->unChannelID)
			tmpPlayer.unSessionID = it->unSessionID;
		else
			tmpPlayer.unSessionID = 0;
	}else if(usCommand == CLEAR_ALL_USER)
	{
		m_UserList.clear();
		m_PlayerList.clear();
	}
	LeaveCriticalSection(&m_csPlayerList);
}

void CWorldServerDlg::AccessGroupList(USHORT usCommand, UINT unGroupID, std::vector<UINT> &tmpIDList)
{
  EnterCriticalSection(&m_csGroupList);
  // Get Group's Card

  if( usCommand == GET_USERID_LIST)
  {
	  UserGroupit itBeg = m_UserGroup.lower_bound(unGroupID),
		          itEnd = m_UserGroup.upper_bound(unGroupID);
	  tmpIDList.clear();
	  while(itBeg != itEnd)
	  {
		  tmpIDList.push_back(itBeg->second);
		  itBeg++;
	  }

  } else if(usCommand == ADD_ID_TO_GROUP) {
	  for(std::vector<UINT>::iterator beg = tmpIDList.begin();
		  beg != tmpIDList.end(); ++beg)
	  {
		m_UserGroup.insert(GroupType(unGroupID, *beg));
	  }
  } else if(usCommand == REMOVE_ID_FORM_GROUP) {
	  UserGroupit GBeg = m_UserGroup.lower_bound(unGroupID),
		          GEnd = m_UserGroup.upper_bound(unGroupID);
	  while(GBeg != GEnd)
	  {	 
		  for(std::vector<UINT>::iterator Ubeg = tmpIDList.begin();
			  Ubeg != tmpIDList.end(); ++Ubeg)
		  {
			  if(GBeg->second == *Ubeg )
			  {
				  GBeg = m_UserGroup.erase(GBeg);
				  break;
			  }
		  }
		  if(GBeg != GEnd)
				GBeg++;
	  }
  } else if(usCommand == CLEAR_ALL_GROUP) {
	  m_UserGroup.clear();
  }
  LeaveCriticalSection(&m_csGroupList);
}

void CWorldServerDlg::AccessAcceptClientList(USHORT usCommand, std::vector<AcceptClient> &tmpAcceptClient)
{
	EnterCriticalSection(&m_csAcceptConnection);
	// Get Group's Card

	if( usCommand == GET_CONNECTION_LIST)
	{
		for(AcceptClientit it = m_AcceptClientList.begin();
			it != m_AcceptClientList.end(); ++it)
		{
			AcceptClient tmp;
			tmp.m_hReceivePacketThread     = it->m_hReceivePacketThread;
			tmp.m_soClientSocket           = it->m_soClientSocket;
			tmp.m_bStopReceivePacketThread = it->m_bStopReceivePacketThread;
			tmpAcceptClient.push_back(tmp);
		}
	} else if(usCommand == GET_STOPRECEIVEPACKET) {
		for(AcceptClientit it = m_AcceptClientList.begin();
			it != m_AcceptClientList.end(); ++it)
		{
			if(it->m_soClientSocket == tmpAcceptClient[0].m_soClientSocket )
			{
				tmpAcceptClient[0].m_bStopReceivePacketThread = it->m_bStopReceivePacketThread;
				break;
			}
		}
	}  else if(usCommand == SET_STOPRECEIVEPACKET) {
		for(AcceptClientit it = m_AcceptClientList.begin();
			it != m_AcceptClientList.end(); ++it)
		{
			if(it->m_soClientSocket == tmpAcceptClient[0].m_soClientSocket )
			{
			  it->m_bStopReceivePacketThread = tmpAcceptClient[0].m_bStopReceivePacketThread;
			  break;
			}
		}
	} else if(usCommand == ADD_CONNECTION) {
		m_AcceptClientList.push_back(tmpAcceptClient[0]);
	} else if(usCommand == REMOVE_CONNECTION) {
		for(AcceptClientit it = m_AcceptClientList.begin();
			it != m_AcceptClientList.end(); ++it)
		{
			if( tmpAcceptClient[0].m_soClientSocket == it->m_soClientSocket )
			{
				if( WaitForSingleObject(it->m_hReceivePacketThread, 300) != WAIT_OBJECT_0)
				{
					TerminateThread(it->m_hReceivePacketThread, 0xffffffff);	
				}
				CloseHandle(it->m_hReceivePacketThread);
				it->m_hReceivePacketThread = NULL;

				if(it->m_soClientSocket != INVALID_SOCKET)
				{
					if(shutdown(it->m_soClientSocket, SD_BOTH)==SOCKET_ERROR)
					{
						m_stServerMsg.Format(_T("shutdown Client socket, ERROR CODE = %d"), WSAGetLastError());
					}
					closesocket(it->m_soClientSocket);
					it->m_soClientSocket = INVALID_SOCKET;
				}

				m_AcceptClientList.erase(it);
				break;
			}
		}
	} else if(usCommand == SEARCH_CONNECTION_BYUSERID) {
		AcceptClientit it = std::find_if(m_AcceptClientList.begin(),
			                             m_AcceptClientList.end(),
			                             CFindeAcceptClientByID(static_cast<UINT>(tmpAcceptClient[0].m_soClientSocket)));
		if( it !=  m_AcceptClientList.end())
		{
			tmpAcceptClient[0].m_soClientSocket = it->m_soClientSocket;
		} else {
			tmpAcceptClient[0].m_soClientSocket = 0;
		}
	} else if(usCommand == CLEAR_ALL_CONNECTION) {
		m_AcceptClientList.clear();
	}
	LeaveCriticalSection(&m_csAcceptConnection);
}

void CWorldServerDlg::RefreshUserList()
{
	if(m_UserList.size())
	{
		CString stTmp = _T("");
		for(std::set<CString>::iterator beg =  m_UserList.begin();
			beg != m_UserList.end(); ++beg)
		{
			stTmp = stTmp + *beg + _T("\r\n");
		}
		m_pedUserList->SetWindowText( stTmp );
	} else {
			m_pedUserList->SetWindowText( _T("線上沒有任何使用者...\r\n"));
	}

}
CString CWorldServerDlg::MB2WCString(char *chStr)
{	
	int nNum  = MultiByteToWideChar(CP_ACP, NULL, (LPCSTR)chStr, -1, NULL, 0);
	TCHAR *ptchTmpStr = new TCHAR[nNum];
	if(ptchTmpStr)
		MultiByteToWideChar(CP_ACP, NULL, chStr, -1, ptchTmpStr, nNum);

	CString csStr(ptchTmpStr);
	if(ptchTmpStr)
		delete [] ptchTmpStr;

	return csStr;
}

void CWorldServerDlg::W2MBChar(CString csStr, char *pchStr, int nchLength)
{
	int nNum = WideCharToMultiByte(CP_ACP, NULL, csStr, -1, NULL, 0, NULL, NULL);
	if(nNum <= nchLength)
	{
		char *pchTmpStr = NULL;
		pchTmpStr = new char[nNum];
		if(pchTmpStr)
			WideCharToMultiByte(CP_ACP, NULL, csStr, -1, pchTmpStr , nNum, NULL, NULL);

		memset(pchStr, 0x0, nchLength);
		memcpy(pchStr, pchTmpStr, nNum);

		if(pchTmpStr)
			delete [] pchTmpStr;
	}
}

BOOL CWorldServerDlg::CheckINIFile()
{
	CString	csPath;
	CString csEXE;
	int		nLen        =   0;
	int		nLenEXE     =   0;
	HANDLE  hFindFile   =   INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA     FindFileData;

	csPath	=	AfxGetApp()->m_pszHelpFilePath;
	csEXE	=	AfxGetApp()->m_pszExeName;
	nLen    =	csPath.GetLength();
	nLenEXE =	csEXE.GetLength();

	if(nLen && nLenEXE)
	{
		csPath.Delete(nLen-nLenEXE-4,nLenEXE+4);
		m_csFile = csPath + _T("WorldServer_Config.ini");

		hFindFile = FindFirstFile(m_csFile, &FindFileData);
		if (hFindFile == INVALID_HANDLE_VALUE) 
			return FALSE;
		else
			return TRUE;
	}			
	return FALSE;   
}
BOOL CWorldServerDlg::GetUserListFormINI()
{
	CIniFile iniFile;

	INI_SESSION  *pSessionHEAD   =   NULL;
	INI_SESSION  *pSessionTemp   =   NULL;
	INI_VARIABLE *pVariableHEAD	 =	NULL;
	iniFile.SetFileName((LPTSTR)(LPCTSTR)m_csFile);

	if(!iniFile.ReadAllSessionValue(&pSessionTemp))	
		return FALSE;

	pSessionHEAD  = pSessionTemp;
	while(pSessionTemp)
	{
		pVariableHEAD = pSessionTemp->pVars;

		if(wcscmp(pSessionTemp->pszSessName,L"User List") == 0)
		{
			while(pSessionTemp->pVars)
			{
				INI_USER_LIST iniUser;
				POSITION   pos = NULL;

				// Get Enable-Auto-connect config
				CString	csEnbaleAuto = _T("");

				// Handle Device name
				iniUser.m_csUsername.Format(L"%s",pSessionTemp->pVars->pszVarName);
				iniUser.m_csValue.Format(L"%s",pSessionTemp->pVars->pszVarValue);
				if(iniUser.m_csValue != _T("0"))
				{
					m_IniUserList.AddHead(iniUser);
				}
				// Get next...
				pSessionTemp->pVars = pSessionTemp->pVars->pNext;
			}
		}
		pSessionTemp = pSessionTemp->pNext;	
		iniFile.FreeVariable(&pVariableHEAD);
	}
	iniFile.FreeSession(&pSessionHEAD);
	return TRUE;
}

void CWorldServerDlg::SaveUserListToINI()
{
	if(!m_IniUserList.IsEmpty())
	{
		INI_USER_LIST	iniUser;
		CString  UserTag   = _T("User List");
		CIniFile iniFile;

		POSITION pos = m_IniUserList.GetHeadPosition();
		while( pos != NULL)
		{
			iniUser = m_IniUserList.GetAt(pos);
			iniFile.SetFileName((LPTSTR)(LPCTSTR)m_csFile);          
			iniFile.WriteKeyValue((LPTSTR)(LPCTSTR)UserTag, (LPTSTR)(LPCTSTR)iniUser.m_csUsername,(LPTSTR)(LPCTSTR)iniUser.m_csValue);
			// Get Next Pos
			iniUser = m_IniUserList.GetNext(pos);
		}
	}
}
void CWorldServerDlg::OnClose()
{

	int nRet = IDOK;
	if(m_Connection.m_soSocket != INVALID_SOCKET)
	{
		nRet = MessageBox(_T("確定直接關閉伺服器嗎?"), _T("警告"), MB_OKCANCEL );
	}

	if(nRet == IDOK)
	{
		m_IniUserList.RemoveAll();
		StopAcceptConnection();
		StopServer();
		CDialog::OnCancel();
	}
}

void CWorldServerDlg::OnCancel()
{}

void CWorldServerDlg::OnBnClickedBtnAdduser()
{
	CString csAddUser = _T("");
	m_pedUser->GetWindowText(csAddUser);
	if(!csAddUser.IsEmpty())
	{
		INI_USER_LIST	iniUser;
		iniUser.m_csUsername = csAddUser;
		POSITION pos = NULL;
		pos = m_IniUserList.Find(iniUser);
		if(pos == NULL)
		{
			// Set value
			iniUser.m_csValue = _T("1");
			m_IniUserList.AddHead(iniUser);

			// Add to ListCtrl
			m_pliUserList->AddString(csAddUser);

			// Reset Edit
			m_pedUser->SetWindowText(_T(""));
			
			// Save to INI file
			SaveUserListToINI();
		}
	}
}

void CWorldServerDlg::OnBnClickedBtnDeluser()
{
	INT index = m_pliUserList->GetCurSel();
	if(index != -1)
	{
		CString csDelUser;
		m_pliUserList->GetText(index, csDelUser);
		if(!csDelUser.IsEmpty())
		{
			INI_USER_LIST	iniUser;
			iniUser.m_csUsername = csDelUser;
			POSITION pos = m_IniUserList.Find(iniUser);
			if(pos != NULL)
			{
				// Set ini file
				iniUser.m_csValue = _T("0");
				m_IniUserList.SetAt(pos, iniUser);

				// Delete List Control String
				m_pliUserList->DeleteString(index);

				// Reset Edit Control
				m_pedUser->SetWindowText(_T(""));

				// Save to INI File
				SaveUserListToINI();
			}
		}
	}
}

void CWorldServerDlg::SetUserListToListUI()
{
	if(!m_IniUserList.IsEmpty())
	{
		INI_USER_LIST	iniUser;
		POSITION pos = m_IniUserList.GetHeadPosition();
		while( pos != NULL)
		{
			iniUser = m_IniUserList.GetAt(pos);
			m_pliUserList->AddString(iniUser.m_csUsername);

			// Get Next Pos
			iniUser = m_IniUserList.GetNext(pos);
		}
	}
}

LRESULT CWorldServerDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_UPDATE_USER_LIST_MESSAGE:
		RefreshUserList();
		break;
	case WM_UPDATE_SERVER_MSG_MESSAGE:
		SetServerMsg();
		break;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}
