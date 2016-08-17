#include "RFM69registers.h"
#include "RFM69.h"
#include "rfpackets.h"
#include "avr/io.h"
#include <stdlib.h>

//необходимо реализовать данные функции
void spi_init();
void spi_unselect();
void spi_select();
uint8_t spi_read(uint8_t data);

//внутренние функции
void rfm69_checkstate();
void rfm69_receive_start();
byte rfm69_readreg(byte addr);
void rfm69_writereg(byte addr, byte val);
void rfm69_set_mode(byte newMode);

void var_delay();

void rfm69_set_poweramp(byte state)
{
	if (state)
	{
		if (rfm69_power_pa_mode==0){rfm69_writereg(REG_PALEVEL, (RF_PALEVEL_PA0_ON | RF_PALEVEL_PA1_OFF| RF_PALEVEL_PA2_OFF)|rfm69_power_level);	}
		if (rfm69_power_pa_mode==1){rfm69_writereg(REG_PALEVEL, (RF_PALEVEL_PA0_OFF | RF_PALEVEL_PA1_ON| RF_PALEVEL_PA2_OFF)|rfm69_power_level);	}
		if (rfm69_power_pa_mode==2){rfm69_writereg(REG_PALEVEL, (RF_PALEVEL_PA0_OFF | RF_PALEVEL_PA1_ON| RF_PALEVEL_PA2_ON)|rfm69_power_level);	}	
	}else
	{
		rfm69_writereg(REG_PALEVEL, (RF_PALEVEL_PA0_ON | RF_PALEVEL_PA1_OFF | RF_PALEVEL_PA2_OFF)|rfm69_power_level);
	}
}
byte rfm69_syncreceived=0;

void rfm69_init_base()
{
	const byte base_config[][2] =
	{
		/* 0x01 */ { REG_OPMODE,		RF_OPMODE_SEQUENCER_ON | RF_OPMODE_LISTEN_OFF | RF_OPMODE_STANDBY },
		/* 0x02 */ { REG_DATAMODUL,		RF_DATAMODUL_DATAMODE_PACKET | RF_DATAMODUL_MODULATIONTYPE_FSK | RF_DATAMODUL_MODULATIONSHAPING_01 },
		//Common
		/* 0x03 */ { REG_BITRATEMSB,	RF_BITRATEMSB_2400},
		/* 0x04 */ { REG_BITRATELSB,	RF_BITRATEMSB_2400},
		/* 0x05 */ { REG_FDEVMSB,		RF_FDEVMSB_4800 },
		/* 0x06 */ { REG_FDEVLSB,		RF_FDEVLSB_4800 },
		/* 0x07 */ { REG_FRFMSB, RF_FRFMSB_868},
		/* 0x08 */ { REG_FRFMID, RF_FRFMID_868},
		/* 0x09 */ { REG_FRFLSB, RF_FRFLSB_868},
		//Transmitter
		/* 0x11 */ { REG_PALEVEL, RF_PALEVEL_PA0_ON | RF_PALEVEL_PA1_OFF | RF_PALEVEL_PA2_OFF },
		/* 0x11 */ { REG_PARAMP, RF_PARAMP_40},
		/* 0x13 */ { REG_OCP, RF_OCP_ON | RF_OCP_TRIM_95 }, //over current protection (default is 95mA)
		//Receiver
		/* 0x18*/  { REG_LNA,  RF_LNA_ZIN_50 | RF_LNA_GAINSELECT_AUTO },
		/* 0x19 */ { REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_20| RF_RXBW_EXP_5 },
		/* 0x19 */ { REG_AFCBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_20| RF_RXBW_EXP_5 },
		/* 0x09 */ { REG_AFCFEI, RF_AFCFEI_AFCAUTO_OFF},
		//IRQ
		/* 0x25 */ { REG_DIOMAPPING1, 0 },
		/* 0x25 */ { REG_DIOMAPPING2, RF_DIOMAPPING2_CLKOUT_OFF },
		/* 0x29 */ { REG_RSSITHRESH, 120*2 }, //must be set to dBm = (-Sensitivity / 2) - default is 0xE4=228 so -114dBm
		/* 0x29 */ { REG_RXTIMEOUT1, 0 },
		/* 0x29 */ { REG_RXTIMEOUT2, 0 },
		//Packet Engine
		/* 0x2d */ { REG_PREAMBLEMSB, 0 },
		/* 0x2d */ { REG_PREAMBLELSB, 5 },
		/* 0x2e */ { REG_SYNCCONFIG, RF_SYNC_ON | RF_SYNC_FIFOFILL_AUTO| RF_SYNC_SIZE_2 | RF_SYNC_TOL_0 },
		/* 0x2f */ { REG_SYNCVALUE1, 0b01000110 },
		/* 0x30 */ { REG_SYNCVALUE2, 0b11100101 },
		/* 0x2f */ { REG_SYNCVALUE3, 0b00101101 },
		/* 0x30 */ { REG_SYNCVALUE4, 0b00010101 },	//The Si443x chips default to using a 2-byte (16-bit) Sync Word with a value of 0x2DD4h. If a longer Sync Word is desired, Silicon Labs recommends using a 4-byte (32-bit) Sync Word with a value of 0x5A0FBE66h.
		/* 0x37 */ { REG_PACKETCONFIG1, RF_PACKET1_FORMAT_VARIABLE | RF_PACKET1_DCFREE_WHITENING | RF_PACKET1_CRC_ON | RF_PACKET1_CRCAUTOCLEAR_ON | RF_PACKET1_ADRSFILTERING_OFF },
		/* 0x38 */ { REG_PAYLOADLENGTH, 0x40 }, //in variable length mode: the max frame size, not used in TX
		/* 0x39 */ { REG_NODEADRS, 0 }, //turned off because we're not using address filtering
		/* 0x39 */ { REG_BROADCASTADRS, 0 }, //turned off because we're not using address filtering
		/* 0x3B */ { REG_AUTOMODES, 0 }, //turned off because we're not using address filtering
		/* 0x3C */ { REG_FIFOTHRESH, RF_FIFOTHRESH_TXSTART_FIFONOTEMPTY | RF_FIFOTHRESH_VALUE }, //TX on FIFO not empty
		/* 0x3d */ { REG_PACKETCONFIG2, RF_PACKET2_RXRESTARTDELAY_4BITS | RF_PACKET2_AUTORXRESTART_ON | RF_PACKET2_AES_OFF}, //RXRESTARTDELAY must match transmitter PA ramp-down time (bitrate dependent)
		//Test registers
		/* 0x09 */ { REG_AFCCTRL, RF_AFCLOWBETA_OFF},
		/* 0x6F */ { REG_TESTLNA, RF_TESTLNA_HIGH },
		/* 0x6F */ { REG_TESTPA1, 0x55 },
		/* 0x6F */ { REG_TESTPA2, 0x70 },
		/* 0x6F */ { REG_TESTDAGC, RF_DAGC_IMPROVED_LOWBETA0 }, // run DAGC continuously in RX mode, recommended default for AfcLowBetaOn=0
		/* 0x6F */ { REG_TESTAFC, 0 },
		/* TERM */ { 255, 0 }		// Terminator
	};	
	spi_init();	
	do rfm69_writereg(REG_SYNCVALUE1, 0xaa); while (rfm69_readreg(REG_SYNCVALUE1) != 0xaa);
	do rfm69_writereg(REG_SYNCVALUE1, 0x55); while (rfm69_readreg(REG_SYNCVALUE1) != 0x55);	
	for (byte i = 0; base_config[i][0] != 255; i++)
	{
		rfm69_writereg(base_config[i][0], base_config[i][1]);		
	}
	rfm69_set_encrypt(0);
	rfm69_set_mode(RF69_MODE_STANDBY);
}

void rfm69_sdr_init()
{
		rfm69_init_base();
		const byte sdr_config[][2] =
		{
			/* 0x02 */ { REG_DATAMODUL,		RF_DATAMODUL_DATAMODE_PACKET | RF_DATAMODUL_MODULATIONTYPE_FSK | RF_DATAMODUL_MODULATIONSHAPING_01 },
			//Transmitter
			/* 0x11 */ { REG_PARAMP, RF_PARAMP_40},
			/* 0x13 */ { REG_OCP, RF_OCP_ON | RF_OCP_TRIM_95 }, //over current protection (default is 95mA)
			//Packet Engine
			/* 0x2d */ { REG_PREAMBLEMSB, 0 },
			/* 0x2d */ { REG_PREAMBLELSB, 0 },
			/* 0x2e */ { REG_SYNCCONFIG, RF_SYNC_OFF | RF_SYNC_FIFOFILL_AUTO| RF_SYNC_SIZE_2 | RF_SYNC_TOL_0 },
			/* 0x37 */ { REG_PACKETCONFIG1, RF_PACKET1_FORMAT_FIXED| RF_PACKET1_DCFREE_OFF| RF_PACKET1_CRC_OFF | RF_PACKET1_CRCAUTOCLEAR_OFF | RF_PACKET1_ADRSFILTERING_OFF },
			/* 0x38 */ { REG_PAYLOADLENGTH, 0 }, //in variable length mode: the max frame size, not used in TX
			/* 0x3C */ { REG_FIFOTHRESH, RF_FIFOTHRESH_TXSTART_FIFONOTEMPTY | RF_FIFOTHRESH_VALUE }, //TX on FIFO not empty
			/* 0x3d */ { REG_PACKETCONFIG2, RF_PACKET2_RXRESTARTDELAY_4BITS | RF_PACKET2_AUTORXRESTART_ON | RF_PACKET2_AES_OFF}, //RXRESTARTDELAY must match transmitter PA ramp-down time (bitrate dependent)
			/* TERM */ { 255, 0 }		// Terminator
		};
		for (byte i = 0; sdr_config[i][0] != 255; i++)
		{rfm69_writereg(sdr_config[i][0], sdr_config[i][1]);}		
}

void rfm69_normal_init()
{
	rfm69_init_base();
}

/*
uint32_t rfm69_freq2syntvalue(double freq)
{
	return freq*1000000.0/61.03515625;
}*/
/************************************************************************/
/*deviation:	1: 2000,		2: 4800,	3: 5000,	4: 10000,	5:20000,	6:55000 */
/*rxbw:		1: 3.9,			2: 7.8		3: 10.4,	4:15.6,		5: 20.8,	6: 25.0,	7: 31.3,	8: 41.7,	9: 62.5,	10: 100.0,	11: 166.7       */
/*bitrate:	1: 1200,		2: 2400,	3: 4800,	4: 9600,	5: 19200,	6: 38400,	7: 55555*/                                                              
/************************************************************************/
void rfm69_set_params(int deviation, int rxbw, int bitrate, int power)
{
	rfm69_set_mode(RF_OPMODE_STANDBY);
	if (power<16)
	{
		rfm69_power_pa_mode=0;
		rfm69_power_level=power;
	}else
	{
		rfm69_power_pa_mode=2;
		rfm69_power_level=power;		
	}
	switch (bitrate)
	{
		case 1:
		rfm69_writereg(	REG_BITRATEMSB,	RF_BITRATEMSB_1200);
		rfm69_writereg( REG_BITRATELSB,	RF_BITRATELSB_1200);
		break;
		case 2 :
		rfm69_writereg( REG_BITRATEMSB,	RF_BITRATEMSB_2400);
		rfm69_writereg( REG_BITRATELSB,	RF_BITRATELSB_2400);
		break;
		case 3:
		rfm69_writereg( REG_BITRATEMSB,	RF_BITRATEMSB_4800);
		rfm69_writereg( REG_BITRATELSB,	RF_BITRATELSB_4800);
		break;
		case 4:
		rfm69_writereg( REG_BITRATEMSB,	RF_BITRATEMSB_9600);
		rfm69_writereg( REG_BITRATELSB,	RF_BITRATELSB_9600);
		break;
		case 5:
		rfm69_writereg( REG_BITRATEMSB,	RF_BITRATEMSB_19200);
		rfm69_writereg( REG_BITRATELSB,	RF_BITRATELSB_19200);
		break;
		case 6:
		rfm69_writereg( REG_BITRATEMSB,	RF_BITRATEMSB_38400);
		rfm69_writereg( REG_BITRATELSB,	RF_BITRATELSB_38400);
		break;
		case 7:
		rfm69_writereg( REG_BITRATEMSB,	RF_BITRATEMSB_55555);
		rfm69_writereg( REG_BITRATELSB,	RF_BITRATELSB_55555);
		break;
		case 8:
		rfm69_writereg(REG_BITRATEMSB,	RF_BITRATEMSB_600);
		rfm69_writereg( REG_BITRATELSB,	RF_BITRATELSB_600);
		break;	
	}
	
	switch (deviation)
	{
		case 1:
		rfm69_writereg(REG_FDEVMSB,		RF_FDEVMSB_2000 );
		rfm69_writereg(REG_FDEVLSB,		RF_FDEVLSB_2000 );
		break;
		case 2:
		rfm69_writereg(REG_FDEVMSB,		RF_FDEVMSB_4800 );
		rfm69_writereg(REG_FDEVLSB,		RF_FDEVLSB_4800 );
		break;
		case 3:
		rfm69_writereg(REG_FDEVMSB,		RF_FDEVMSB_5000 );
		rfm69_writereg(REG_FDEVLSB,		RF_FDEVLSB_5000 );
		break;
		case 4:
		rfm69_writereg(REG_FDEVMSB,		RF_FDEVMSB_10000 );
		rfm69_writereg(REG_FDEVLSB,		RF_FDEVLSB_10000 );
		break;
		case 5:
		rfm69_writereg(REG_FDEVMSB,		RF_FDEVMSB_20000 );
		rfm69_writereg(REG_FDEVLSB,		RF_FDEVLSB_20000 );
		break;
		case 6:
		rfm69_writereg(REG_FDEVMSB,		RF_FDEVMSB_50000);
		rfm69_writereg(REG_FDEVLSB,		RF_FDEVLSB_50000 );
		break;
		case 7:
		rfm69_writereg(REG_FDEVMSB,		RF_FDEVMSB_1000 );
		rfm69_writereg(REG_FDEVLSB,		RF_FDEVLSB_1000 );
		break;
	}
	
	switch (rxbw)
	{	
		case 1://7.8
		rfm69_writereg( REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_16| RF_RXBW_EXP_6 );
		rfm69_writereg( REG_AFCBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_16| RF_RXBW_EXP_6 );
		break;
		case 2://10.4
		rfm69_writereg( REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24| RF_RXBW_EXP_5 );
		rfm69_writereg( REG_AFCBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24| RF_RXBW_EXP_5 );
		break;
		case 3://12.5
		rfm69_writereg( REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_20| RF_RXBW_EXP_5 );
		rfm69_writereg( REG_AFCBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_20| RF_RXBW_EXP_5 );
		break;
		case 4://15.6
		rfm69_writereg( REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_16| RF_RXBW_EXP_5 );
		rfm69_writereg( REG_AFCBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_16| RF_RXBW_EXP_5 );
		break;
		case 5://20.8
		rfm69_writereg( REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24| RF_RXBW_EXP_4 );
		rfm69_writereg( REG_AFCBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24| RF_RXBW_EXP_4 );
		break;
		case 6://25.0
		rfm69_writereg( REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_20| RF_RXBW_EXP_4 );
		rfm69_writereg( REG_AFCBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_20| RF_RXBW_EXP_4 );
		break;
		case 7://31.3
		rfm69_writereg( REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_16| RF_RXBW_EXP_4 );
		rfm69_writereg( REG_AFCBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_16| RF_RXBW_EXP_4 );
		break;
		case 8://41.7
		rfm69_writereg( REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24| RF_RXBW_EXP_3 );
		rfm69_writereg( REG_AFCBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24| RF_RXBW_EXP_3 );
		break;
		case 9://62.5
		rfm69_writereg( REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_16| RF_RXBW_EXP_3 );
		rfm69_writereg( REG_AFCBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_16| RF_RXBW_EXP_3 );
		break;
		case 10://100.0
		rfm69_writereg( REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_20| RF_RXBW_EXP_2 );
		rfm69_writereg( REG_AFCBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_20| RF_RXBW_EXP_2 );
		break;
		case 11://166.7
		rfm69_writereg( REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24| RF_RXBW_EXP_1 );
		rfm69_writereg( REG_AFCBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24| RF_RXBW_EXP_1 );
		break;
		case 12://3.9
		rfm69_writereg( REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_16| RF_RXBW_EXP_7 );
		rfm69_writereg( REG_AFCBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_16| RF_RXBW_EXP_7 );
		break;
	}
}

void rfm69_set_syntfreq(uint32_t syntvalue)
{
	rfm69_writereg(REG_FRFMSB, syntvalue >> 16);
	rfm69_writereg(REG_FRFMID, syntvalue >> 8);
	rfm69_writereg(REG_FRFLSB, syntvalue);
}

void rfm69_set_mode(byte newMode)
{
	if ( newMode == rfm69_internal.mode ) return;
	switch ( newMode )
	{
		case RF69_MODE_TX:
		rfm69_writereg(REG_OPMODE, (rfm69_readreg(REG_OPMODE) & 0xE3) | RF_OPMODE_TRANSMITTER);
		break;
		case RF69_MODE_RX:
		rfm69_writereg(REG_OPMODE, (rfm69_readreg(REG_OPMODE) & 0xE3) | RF_OPMODE_RECEIVER);
		break;
		case RF69_MODE_SYNTH:
		rfm69_writereg(REG_OPMODE, (rfm69_readreg(REG_OPMODE) & 0xE3) | RF_OPMODE_SYNTHESIZER);
		break;
		case RF69_MODE_STANDBY:
		rfm69_writereg(REG_OPMODE, (rfm69_readreg(REG_OPMODE) & 0xE3) | RF_OPMODE_STANDBY);
		break;
		case RF69_MODE_SLEEP:
		rfm69_writereg(REG_OPMODE, (rfm69_readreg(REG_OPMODE) & 0xE3) | RF_OPMODE_SLEEP);
		break;
		default: return;
	}

	while ( rfm69_internal.mode == RF69_MODE_SLEEP && ( rfm69_readreg(REG_IRQFLAGS1) & RF_IRQFLAGS1_MODEREADY ) == 0x00 );
	rfm69_internal.mode = newMode;
}

void rfm69_sleep()
{
	rfm69_set_mode(RF69_MODE_SLEEP);
}

byte rfm69_can_send()
{
	if ( rfm69_internal.mode == RF69_MODE_RX && rfm69_data.data_len == 0 && rfm69_read_rssi() < -90  )
	{
		rfm69_set_mode(RF69_MODE_STANDBY);
		return 1;
	}
	return 0;
}

void rfm69_send_preamble(byte length)
{
	byte current_lsb=rfm69_readreg(REG_PREAMBLELSB);
	byte current_msb=rfm69_readreg(REG_PREAMBLEMSB);
	byte current_sync=rfm69_readreg(REG_SYNCCONFIG);
	rfm69_writereg(REG_PREAMBLELSB, length);
	rfm69_writereg(REG_SYNCCONFIG, RF_SYNC_OFF | RF_SYNC_FIFOFILL_AUTO| RF_SYNC_SIZE_1 | RF_SYNC_TOL_0 ),
	rfm69_send(0,0,0);
	rfm69_writereg(REG_PREAMBLELSB,current_lsb);
	rfm69_writereg(REG_PREAMBLEMSB,current_msb);
	rfm69_writereg(REG_SYNCCONFIG,current_sync);
}

void _rfm69_clean_rx()
{
	//clean
	spi_select();
	spi_read(REG_FIFO & 0x7f);
	for ( byte i = 0; i < 4; i++ )
	spi_read(0);
	spi_unselect();	
	
	rfm69_set_poweramp(1);
	
	rfm69_writereg(REG_PACKETCONFIG2, ( rfm69_readreg(REG_PACKETCONFIG2) & 0xFB ) | RF_PACKET2_RXRESTART); // avoid RX deadlocks
	rfm69_set_mode(RF69_MODE_STANDBY);	
}
/*
void request_sendbytes()
{
	byte zero=255;
	byte one=0;
	byte result[65]={0};
	byte j=0;
	for (unsigned int k=0; k<sserial_request.datalength-1; k++)
	{
		for (int l=0; l<8; l++)
		{
			if (sserial_request.data[k+1] & 128)	{	result[j]=zero;	}else	{	result[j]=one;	}
			sserial_request.data[k+1]=sserial_request.data[k+1]<<1;
			j++;
			if (j>64){j=64;}
		}
	}
	rfm69_send(result,0,j);
	sserial_response.result=0x51;
	sserial_send_response(0);
}*/

byte _rfm69_sdr_buffer[8];
void _rfm69_sdr_fill_buffer(byte data)
{
	const byte zero=255;
	const byte one=0;	
	for (byte i=0; i<8; i++)
	{
		if (data & 128)	{	_rfm69_sdr_buffer[i]=zero;	}else	{	_rfm69_sdr_buffer[i]=one;	}
		data=data<<1;
	}
}

void _rfm69_sdr_send_buffer()
{
	spi_select();
	spi_read(REG_FIFO | 0x80);
	for ( byte i = 0; i < 8; i++ )
	spi_read(_rfm69_sdr_buffer[i]);
	spi_unselect();
}

void rfm69_sdr_send(const byte* buffer, byte start, byte bufferSize)
{
	_rfm69_clean_rx();
	_rfm69_sdr_fill_buffer(buffer[start]);
	_rfm69_sdr_send_buffer();
	rfm69_set_mode(RF69_MODE_TX);	
	for ( byte i = start+1; i < bufferSize+start; i++ )
	{
		while (( rfm69_readreg(REG_IRQFLAGS2) & RF_IRQFLAGS2_FIFONOTEMPTY) != 0x00 );	
		_rfm69_sdr_fill_buffer(buffer[i]);
		_rfm69_sdr_send_buffer();		
	}
	
	while (( rfm69_readreg(REG_IRQFLAGS2) & RF_IRQFLAGS2_PACKETSENT) == 0x00 );
		
	rfm69_set_mode(RF69_MODE_STANDBY);
	rfm69_set_poweramp(0);
}

void rfm69_send(const byte* buffer, byte start, byte bufferSize)
{
	_rfm69_clean_rx();

	if ( bufferSize > MAX_DATA_LEN ) bufferSize = MAX_DATA_LEN;

	spi_select();
	spi_read(REG_FIFO | 0x80);
	spi_read(bufferSize);
	for ( byte i = start; i < bufferSize+start; i++ )
	spi_read(((byte*)buffer)[i]);
	spi_unselect();

	rfm69_set_mode(RF69_MODE_TX);
	while (( rfm69_readreg(REG_IRQFLAGS2) & RF_IRQFLAGS2_PACKETSENT) == 0x00 );
	
	rfm69_set_mode(RF69_MODE_STANDBY);
	rfm69_set_poweramp(0);
}

void rfm69_checkstate()
{
	if ( rfm69_internal.mode == RF69_MODE_RX)
	{

		if( rfm69_readreg(REG_IRQFLAGS2) & RF_IRQFLAGS2_PAYLOADREADY )
		{
			if (rfm69_syncreceived==1)
			{
				rfm69_data.fei_value=(rfm69_readreg(REG_FEIMSB)<<8)+rfm69_readreg(REG_FEILSB);
				rfm69_data.fei_valid=1;
				rfm69_syncreceived=0;
			}
			rfm69_set_mode(RF69_MODE_STANDBY);
			spi_select();
			spi_read(REG_FIFO & 0x7f);
			rfm69_data.data_len = spi_read(0);
			for ( byte i = 0; i < rfm69_data.data_len; i++)	{rfm69_data.data[i] = spi_read(0);	}
			spi_unselect();
			rfm69_set_mode(RF69_MODE_RX);
			//byte flags=rfm69_data.data[4];
		/*	if ((rfm69_settings.fei_method==RFM69_FEIMETHOD_ON_NEXT_PREAMBLE) && (flags&FLAGS_POSTAMBLE))
			{
				var_delay(rfm69_settings.fei_postamble_delay);
				if (rfm69_settings.fei_precious)
				{
					int f1=rfm69_read_fei();var_delay(1);
					int f2=rfm69_read_fei();
					var_delay(1);
					int f3=rfm69_read_fei();
					rfm69_data.fei_valid=1;
					if (abs(f1-f2)>10) rfm69_data.fei_valid=0;
					if (abs(f1-f3)>10) rfm69_data.fei_valid=0;
					if (abs(f2-f3)>10) rfm69_data.fei_valid=0;
					rfm69_data.fei_value=((f1+f2+f3)/3);	
				}else
				{
					int f1=rfm69_read_fei();
					rfm69_data.fei_valid=1;
					rfm69_data.fei_value=f1;				
				}
				//rfm69_send_preamble(3);
				rfm69_syncreceived=0;
			}*/
		}
		
		if(rfm69_readreg(REG_IRQFLAGS1) & RF_IRQFLAGS1_SYNCADDRESSMATCH )
		{
			rfm69_data.rssi=rfm69_read_rssi();
			if((rfm69_settings.fei_method==RFM69_FEIMETHOD_ON_DATA)&&(rfm69_syncreceived==0))
			{
				rfm69_syncreceived=1;
				rfm69_writereg(REG_AFCFEI, RF_AFCFEI_FEI_START);
			}
		}
	}
}

int rfm69_read_fei()
{
	rfm69_set_mode(RF69_MODE_RX);
	while ( (rfm69_readreg(REG_IRQFLAGS1) & RF_IRQFLAGS1_RXREADY) == 0x00 ){}

	rfm69_writereg(REG_AFCFEI, RF_AFCFEI_FEI_START);
	while ( (rfm69_readreg(REG_AFCFEI) & RF_AFCFEI_FEI_DONE) == 0x00 );

	int result=(rfm69_readreg(REG_FEIMSB)<<8)+rfm69_readreg(REG_FEILSB);
	rfm69_data.rssi=rfm69_read_rssi();
	return result;
}

void rfm69_receive_start()
{
	rfm69_set_mode(RF69_MODE_STANDBY);
	rfm69_data.data_len = 0;
	rfm69_data.rssi = 0;
	if ( rfm69_readreg(REG_IRQFLAGS2) & RF_IRQFLAGS2_PAYLOADREADY )
	rfm69_writereg(REG_PACKETCONFIG2, ( rfm69_readreg(REG_PACKETCONFIG2) & 0xFB ) | RF_PACKET2_RXRESTART);
	rfm69_set_mode(RF69_MODE_RX);
	
	while ( (rfm69_readreg(REG_IRQFLAGS1) & RF_IRQFLAGS1_RXREADY) == 0x00 ){;}
	rfm69_writereg(REG_AFCFEI, RF_AFCFEI_AFC_CLEAR);
	
	rfm69_data.fei_value=0;
	rfm69_data.fei_valid=0;
	rfm69_syncreceived=0;
}

byte rfm69_check_receive()
{
	rfm69_checkstate();
	if ( rfm69_internal.mode == RF69_MODE_RX && rfm69_data.data_len > 0 )
	{
		rfm69_set_mode(RF69_MODE_STANDBY);
		return 1;
	}
	else
	if ( rfm69_internal.mode == RF69_MODE_RX )
	{
		return 0;
	}
	rfm69_receive_start();
	return 0;
}

void rfm69_set_encrypt(const char* key)
{
	rfm69_set_mode(RF69_MODE_STANDBY);
	if ( key != 0 )
	{
		spi_select();
		spi_read(REG_AESKEY1 | 0x80);
		for ( byte i = 0; i < 16; i++ )
		spi_read(key[i]);
		spi_unselect();
	}
	rfm69_writereg(REG_PACKETCONFIG2, ( rfm69_readreg(REG_PACKETCONFIG2) & 0xFE ) | ( key ? 1 : 0 ));
}

void rfm69_meashure_rssi() 
{
		rfm69_writereg(REG_RSSICONFIG, RF_RSSI_START);
		while ( (rfm69_readreg(REG_RSSICONFIG) & RF_RSSI_DONE) == 0x00 );	
}

int rfm69_read_rssi()
{
	int rssi = 0;
	rssi = -rfm69_readreg(REG_RSSIVALUE);
	rssi >>= 1;
	return rssi;
}

byte rfm69_readreg(byte addr)
{
	spi_select();
	spi_read(addr & 0x7F);
	byte regval = spi_read(0);
	spi_unselect();
	return regval;
}

void rfm69_writereg(byte addr, byte value)
{
	spi_select();
	spi_read(addr | 0x80);
	spi_read(value);
	spi_unselect();
}

byte rfm69_read_temp()
{
	rfm69_set_mode(RF69_MODE_STANDBY);
	rfm69_writereg(REG_TEMP1, RF_TEMP1_MEAS_START);
	return ~rfm69_readreg(REG_TEMP2) + 90;
}

void rfm69_rccal()
{
	rfm69_writereg(REG_OSC1, RF_OSC1_RCCAL_START);
	while ( (rfm69_readreg(REG_OSC1) & RF_OSC1_RCCAL_DONE) == 0x00 );
}

byte rfpacket_receive()
{
	if (rfm69_check_receive())
	{
		rfpacket_request.rssi=rfm69_data.rssi;
		rfpacket_request.address_for =((uint32_t)rfm69_data.data[0])<<14;
		rfpacket_request.address_for+=((uint32_t)rfm69_data.data[1])<<7;
		rfpacket_request.address_for+=((uint32_t)rfm69_data.data[2]);
		rfpacket_request.command=rfm69_data.data[3];
		rfpacket_request.flags=rfm69_data.data[4];
		rfpacket_request.data_length=rfm69_data.data[5];
		for (int i=0; i<MAX_DATA_LEN-6; i++) rfpacket_request.data[i]=rfm69_data.data[6+i];
		if (rfpacket_request.address_for==0){return 1;}
		if (rfpacket_params.promiscuos){return 1;}
		if (rfpacket_request.address_for==rfpacket_params.address){return 1;}
	}
	return 0;
}

void rfpacket_send()
{
	rfm69_data.data[0]=rfpacket_params.address>>14;
	rfm69_data.data[1]=rfpacket_params.address>>7;
	rfm69_data.data[2]=rfpacket_params.address;
	rfm69_data.data[3]=rfpacket_response.response;
	rfm69_data.data[4]=rfpacket_response.flags;	
	rfm69_data.data[5]=rfpacket_response.data_length;
	for (int i=0; i<MAX_DATA_LEN-6; i++) rfm69_data.data[6+i]=rfpacket_response.data[i];
	
	rfm69_send(rfm69_data.data,0,rfpacket_response.data_length+5);
}