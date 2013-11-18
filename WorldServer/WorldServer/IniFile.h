#if !defined(AFX_INIFILE_H__E70C3CD8_5381_4DFA_8DF2_63DEE87C148A__INCLUDED_)
#define AFX_INIFILE_H__E70C3CD8_5381_4DFA_8DF2_63DEE87C148A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define     BufLen  2048

//-----------------------------------------------------------------------------
// STRUCTURE
//
typedef struct _INI_VARIABLE
{
    TCHAR                   *pszVarName;            // Variable Name
    TCHAR                   *pszVarValue;           // Variable Value
    struct _INI_VARIABLE    *pNext;
} INI_VARIABLE, *PINI_VARIABLE;

typedef struct _INI_SESSION
{
    TCHAR                   *pszSessName;
    struct _INI_VARIABLE    *pVars;
    struct _INI_SESSION     *pNext;
} INI_SESSION, *PINI_SESSION;
//-----------------------------------------------------------------------------

class CIniFile  
{
public:
	bool    WriteKeyValue(TCHAR *pszSessionName,TCHAR *pszKey,TCHAR *pszValue);
	bool    WriteSessionValue(TCHAR *pchSession,INI_VARIABLE *pVariable);
	bool    WriteSession(INI_SESSION *pSession);
	TCHAR   *GetFileName();
	bool    SetFileName(TCHAR *pszFileName);
	void    FreeVariable(INI_VARIABLE **ppVariable);
	void    FreeSession(INI_SESSION **ppSession);
	bool    ReadAllSessionValue(INI_SESSION **ppSession);
	bool    ReadKeyValue(TCHAR *pszSessionName,TCHAR *pszKeyName,TCHAR **ppszValue);
	bool    ReadSessionValue(TCHAR *pszSessionName,INI_VARIABLE **ppVariable);
	bool    ReadAllSessionName(INI_SESSION **ppSession);

	TCHAR  *m_pszError;
	CIniFile();
	virtual ~CIniFile();

private:
	TCHAR   *m_pszFileName;
};

#endif // !defined(AFX_INIFILE_H__E70C3CD8_5381_4DFA_8DF2_63DEE87C148A__INCLUDED_)
