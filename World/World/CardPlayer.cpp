#include "stdafx.h"
#include "CardPlayer.h"

CardPlayer::CardPlayer()
{
	m_nFixX = 0;
	m_nX = 0;
	m_nY = 0;
	m_UpdateCardPos = FALSE;
	m_pCWndParent   = NULL;
	m_uchSortModel  = SORT_BY_SUIT;
}
CardPlayer::~CardPlayer()
{
	if(!m_CardList.empty())
	{
		for(CardIterator it = m_CardList.begin();
			it != m_CardList.end();
			++it)
		{
			if(*it)
			{
				delete *it;
			}
		}
		m_CardList.clear();
	}
}

void CardPlayer::SetParent(CWnd* pCWndParent)
{
	m_pCWndParent = pCWndParent;
}
void CardPlayer::SetPos(int nX, int nY)
{
	m_nFixX = nX;
	m_nX = m_nFixX;
	m_nY = nY;
}

void CardPlayer::RemoveCard(int index)
{
	if(!m_CardList.empty())
	{
		for(CardIterator it = m_CardList.begin();
			it != m_CardList.end();
			++it)
		{
			if( (*it)->GetBitmapIndex() == index)
			{
				delete *it;
				m_CardList.erase(it);
				m_UpdateCardPos = TRUE;
				break;
			}
		}
	}
	UpdateCardPos();
}

void CardPlayer::RemoveAllCard()
{
	if(!m_CardList.empty())
	{
		for(CardIterator it = m_CardList.begin();
			it != m_CardList.end(); ++it)
		{
				delete *it;
		}
		m_CardList.clear();
	}
	m_nX = m_nFixX;
}

void CardPlayer::CreatCard(int nIndex)
{
	CreatCardList(nIndex, nIndex);
}

void CardPlayer::CreatCardList(int nBegin, int nEnd)
{
	if(!nBegin || nBegin > nEnd || nEnd > 52)
		return;

	int nX = m_nX;
	for(int nCardIndex = nBegin; nCardIndex <= nEnd; ++nCardIndex)
	{
		CCard *pCard = new CCard;
		CPoint Point(nX, m_nY);
		pCard->SetStartingPoint(Point);
		pCard->SetBitmapIndex(nCardIndex);
		pCard->Creation(m_pCWndParent, pCard->GetBitmapIndex(), TRUE, TRUE);

		m_CardList.push_back(pCard);

		// add position 
		nX += BETWEEN_CARDS_WITDH;
	}
	m_nX = nX;
}

void CardPlayer::UpdateCardPos()
{
	if(m_UpdateCardPos)
	{
		int nX = m_nFixX;
		
		if(m_uchSortModel == SORT_BY_NUMBER)
		{
			// Sort 3~12
			for(int nCardNumber = 3 ; nCardNumber < 13 ; ++nCardNumber)
			{
				for(CardIterator it = m_CardList.begin();
					it != m_CardList.end();
					++it)
				{
					if( (*it)->GetBitmapIndex() % 13 == nCardNumber)
					{
						(*it)->SetWindowPos(NULL, nX, m_nY, 0, 0,SWP_NOZORDER | SWP_NOSIZE);
						nX += BETWEEN_CARDS_WITDH;
					}
				}
			}
			// Sort 13
			for(CardIterator it = m_CardList.begin();
				it != m_CardList.end();
				++it)
			{
				if( (*it)->GetBitmapIndex() % 13 == 0)
				{
					(*it)->SetWindowPos(NULL, nX, m_nY, 0, 0,SWP_NOZORDER | SWP_NOSIZE);
					nX += BETWEEN_CARDS_WITDH;
				}
			}
			for(int nCardNumber = 1 ; nCardNumber < 3 ; ++nCardNumber)
			{
				for(CardIterator it = m_CardList.begin();
					it != m_CardList.end();
					++it)
				{
					if( (*it)->GetBitmapIndex() % 13 == nCardNumber)
					{
						(*it)->SetWindowPos(NULL, nX, m_nY, 0, 0,SWP_NOZORDER | SWP_NOSIZE);
						nX += BETWEEN_CARDS_WITDH;
					}
				}
			}
		} else if ( m_uchSortModel == SORT_BY_SUIT )
		{
			for(CardIterator it = m_CardList.begin();
				it != m_CardList.end();
				++it)
			{
				(*it)->SetWindowPos(NULL, nX, m_nY, 0, 0,SWP_NOZORDER | SWP_NOSIZE);
				nX += BETWEEN_CARDS_WITDH;
			}
			m_UpdateCardPos = FALSE;
		}
		m_nX = m_nFixX;
	}
}

void CardPlayer::HideCardList(BOOL bHide)
{
	for(CardIterator it = m_CardList.begin();
		it != m_CardList.end();
		++it)
	{
		if(bHide)
			(*it)->ShowWindow(SW_HIDE);
		else
			(*it)->ShowWindow(SW_SHOW);
	}
}

void CardPlayer::SortBy(UCHAR uchSort)
{
	if(uchSort > 0 && uchSort<= SORT_BY_SUIT)
	{
		m_uchSortModel = uchSort;
		m_UpdateCardPos = TRUE;
		UpdateCardPos();
	}
}

