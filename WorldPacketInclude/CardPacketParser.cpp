#include "stdafx.h"
#include "CardPacketParser.h"


CardPacketParser::CardPacketParser(char *chData, int nDataLength)
{
	m_PacketType = PACKET_TYPE_UNKNOW ;
	m_CardSegmentPos     = 0;
	m_WhoCardSegmentPos  = 0;
	m_UserNameSegmentPos = 0;
	m_nSegmentSize       = 0;
	std::vector<unsigned char> vcPacket;
	std::vector<size_t> vcDelCharPos;
	//char k = *(chData + nDataLength - 3);
	//char k1 = *(chData + nDataLength - 2);
	//char k2 = *(chData + nDataLength - 1);
	//char k3 = *(chData + nDataLength);
	//char k4 = *(chData + nDataLength+1);
	//char k5 = *(chData + nDataLength+2);
	// Parse packet ,
	// format ex: 
	//    80,117,93,...,22,!,33,55,...,21,!
	if(*(chData + nDataLength - 1) == SYMBOLE_EXCLAMAION)
	{
		int  nchIndex = 0;
		char chNumber[3] = {0,0,0};
		int  nNumber = 0;
		BOOL bAddchar = FALSE;;
		for(int i = 0; i < nDataLength ; ++i)
		{
            if(*(chData + i) == SYMBOLE_EXCLAMAION)
			{
				vcDelCharPos.push_back(vcPacket.size());
				bAddchar = TRUE;
			}else if( *(chData + i) == SYMBOLE_COMMA )
			{
				bAddchar = TRUE;
			} else {
				chNumber[nchIndex++] = *(chData + i) - SYMBOLE_CHAR0;
			}

			if(bAddchar){
				char k = *(chData + i) ;
				if(nchIndex == 1)
					vcPacket.push_back(1*chNumber[0] );
				if(nchIndex == 2)
					vcPacket.push_back(10*chNumber[0] + 1*chNumber[1]);
				if(nchIndex == 3)
					vcPacket.push_back(100*chNumber[0] + 10*chNumber[1] + 1*chNumber[2]);

				nchIndex = 0;
				chNumber[0] = 0;
				chNumber[1] = 0;
				chNumber[2] = 0;
				bAddchar = FALSE;
			}
		}
	}

	// Del add number
	int chBegPos = 0;
	std::vector<int> vcSegmentPos;
	for(std::vector<size_t>::iterator beg = vcDelCharPos.begin();
		beg != vcDelCharPos.end(); ++beg)
	{
		int  chEndPos = static_cast<int>(*beg) - 1; // skip packet length
		unsigned char chDel = vcPacket[chEndPos + 1]; // Del char
		
		for(;chBegPos < chEndPos; ++chBegPos){
			vcPacket[chBegPos] = vcPacket[chBegPos] - chDel;
			if(vcPacket[chBegPos] == SYMBOLE_END)
				vcSegmentPos.push_back(chBegPos);
		}
		if( (*beg + 1)  < vcPacket.size())
			chBegPos = static_cast<int>(*beg) + 1;
	}

	// Get Data Segment
	chBegPos = 0;
	size_t SegmentPosSize = vcSegmentPos.size();
	int    vcPacketSize   = static_cast<int>(vcPacket.size());
	for(size_t pos = 0; pos <  SegmentPosSize + 1; ++pos)
	{
		std::vector<unsigned char> m_vcChar;
		int  chEndPos = 0 ;
		if(pos == SegmentPosSize)
			chEndPos = static_cast<int>(vcPacket.size()) - 2;
		else
			chEndPos = vcSegmentPos[pos];
		if(chBegPos < chEndPos)
		{
			for(; chBegPos< chEndPos; ++chBegPos)
				m_vcChar.push_back(vcPacket[chBegPos]);
			m_vcSegment.push_back(m_vcChar);

			if(chEndPos + 1 < vcPacketSize)
			{
				if(vcPacket[chEndPos+1] == 51) // First Deal header
				{
					if(  chEndPos >= 6 &&
						(vcPacket[chEndPos-6] == 114) &&
						(vcPacket[chEndPos-5] == 101) &&
						(vcPacket[chEndPos-4] == 110) &&
						(vcPacket[chEndPos-3] ==  83) &&
						(vcPacket[chEndPos-2] == 104) &&
						(vcPacket[chEndPos-1] == 117))
					{
						chBegPos = chEndPos + 4;// skip, 124, 51, header len, del char
					} else {
						chBegPos = chEndPos + 1;
					}
				} else {
					chBegPos = chEndPos + 1;
				}
			}else {
				break;
			}
		}
	}
	m_nSegmentSize = static_cast<int>(m_vcSegment.size());
	ParseType();
}

CardPacketParser::~CardPacketParser()
{

}

void CardPacketParser::ParseType()
{
	if(m_nSegmentSize != 0)
	{
		if(m_vcSegment[0].size() == 6)
		{
			if( (m_vcSegment[0][0] ==  67) &&
				(m_vcSegment[0][1] == 104) &&
				(m_vcSegment[0][2] == 117) &&
				(m_vcSegment[0][3] ==  80) &&
				(m_vcSegment[0][4] ==  97) &&
				(m_vcSegment[0][5] == 105))
			{
				m_PacketType = PACKET_TYPE_DEAL;

				if( m_nSegmentSize == 5)
				{
					m_CardSegmentPos = 1;

					if( (m_vcSegment[3].size() == 1))
					{
						m_WhoCardSegmentPos  = 3;
					}
				}
			} else if( (m_vcSegment[0][0] == 114) && // 2 header
				 	   (m_vcSegment[0][1] == 101) &&
					   (m_vcSegment[0][2] == 110) &&
					   (m_vcSegment[0][3] ==  83) &&
					   (m_vcSegment[0][4] == 104) &&
					   (m_vcSegment[0][5] == 117))
			{
				if(  m_nSegmentSize >= 2 &&
					(m_vcSegment[1][0] ==  75) && 
					(m_vcSegment[1][1] ==  97) &&
					(m_vcSegment[1][2] == 105) &&
					(m_vcSegment[1][3] ==  83) &&
					(m_vcSegment[1][4] == 104) &&
					(m_vcSegment[1][5] == 105))
				{
					m_PacketType = PACKET_TYPE_START_DEAL;
					if( m_nSegmentSize == 8)
					{
						m_CardSegmentPos = 2;
						m_UserNameSegmentPos = 7;
						ParseName();
					}
				}
			} else if( (m_vcSegment[0][0] ==  75) && // one header
					   (m_vcSegment[0][1] ==  97) &&
					   (m_vcSegment[0][2] == 105) &&
					   (m_vcSegment[0][3] ==  83) &&
					   (m_vcSegment[0][4] == 104) &&
					   (m_vcSegment[0][5] == 105))
			{
				m_PacketType = PACKET_TYPE_START_DEAL;
				if( m_nSegmentSize == 7)
				{
					m_CardSegmentPos = 1;
					m_UserNameSegmentPos = 6;
					ParseName();
				}
			}
		}
	}
}

UCHAR CardPacketParser::WhosCard()
{
	if(m_WhoCardSegmentPos != 0)
	{
		return m_vcSegment[m_WhoCardSegmentPos][0];
	}
	return 0;
}

UINT64 CardPacketParser::GetCard()
{
	UINT64 unCard = 0;
	if(m_CardSegmentPos != 0)
	{	
		// Remap card
		std::vector<USHORT> usCard;
		int uchCardIndex = 0;
		unsigned char uchCard[2] = {0,0};
		size_t CardSize = m_vcSegment[m_CardSegmentPos].size();
		for(size_t i = 0; i < CardSize + 1; ++i)
		{
			if( i == CardSize)
			{
				if(uchCardIndex == 1)
					usCard.push_back(uchCard[0]);
				if(uchCardIndex == 2)
					usCard.push_back(uchCard[0]*10 + uchCard[1]);
				uchCardIndex = 0;
				break;
			}
			if(m_vcSegment[m_CardSegmentPos][i] == SYMBOLE_SPACE)
			{
				if(uchCardIndex == 1)
					usCard.push_back(uchCard[0]);
				if(uchCardIndex == 2)
					usCard.push_back(uchCard[0]*10 + uchCard[1]);
				uchCardIndex = 0;
			}else {
				uchCard[uchCardIndex++] = m_vcSegment[m_CardSegmentPos][i] - SYMBOLE_CHAR0;
			}
		}

		
		UINT64 bitOn  = 0x01;
		for(std::vector<USHORT>::iterator beg = usCard.begin(); beg != usCard.end(); ++beg)
		{
			// Remap card
			if( *beg == 11)
				*beg = 1;
			else if(*beg == 12)
				*beg = 2;
			else if(*beg == 24)
				*beg = 14;
			else if(*beg == 25)
				*beg = 15;
			else if(*beg == 37)
				*beg = 27;
			else if(*beg == 38)
				*beg = 28;
			else if(*beg == 50)
				*beg = 40;
			else if(*beg == 51)
				*beg = 41;
			else
				*beg += 3;

			// Parse to UINT64 format
			unCard = unCard | ( bitOn << *beg );
		}
	}
	return unCard;
}

void CardPacketParser::ParseName()
{
	if(m_UserNameSegmentPos != 0)
	{
		size_t NameSize = m_vcSegment[m_UserNameSegmentPos].size();
		BOOL   bGetName = FALSE;
		int    nCharNameINdex = 0;
		char   pchtmpName[64] = "";
		memset(pchtmpName, 0x0, 64);
		for(size_t i = 0; i < NameSize; ++i)
		{
			if( m_vcSegment[m_UserNameSegmentPos][i] ==  SYMBOLE_SPACE)
			{
				if( i + 3 < NameSize)
				{
					if( (m_vcSegment[m_UserNameSegmentPos][i + 1] == 49) &&
						(m_vcSegment[m_UserNameSegmentPos][i + 2] == 32))
					{
						i +=  3;
						bGetName = TRUE;
					}
				}
			}
			if(m_vcSegment[m_UserNameSegmentPos][i] == SYMBOLE_DASH )
			{ 
				m_vcUsername.push_back(CString(pchtmpName));

				nCharNameINdex = 0;
				bGetName = FALSE;
				memset(pchtmpName, 0x0, 64);	
			}
			if(bGetName)
			{
				pchtmpName[nCharNameINdex++] = m_vcSegment[m_UserNameSegmentPos][i];
			}
		}
	}
}

void CardPacketParser::GetUserNameList(std::vector<CString> &vcUserName)
{
	vcUserName = m_vcUsername;
}
