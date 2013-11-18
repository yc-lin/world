#ifndef PACKET_HEADER_H
#define PACKET_HEADER_H

#define ETHERNET_HEARDER_SIZE 14
#define IP_HEADER_SIZE  20
#define TCP_HEADER_SIZE 20

#pragma pack(1)
/* 4 bytes IP address */
typedef struct ip_address
{
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ip_address;

/* IPv4 header */
typedef struct ip_header
{
	u_char	ver_ihl;		// Version (4 bits) + Internet header length (4 bits)
	u_char	tos;			// Type of service 
	u_short tlen;			// Total length 
	u_short identification; // Identification
	u_short flags_fo;		// Flags (3 bits) + Fragment offset (13 bits)
	u_char	ttl;			// Time to live
	u_char	proto;			// Protocol
	u_short crc;			// Header checksum
	ip_address	saddr;		// Source address
	ip_address	daddr;		// Destination address
}ip_header;

/* UDP header*/
typedef struct udp_header
{
	u_short sport;			// Source port
	u_short dport;			// Destination port
	u_short len;			// Datagram length
	u_short crc;			// Checksum
}udp_header;

/* UDP header*/
typedef struct tcp_header
{
	u_short  usSourcePort;			// Source port
	u_short  usDestinationPort;		// Destination port
	unsigned int  unSequenceN;		// Sequence number
	unsigned int  unAckN;			// Ack Number
	unsigned char ucTCPoffset;      // TCP offset
	union
	{
		struct
		{
			UCHAR	ucFlagBit0:1; // ( Congestion Window Reduction )
			UCHAR	ucFlagBit1:1; // ( ENC-Echo )	
			UCHAR	ucFlagBit2:1; // ( Urgent Pointer )	
			UCHAR	ucFlagBit3:1; // ( Ack )	
			UCHAR	ucFlagBit4:1; // ( Push )	
			UCHAR	ucFlagBit5:1; // ( Reset )
			UCHAR	ucFlagBit6:1; // ( SYN )	
			UCHAR	ucFlagBit7:1; // ( FIN )	
		};
		unsigned char ucFlag;       // TCP Flag
	};
	u_short  usWindow;
	unsigned char ucCheckSum[2];
	unsigned char ucUrgentPointer[2];
}tcp_header;
#pragma pack()
#endif //PACKET_HEADER_H