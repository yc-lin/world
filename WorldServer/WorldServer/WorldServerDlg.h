// WorldServerDlg.h : header file
//

#pragma once
#include <Winsock2.h>
#include <IPHlpApi.h>
#include "WorldServer.h"
#include "GenericConnection.h"
#include <set>
#include <vector>
#include <queue>
#include <map>
#include <algorithm>
#include "WorldPacketDefine.h"
#include <utility>
#include "IniFile.h"

#define DEFAULT_SERVER_PORT 52380
#define SERVER_ACCEPT_CONNECTION_NUMBER 256

#define UI_STATUS_NORMAL 1
#define UI_STATUS_START_SERVER 2
#define UI_STATUS_STOP_SERVER  3

// Accpet connection List Operation
#define ADD_CONNECTION        0
#define REMOVE_CONNECTION     1
#define GET_CONNECTION_LIST   2
#define GET_STOPRECEIVEPACKET 3
#define SET_STOPRECEIVEPACKET 4
#define SEARCH_CONNECTION_BYUSERID 5
#define CLEAR_ALL_CONNECTION 6

// User List Operation
#define ADD_USER    0
#define REMOVE_USER 1
#define UPDATE_USER_SESSIONID 2
#define UPDATE_USER_GROUPID 3
#define UPDATE_USER_CARD 4
#define GET_USER_NAME 5
#define GET_USER_GROUPID 6
#define GET_USER_CARD 7
#define SEARCH_USER_BYID 8
#define SEARCH_USER_BYNAME 9
#define CLEAR_ALL_USER 10

// Group List Operation
#define ADD_ID_TO_GROUP    0
#define REMOVE_ID_FORM_GROUP  1
#define GET_USERID_LIST 2
#define CLEAR_ALL_GROUP 3

// WINDOW MESSAGE
#define WM_UPDATE_USER_LIST_MESSAGE 0x123
#define WM_UPDATE_SERVER_MSG_MESSAGE 0x124




class CFindeUserByID{
public:
	CFindeUserByID(UINT unUserID):m_unUserID(unUserID){}
	BOOL operator()(Player &lhs)
	{
		return (lhs.unSessionID == m_unUserID);
	}
private:
	UINT m_unUserID;
};

class CFindeUserByName{
public:
	CFindeUserByName(unsigned char* pchName){
		DWORD dwNum = MultiByteToWideChar(CP_ACP, NULL, (LPCSTR)pchName, -1, NULL, 0);
		TCHAR *ptchName = NULL;
		ptchName = new TCHAR[dwNum];
		if(ptchName)
		{
			MultiByteToWideChar(CP_ACP, NULL, (LPCSTR)pchName, -1, ptchName, dwNum);
		}

		m_csRhs.Format(_T("%s"), ptchName);
		if(ptchName)
			delete [] ptchName;
	}
	BOOL operator()(Player &lhs)
	{
		CString csLhs(lhs.uhUserName);
		return csLhs == m_csRhs;
	}
private:
	CString m_csRhs;
};

typedef struct _AcceptClient{
	HANDLE m_hReceivePacketThread;
	SOCKET m_soClientSocket;
	BOOL   m_bStopReceivePacketThread;
	_AcceptClient():m_hReceivePacketThread(NULL), m_soClientSocket(INVALID_SOCKET), m_bStopReceivePacketThread(FALSE){}
}AcceptClient;

class CFindeAcceptClientByID{
public:
	CFindeAcceptClientByID(UINT unID):m_unID(unID){}
	BOOL operator()(AcceptClient &lhs)
	{
		return (lhs.m_soClientSocket == m_unID);
	}
private:
	UINT m_unID;
};

typedef struct _INI_USER_LIST
{
	CString	m_csUsername;
	CString	m_csValue;

	bool operator == (const _INI_USER_LIST& rhs) const
	{
		return m_csUsername == rhs.m_csUsername;
	}
	_INI_USER_LIST():m_csUsername(_T("")), m_csValue(_T("0")){}
} INI_USER_LIST;


// CWorldServerDlg dialog
class CWorldServerDlg : public CDialog
{
// Construction
public:
	CWorldServerDlg(CWnd* pParent = NULL);	// standard constructor
	~CWorldServerDlg();
// Dialog Data
	enum { IDD = IDD_WORLDSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
private:
	CStatic   *m_pstServerMsg;
	CComboBox *m_pcbServerIPList;
	CEdit     *m_pedUserList;
	CButton   *m_pbuServerStart;
	CButton   *m_pbuServerStop;
	CListBox  *m_pliUserList;
	CEdit     *m_pedUser;
	CButton   *m_pbnAddUser;
	CButton   *m_pbnDelUser;
	UINT       m_unPort;
	CString    m_csIP;
	CString    m_stServerMsg;



// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()

	void UpdateUIStatus(unsigned short usUIStatus);
	static DWORD AcceptConnection(LPVOID pObject);
	static DWORD ReceivePacket(LPVOID pObject);
	static DWORD SendPacket(LPVOID pObject);
	void ParseRecivePacket(char *chReceivePacket, int nReceivePacketLength, UINT unSessionID);
	void SendSerevrOffLinePacket();
	void StopServer();
	void StopAcceptConnection();
public:
	void Initialize();
	void DetectIPlist();
	void SetServerMsg();
	afx_msg void OnCbnSelchangeCbIplist();
	afx_msg void OnBnClickedBtnStartserver();
	afx_msg void OnBnClickedBtnStopserver();
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnAdduser();
	afx_msg void OnBnClickedBtnDeluser();

	// Accept Client connect Operation
	typedef std::vector<AcceptClient>::iterator AcceptClientit;
	std::vector<AcceptClient> m_AcceptClientList;
	CRITICAL_SECTION m_csAcceptConnection;
	void AccessAcceptClientList(USHORT usCommand, std::vector<AcceptClient> &tmpAcceptClient);
	
	// User Operation
	typedef std::vector<Player>::iterator Playerit;
	std::vector<Player> m_PlayerList;
	CRITICAL_SECTION m_csPlayerList;
	void AccessUserList(USHORT usCommand, Player &tmpPlayer);
	
	// Group Oprtation
	typedef std::multimap<UINT, UINT>::iterator UserGroupit;
	typedef std::multimap<UINT, UINT>::value_type GroupType;
	void AccessGroupList(USHORT usCommand, UINT unGroupID, std::vector<UINT> &tmpIDList);
	CRITICAL_SECTION m_csGroupList;
	std::multimap<UINT, UINT> m_UserGroup;

	GenericConnection m_Connection;
	// Accept Connection Thread
	HANDLE m_hAcceptConnectionThread;
	HANDLE m_hAcceptConnectionCompleteEvent;
	BOOL   m_bStopAcceptConnectionThread;
	SOCKET m_soTmpClientSocket;
	
	// Send Packet Thread
	HANDLE m_hSendPacketThread;
	BOOL   m_bStopSendPacketThread;
	std::queue<world_header> m_SendPacketList;

	// Record User List
	std::set<CString> m_UserList;
	void RefreshUserList();

	// Utilty
	CString MB2WCString(char *chStr);
	void W2MBChar(CString csStr, char *pchStr, int nchLength);
	CString  m_csFile;
	CList<INI_USER_LIST, INI_USER_LIST&> m_IniUserList;
	BOOL CheckINIFile();
	BOOL GetUserListFormINI();
	void SaveUserListToINI();
	void SetUserListToListUI();

	CRITICAL_SECTION m_csParsePacket;


protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
