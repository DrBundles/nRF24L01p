/* NRF24L01p.h - Library for NRF24L01p Radio
	Created by: Steve Lammers, 2/21/2015
	Released to the public domain.

 SPI Settings
	2MHz
	Mode 0
	MSB First
  SPI is started in the Arduino loop, Settings changed in this file, search for "SPI"
	
 Pins
	CE   9
	CSN  10
	MOSI 11
	MISO 12
	SCK  13
	IRQ  2
*/ 

//#define AVR
#define ARDUINO

#ifndef ARDUINO
  #include <avr/io.h>
  #include <util/delay.h>
  #include "USART.h"
#endif

#include "NRF24L01p.h"
#include "nRF24L01_define_map.h"
#include "string.h"
#include "SPI.h"

// Used to check the status of a given bit in a variable
#define CHECK_BIT(var,pos) ((var & (1 << pos)) == (1 << pos))

#ifdef ARDUINO
  NRF24L01p::NRF24L01p(int _cepin, int _csnpin)
  {
    ce_pin = _cepin;
    csn_pin = _csnpin;
  }
#else
  //NRF24L01p::NRF24L01p(int _cepin, int _csnpin)
  NRF24L01p::NRF24L01p()
  {
    //ce_pin = _cepin;
    //csn_pin = _csnpin;
    initSPImaster();
  }
  
#endif

// digitalWrite_csn 
#ifdef ARDUINO
  void NRF24L01p::digitalWrite_csn(bool val)
  {
    if (val == HIGH){
	    digitalWrite(csn_pin, HIGH);
    }
    else{
	    digitalWrite(csn_pin, LOW);
    }
  }
#else
  //digitalWrite function for ATMEL
  void NRF24L01p::digitalWrite_csn(bool val)
  {
    if (val == HIGH){
	    SPI_CSN_PORT |= (1 << SPI_CSN);                       /* Write CSN pin HIGH */
    }
    else{
	    SPI_CSN_PORT &= ~(1 << SPI_CSN);                      /* Write CNS pin LOW */
    }
  }
#endif

// digitalWrite_csn 
#ifdef ARDUINO
  void NRF24L01p::digitalWrite_ce(bool val)
  {
    if (val == HIGH){
	    digitalWrite(ce_pin, HIGH);
    }
    else{
	    digitalWrite(ce_pin, LOW);
    }
  }
#else
  //digitalWrite function for ATMEL
  void NRF24L01p::digitalWrite_ce(bool val)
  {
    if (val == HIGH){
	    SPI_CE_PORT |= (1 << SPI_CE);                       /* Write CSN pin HIGH */
    }
    else{
	    SPI_CE_PORT &= ~(1 << SPI_CE);                      /* Write CNS pin LOW */
    }
  }
#endif

// delay function for ATMEL
#ifndef ARDUINO
  void NRF24L01p::delay(int val)
  {
    _delay_ms(val);
  }
#endif

int NRF24L01p::get_ce_pin(void) 
{ 
	return ce_pin; 
}

void NRF24L01p::setDebugVal(int tmp_debug_val)
{
	debug_val = tmp_debug_val;
}
	
int NRF24L01p::getDebugVal(void)
{
	return debug_val;
}
/*SET BIT
Used to more easily set or clear bits in registers etc
@param byteIn is the byte to be worked on
@param bitNum is the bit to change, 0-7
@param setClear is the boolean value to set the bit 1 or 0
*/
unsigned char NRF24L01p::setBit(unsigned char byteIn, int bitNum, bool setClear)
{
	if(setClear == 1)
		byteIn |= (1<<bitNum);
	else
		byteIn &= ~(1<<bitNum);
		
	return byteIn;
}
	

void NRF24L01p::begin(void)
{
  #ifdef ARDUINO
    // Initialize pins-initialized already in ATMEGA-328-pinDefines.h
    pinMode(ce_pin, OUTPUT);
    pinMode(csn_pin, OUTPUT);
    
    //These SPI commands are for the Arduino SPI library, which is not used here
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
    // Arduino Uno operates at 16MHz, we want SPI to run at 2MHz
    SPI.setClockDivider(SPI_CLOCK_DIV8);
  #endif

}

void NRF24L01p::setup_data_pipes(unsigned char pipesOn [], const int fixedPayloadWidth)
{
	writeRegister(EN_RXADDR, pipesOn, 1);
	unsigned char widthArg [] = {(unsigned char)fixedPayloadWidth};
	writeRegister(RX_PW_P0, widthArg, 1);
}


void NRF24L01p::set_data_rate(const int dataRate)
{
	int RF_DR_LOW_val = 0;
	int RF_DR_HIGH_val = 1;
	switch(dataRate)
	{
		case 250:{ // 250-kBPS
		  RF_DR_LOW_val = 1;
		  RF_DR_HIGH_val = 0;
      break;}
		case 1:{ // 1-MBPS
		  RF_DR_LOW_val = 0;
		  RF_DR_HIGH_val = 0;
      break;}
		case 2:{ // 2-MBPS
		  RF_DR_LOW_val = 0;
		  RF_DR_HIGH_val = 1;
      break;}
		default:{
      #ifdef ARDUINO
		    printf("Data rate must be set to either 250 (kBPS), 1 (MBPS) or 2 (MBPS)");
      #else
		    printString("Data rate must be set to either 250 (kBPS), 1 (MBPS) or 2 (MBPS)");
      #endif
      break;}
	}
	unsigned char* tmp_RF_SETUP = readRegister(RF_SETUP, 1);
	*tmp_RF_SETUP = setBit(*tmp_RF_SETUP, RF_DR_LOW, RF_DR_LOW_val);   // Set RF_DR_LOW bit 
	*tmp_RF_SETUP = setBit(*tmp_RF_SETUP, RF_DR_HIGH, RF_DR_HIGH_val); // Set RF_DR_HIGH bit 
	unsigned char tmp_val [] = {*tmp_RF_SETUP};
	writeRegister(RF_SETUP, tmp_val, 1);
}


void NRF24L01p::writeRegister(unsigned char thisRegister, unsigned char thisValue [], int byteNum)
{
	// Must start with CSN pin high, then bring CSN pin low for the transfer
	// Transmit the command byte
	// Bring CSN pin back to high
	thisRegister = 0x20 | thisRegister;
	//digitalWrite(csn_pin, LOW);
	//SPI_CSN_PORT &= ~(1 << SPI_CSN);                      /* Write CNS pin LOW */
  digitalWrite_csn(LOW);
	
	SPI.transfer(thisRegister); // This is the register that is being written to
	int ind=0;
	while (ind < byteNum)
	{
		SPI.transfer(thisValue[ind]);
		ind = ind+1;
	}
	
	//digitalWrite(csn_pin, HIGH);
	//SPI_CSN_PORT |= (1 << SPI_CSN);                       /* Write CSN pin HIGH */
  digitalWrite_csn(HIGH);
}




unsigned char * NRF24L01p::readRegister(unsigned char thisRegister, int byteNum)
{
	// Must start with CSN pin high, then bring CSN pin low for the transfer
	// Transmit the command byte and the same number of dummy bytes as expected to receive from the register
	// Read the same number of bytes from radio plus the STATUS register as the first byte returned
	// Bring CSN pin back to high
	//digitalWrite(csn_pin, LOW);
	//SPI_CSN_PORT &= ~(1 << SPI_CSN);                      /* Write CSN pin LOW */
  digitalWrite_csn(LOW);
	
	SPI.transfer(thisRegister); // This is the register that is being read from
	int ind = 0;
	
	while (ind <= byteNum)
	{
    #ifdef ARDUINO
		  register_value[ind] = SPI.transfer(0x00); // First byte returned is the status, subsequent bytes are from register
    #else
		  SPI.transfer(0x00); // First byte returned is the status, subsequent bytes are from register
		  register_value[ind] = SPDR; 
    #endif
		//Serial.print("Register byte ");
		//Serial.print(ind);
		//Serial.print(" value = ");
		//Serial.println(register_value[ind], BIN);
		ind = ind+1;
	}
	
	//digitalWrite(csn_pin, HIGH);
	//SPI_CSN_PORT |= (1 << SPI_CSN);                       /* Write CSN pin HIGH */
  digitalWrite_csn(HIGH);
	
	return register_value;
	
}


/* CONFIG
Configure the NRF24L01p and startup
@param RXTX sets the radio into 1:Receive 0:Transmit
@param PWRUP_PWRDOWN 1:Power Up 0:Power Down
*/
void NRF24L01p::configRadio(bool RXTX, bool PWRUP_PWRDOWN)
{
	// CRC is enabled with a 2-byte encoding scheme
	unsigned char configByte = 0b00001111;
	// Set RXTX bit
	configByte = setBit(configByte,PRIM_RX,RXTX);
	// Set PWRUP_PWRDOWN bit
	configByte = setBit(configByte,PWR_UP,PWRUP_PWRDOWN);
	
	// Write array
	unsigned char writeBytes [5];
	writeBytes[0]=configByte;
	// Write to config register
	writeRegister(CONFIG, writeBytes, 1);
	
}


/* IRQ_reset_and_respond
Reset the IRQ in the radio STATUS register
Also resolve the condition which triggered the interrupt
*/
unsigned char NRF24L01p::IRQ_reset_and_respond(void)
{
	// Serial.println(" ------------------ RESPOND TO IRQ --------------------- ");
	unsigned char tmp_status = * readRegister(STATUS,1);
	
	clear_interrupts();
	
	return tmp_status;
	
}


void NRF24L01p::clear_interrupts(void)
{
	// Clear any interrupts
	unsigned char tmp_state [] = {1<<RX_DR};
	writeRegister(STATUS, tmp_state, 1);
	tmp_state [0] = 1<<TX_DS;
	writeRegister(STATUS, tmp_state, 1);
	tmp_state [0] = 1<<MAX_RT;
	writeRegister(STATUS, tmp_state, 1);
	// Flush the TX register
	flushTX();
}


/* txMode Transmit Mode
Put radio into transmission mode
*/
void NRF24L01p::txMode(void)
{
	configRadio(0,1);
	// CE is held LOW unless a packet is being actively transmitted, In which case it is toggled high for >10us
	//digitalWrite(ce_pin, LOW);
	//SPI_CE_PORT &= ~(1 << SPI_CE);                      /* Write CE pin low */
  digitalWrite_ce(LOW);
}
	
/* rMode Receive Mode
Put radio into receiving mode
*/
void NRF24L01p::rMode(void)
{
	configRadio(1,1);
	// CE HIGH monitors air and receives packets while in receive mode
	//digitalWrite(ce_pin, HIGH);
	//SPI_CE_PORT |= (1 << SPI_CE);                       /* Write CE pin HIGH */
  digitalWrite_ce(HIGH);
	// CE LOW puts the chip in standby and it no longer monitors the air
}


/* txData Transmit Data
Transmit data
@param DATA is the data to transmit
@param BYTE_NUM is the number of bytes to transmit 1-5
*/
void NRF24L01p::txData(unsigned char DATA [], int BYTE_NUM)
{
	// First the command byte (0xA0, W_TX_PAYLOAD) is sent and then the payload. 
	// The number of payload bytes sent must match the payload length of the receiver you are sending the payload to
	
	// Must start with CSN pin high, then bring CSN pin low for the transfer
	// Transmit the command byte
	// Bring CSN pin back to high
	
	//digitalWrite(csn_pin, LOW);
	//SPI_CSN_PORT &= ~(1 << SPI_CSN);                      /* Write CNS pin LOW */
  digitalWrite_csn(LOW);

	SPI.transfer(W_TX_PAYLOAD); // This is the register that is being written to
	int ind=0;
	//Serial.println("DATA"); //DEBUG
	while (ind < BYTE_NUM)
	{
		/*
		//DEBUG
		printString("Element ");
		printWord(ind);
		printString(": ");
		printWord(DATA[ind]);
		*/
		SPI.transfer(DATA[ind]);
		ind = ind+1;
		
	}
	digitalWrite_csn(HIGH);                                 /* Write CSN pin HIGH */

	// When sending packets, the CE pin (which is normally held low in TX operation) is set to high for a minimum of 10us to send the packet.
	digitalWrite_ce(HIGH);
	delay(1); 
	digitalWrite_ce(LOW);
	
	// Once the packet was sent, a TX_DS interrupt will occur
	// If auto-ack is enabled on the pipe, then TX_DS flag will only be sent if the packet actually gets through
	/*
	If the maximum amount of retries is hit, then the MAX_RT interrupt will
	become active. At this point, you should clear the interrupts and continue based on
	which interrupt was asserted. Also remember that, like the RX FIFO, the TX FIFO is
	three levels deep. This means that you can load up to three packets into the 24L01’s TX
	FIFO before you do the CE toggle to send them on their way.
	*/
	
	
}


/* rData Receive Data
Receive data
@param BYTE_NUM is the number of bytes to receive 1-5 (Why 1-5? is this true? test if this can be larger)
register values are read into nRF24L01Class.register_value array
*/
unsigned char * NRF24L01p::rData(int byteNum)
{

	// Bring CE low to disable the receiver
  digitalWrite_ce(LOW);                                    /* Write CE pin low */
	
	// Execute R_RX_PAYLOAD operation
	// First the command byte (0x61, R_RX_PAYLOAD) is sent and then the payload.
	// The number of payload bytes sent must match the payload length of the receiver you are sending the payload to
	
	digitalWrite_csn(LOW);                                   /* Write CSN pin low */
	
	SPI.transfer(R_RX_PAYLOAD); // This is the register that is being read from
	int ind = 0;
	
	while (ind <= byteNum)
	{
	  #ifdef ARDUINO
      register_value[ind] = SPI.transfer(0x00); // First byte returned is the status, subsequent bytes are from register
    #else
		  SPI.transfer(0x00); // First byte returned is the status, subsequent bytes are from register
		  register_value[ind] = SPDR;
    #endif
		//Serial.print("Register byte ");
		//Serial.print(ind);
		//Serial.print(" value = ");
		//Serial.println(register_value[ind]);
		ind = ind+1;
	}
	
	digitalWrite_csn( HIGH);

	// Bring CE high to re-enable the receiver
	digitalWrite_ce(HIGH);
	
	return register_value;
	
}


/* flushTX Flush TX FIFO

*/
void NRF24L01p::flushTX(void)
{
	// Must start with CSN pin high, then bring CSN pin low for the transfer
	// Transmit the command byte
	// Bring CSN pin back to high
	digitalWrite_csn(LOW);
	SPI.transfer(FLUSH_TX); // This is the register that is being written to
	digitalWrite_csn(HIGH);
}

/* flushTX Flush TX FIFO

*/
void NRF24L01p::flushRX(void)
{
	// Must start with CSN pin high, then bring CSN pin low for the transfer
	// Transmit the command byte
	// Bring CSN pin back to high
	digitalWrite_csn(LOW);
	SPI.transfer(FLUSH_RX); // This is the register that is being written to
	digitalWrite_csn(HIGH);
}



//NRF24L01p NRF24L01p;



