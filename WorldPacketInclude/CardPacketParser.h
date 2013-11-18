#ifndef CARD_PACKET_PARSER_H
#define CARD_PACKET_PARSER_H

#include "vector"
#define SYMBOLE_EXCLAMAION 33
#define SYMBOLE_COMMA 44
#define SYMBOLE_END 124
#define SYMBOLE_SPACE 32
#define SYMBOLE_DASH 45
#define SYMBOLE_CHAR0 48
#define OFFCARD_PACKET_HEADER_SIZE 6
#define PACKET_TYPE_UNKNOW      0
#define PACKET_TYPE_START_DEAL  1
#define PACKET_TYPE_DEAL        2

class CardPacketParser{
public:
	CardPacketParser(char *chData, int nDataLength);
	~CardPacketParser();
	UCHAR m_PacketType;
	UINT64 GetCard();
	UCHAR  WhosCard();
	std::vector<CString> m_vcUsername;
	void GetUserNameList(std::vector<CString> &vcUserName);

protected:
	void ParseType();
	void ParseName();
	USHORT m_CardSegmentPos;
	USHORT m_WhoCardSegmentPos;
	USHORT m_UserNameSegmentPos;
	int    m_nSegmentSize;
	std::vector<std::vector<unsigned char> > m_vcSegment;
};

#endif