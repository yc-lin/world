#ifndef CARD_PLAYER_H
#define CARD_PLAYER_H

#include "vector"
#include "Card.h"
#define BETWEEN_CARDS_WITDH 20
#define SORT_BY_NUMBER 1
#define SORT_BY_SUIT 2
#define CARD_SPACE 2

class CardPlayer {
public:
	CardPlayer();
	~CardPlayer();
	typedef std::vector<CCard*>::iterator CardIterator;

	void SetParent(CWnd* pCWndParent);
	void RemoveCard(int nindex);
	void RemoveAllCard();
	void CreatCard(int nIndex);
	void CreatCardList(int nBegin, int nEnd);
	void UpdateCardPos();
	void HideCardList(BOOL bHide);
	void SetPos(int nX, int nY);
	void SortBy(UCHAR uchSort);
	BOOL m_UpdateCardPos;
	
private:
	CWnd* m_pCWndParent;
	int m_nFixX;
	int m_nX;
	int m_nY;
	UCHAR m_uchSortModel;
	std::vector<CCard*> m_CardList;
};
#endif