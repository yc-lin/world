#ifndef BITMAP_BUTTON_CARD_H
#define BITMAP_BUTTON_CARD_H

#define CARD_WIDTH 18
#define CARD_HEIGHT 34
#define CARD_BEGIN 1
#define CARD_END 52
class CCard;

class CSelectionMark
{
protected:
	CBitmap m_Bmp;
	int m_BitmapHeight;
	int m_BitmapWidth;
	CPoint m_Point;
public:
	CSelectionMark();
	~CSelectionMark();
	void On(CView *pView, CCard *pCard, BOOL bLastCard);
	void Off(CView *pView);
};

/////////////////////////////////////////////////////////////////////////////
// CCard window
//	I derived CCard from CBitmapButtonCard because with CBitmapButtonCard I don't 
//  have to write drawing routines to draw the bitmap representing the card.
//  Besides drawing there are also many more advantages to using CBitmapButtonCard.

class CCard : public CBitmapButton
{
protected:
	DECLARE_DYNCREATE(CCard)
private:
	int m_BitmapIndex;		// identifier which denotes the card
	// this identifier can rangle from 1 to 52
	// 1 represents Ace of clubs and 52 represents King of spades

	CPoint m_StartingPoint; // the upper-left corner of the CCard object

	// Construction
public:
	CSelectionMark m_SelectionMark;		//An arrow is shown above the card if the user selects it
	BOOL m_bSelection;		// this indicates whether the card is selected or not		

	BOOL m_bFrontView;		//this identifer idicates whether the front face of the card
	// is shown or the back of the card is shown. The back face is 
	// same for all the cards when the player chooses a particular deck
	CCard();

public:
	virtual void Serialize(CArchive& ar);

public:
	void operator=(CCard &);		

	int GetBitmapIndex();			//returns the bitmap index for a particular card

	void SetBitmapIndex(int BmpIndex);		//sets the bitmap index of a particular card

	BOOL HitTest(CView *pView, CPoint Point, BOOL bLastCard = FALSE, BOOL bMoving = FALSE);		//tests whether the point lies inside the card or not
	//when the bLastCard is True it indicates the card to be 
	//the last card in the array of cards that each player has.
	//bMoving indicates if this HitTesting is done while the card is 
	//being moved. I don't want the arrow mark to be shown on the card
	//while the player drags the card.

	CPoint GetStartingPoint();		//gets the upper-left position of the card

	void SetStartingPoint(CPoint point);		//sets the upper-left position of the card

	BOOL Creation(CWnd *pParentWnd, int nID, BOOL bFrontView, BOOL bVisible = FALSE);		//creates the windows window associated with the CCard

	BOOL IsViewChanged(BOOL bFrontView);		//indicates whether the front or back view of the card has changed or not

	BOOL IsSelected();		//indicates whether the card is selected or not
	void SetSelectionFalse(CView *pView);		//sets the selection of the card to false
	// Implementation
public:
	virtual ~CCard();
protected:

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#endif
