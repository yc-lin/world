// Card.cpp : implementation file
//
#include "stdafx.h"
#include "Card.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCard

CCard::CCard()
{
	m_BitmapIndex = 0;
	m_bFrontView = FALSE;
	m_bSelection = FALSE;
	m_hWnd = NULL;
}

CCard::~CCard()
{
}

IMPLEMENT_DYNCREATE(CCard, CBitmapButton)

BEGIN_MESSAGE_MAP(CCard, CBitmapButton)
	//{{AFX_MSG_MAP(CCard)
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCard message handlers

//  I have used CSocket for the communication between the client(s) and the host.
//  Right now I only need m_BitmapIndex to be serialized. Maybe in the future 
//  I may want to serialize the other members of the CCard.

void CCard::Serialize(CArchive& ar) 
{
	if (ar.IsStoring())
	{
		ar << (WORD) m_BitmapIndex;
	}
	else
	{
		WORD wd;
		ar >> wd;
		m_BitmapIndex = (int)wd;
	}
}

BOOL CCard::Creation(CWnd *pParentWnd, int nID, BOOL bFrontView, BOOL bVisible)
{
	ASSERT(pParentWnd != NULL);
	DestroyWindow();
	CSize SizeCard(CARD_WIDTH, CARD_HEIGHT);		//the size for each card is constant
	//i.e. 71 pixels width and 96 pixels height

	CRect Rect(m_StartingPoint,SizeCard);		//the rectangle of the card

	DWORD Style;
	if(bVisible)
		Style = WS_VISIBLE | WS_CHILD | WS_DISABLED | BS_PUSHBUTTON | BS_OWNERDRAW;		//BS_PUSHBUTTON is required for CBitmapButton derived classes
	else
		Style = WS_CHILD | WS_DISABLED | BS_PUSHBUTTON | BS_OWNERDRAW;		//BS_PUSHBUTTON is required for CBitmapButton derived classes

	if(CBitmapButton::Create(NULL,Style,Rect,pParentWnd,nID) == 0)
	{
		return FALSE;
	}

	if(bFrontView)
	{
		if(LoadBitmaps(m_BitmapIndex)==0)
		{
			return FALSE;
		}
	}
	else
	{
		//if(LoadBitmaps(FIVEINONE_APP->m_ImageDialog.m_ImageID)==0)
		//{
		//	AfxMessageBox("Failed to load bitmap");
		//	return FALSE;
		//}
	}

	m_bFrontView = bFrontView;
	return TRUE;
}

int CCard::GetBitmapIndex()
{
	return m_BitmapIndex;
}

BOOL CCard::HitTest(CView *pView, CPoint Point, BOOL bLastCard, BOOL bMoving)
{
	CSize Size;
	if(!bLastCard)
		Size.cx = 20;
	else
		Size.cx = 18;
	Size.cy = 18;

	CRect Rect(m_StartingPoint,Size);

	if (Rect.PtInRect(Point) && !m_bSelection)
	{
		if(!bMoving)
		{
			m_bSelection = TRUE;
			m_SelectionMark.On(pView, this, bLastCard);
		}
		return TRUE;
	}
	else
		return FALSE;
}

void CCard::operator =(CCard &Card)
{
	if (&Card == this) return;

	m_BitmapIndex = Card.m_BitmapIndex;
	m_StartingPoint.x = Card.m_StartingPoint.x;
	m_StartingPoint.y = Card.m_StartingPoint.y;
	m_bFrontView = Card.m_bFrontView;
	m_hWnd = Card.m_hWnd;
}

void CCard::SetBitmapIndex(int BmpIndex)
{
	ASSERT(BmpIndex >= 1 && BmpIndex <= 52);
	m_BitmapIndex = BmpIndex;
}

CPoint CCard::GetStartingPoint()
{
	return m_StartingPoint;
}

void CCard::SetStartingPoint(CPoint point)
{
	m_StartingPoint = point;
}

BOOL CCard::IsViewChanged(BOOL bFrontView)
{
	if(m_bFrontView == bFrontView)
		return FALSE;
	else 
		return TRUE;
}

void CCard::SetSelectionFalse(CView *pView)
{
	ASSERT(pView != NULL);

	m_bSelection = FALSE;
	m_SelectionMark.Off(pView);
}

BOOL CCard::IsSelected()
{
	return m_bSelection;
}

CSelectionMark::CSelectionMark()
{
	//BITMAP BM;
	//m_Bmp.LoadBitmap(IDB_SELECTION);
	//m_Bmp.GetObject(sizeof(BM), &BM);
	//m_BitmapWidth = BM.bmWidth;
	//m_BitmapHeight = BM.bmHeight;
	//m_Point = CPoint(0,0);
}

CSelectionMark::~CSelectionMark()
{
}


void CSelectionMark::On(CView *pView, CCard *pCard, BOOL bLastCard)
{
	ASSERT(pView != NULL);
	ASSERT(pCard != NULL);

	CRect RectCard;
	pCard->GetClientRect(&RectCard);
	pCard->ClientToScreen(&RectCard);
	pView->ScreenToClient(&RectCard);

	if(bLastCard)
	{
		m_Point.x = RectCard.TopLeft().x + RectCard.Width()/2 - m_BitmapWidth/2;
		m_Point.y = RectCard.TopLeft().y - m_BitmapHeight;
	}
	else
	{
		m_Point.x = RectCard.TopLeft().x + 10 - m_BitmapWidth/2;
		m_Point.y = RectCard.TopLeft().y - m_BitmapHeight;
	}

	CDC MemDC;

	MemDC.CreateCompatibleDC(NULL);
	MemDC.SelectObject(&m_Bmp);

	CDC *pDC;
	pDC = pView->GetDC();

	pDC->TransparentBlt(m_Point.x, m_Point.y, m_BitmapWidth, m_BitmapHeight, &MemDC, 0, 0, m_BitmapWidth, m_BitmapHeight, RGB(128, 128, 128));
}

void CSelectionMark::Off(CView *pView)
{
	ASSERT(pView != NULL);

	CRect Rect(m_Point, CSize(m_BitmapWidth, m_BitmapHeight));
	pView->InvalidateRect(&Rect);
}


BOOL CCard::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return CBitmapButton::OnEraseBkgnd(pDC);
}
