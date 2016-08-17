#ifndef RFPACKET_H_
#define RFPACKET_H_

typedef unsigned char byte;

void rfpacket_send();
byte rfpacket_receive();

#define FLAGS_POSTAMBLE 0b00000001
#define FLAGS_FROM_BASESTATION 0b00000010
#define FLAGS_FROM_LONGWORK 0b00001000
#define RFPACKET_BUFFER_SIZE 64

struct
{
	uint32_t address;
	byte promiscuos;
} rfpacket_params;

 struct
{
	int rssi;
	uint32_t address_for;
	byte flags;
	byte command;
	byte data[RFPACKET_BUFFER_SIZE];
	byte data_length;
} rfpacket_request;

 struct
{
	uint32_t address;
	byte response;
	byte flags;	
	byte data[RFPACKET_BUFFER_SIZE];
	byte data_length;	
} rfpacket_response;

#endif /* RFPACKET_H_ */