#include "nRF24L01p.h"
#include "nRF24L01_define_map.h"
#include "SPI/SPI.h"

NRF24L01pClass * myRadio;

// Interrupt variable
volatile unsigned char IRQ_state = 0x00;

// Used to check the status of a given bit in a variable
#define CHECK_BIT(var,pos) ((var & (1 << pos)) == (1 << pos))




void setup()
{
  /* add setup code here */
  Serial.begin(9600);
  Serial.println("Begin startup");
  
  // Begin SPI communication
  SPI.begin();
  
  myRadio = new NRF24L01pClass;
  myRadio->init(9,10);
  
  myRadio->begin();
  Serial.println("Configure Radio");
  myRadio->configRadio(0,1);

  // Clear any interrupts
  clear_interrupts();

  unsigned char tmp_state [] = {0x00};
  tmp_state[0] = *myRadio->readRegister(STATUS, 0);
  Serial.print("STATUS: ");
  Serial.println(tmp_state[0], BIN);
 
  myRadio->setDebugVal(123);
  Serial.print("**************************************  debug_val  = ");
  Serial.println(myRadio->getDebugVal());
  
    // Attach interrupt 0 (digital pin 2 on the Arduino Uno)
    //	This interrupt monitors the interrupt pin (IRQ) from the nRF24L01 Radio
    //  The IRQ is normally high, and active low
	//  The IRQ is triggered at:
	delay(100); // Make sure all the configuration is completed before attaching the interrupt
    attachInterrupt(0, IRQ_resolve, FALLING);
}




void loop()
{

  /* add main program code here */
  
  if (IRQ_state == 1)
  {
	  IRQ_reset_and_respond();
  }
  
	unsigned char * ptmp_array;
	Serial.println("----------------------------------");
	Serial.print("STATUS: ");
	ptmp_array = myRadio->readRegister(STATUS,0);
	Serial.println(*ptmp_array, BIN);
	
	Serial.println("Transmit code abc");
	unsigned char tmpData [] = {1};
	myRadio->txData(tmpData, 1);

	delay(2000);
}






unsigned char setBit(unsigned char byteIn, int bitNum, boolean setClear)
{
	if(setClear == 1)
	byteIn |= (1<<bitNum);
	else
	byteIn &= ~(1<<bitNum);
	
	return byteIn;
}



//******* INTERRUPTS **************** INTERRUPTS ***************** INTERRUPTS ****************************

/* IRQ_resolve
Resolve the attachInterrupt function quickly
*/
void IRQ_resolve()
{
	// Get the IRQ code from the receiver and assign it to IRQ_state variable
	//unsigned char * p_tmp;
	//Serial.println("IRQ");
	//IRQ_state = * myRadio->readRegister(STATUS,1); // this returns a pointer, so I dereferenced it to the unsigned char for IRQ_state
	IRQ_state = 1;
}



/* IRQ_reset_and_respond
Reset the IRQ in the radio STATUS register
Also resolve the condition which triggered the interrupt
*/
void IRQ_reset_and_respond(void)
{
	Serial.println(" ------------------ RESPOND TO IRQ --------------------- ");
	unsigned char tmp_state [] = {0x00};
	unsigned char tmp_status = * myRadio->readRegister(STATUS,1);
	
	if CHECK_BIT(tmp_status,4) // Maximum number of TX retries interrupt
	{
		Serial.println("Max TX retries IRQ");
	}
	if CHECK_BIT(tmp_status,5) // Data sent TX FIFO interrupt
	{
		Serial.println("Data Sent TX FIFO IRQ");
	}
	if CHECK_BIT(tmp_status,6) // Data ready RX FIFO interrupt
	{
		Serial.println("Data ready RX FIFO IRQ");
	}
	
	clear_interrupts();
	
}

void clear_interrupts(void)
{
	// Clear any interrupts
	unsigned char tmp_state [] = {1<<RX_DR};
	myRadio->writeRegister(STATUS, tmp_state, 1);
	tmp_state [0] = 1<<TX_DS;
	myRadio->writeRegister(STATUS, tmp_state, 1);
	tmp_state [0] = 1<<MAX_RT;
	myRadio->writeRegister(STATUS, tmp_state, 1);
	// Flush the TX register
	myRadio->flushTX();
}



