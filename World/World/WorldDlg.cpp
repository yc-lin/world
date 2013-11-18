// WorldDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WorldDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWorldDlg dialog

static CWorldDlg* gWorldDlg = NULL;

CWorldDlg::CWorldDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWorldDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	gWorldDlg = this;
}

CWorldDlg::~CWorldDlg()
{

}
void CWorldDlg::OnOK(){}

void CWorldDlg::OnPaint()
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

HCURSOR CWorldDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CWorldDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BTN_HIDE_PLAYER2_CARDLIST, m_btnHidePlayer2CardList);
	DDX_Control(pDX, IDC_BTN_HIDE_SELF_CARDLIST, m_btnHideSelfCardList);
	DDX_Control(pDX, IDC_BTN_HIDE_OPTIONS, m_btnHideOptions);
}



BOOL CWorldDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	initialize();


	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CWorldDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
//	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_CONNECT_SERVER, &CWorldDlg::OnBnClickedBtnConnectServer)
	ON_BN_CLICKED(IDC_BTN_DISCONNECT_SERVER, &CWorldDlg::OnBnClickedBtnDisconnectServer)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_HIDE_PLAYER2_CARDLIST , &CWorldDlg::OnBnClickedBtnHidePlaye2Cardlist)
	ON_BN_CLICKED(IDC_BTN_HIDE_SELF_CARDLIST , &CWorldDlg::OnBnClickedBtnHideSelfCardlist)
	ON_BN_CLICKED(IDC_BTN_HIDE_OPTIONS, &CWorldDlg::OnBnClickedBtnHideOptions)
	ON_BN_CLICKED(IDC_BTN_SORT_BY_SUIT, &CWorldDlg::OnBnClickedBtnSortBySuit)
	ON_BN_CLICKED(IDC_BTN_SORT_BY_NUMBER, &CWorldDlg::OnBnClickedBtnSortByNumber)
END_MESSAGE_MAP()


void CWorldDlg::initialize()
{

	HDC desktop = ::GetDC(NULL);
	int ndpi = ::GetDeviceCaps(desktop, LOGPIXELSY);
	if( ndpi == 96)
	{
		m_dWx = 1.5;
		m_dWy = 1.625;
	}else if( ndpi == 120)
	{
		m_dWx = 1.747;
		m_dWy = 2;
	}


	// Server Packet Receiver
	m_hRecvServerPacketThread = NULL;
	m_bStopRecvServerPacketThread = FALSE;

	// Record Game packet
	m_bStopRecvGamePacketThread = FALSE;
	m_hRecvGamePacketEvent = NULL;
	m_pTmpNIC  = NULL;

	// Get UI Control
	m_pbnSortByNumber = static_cast<CButton*>(GetDlgItem(IDC_BTN_SORT_BY_NUMBER));
	m_pbnSortBySuit   = static_cast<CButton*>(GetDlgItem(IDC_BTN_SORT_BY_SUIT));
	m_pstMsg         = static_cast<CStatic*>(GetDlgItem(IDC_STA_MSG));
	m_pstUserName    = static_cast<CStatic*>(GetDlgItem(IDC_STA_USERNAME));
	m_pstChannelID   = static_cast<CStatic*>(GetDlgItem(IDC_STA_CHANNELID));;
	m_pedUserName    = static_cast<CEdit*>(GetDlgItem(IDC_EDT_USERNAME));
	m_pcbChannelID   = static_cast<CComboBox*>(GetDlgItem(IDC_CB_CHANNELID));
	m_pstMsgShow     = static_cast<CStatic*>(GetDlgItem(IDC_STA_MSG_SHOW));
	m_pbnConnect     = static_cast<CButton*>(GetDlgItem(IDC_BTN_CONNECT_SERVER));
	m_pbnDisconnect  = static_cast<CButton*>(GetDlgItem(IDC_BTN_DISCONNECT_SERVER));
	m_pstSelfName    = static_cast<CStatic*>(GetDlgItem(IDC_STA_SELF_NAME));
	m_pstPlayer1Name = static_cast<CStatic*>(GetDlgItem(IDC_STA_PLAYER1_NAME));
	m_pstPlayer2Name = static_cast<CStatic*>(GetDlgItem(IDC_STA_PLAYER2_NAME));
	m_pstLine1       = static_cast<CStatic*>(GetDlgItem(IDC_STA_LINE1));
	m_pstLine2       = static_cast<CStatic*>(GetDlgItem(IDC_STA_LINE2));
	m_pstLine3       = static_cast<CStatic*>(GetDlgItem(IDC_STA_LINE3));
	m_pstLine4       = static_cast<CStatic*>(GetDlgItem(IDC_STA_LINE4));
	
	m_pstSelfName->SetWindowText(_T("自己"));
	m_pstPlayer1Name->SetWindowText(_T("玩家1"));
	m_pstPlayer2Name->SetWindowText(_T("玩家2"));
	m_pstUserName->SetWindowText(_T("使用者帳號 :"));
	m_pstMsg->SetWindowText(_T("訊息 : "));
	m_pstChannelID->SetWindowText(_T("群組頻道"));
	m_pbnSortByNumber->SetWindowText(_T("排序依數字"));
	m_pbnSortBySuit->SetWindowText(_T("排序依花色"));
    m_pbnConnect->SetWindowText(_T("連接到伺服器"));
	m_pbnDisconnect->SetWindowText(_T("從伺服器離線"));
	(static_cast<CStatic*>(GetDlgItem(IDC_STA_SORT )))->SetWindowText(_T("排序 :")) ;

	m_cpSelfControl.SetParent(this);
	m_cpPlayer1Control.SetParent(this);
	m_cpPlayer2Control.SetParent(this);
	m_cpPlayer1Control.SetPos(10, 20);
	m_cpPlayer2Control.SetPos(10, 77);
	m_cpSelfControl.SetPos(10, 143);
	m_pbnSortByNumber->SetCheck(BST_CHECKED);
	m_cpSelfControl.SortBy(SORT_BY_NUMBER);
	m_cpPlayer1Control.SortBy(SORT_BY_NUMBER);
	m_cpPlayer2Control.SortBy(SORT_BY_NUMBER);

	m_pcbChannelID->AddString(_T(" 頻道0"));
	m_pcbChannelID->AddString(_T(" 頻道1"));
	m_pcbChannelID->AddString(_T(" 頻道2"));
	m_pcbChannelID->AddString(_T(" 頻道3"));
	m_pcbChannelID->AddString(_T(" 頻道4"));
	m_pcbChannelID->AddString(_T(" 頻道5"));
	m_pcbChannelID->AddString(_T(" 頻道6"));
	m_pcbChannelID->AddString(_T(" 頻道7"));
	m_pcbChannelID->AddString(_T(" 頻道8"));
	m_pcbChannelID->AddString(_T(" 頻道9"));
	m_pcbChannelID->AddString(_T(" 頻道10"));
	m_pcbChannelID->SetCurSel(0);
	

	m_stSelfName    = _T("");
	m_stPlayer1Name = _T("");
	m_stPlayer2Name = _T("");
	m_unSessionID  = 0;
	m_unGroupID    = 0;
	m_unChannelID  = 0;
	m_unSelfCard   = 0;
	m_unPlayer1Card = 0;
	m_unPlayer2Card = 0;
	m_unPlayer1SessionID = 0;
	m_unPlayer2SessionID = 0;
	m_unSelfNewCard = 0;
	m_unPlayer1NewCard = 0;
	m_unPlayer2NewCard = 0;
	m_uchSelfCardIdentify = 0;
	m_uchPlayer1CardIdentify = 0;
	m_uchPlayer2CardIdentify = 0;
	m_bRemoveAllCard  = FALSE;

	// Timer
	m_hSendUserListPacketResponseEvent = NULL;
	m_hSendUpdateSelfCardListPacketResponseEvent = NULL;


	m_btnHidePlayer2CardList.SetIcon(ID_ICON_UP, (int)BTNST_AUTO_GRAY );
	m_btnHidePlayer2CardList.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);
	m_bPlayer2CardListHide = FALSE;
	m_btnHideSelfCardList.SetIcon(ID_ICON_UP, (int)BTNST_AUTO_GRAY);
	m_btnHideSelfCardList.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);
	m_bSelfCardListHide = FALSE;
	m_btnHideOptions.SetIcon(ID_ICON_UP, (int)BTNST_AUTO_GRAY);
	m_btnHideOptions.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, 30);
	m_bOptionsHide = FALSE;

	// Set Server Location and Port Number
	memset(m_pchIP, 0x0, 16);
	sprintf_s( m_pchIP, 16, SERVERIP);
	m_usPort = SERVERPORT;

	// Update UI
	UpdateUIstatus(UI_STATUS_NORMAL);
}

void CWorldDlg::StartReceiveGamePacketThread()
{
	pcap_if_t *AllNICs   = NULL;
	char  chErrbuf[PCAP_ERRBUF_SIZE];
	m_hRecvGamePacketEvent = ::CreateEvent(NULL,TRUE,FALSE,NULL);
	m_bStopRecvGamePacketThread = FALSE;
	/* Retrieve the device list */
	if(pcap_findalldevs(&AllNICs, chErrbuf) == -1)
	{
		m_stClientMsg = MB2WCString(chErrbuf);
		//msg.Format(_T("Error in pcap_findalldevs: %s\n"), cstmp);
		SetClientMsg();
		return;
	}

	/* Create Thread to each NIC card */
	for(pcap_if_t *NIC = AllNICs; NIC ; NIC = NIC->next)
	{
		if (NIC->description)
		{
			m_pTmpNIC = NIC;
			m_vcReceiveGamePacketThread.push_back(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetGamePacket, this, 0, NULL));
		}
		WaitForSingleObject(m_hRecvGamePacketEvent, INFINITE);
		ResetEvent(m_hRecvGamePacketEvent);
	}
	pcap_freealldevs(AllNICs);
}

void CWorldDlg::StopReceiveGamePacketThread()
{
	// Stop Record Game Packet
	m_bStopRecvGamePacketThread = TRUE;
	for(std::vector<HANDLE>::iterator beg = m_vcReceiveGamePacketThread.begin();
		beg != m_vcReceiveGamePacketThread.end();
		++beg)
	{
		// Stop Game Packet
		if(*beg)
		{
			if(WaitForSingleObject(*beg,200) == WAIT_OBJECT_0)
			{
				TerminateThread(*beg, -1);
			}
			CloseHandle(*beg);
		}
	}
	m_vcReceiveGamePacketThread.clear();
}

void CWorldDlg::OnClose()
{

	int nRet = IDOK;
	if(m_Connection.m_soSocket != INVALID_SOCKET)
	{
		nRet = MessageBox(_T("確定直接關閉嗎?"), _T("警告"), MB_OKCANCEL );
	}
	
	if(nRet == IDOK)
	{
		StopReceiveGamePacketThread();

		// Disconnect Server
		Disconnect();

		// close Dlg
		CDialog::OnCancel(); 
	}

}

void CWorldDlg::OnBnClickedBtnConnectServer()
{
	m_stClientMsg = _T("連線到伺服器 ...");
	SetClientMsg();
	CString cstmp = _T("");
	// Get Server IP
	//m_pedServerLocation->GetWindowText(cstmp);
	UINT   unIP   = m_Connection.GetIPAddress( CString(m_pchIP) );
	
	// Get Server Port
    //cstmp = _T("");
    //m_pedServerPort->GetWindowText(cstmp);
	//USHORT usPort =  _ttoi(cstmp);

	// Get Channel ID
	
	m_unChannelID = m_pcbChannelID->GetCurSel();


	// validate data
	m_pedUserName->GetWindowText(cstmp);
	if(cstmp.IsEmpty() || !unIP || !m_usPort)
	{
		UpdateUIstatus(UI_STATUS_DISCONNECT_SERVER);
		MessageBox(_T("請輸入正確的資料"), _T("警告"));
		m_stClientMsg = _T("連線失敗");
	} else {
		// Prepare Socket...
		m_Connection.SetIPandPort( unIP, m_usPort);
		m_Connection.SetSocketAttribute(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		m_Connection.SetSocketOptions(TCP_NODELAY, 1);

		if(m_Connection.CreateSocket())
		{		
			if(m_Connection.TcpConnect())
			{
				m_bStopRecvServerPacketThread = FALSE;

				// Create Recv Server Packet thread
				if(m_hRecvServerPacketThread == NULL)
				{
					if( !(m_hRecvServerPacketThread  = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceivePacket, this, 0, NULL )) )
					{
						//m_stClientMsg= _T(" Create Send Packet Thread Error !");
						UpdateUIstatus(UI_STATUS_DISCONNECT_SERVER);
					} else {
						// Start register User name...
						SendRegisterUserPacket();
					}
				} else {
					//m_stClientMsg = _T("Send packet Thread has already create !");
				}
			} else {
				MessageBox(_T("伺服器沒有回應..."), _T("警告"));
				m_stClientMsg = _T("連線失敗");
			}
		} else {
			m_stClientMsg = m_Connection.GetLastErrorMsg();
			UpdateUIstatus(UI_STATUS_DISCONNECT_SERVER);
			m_stClientMsg = _T("連線失敗");
		}
	}
	SetClientMsg();
}


DWORD CWorldDlg::ReceivePacket(LPVOID pObject)
{
	CWorldDlg* pWDlg = static_cast<CWorldDlg*>(pObject);
	
	int  nSOCKADDRSIZE = sizeof(SOCKADDR);
	char chRecvPacket[RECEIVE_PACKET_SIZE] = "";
	int  nRecvPacketLen = 0;
	CString csMsg;

	while(!pWDlg->m_bStopRecvServerPacketThread)
	{
		// Initialize receive char array
		memset(chRecvPacket,0,sizeof(chRecvPacket));
		if(pWDlg->m_Connection.m_soSocket != INVALID_SOCKET)
		{
			// Receive Packet Error
			if( (nRecvPacketLen = recv(pWDlg->m_Connection.m_soSocket,
				                       chRecvPacket, RECEIVE_PACKET_SIZE,
				                       0)) == SOCKET_ERROR)
			{
				DWORD dwRet = WSAGetLastError();

				if(dwRet == WSAEWOULDBLOCK)
					Sleep(WSAEWOULDBLOCK_SLEEP_TIME);
				else {
					pWDlg->m_bStopRecvServerPacketThread = TRUE;
				}
			} else {
				pWDlg->ParseRecivePacket(chRecvPacket, nRecvPacketLen);
			}
		} else {
			pWDlg->m_bStopRecvServerPacketThread = TRUE;
		}
	}

	return 0;
}

void CWorldDlg::ParseRecivePacket(char *chReceivePacket, int nReceivePacketLength)
{
	world_header *pPacket = reinterpret_cast<world_header*>(chReceivePacket);

	if(pPacket->uhCommand == REGISTER_USERNAME_RESPONSE)
	{
		// First Get identify...
		CString csRemoteUserName = MB2WCString(reinterpret_cast<char *>(pPacket->Receiver.uhUserName));

		// Get UserName
		CString csLocalUserName = _T("");
		m_pedUserName->GetWindowText(csLocalUserName);

		if( (csLocalUserName == csRemoteUserName) && (pPacket->Receiver.unSessionID != 0) )
		{
			m_unSessionID = pPacket->Receiver.unSessionID;
			m_stClientMsg = _T("登入伺服器成功!");
			PostMessage(WM_UPDATE_CLIENT_MSG_MESSAGE);
			m_pstSelfName->SetWindowText(csLocalUserName);

			StartReceiveGamePacketThread();
			m_hSendUserListPacketResponseEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL);
			m_hSendUpdateSelfCardListPacketResponseEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL);

			// Start Auto Send Packet
			SendUserListPacket();
		}
	}  else if(pPacket->uhCommand == REGISTER_USERNAME_DENY_RESPONSE){
		MessageBox(_T("伺服器拒絕連線..."), _T("警告"));
		m_stClientMsg = _T("連線失敗");
		PostMessage(WM_UPDATE_CLIENT_MSG_MESSAGE);
		Disconnect();
		UpdateUIstatus(UI_STATUS_DISCONNECT_SERVER);

	}else if(pPacket->uhCommand == UPDATE_USER_LIST_RESPONSE){
		if(m_unSessionID == pPacket->Receiver.unSessionID)
		{
			m_unGroupID = pPacket->Receiver.unGroupID;
			m_unPlayer1SessionID = pPacket->Player1.unSessionID;
			m_unPlayer2SessionID = pPacket->Player2.unSessionID; 
			::SetEvent(m_hSendUserListPacketResponseEvent);
		}
	}  else if(pPacket->uhCommand == UPDATE_SELF_CARD_RESPONSE){
		if(m_unSessionID == pPacket->Receiver.unSessionID)
		{
			// ?
		}
	} else if(pPacket->uhCommand == GET_OTHER_USER_CARD_RESPONSE){
		if(m_unSessionID == pPacket->Receiver.unSessionID)
		{
			// ID Order maybe change
			// Update Player 1
			if(m_unPlayer1SessionID != 0)
			{
				if(m_unPlayer1SessionID == pPacket->Player1.unSessionID)
				{
					if(m_unPlayer1Card == 0 && pPacket->Player1.unCard != 0 )
					{
						m_unPlayer1NewCard = pPacket->Player1.unCard;
					}	
				}
				if(m_unPlayer1SessionID == pPacket->Player2.unSessionID)
				{
					if(m_unPlayer1Card == 0 && pPacket->Player2.unCard != 0 )
					{
						m_unPlayer1NewCard = pPacket->Player2.unCard;
					}
				}
			}

			// Update Player 2 
			if(m_unPlayer2SessionID != 0)
			{
				if(m_unPlayer2SessionID == pPacket->Player2.unSessionID)
				{
					if(pPacket->Player2.unCard != 0 && m_unPlayer2Card == 0)
					{
						m_unPlayer2NewCard = pPacket->Player2.unCard;
					}
				}

				if(m_unPlayer2SessionID == pPacket->Player1.unSessionID)
				{
					if(m_unPlayer2Card == 0 && pPacket->Player1.unCard != 0 )
					{
						m_unPlayer2NewCard = pPacket->Player1.unCard;
					}	
				}
			}

			if(m_unPlayer2NewCard != 0 || m_unPlayer1NewCard != 0)
				::SetEvent(m_hSendUpdateSelfCardListPacketResponseEvent);
		}
	} else if(pPacket->uhCommand == SERVER_OFFLINE){
		m_stClientMsg = _T("伺服器離線....");
		PostMessage(WM_UPDATE_CLIENT_MSG_MESSAGE);
		Disconnect();
		UpdateUIstatus(UI_STATUS_DISCONNECT_SERVER);
	}

	
}

void CWorldDlg::UpdateUIstatus(USHORT usStatus)
{
	if(usStatus == UI_STATUS_NORMAL)
	{
		m_pedUserName->EnableWindow(TRUE);
		m_pcbChannelID->EnableWindow(TRUE);
		m_pbnConnect->EnableWindow(TRUE);
		m_pbnDisconnect->EnableWindow(FALSE);
	} else if(usStatus == UI_STATUS_CONNECT_SERVER)
	{
		m_pedUserName->EnableWindow(FALSE);
		m_pcbChannelID->EnableWindow(FALSE);
		m_pbnConnect->EnableWindow(FALSE);
		m_pbnDisconnect->EnableWindow(TRUE);
	} else if(usStatus == UI_STATUS_DISCONNECT_SERVER)
	{
		m_pstMsg->EnableWindow(TRUE);
		m_pedUserName->EnableWindow(TRUE);
		m_pcbChannelID->EnableWindow(TRUE);
		m_pbnConnect->EnableWindow(TRUE);
		m_pbnDisconnect->EnableWindow(FALSE);
	}
}

void CWorldDlg::Disconnect()
{
	SendLeaveServerPacket();
	// Close Socket
	m_Connection.CloseSocket();

	// Reset Data
	m_unSessionID        = 0;
	m_unGroupID          = 0;
	m_unChannelID        = 0;
	m_unPlayer1SessionID = 0;
	m_unPlayer2SessionID = 0;

	// Stop Auto Send
	m_bStopRecvServerPacketThread = TRUE;

	// Stop Receiver Server Packet thread
	if(m_hRecvServerPacketThread)
	{
		if(WaitForSingleObject(m_hRecvServerPacketThread,200) == WAIT_OBJECT_0)
			TerminateThread(m_hRecvServerPacketThread, -1);
		CloseHandle(m_hRecvServerPacketThread); 
		m_hRecvServerPacketThread  =   NULL;
	}

	if(m_hSendUserListPacketResponseEvent != NULL)
	{
		CloseHandle(m_hSendUserListPacketResponseEvent);
		m_hSendUserListPacketResponseEvent = NULL;
	}
	if(m_hSendUpdateSelfCardListPacketResponseEvent != NULL)
	{
		CloseHandle(m_hSendUpdateSelfCardListPacketResponseEvent);
		m_hSendUpdateSelfCardListPacketResponseEvent = NULL;
	}

	UpdateUIstatus(UI_STATUS_DISCONNECT_SERVER);
}
void CWorldDlg::OnBnClickedBtnDisconnectServer()
{
	Disconnect();
	m_stClientMsg = _T("已從伺服器登出...");
	SetClientMsg();
}

void CWorldDlg::SendLeaveServerPacket()
{
	world_header header;

	header.uhCommand = CLIENT_OFFLINE;
	header.Receiver.unSessionID = m_unSessionID;
	header.Receiver.unGroupID = m_unGroupID;
	if(m_unSessionID && m_Connection.m_soSocket != INVALID_SOCKET)
	{
		if( send(m_Connection.m_soSocket, (char *)&header, sizeof(header), 0) == SOCKET_ERROR)
		{
			//
		}
	}
}

void CWorldDlg::SendRegisterUserPacket()
{
	world_header header;
	header.uhCommand = REGISTER_USERNAME;
	header.Receiver.unChannelID = m_unChannelID;

	CString cstmp = _T("");
	m_pedUserName->GetWindowText(cstmp);

	if(!cstmp.IsEmpty())
	{
		W2MBChar(cstmp, (char *)header.Receiver.uhUserName, PACKET_USERNAME_CHARSIZE);

		if( send(m_Connection.m_soSocket, (char *)&header, sizeof(header), 0) == SOCKET_ERROR)
		{
			
		} else {
			UpdateUIstatus(UI_STATUS_CONNECT_SERVER);
			return;
		}
	} else {
		MessageBox(_T("帳號不正確"), _T("警告"));
	}
	UpdateUIstatus(UI_STATUS_DISCONNECT_SERVER);
}

void CWorldDlg::SendUserListPacket()
{
	if(m_unSessionID)
	{
		world_header header;

		if(!m_stPlayer1Name.IsEmpty())
			W2MBChar(m_stPlayer1Name, (char *)header.Player1.uhUserName, PACKET_USERNAME_CHARSIZE);

		if(!m_stPlayer2Name.IsEmpty())
			W2MBChar(m_stPlayer2Name, (char *)header.Player2.uhUserName, PACKET_USERNAME_CHARSIZE);

		header.uhCommand = UPDATE_USER_LIST;
		header.Receiver.unSessionID = m_unSessionID;
		header.Receiver.unChannelID = m_unChannelID;
		header.Receiver.unGroupID   = m_unGroupID;

		if( (m_unPlayer1SessionID == 0) || (m_unPlayer2SessionID == 0) )
			if( send(m_Connection.m_soSocket, (char *)&header, sizeof(header), 0) == SOCKET_ERROR)
			{
				
			}
	}
}

void CWorldDlg::SendUpdateSelfCardListPacket()
{
	if(m_unSessionID && m_unGroupID && m_Connection.m_soSocket != INVALID_SOCKET)
	{
		world_header header;
		header.uhCommand = UPDATE_SELF_CARD;
		header.Receiver.unSessionID = m_unSessionID;
		header.Receiver.unGroupID   = m_unGroupID;
		header.Receiver.unChannelID = m_unChannelID;
		header.Receiver.unCard      = m_unSelfNewCard;
		if(!m_stPlayer1Name.IsEmpty())
			W2MBChar(m_stPlayer1Name, (char *)header.Player1.uhUserName, PACKET_USERNAME_CHARSIZE);

		if(!m_stPlayer2Name.IsEmpty())
			W2MBChar(m_stPlayer2Name, (char *)header.Player2.uhUserName, PACKET_USERNAME_CHARSIZE);


		if( send(m_Connection.m_soSocket, (char *)&header, sizeof(header), 0) == SOCKET_ERROR)
		{
			
		}
	}
}


DWORD CWorldDlg::GetGamePacket(LPVOID pObject)
{
	CWorldDlg* pWDlg = static_cast<CWorldDlg*>(pObject);
	// Get Select NIC Card
	pcap_if_t *SelectNIC = pWDlg->m_pTmpNIC;

	// Get Select NIC IP, and Set filter
	//u_int unIP = ((struct sockaddr_in *)(SelectNIC->addresses->addr ))->sin_addr.S_un.S_addr;
	//unIP  = ntohl(unIP);
	//char packet_filter[46] = "";
	//sprintf_s( packet_filter,
	//	       46,
	//		   "tcp src port 10024 and ip dst %u.%u.%u.%u",
	//		   unIP>> 24, (unIP>>  16) %256, (unIP >>  8)  %256, unIP  %256);


	char packet_filter[] = PACKET_FILTER;

	/* Open the adapter */
	pcap_t *adhandle;
	char  chErrbuf[PCAP_ERRBUF_SIZE];
	if ((adhandle= pcap_open_live( SelectNIC->name,	// name of the device
		65536,			// portion of the packet to capture. 
		                // 65536 grants that the whole packet will be captured on all the MACs.
		1,				// promiscuous mode (nonzero means promiscuous)
		1000,			// read timeout
		chErrbuf			// error buffer
		)) == NULL)
	{
		return 0;
	}

	u_int netmask;
	if(SelectNIC->addresses != NULL)
		/* Retrieve the mask of the first address of the interface */
		netmask=((struct sockaddr_in *)(SelectNIC->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		/* If the interface is without addresses we suppose to be in a C class network */
		netmask=0xffffff; 

	if(pWDlg->m_hRecvGamePacketEvent != NULL)
		::SetEvent(pWDlg->m_hRecvGamePacketEvent);

	/* Check the link layer. We support only Ethernet for simplicity. */
	if(pcap_datalink(adhandle) != DLT_EN10MB)
	{
		return 0;
	}

	// compile the filter
	struct bpf_program fcode;
	if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) <0 )
		return 0;

	// set the filter
	if (pcap_setfilter(adhandle, &fcode)<0)
		return 0;

	// Start capture Packet
	int res = 0;
	const u_char *pkt_data;
	struct pcap_pkthdr *header;
	while( (res = pcap_next_ex( adhandle, &header, &pkt_data)) >= 0 && 
			!pWDlg->m_bStopRecvGamePacketThread)
	{
		if(res == 0)
			continue;
		pWDlg->Receive_Game_Packet_handler(pkt_data);
	}

	if(res == -1){
		return 0;
	}

	return 0;
}


void CWorldDlg::Receive_Game_Packet_handler(const u_char *pkt_data)
{
	ip_header  *iph  = NULL;
	u_int ip_len = 0;

	char *pchData = NULL;

	/* retireve the position of the ip header */
	iph = (ip_header *) (pkt_data + ETHERNET_HEARDER_SIZE /*length of ethernet header */); 

	int nTcpDataSize = ntohs(iph->tlen) - IP_HEADER_SIZE - TCP_HEADER_SIZE;;

	pchData = (char *)((unsigned char*)pkt_data + ETHERNET_HEARDER_SIZE + IP_HEADER_SIZE + TCP_HEADER_SIZE );

	CardPacketParser tmpParser(pchData, nTcpDataSize);
	if(tmpParser.m_PacketType  == PACKET_TYPE_START_DEAL)
	{
		m_bRemoveAllCard = TRUE;
		m_uchSelfCardIdentify    = 0;
		m_uchPlayer1CardIdentify = 0;
		m_uchPlayer2CardIdentify = 0;
		m_unSelfCard    = 0;
		m_unPlayer1Card = 0;
		m_unPlayer2Card = 0;
		m_unSelfNewCard    = 0;
		m_unPlayer1NewCard = 0;
		m_unPlayer2NewCard = 0;
		m_stPlayer1Name = _T("");
		m_stPlayer2Name = _T("");

		// Update Name
		char pchTmpName[64] = "";
		std::vector<CString> vcUserName;
		tmpParser.GetUserNameList(vcUserName);

		CString stSelfName = _T("");
		m_pedUserName->GetWindowText(stSelfName);
		int nSelfIndex = 0;
		for(std::vector<CString>::iterator beg = vcUserName.begin();
			beg != vcUserName.end(); ++beg)
		{	
			if(stSelfName == *beg)
			{
				m_pstSelfName->SetWindowText(stSelfName);
				break;
			}
			++nSelfIndex;
		}

		if(nSelfIndex == 0)
		{
			m_uchSelfCardIdentify = 49;
			m_uchPlayer1CardIdentify = 50;
			m_uchPlayer2CardIdentify = 51;
			m_stPlayer1Name = vcUserName[1];
			m_stPlayer2Name = vcUserName[2];
		} else if( nSelfIndex == 1)
		{
			m_uchSelfCardIdentify = 50;
			m_uchPlayer1CardIdentify = 49;
			m_uchPlayer2CardIdentify = 51;
			m_stPlayer1Name = vcUserName[0];
			m_stPlayer2Name = vcUserName[2];
		} else if( nSelfIndex == 2)
		{
			m_uchSelfCardIdentify = 51;
			m_uchPlayer1CardIdentify = 49;
			m_uchPlayer2CardIdentify = 50;
			m_stPlayer1Name = vcUserName[0];
			m_stPlayer2Name = vcUserName[1];
		}
		
		// Reset Card
		DWORD dwResult = 0;
		if(nSelfIndex < 3)
		{
			m_unSelfNewCard = tmpParser.GetCard();
			// Get User List
			SendUserListPacket();
			dwResult = WaitForSingleObject(m_hSendUserListPacketResponseEvent, 500);

			// Get User Card
			SendUpdateSelfCardListPacket();
			dwResult = WaitForSingleObject(m_hSendUpdateSelfCardListPacketResponseEvent,1000);

		}

		if(m_unPlayer1NewCard == 0 && m_unPlayer1NewCard == 0)
		{
			// Re Get User Card
			SendUpdateSelfCardListPacket();
			dwResult = WaitForSingleObject(m_hSendUpdateSelfCardListPacketResponseEvent,2000);
		}

		if(m_unPlayer1NewCard == 0 && m_unPlayer2NewCard != 0)
		{
			m_unPlayer1NewCard = ~( m_unSelfNewCard | m_unPlayer2NewCard);
			m_pstPlayer1Name->SetWindowText(m_stPlayer1Name);
			m_pstPlayer2Name->SetWindowText(m_stPlayer2Name);
		}
		if(m_unPlayer2NewCard == 0 && m_unPlayer1NewCard != 0)
		{
			m_unPlayer2NewCard = ~( m_unSelfNewCard | m_unPlayer1NewCard);
			m_pstPlayer1Name->SetWindowText(m_stPlayer1Name);
			m_pstPlayer2Name->SetWindowText(m_stPlayer2Name);
		}
		if(m_unPlayer1NewCard == 0 && m_unPlayer1NewCard == 0)
		{
			if(m_unSelfNewCard != 0)
			{
				m_unPlayer1NewCard = ~ m_unSelfNewCard;
			}

			m_pstPlayer1Name->SetWindowText(m_stPlayer1Name + _T(" -- ") + m_stPlayer2Name);
			m_pstPlayer2Name->SetWindowText(_T(" -- "));
		}

		
	} else if(tmpParser.m_PacketType == PACKET_TYPE_DEAL) 
	{
		//
		UCHAR unWhosCard = tmpParser.WhosCard();
		UINT64  tmpCard  = tmpParser.GetCard();
		if( unWhosCard == m_uchSelfCardIdentify )
		{
			if(m_unSelfCard != 0)
			{
				m_unSelfNewCard = tmpCard;
			}
		} else if( unWhosCard == m_uchPlayer1CardIdentify)
		{
			if(m_unPlayer1Card != 0)
			{
				m_unPlayer1NewCard = tmpCard;
			}
		} else if( unWhosCard == m_uchPlayer2CardIdentify )
		{
			if(m_unPlayer2Card != 0)
			{
				m_unPlayer2NewCard = tmpCard;
				
			} else {
				if(m_unPlayer1Card != 0)
				{
					m_unPlayer1NewCard = tmpCard;
				}
			}
		}
	}
	PostMessage(WM_UPDATE_CARD_LIST_MESSAGE);
}


void CWorldDlg::SetClientMsg()
{
	m_pstMsgShow->SetWindowText(m_stClientMsg);
}
void CWorldDlg::UpdateCardList()
{
	// Reset All Card
	if(m_bRemoveAllCard)
	{
		m_cpSelfControl.RemoveAllCard();
		m_cpPlayer1Control.RemoveAllCard();
		m_cpPlayer2Control.RemoveAllCard();
		m_bRemoveAllCard = FALSE;
	}

	UINT64 bitOn      = 0x01;

	// Update SelfCard
	if(m_unSelfNewCard != 0)
	{	
		if(m_unSelfCard == 0){
			// Crete new Card
			for(int i=CARD_BEGIN ;i<=CARD_END;++i)
			{
				if(m_unSelfNewCard & (bitOn << i) )
				{
					m_cpSelfControl.CreatCard(i);
				}
			}
			m_unSelfCard = m_unSelfNewCard;
			SortingCardList();
		} else {
			for(int i=CARD_BEGIN ;i<=CARD_END;++i)
			{
				if(m_unSelfNewCard & (bitOn << i) )
				{
					m_cpSelfControl.RemoveCard(i);
				}
			}
			m_unSelfCard -= m_unSelfNewCard;
		}	
		m_unSelfNewCard = 0;
	}

	// Update player 1 Card
	if(m_unPlayer1NewCard != 0)
	{
		if(m_unPlayer1Card == 0){
			for(int i=CARD_BEGIN ;i<=CARD_END;++i)
			{
				if(m_unPlayer1NewCard & (bitOn << i) )
				{
					m_cpPlayer1Control.CreatCard(i);
				}
			}
			m_unPlayer1Card = m_unPlayer1NewCard;
			SortingCardList();
		} else {
			for(int i=CARD_BEGIN ;i<=CARD_END;++i)
			{
				if(m_unPlayer1NewCard & (bitOn << i) )
				{
					m_cpPlayer1Control.RemoveCard(i);
				}
			}
			m_unPlayer1Card -= m_unPlayer1NewCard;
		}
		m_unPlayer1NewCard = 0;
	}


	// Update player 2 Card
	if(m_unPlayer2NewCard != 0)
	{
		if(m_unPlayer2Card == 0){
			for(int i=CARD_BEGIN ;i<=CARD_END;++i)
			{
				if(m_unPlayer2NewCard & (bitOn << i) )
				{
					m_cpPlayer2Control.CreatCard(i);
				}
			}
			m_unPlayer2Card = m_unPlayer2NewCard;
			SortingCardList();
		} else {
			for(int i=CARD_BEGIN ;i<=CARD_END;++i)
			{
				if(m_unPlayer2NewCard & (bitOn << i) )
				{
					m_cpPlayer2Control.RemoveCard(i);
				}
			}
		    m_unPlayer2Card -= m_unPlayer2NewCard;
		}
		m_unPlayer2NewCard = 0;
	}
}

CString CWorldDlg::MB2WCString(char *chStr)
{	
	int nNum  = MultiByteToWideChar(CP_ACP, NULL, (LPCSTR)chStr, -1, NULL, 0);
	TCHAR *ptchStr = new TCHAR[nNum];
	if(ptchStr)
		MultiByteToWideChar(CP_ACP, NULL, chStr, -1, ptchStr, nNum);

	CString csStr(ptchStr);
	if(ptchStr)
		delete [] ptchStr;

	return csStr;
}

void CWorldDlg::W2MBChar(CString csStr, char *pchStr, int nchLength)
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

void CWorldDlg::OnBnClickedBtnHidePlaye2Cardlist()
{
	CRect DialogRect;
	GetWindowRect(&DialogRect);
	if(m_bPlayer2CardListHide == FALSE)
	{
		m_btnHidePlayer2CardList.SetIcon(ID_ICON_DOWN, (int)BTNST_AUTO_GRAY);
		m_bPlayer2CardListHide = TRUE;
		m_pstPlayer2Name->ShowWindow(SW_HIDE);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR5)))->ShowWindow(SW_HIDE);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR6)))->ShowWindow(SW_HIDE);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR7)))->ShowWindow(SW_HIDE);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR8)))->ShowWindow(SW_HIDE);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR9)))->ShowWindow(SW_HIDE);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR10)))->ShowWindow(SW_HIDE);
		SetWindowPos(NULL, 0, 0, DialogRect.Width() ,DialogRect.Height() - 50, SWP_NOZORDER | SWP_NOMOVE);
		
	} else {
		m_btnHidePlayer2CardList.SetIcon(ID_ICON_UP, (int)BTNST_AUTO_GRAY);
		m_bPlayer2CardListHide = FALSE;
		m_pstPlayer2Name->ShowWindow(SW_SHOW);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR5)))->ShowWindow(SW_SHOW);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR6)))->ShowWindow(SW_SHOW);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR7)))->ShowWindow(SW_SHOW);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR8)))->ShowWindow(SW_SHOW);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR9)))->ShowWindow(SW_SHOW);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR10)))->ShowWindow(SW_SHOW);
		SetWindowPos(NULL, 0, 0, DialogRect.Width() , DialogRect.Height() + 50, SWP_NOZORDER | SWP_NOMOVE);
	}
	m_cpPlayer2Control.HideCardList(m_bPlayer2CardListHide);
	MoveSlefCardListUI();
	MoveOptonsUI();
}

void CWorldDlg::OnBnClickedBtnHideSelfCardlist()
{
	CRect DialogRect;
	GetWindowRect(&DialogRect);
	if(m_bSelfCardListHide == FALSE)
	{
		m_btnHideSelfCardList.SetIcon(ID_ICON_DOWN, (int)BTNST_AUTO_GRAY);
		m_bSelfCardListHide = TRUE;
		m_pstSelfName->ShowWindow(SW_HIDE);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR12)))->ShowWindow(SW_HIDE);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR13)))->ShowWindow(SW_HIDE);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR14)))->ShowWindow(SW_HIDE);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR15)))->ShowWindow(SW_HIDE);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR16)))->ShowWindow(SW_HIDE);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR17)))->ShowWindow(SW_HIDE);

		SetWindowPos(NULL, 0, 0, DialogRect.Width() , DialogRect.Height() - 50, SWP_NOZORDER | SWP_NOMOVE);

	} else {
		m_btnHideSelfCardList.SetIcon(ID_ICON_UP, (int)BTNST_AUTO_GRAY);
		m_bSelfCardListHide = FALSE;
		m_pstSelfName->ShowWindow(SW_SHOW);	
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR12)))->ShowWindow(SW_SHOW);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR13)))->ShowWindow(SW_SHOW);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR14)))->ShowWindow(SW_SHOW);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR15)))->ShowWindow(SW_SHOW);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR16)))->ShowWindow(SW_SHOW);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR17)))->ShowWindow(SW_SHOW);
		SetWindowPos(NULL, 0, 0, DialogRect.Width() , DialogRect.Height() + 50, SWP_NOZORDER | SWP_NOMOVE);
	}
	m_cpSelfControl.HideCardList(m_bSelfCardListHide);
	MoveSlefCardListUI();
	MoveOptonsUI();
}

void CWorldDlg::OnBnClickedBtnHideOptions()
{
	CRect DialogRect;
	GetWindowRect(&DialogRect);
	if(m_bOptionsHide == FALSE)
	{
		m_btnHideOptions.SetIcon(ID_ICON_DOWN, (int)BTNST_AUTO_GRAY);
		m_bOptionsHide = TRUE;
		m_pstUserName->ShowWindow(SW_HIDE);
		m_pedUserName->ShowWindow(SW_HIDE);
		m_pstChannelID->ShowWindow(SW_HIDE);
		m_pcbChannelID->ShowWindow(SW_HIDE);
		m_pbnConnect->ShowWindow(SW_HIDE);
		m_pbnDisconnect->ShowWindow(SW_HIDE);
		m_pstLine4->ShowWindow(SW_HIDE);
		m_pstMsg->ShowWindow(SW_HIDE);
		m_pstMsgShow->ShowWindow(SW_HIDE);
		SetWindowPos(NULL, 0, 0, DialogRect.Width() ,DialogRect.Height() - 60, SWP_NOZORDER | SWP_NOMOVE);
	} else {
		m_btnHideOptions.SetIcon(ID_ICON_UP, (int)BTNST_AUTO_GRAY);
		m_bOptionsHide = FALSE;
		m_pstUserName->ShowWindow(SW_SHOW);
		m_pedUserName->ShowWindow(SW_SHOW);
		m_pstChannelID->ShowWindow(SW_SHOW);
		m_pcbChannelID->ShowWindow(SW_SHOW);
		m_pbnConnect->ShowWindow(SW_SHOW);
		m_pbnDisconnect->ShowWindow(SW_SHOW);
		m_pstLine4->ShowWindow(SW_SHOW);
		m_pstMsg->ShowWindow(SW_SHOW);
		m_pstMsgShow->ShowWindow(SW_SHOW);
		SetWindowPos(NULL, 0, 0, DialogRect.Width() ,DialogRect.Height() + 60, SWP_NOZORDER | SWP_NOMOVE);
	}
	MoveOptonsUI();
}
void CWorldDlg::MoveSlefCardListUI()
{
	int base = 74;
	if(m_bPlayer2CardListHide)
	{
		base -= 30;
	}
	m_btnHideSelfCardList.SetWindowPos(NULL, static_cast<INT>(477*m_dWx), static_cast<INT>(base*m_dWy), 0, 0,SWP_NOZORDER | SWP_NOSIZE);
	base += 2;
	m_pstLine2->SetWindowPos(NULL,  static_cast<INT>(7*m_dWx), static_cast<INT>(base*m_dWy), 0, 0,SWP_NOZORDER | SWP_NOSIZE);
	if(!m_bSelfCardListHide)
	{
		base += 2;
		m_pstSelfName->SetWindowPos(NULL,  static_cast<INT>(7*m_dWx), static_cast<INT>(base*m_dWy), 0, 0,SWP_NOZORDER | SWP_NOSIZE);

		// Anchor
		base += 2;
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR12)))->SetWindowPos(NULL,  static_cast<INT>(63*m_dWx), static_cast<INT>(base*m_dWy), 0, 0,SWP_NOZORDER | SWP_NOSIZE);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR13)))->SetWindowPos(NULL,  static_cast<INT>(130*m_dWx), static_cast<INT>(base*m_dWy), 0, 0,SWP_NOZORDER | SWP_NOSIZE);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR14)))->SetWindowPos(NULL,  static_cast<INT>(197*m_dWx), static_cast<INT>(base*m_dWy), 0, 0,SWP_NOZORDER | SWP_NOSIZE);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR15)))->SetWindowPos(NULL,  static_cast<INT>(264*m_dWx), static_cast<INT>(base*m_dWy), 0, 0,SWP_NOZORDER | SWP_NOSIZE);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR16)))->SetWindowPos(NULL,  static_cast<INT>(331*m_dWx), static_cast<INT>(base*m_dWy), 0, 0,SWP_NOZORDER | SWP_NOSIZE);
		(static_cast<CStatic*>(GetDlgItem(IDC_STA_ANCHOR17)))->SetWindowPos(NULL,  static_cast<INT>(398*m_dWx), static_cast<INT>(base*m_dWy), 0, 0,SWP_NOZORDER | SWP_NOSIZE);
		

		base += 15;
		m_cpSelfControl.SetPos(10, static_cast<INT>(base*m_dWx));
		m_cpSelfControl.m_UpdateCardPos = TRUE;
		m_cpSelfControl.UpdateCardPos();
	}
}

void CWorldDlg::MoveOptonsUI()
{
	int base = 110;
	if(m_bPlayer2CardListHide)
	{
		base -= 30;
	}
	if(m_bSelfCardListHide)
	{
		base -= 30;
	}

	m_btnHideOptions.SetWindowPos(NULL, static_cast<INT>(477*m_dWx), static_cast<INT>(base*m_dWy), 0, 0,SWP_NOZORDER | SWP_NOSIZE);
	base+= 2;
	m_pstLine3->SetWindowPos(NULL,  static_cast<INT>(7*m_dWx), static_cast<INT>(base*m_dWy), 0, 0,SWP_NOZORDER | SWP_NOSIZE);

	if(!m_bOptionsHide)
	{
		base += 5;
		m_pedUserName->SetWindowPos(NULL, static_cast<INT>(57*m_dWx), static_cast<INT>(base*m_dWy), 0, 0,SWP_NOZORDER | SWP_NOSIZE);
		m_pcbChannelID->SetWindowPos(NULL, static_cast<INT>(218*m_dWx), static_cast<INT>(base*m_dWy), 0, 0,SWP_NOZORDER | SWP_NOSIZE);
		m_pbnConnect->SetWindowPos(NULL, static_cast<INT>(332*m_dWx), static_cast<INT>(base*m_dWy), 0, 0,SWP_NOZORDER | SWP_NOSIZE);
		m_pbnDisconnect->SetWindowPos(NULL, static_cast<INT>(405*m_dWx), static_cast<INT>(base*m_dWy), 0, 0,SWP_NOZORDER | SWP_NOSIZE);
		base += 2;
		m_pstUserName->SetWindowPos(NULL, static_cast<INT>(9*m_dWx), static_cast<INT>(base*m_dWy), 0, 0,SWP_NOZORDER | SWP_NOSIZE);
		m_pstChannelID->SetWindowPos(NULL, static_cast<INT>(172*m_dWx), static_cast<INT>(base*m_dWy), 0, 0,SWP_NOZORDER | SWP_NOSIZE);
		base += 16;
		m_pstLine4->SetWindowPos(NULL, static_cast<INT>(7*m_dWx), static_cast<INT>(base*m_dWy), 0, 0,SWP_NOZORDER | SWP_NOSIZE);
		base += 3;
		m_pstMsg->SetWindowPos(NULL, static_cast<INT>(9*m_dWx), static_cast<INT>(base*m_dWy), 0, 0,SWP_NOZORDER | SWP_NOSIZE);
		m_pstMsgShow->SetWindowPos(NULL, static_cast<INT>(59*m_dWx), static_cast<INT>(base*m_dWy), 0, 0,SWP_NOZORDER | SWP_NOSIZE);
	}
}
void CWorldDlg::OnCancel()
{
}

void CWorldDlg::OnBnClickedBtnSortBySuit()
{
	SortingCardList();
}

void CWorldDlg::OnBnClickedBtnSortByNumber()
{
	SortingCardList();
}

void CWorldDlg::SortingCardList()
{
	if(m_pbnSortByNumber->GetCheck())
	{
		m_pbnSortBySuit->SetCheck(BST_UNCHECKED);
		m_cpSelfControl.SortBy(SORT_BY_NUMBER);
		m_cpPlayer1Control.SortBy(SORT_BY_NUMBER);
		m_cpPlayer2Control.SortBy(SORT_BY_NUMBER);
	} else {
		m_pbnSortByNumber->SetCheck(BST_UNCHECKED);
		m_cpSelfControl.SortBy(SORT_BY_SUIT);
		m_cpPlayer1Control.SortBy(SORT_BY_SUIT);
		m_cpPlayer2Control.SortBy(SORT_BY_SUIT);
	}
}


LRESULT CWorldDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_UPDATE_CARD_LIST_MESSAGE:
		UpdateCardList();
		break;
	case WM_UPDATE_CLIENT_MSG_MESSAGE:
		SetClientMsg();
		break;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}
