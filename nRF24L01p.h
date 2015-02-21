// nRF24L01p.h

#ifndef _NRF24L01P_h
#define _NRF24L01P_h


#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

class NRF24L01pClass
{
 protected:

	int ce_pin;  // Chip Enable pin (usually digital pin 9)
	int csn_pin; // Chip Select Not pin (usually digital pin 10)
	int payload_size; // Fixed size of payloads
	int pipe0_reading_address[5]; // Last address set on pipe 0 for reading
	int addr_width; // The address width to use - 3,4,or 5 bytes
	unsigned char register_value [6]; // The value of the last register read
	
	int debug_val;

 public:
	/*CONSTRUCTOR
		@param _cepin id the pin attached to Chip Enable on RF Module
		@param _cspin is the pin attached to Chip Select
	*/
	void init(int _cepin, int _csnpin); // Constructor prototype declaration, See Radio.cpp for defination
	
	
	/*DEBUG
		Tests for debugging
	*/
	int get_ce_pin(void);
	
	void setDebugVal(int debug_val);
	int getDebugVal(void);
	
	/*SET BIT
	Used to more easily set or clear bits in registers etc
	@param byteIn is the byte to be worked on
	@param bitNum is the bit to change, 0-7
	@param setClear is the boolean value to set the bit 1 or 0
	*/
	unsigned char setBit(unsigned char byteIn, int bitNum, boolean setClear);

	/*BEGIN
	Call this in setup, before calling any other methods
	*/
	void begin(void);
	

	/* Write Register
	
	*/
	void writeRegister(unsigned char thisRegister, unsigned char thisValue [5], int byteNum);
	
	/* Read Register
	
	*/
	unsigned char * readRegister(unsigned char thisRegister, int byteNum);
	
	/* CONFIG
	Configure the nRF24L01p and startup
	@param RXTX sets the radio into 1:Receive 0:Transmit
	@param PWRUP_PWRDOWN 1:Power Up 0:Power Down
	*/
	void configRadio(boolean RXTX, boolean PWRUP_PWRDOWN);
	
	
	/* txMode Transmit Mode
	Put radio into transmission mode
	*/
	void txMode(void);
	
	/* rMode Receive Mode
	Put radio into receiving mode
	*/
	void rMode(void);
	
	/* txData Transmit Data
	Transmit data
	@param DATA is the data to transmit
	@param BYTE_NUM is the number of bytes to transmit 1-5
	*/
	void txData(unsigned char DATA [5], int BYTE_NUM);
	
	
	/* rData Receive Data
	Receieve data
	@param DATA is the data to transmit
	@param BYTE_NUM is the number of bytes to transmit 1-5
	register values are read into NRF24L01Class.register_value array
	*/
	void rData(void);
	
	
	/* flushTX Flush tX FIFO
	*/
	void flushTX(void);
	

};

extern NRF24L01pClass NRF24L01p;

#endif

