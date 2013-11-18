// WorldDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "World.h"
#include "pcap.h"
#include "Packet_header.h"
#include "CardPlayer.h"
#include "vector"
#include <time.h>
#include <stdlib.h>
#include "GenericConnection.h"
#include "WorldPacketDefine.h"
#include "CardPacketParser.h"
#include "BtnST.h"

//
//#define SERVERIP  "123.195.7.117"
#define SERVERIP  "192.168.3.101"
#define SERVERPORT 52380
//#define PACKET_FILTER "tcp src port 10024"
//#define PACKET_FILTER  "tcp src port 10024 and ip dst 192.168.1.102";
#define PACKET_FILTER  "tcp src port 10024 and ip dst 192.168.1.116";

#define WM_UPDATE_CARD_LIST_MESSAGE 0x123
#define WM_UPDATE_CLIENT_MSG_MESSAGE 0x1234
#define UI_STATUS_NORMAL 1
#define UI_STATUS_CONNECT_SERVER 2
#define UI_STATUS_DISCONNECT_SERVER  3

// Update CardList Timer
#define AUTO_UPDATE_CARDLIST_ID   0xFA
#define AUTO_UPDATE_CARDLIST_TIME   1000


class CWorldDlg : public CDialog
{
// Construction
public:
	CWorldDlg(CWnd* pParent = NULL);	// standard constructor
	~CWorldDlg();	// standard constructor
// Dialog Data
	enum { IDD = IDD_WORLD_DIALOG };
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnConnectServer();
	afx_msg void OnBnClickedStaGroupSelfCardlist3();
	afx_msg void OnBnClickedBtnDisconnectServer();
	afx_msg void OnBnClickedBtnHidePlaye2Cardlist();
	afx_msg void OnBnClickedBtnHideSelfCardlist();
	afx_msg void OnBnClickedBtnHideOptions();
	afx_msg void OnBnClickedBtnSortBySuit();
	afx_msg void OnBnClickedBtnSortByNumber();


// Implementation
protected:
	HICON m_hIcon;
	DECLARE_MESSAGE_MAP()
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnCancel();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();
	
	double m_dWx;
	double m_dWy;
	char     m_pchIP[16];
	USHORT   m_usPort;
	// UI control
	CEdit*   m_pedUserName;
	CComboBox *m_pcbChannelID;
	CStatic* m_pstUserName;
	CStatic* m_pstChannelID;
	CStatic* m_pstMsg;
	CStatic* m_pstMsgShow;
	CStatic* m_pstSelfName;
	CStatic* m_pstPlayer1Name;
	CStatic* m_pstPlayer2Name;
	CStatic* m_pstLine1;
	CStatic* m_pstLine2;
	CStatic* m_pstLine3;
	CStatic* m_pstLine4;
	CButton* m_pbnConnect;
	CButton* m_pbnDisconnect;
	CButton* m_pbnSortByNumber;
	CButton* m_pbnSortBySuit;
	CardPlayer m_cpSelfControl;
	CardPlayer m_cpPlayer1Control;
	CardPlayer m_cpPlayer2Control;

	// Data
	UINT      m_unSessionID;
	UINT      m_unGroupID;
	UINT      m_unChannelID;
	UINT      m_unPlayer1SessionID;
	UINT      m_unPlayer2SessionID;
	CString   m_stSelfName;
	CString   m_stPlayer1Name;
	CString   m_stPlayer2Name;
	UINT64    m_unSelfCard;
	UINT64    m_unSelfNewCard;
	UINT64    m_unPlayer1Card;
	UINT64    m_unPlayer1NewCard;
	UINT64    m_unPlayer2Card;
	UINT64    m_unPlayer2NewCard; 
	CButtonST m_btnHidePlayer2CardList;
	CButtonST m_btnHideSelfCardList;
	CButtonST m_btnHideOptions;
	BOOL      m_bPlayer2CardListHide;
	BOOL      m_bSelfCardListHide;
	BOOL      m_bOptionsHide;
	GenericConnection m_Connection;
	BOOL      m_bRemoveAllCard;



	// Receiver Server Packet
	void ParseRecivePacket(char *chReceivePacket, int nReceivePacketLength);
	static DWORD ReceivePacket(LPVOID pObject);
	HANDLE m_hRecvServerPacketThread;
	BOOL   m_bStopRecvServerPacketThread;

	// Record Game Packet
	pcap_if_t *m_pTmpNIC;
	void StartReceiveGamePacketThread();
	void StopReceiveGamePacketThread();
	static DWORD GetGamePacket(LPVOID pObject);
	HANDLE m_hRecvGamePacketEvent;
	std::vector<HANDLE> m_vcReceiveGamePacketThread;
	BOOL   m_bStopRecvGamePacketThread;
	void Receive_Game_Packet_handler(const u_char *pkt_data);


	// Update Card List
	void UpdateCardList();

	// UI Remove
	void MoveSlefCardListUI();
	void MoveOptonsUI();
	// Other
	void UpdateUIstatus(USHORT usStatus);
	void Disconnect();
	void initialize();
	
public:
	UCHAR m_uchSelfCardIdentify;
	UCHAR m_uchPlayer1CardIdentify;
	UCHAR m_uchPlayer2CardIdentify;
	CString m_stClientMsg;
	CString MB2WCString(char *chStr);
	void W2MBChar(CString csStr, char *chStr, int nchLength);
	void SetClientMsg();
	void SortingCardList();
	void SendLeaveServerPacket();
	void SendRegisterUserPacket();
	void SendUserListPacket();
	HANDLE m_hSendUserListPacketResponseEvent;
	void SendUpdateSelfCardListPacket();
	HANDLE m_hSendUpdateSelfCardListPacketResponseEvent;

protected:
	
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


