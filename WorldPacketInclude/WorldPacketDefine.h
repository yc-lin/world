#ifndef WORLD_PACKET_DEFINE_H
#define WORLD_PACKET_DEFINE_H

#define REGISTER_USERNAME 0x01
#define REGISTER_USERNAME_RESPONSE 0x02
#define REGISTER_USERNAME_DENY_RESPONSE 0x03
#define UPDATE_USER_LIST 0x04
#define UPDATE_USER_LIST_RESPONSE 0x05
#define UPDATE_SELF_CARD 0x06
#define UPDATE_SELF_CARD_RESPONSE 0x07
#define GET_OTHER_USER_CARD 0x08
#define GET_OTHER_USER_CARD_RESPONSE 0x09
#define SERVER_OFFLINE 0xFA
#define CLIENT_OFFLINE 0xFB
#define PACKET_USERNAME_CHARSIZE   64
#pragma pack(1)
typedef struct _Player {
public:
	unsigned char  uhUserName[PACKET_USERNAME_CHARSIZE];
	unsigned long  ulIPAddress;
	unsigned int   unSessionID;
	unsigned int   unGroupID;
	unsigned int   unChannelID;
	UINT64         unCard;
	void clear()
	{
		memset(uhUserName, 0x00, PACKET_USERNAME_CHARSIZE);
		ulIPAddress = 0;
		unSessionID = 0;
		unGroupID   = 0;
		unCard      = 0;
		unChannelID = 0;
	}

	_Player(){
		clear();
	}
}Player;

typedef struct _WORLD_HEADER
{
	unsigned char  uhCommand;
	Player Receiver;
	Player Player1;
	Player Player2;
	_WORLD_HEADER():uhCommand(0){}
}world_header;

#pragma pack()
#endif
