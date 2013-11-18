#include "stdafx.h"
#include "IniFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
//  ©w¸q¥¨¶°
#ifndef MEM_NEW
#define MEM_NEW(pData,type,nLen)	        \
	{								        \
		if (pData != NULL)			        \
		{							        \
			delete pData;			        \
			pData	=	NULL;               \
		}							        \
        pData   =   new type[nLen];         \
        memset(pData,0,sizeof(type)*nLen);  \
	}
#endif //MEM_NEW

#ifndef MEM_DEL
#define MEM_DEL(pData)                      \
	{								        \
		if (pData != NULL)			        \
		{							        \
			delete pData;			        \
			pData	=	NULL;               \
		}							        \
	}
#endif //MEM_NEW

CIniFile::CIniFile()
{
    m_pszFileName   =   NULL;
    m_pszError      =   NULL;

}

CIniFile::~CIniFile()
{
    MEM_DEL(m_pszFileName);
    MEM_DEL(m_pszError);
}


bool CIniFile::ReadAllSessionName(INI_SESSION **ppSession)
{
    INI_SESSION     *pSession   =   NULL;
    INI_SESSION     *pTemp      =   NULL;
    DWORD            dwBufLen   =   100;
    DWORD            dwRet      =   0;
    TCHAR            *pBuf      =   NULL;
    TCHAR            *pchTemp   =   0;
    size_t           nLen       =   0;


    if(ppSession == NULL)
        return false;
    if(m_pszFileName == NULL)
        return false;
    MEM_NEW(pBuf,TCHAR,dwBufLen);
    dwRet = GetPrivateProfileSectionNames(pBuf,dwBufLen,m_pszFileName);
    if(dwRet == 0)
    {
        MEM_DEL(pBuf);
        return false;
    }
    while(dwRet == (dwBufLen - 2))
    {
        dwBufLen +=  dwBufLen;
        MEM_DEL(pBuf);
        MEM_NEW(pBuf,TCHAR,dwBufLen);
        dwRet = GetPrivateProfileSectionNames(pBuf,dwBufLen,m_pszFileName);
        if(dwRet == 0)
        {
            MEM_DEL(pBuf);
            return false;
        }
    }
    pchTemp =   pBuf;
    pSession=   new INI_SESSION;
    memset(pSession,0,sizeof(INI_SESSION));
    pTemp   =   pSession;
    while(dwRet)
    {
        nLen    =   wcslen(pchTemp) + 1;    // String Length and '\0'
        MEM_NEW(pTemp->pszSessName,TCHAR,nLen);
        wcscpy(pTemp->pszSessName,pchTemp);
        pTemp->pNext    =   NULL;
        pchTemp +=  nLen;

#pragma warning(disable:4267)
        dwRet   -=  nLen;
#pragma warning(default:4267)
        if(dwRet)
        {
            pTemp->pNext    =   new INI_SESSION;
            memset(pTemp->pNext,0,sizeof(INI_SESSION));
            pTemp   =   pTemp->pNext;
        }
    }
    MEM_DEL(pBuf);
    *ppSession  =   pSession;
    return true;
}

bool CIniFile::ReadSessionValue(TCHAR *pszSessionName, INI_VARIABLE **ppVariable)
{
    INI_VARIABLE    *pVariable  =   NULL;
    INI_VARIABLE    *pTemp      =   NULL;
    DWORD            dwBufLen   =   100;
    DWORD            dwRet      =   0;
    TCHAR           *pBuf       =   0;
    TCHAR           *pchTemp    =   NULL;
    TCHAR           *pchPos     =   NULL;
    size_t           nLen       =   0;

    if(m_pszFileName == NULL)
        return false;
    if(pszSessionName == NULL)
        return false;
    if(ppVariable == NULL)
        return false;

    MEM_NEW(pBuf,TCHAR,dwBufLen);
    dwRet = GetPrivateProfileSection(pszSessionName,pBuf,dwBufLen,m_pszFileName);
    if(dwRet == 0)
    {
        MEM_DEL(pBuf);
        return false;
    }
    while(dwRet == (dwBufLen - 2))
    {
        dwBufLen +=  dwBufLen;
        MEM_DEL(pBuf);
        MEM_NEW(pBuf,TCHAR,dwBufLen);
        dwRet = GetPrivateProfileSection(pszSessionName,pBuf,dwBufLen,m_pszFileName);
        if(dwRet == 0)
        {
            MEM_DEL(pBuf);
            return false;
        }
    }

    pchTemp     =   pBuf;
    pVariable   =   new INI_VARIABLE;
    memset(pVariable,0,sizeof(INI_VARIABLE));
    pTemp       =   pVariable;
    while(dwRet)
    {
        nLen    =   wcslen(pchTemp);

#pragma warning(disable:4267)
        dwRet   -=  (nLen + 1);
#pragma warning(default:4267)

        // Get Variable name
        pchPos  =   wcschr(pchTemp,'=');
        *pchPos =   '\0';
#pragma warning(disable:4244)
        nLen    =   (pchPos - pchTemp) + 1;
#pragma warning(default:4244)
        MEM_NEW(pTemp->pszVarName,TCHAR,nLen);
        wcscpy(pTemp->pszVarName,pchTemp);

        //Get Variable value
        pchTemp =   pchPos + 1;
        nLen    =   (wcslen(pchTemp) + 1);
        MEM_NEW(pTemp->pszVarValue,TCHAR,nLen);
        wcscpy(pTemp->pszVarValue,pchTemp);

        pchTemp +=  nLen;
        pTemp->pNext    =   NULL;

        if(dwRet)
        {
            pTemp->pNext    =   new INI_VARIABLE;
            memset(pTemp->pNext,0,sizeof(INI_VARIABLE));
            pTemp   =   pTemp->pNext;
        }
    }
    MEM_DEL(pBuf);
    *ppVariable =   pVariable;
    return true;
}

bool CIniFile::ReadKeyValue(TCHAR *pszSessionName, TCHAR *pszKeyName, TCHAR **ppszValue)
{
    DWORD       dwRet       =   0;
    DWORD       dwBufLen    =   100;
    TCHAR      *pBuf        =   NULL;
    size_t      nLen        =   0;
    if(pszSessionName == NULL || pszKeyName == NULL)
        return false;
    if(m_pszFileName == NULL)
        return false;
    if(ppszValue == NULL)
        return false;
    MEM_NEW(pBuf,TCHAR,dwBufLen);
    dwRet   =   GetPrivateProfileString(pszSessionName,
                                        pszKeyName,L"",
                                        pBuf,
                                        dwBufLen,
                                        m_pszFileName);
    if(dwRet == 0)
    {
        MEM_DEL(pBuf);
        return false;
    }
    while(dwRet == (dwBufLen - 2))
    {
        dwBufLen    +=  dwBufLen;
        MEM_DEL(pBuf);
        MEM_NEW(pBuf,TCHAR,dwBufLen);
        dwRet   =   GetPrivateProfileString(pszSessionName,
                                            pszKeyName,L"",
                                            pBuf,
                                            dwBufLen,
                                            m_pszFileName);
        if(dwRet == 0)
        {
            MEM_DEL(pBuf);
            return false;
        }
    }
    nLen    =   (wcslen(pBuf) + 1);
    MEM_NEW((*ppszValue),TCHAR,(nLen));
    wcscpy((*ppszValue),pBuf);
    MEM_DEL(pBuf);
    return true;
}

bool CIniFile::ReadAllSessionValue(INI_SESSION **ppSession)
{
    INI_SESSION     *pSession   =   NULL;
    INI_SESSION     *pSeTemp    =   NULL;
    if(ppSession == NULL)
        return false;
    if(m_pszFileName == NULL)
        return false;

    if(!ReadAllSessionName(&pSession))
        return false;
    if(pSession == NULL)
        return true;
    else
    {
        pSeTemp =   pSession;
        while(pSeTemp)
        {
            ReadSessionValue(pSeTemp->pszSessName,&(pSeTemp->pVars));
            pSeTemp =   pSeTemp->pNext;
        }
    }
    *ppSession  =   pSession;
    return true;
}

bool CIniFile::SetFileName(TCHAR *pszFileName)
{
    size_t     nLen    =   0;
    if(m_pszFileName)
        MEM_DEL(m_pszFileName);

    nLen    =   wcslen(pszFileName);
    MEM_NEW(m_pszFileName,TCHAR,(nLen+1));
    wcscpy(m_pszFileName,pszFileName);

    return true;
}

TCHAR *CIniFile::GetFileName()
{
    return m_pszFileName;
}

void CIniFile::FreeSession(INI_SESSION **ppSession)
{
    INI_SESSION *pTemp  =   NULL;
    if(ppSession == NULL)
        return;
    while(*ppSession)
    {
        pTemp   =   (*ppSession);
        (*ppSession)    =   (*ppSession)->pNext;
        pTemp->pNext    =   NULL;
        MEM_DEL(pTemp->pszSessName);
        FreeVariable(&(pTemp->pVars));
        MEM_DEL(pTemp);
    }
}

void CIniFile::FreeVariable(INI_VARIABLE **ppVariable)
{
    INI_VARIABLE    *pTemp  =   NULL;
   if(ppVariable == NULL)
        return;
   while(*ppVariable)
   {
       pTemp   =   *ppVariable;
       (*ppVariable)    =   (*ppVariable)->pNext;
       pTemp->pNext     =   NULL;
       MEM_DEL(pTemp->pszVarName);
       MEM_DEL(pTemp->pszVarValue);
       MEM_DEL(pTemp);
   }
}

bool CIniFile::WriteSession(INI_SESSION *pSession)
{
    INI_SESSION     *pSeTemp    =   NULL;
    INI_VARIABLE    *pVarTemp   =   NULL;
    
    if(m_pszFileName == NULL)
        return false;
    if(pSession == NULL)
        return false;
    pSeTemp =   pSession;
    while(pSeTemp)
    {
        pVarTemp    =   pSeTemp->pVars;
        while(pVarTemp)
        {
            if(!WritePrivateProfileString(  pSeTemp->pszSessName,
                                            pVarTemp->pszVarName,
                                            pVarTemp->pszVarValue,
                                            m_pszFileName))
                return false;
            pVarTemp    =   pVarTemp->pNext;
        }
        pSeTemp =   pSeTemp->pNext;
    }
    return true;
}

bool CIniFile::WriteSessionValue(TCHAR *pchSession, INI_VARIABLE *pVariable)
{
    INI_VARIABLE    *pVarTemp   =   NULL;
    if(m_pszFileName == NULL)
        return false;
    if(pchSession == NULL || pVariable == NULL)
        return false;
    pVarTemp    =   pVariable;
    while(pVarTemp)
    {
        if(!WritePrivateProfileString(  pchSession,
                                        pVarTemp->pszVarName,
                                        pVarTemp->pszVarValue,
                                        m_pszFileName))
            return false;
        pVarTemp    =   pVarTemp->pNext;
    }
    return true;
}

bool CIniFile::WriteKeyValue(TCHAR *pszSessionName, TCHAR *pszKey, TCHAR *pszValue)
{
    if(m_pszFileName == NULL)
        return false;
    if(pszSessionName == NULL || pszKey == NULL || pszValue == NULL)
        return false;
    if(!WritePrivateProfileString(  pszSessionName,
                                    pszKey,
                                    pszValue,
                                    m_pszFileName))
        return false;
    return true;
}

