/* NRF24L01p.h - Library for NRF24L01p Radio
	Created by: Steve Lammers, 2/21/2015
	Released to the public domain.
*/
#ifndef NRF24L01p_h
#define NRF24L01p_h
//#include "Arduino.h"
#include "SPI.h"

// TODO
// Protected vs private variables (incl _private variable names)

class NRF24L01p
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
	//void init(int _cepin, int _csnpin); // Constructor prototype declaration, See Radio.cpp for definition
	NRF24L01p(int _cepin, int _csnpin); // Constructor prototype declaration, See Radio.cpp for definition
	
	
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
	
	/*SETUP DATA PIPES
	Setup the data pipes for TX and RX
	*/
	void setup_data_pipes(unsigned char pipesOn [], const int fixedPayloadWidth);
	
	/*SETUP RF DATA RATE
	Set data rate between 1MBPS or 2MBPS
	dataRate = 250 for 250-kbps
			 = 1 for 1-MBPS
			 = 2 for 2-MBPS
	*/
	void set_data_rate(const int dataRate);
	

	/* Write Register
	
	*/
	void writeRegister(unsigned char thisRegister, unsigned char thisValue [5], int byteNum);
	
	/* Read Register
	
	*/
	unsigned char * readRegister(unsigned char thisRegister, int byteNum);
	
	/* CONFIG
	Configure the NRF24L01p and startup
	@param RXTX sets the radio into 1:Receive 0:Transmit
	@param PWRUP_PWRDOWN 1:Power Up 0:Power Down
	*/
	void configRadio(boolean RXTX, boolean PWRUP_PWRDOWN);
	
	/* IRQ_reset_and_respond
	Reset the IRQ in the radio STATUS register
	Also resolve the condition which triggered the interrupt
	*/
	unsigned char IRQ_reset_and_respond(void);
	
	/* IRQ clear interrupts
	*/
	void clear_interrupts(void);
	
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
	Receive data
	@param DATA is the data to transmit
	@param BYTE_NUM is the number of bytes to transmit 1-5
	register values are read into NRF24L01Class.register_value array
	*/
	unsigned char * rData(int byteNum);
	
	
	/* flushTX Flush tX FIFO
	*/
	void flushTX(void);
	
	/* flushTX Flush RX FIFO
	*/
	void flushRX(void);
	

};

#endif

