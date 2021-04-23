#include <Wire.h>
#define SDA_PIN 33
#define SCL_PIN 26
#define I2C 35
#define A0 21
#define A1 20
#define RST 37
#define IRQ_PIN 19

/* Register map definitions */
#define MAX3107_RHR          (0x00)
#define MAX3107_THR          (0x00)
#define MAX3107_IRQEN        (0x01)
#define MAX3107_ISR          (0x02)
#define MAX3107_LSRINTEN     (0x03)
#define MAX3107_LSR          (0x04)
#define MAX3107_SPCLCHRINTEN (0x05)
#define MAX3107_SPCLCHARINT  (0x06)
#define MAX3107_STSINTEN     (0x07)
#define MAX3107_STSINT       (0x08)
#define MAX3107_MODE1        (0x09)
#define MAX3107_MODE2        (0x0a)
#define MAX3107_LCR          (0x0b)
#define MAX3107_RXTIMEOUT    (0x0c)
#define MAX3107_HDPLXDELAY   (0x0d)
#define MAX3107_IRDA         (0x0e)
#define MAX3107_FLOWLVL      (0x0f)
#define MAX3107_FIFOTRGLVL   (0x10)
#define MAX3107_TXFIFOLVL    (0x11)
#define MAX3107_RXFIFOLVL    (0x12)
#define MAX3107_FLOWCTRL     (0x13)
#define MAX3107_XON1         (0x14)
#define MAX3107_XON2         (0x15)
#define MAX3107_XOFF1        (0x16)
#define MAX3107_XOFF2        (0x17)
#define MAX3107_GPIOCONFG    (0x18)
#define MAX3107_GPIODATA     (0x19)
#define MAX3107_PLLCONFIG    (0x1a)
#define MAX3107_BRGCONFIG    (0x1b)
#define MAX3107_DIVLSB       (0x1c)
#define MAX3107_DIVMSB       (0x1d)
#define MAX3107_CLKSOURCE    (0x1e)
#define MAX3107_REVID        (0x1f)

/***********************************************************
** Write one byte to the specified register in the MAX3107
**
** Arguments: 
** port: MAX3107 register address to write to
** val: the value to write to that register
**
** return value:  TRUE – register successfully written
**                FALSE – I2C protocol error of some kind
**
**********************************************************/

bool MAX3107_I2C_Write(unsigned int port, unsigned char val)
{
  byte error;
  Wire.begin(SDA_PIN,SCL_PIN); // Wire comm. begin
  Serial.begin(115200); 
  while(!Serial);
  Serial.println("\nI2C_Write Running");

  Wire.beginTransmission(byte(0x2C)); // Datasheet + I2C scan
  Wire.write(port);
  Wire.write(val);
  error=Wire.endTransmission();

  if(error != 0)
  {
      Serial.println("I2C communication failed");
      return false;
  }
  return true;
}


void setup() {
  /*
   * PIN Definitions
   */
  pinMode(I2C,OUTPUT);
  pinMode(A0,OUTPUT);
  pinMode(A1,OUTPUT);
  pinMode(RST, OUTPUT);
  digitalWrite(I2C,LOW);
  digitalWrite(A0,LOW);
  digitalWrite(A1,LOW);

  /*
   * Wait for the MAX 3107 to come out of reset
   * Wait for IRQ PIN to come up, as this
   * indicates that the MAX3107 reset is complete
   */
  //while(IRQ_PIN == 0);

  /*
   * Perform a hardware reset
   */
  digitalWrite(RST, LOW);
  delay(1);
  digitalWrite(RST, HIGH);
  //while(IRQ_PIN == 0);

 /*
 * Software reset of the MAX3107
 */
  MAX3107_I2C_Write(MAX3107_MODE2, 0x01);
  MAX3107_I2C_Write(MAX3107_MODE2, 0x00);
  MAX3107_I2C_Write(MAX3107_MODE2, 10); // Loop back
}

byte reading = 0;

void loop() {

  MAX3107_I2C_Write(MAX3107_THR,2);
  delay(500);
  Wire.requestFrom(byte(0x2C),1); // Request 2 bytes from the slave device
  if(1 <= Wire.available())
  {
    reading = Wire.read();
    Serial.println(reading); 
  }
  delay(2500);
}
