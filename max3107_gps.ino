#include <Wire.h>
#include <inttypes.h>


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
** Startup with SW and HW reset
**
** Arguments: -
**
** return value:  -
**********************************************************/

void MAX3107_RST_Process()
{
  Serial.println("Reset process running");
  /*
    Wait for the MAX 3107 to come out of reset
    Wait for IRQ PIN to come up, as this
    indicates that the MAX3107 reset is complete
  */

  while (IRQ_PIN == 0);

  /* Perform a hardware reset */

  digitalWrite(RST, LOW);
  delay(1);
  digitalWrite(RST, HIGH);
  while (IRQ_PIN == 0);

  /* Perform a software reset */

  MAX3107_I2C_Write(MAX3107_CLKSOURCE, 0b00011000);
  MAX3107_I2C_Write(MAX3107_MODE2, 0x01);
  MAX3107_I2C_Write(MAX3107_MODE2, 0x00);
  while (IRQ_PIN == 0);

}

/***********************************************************
** Configuring Modes and Clocking
**
** Arguments:
** loopback: TRUE, if loopback should be enabled
**
** return value:  -
**********************************************************/

void MAX3107_I2C_Init(bool loopback)
{
  Serial.println("Initialization process running");
  /* Configure clocking */

  MAX3107_I2C_Write(MAX3107_CLKSOURCE, 0b00011000);
  MAX3107_I2C_Write(MAX3107_DIVLSB, 24);
  MAX3107_I2C_Write(0x0B, 3);

  /*Configure modes*/

  if (loopback)
  {
    MAX3107_I2C_Write(MAX3107_MODE2, 0b10101000); // Loop back
  }
  else
  {
    MAX3107_I2C_Write(MAX3107_MODE2, 0b10001000);
  }

  /*Enable Interrupts*/

  MAX3107_I2C_Write(MAX3107_IRQEN, 0b11111111);

  // Flow control - not needed
  // GPIOs - not needed
}

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

bool MAX3107_I2C_Write(byte port, byte val)
{
  byte error;

  while (!Serial);
  
  Wire.beginTransmission(byte(0x2C)); // Datasheet + I2C scan
  Wire.write(port);
  Wire.write(val);
  error = Wire.endTransmission();

  if (error != 0)
  {
    return false;
  }
  return true;
}

/***********************************************************
** Read one byte from the specified register in the MAX3107
**
** Arguments:
** port: MAX3107 register address
**
** return value:  Register containments
**
**********************************************************/

void MAX3107_I2C_Read(byte port)
{
  uint8_t req;
  char reading;
  byte i2c_error;
  Wire.begin(SDA_PIN, SCL_PIN); // Wire comm. begin
  while (!Serial);

  Wire.beginTransmission(byte(0x2C)); // Datasheet + I2C scan
  Wire.write(port);
  i2c_error = Wire.endTransmission();

  if (i2c_error != 0)
  {
    Serial.println("I2C communication failed at #1");
  }

  static bool checksum = false;
  static uint8_t checksumcnt = 0;
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.requestFrom(44, 8);
  while (1 <= Wire.available())
  {
    reading = Wire.read();
    if (reading == '$')
    {
      Serial.println();
      checksum = false;
      checksumcnt = 0;
    }
    if(reading == '*')
    {
      checksum = true;
    }
    if(checksum)
    {
      checksumcnt = checksumcnt + 1;
    }
    if(!((reading == 10 | reading == 13) | (checksum == 3)))
    {
      Serial.print(reading);
    }
  }
  i2c_error = Wire.endTransmission();
}


/*************************************************************************************************************************************/
void setup() {
  /*
     PIN Definitions
  */
  Serial.begin(115200);
  Serial.println("I2C communication: Start");
  Wire.begin(SDA_PIN, SCL_PIN); // Wire comm. begin

  pinMode(I2C, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(RST, OUTPUT);
  digitalWrite(I2C, LOW);
  digitalWrite(A0, LOW);
  digitalWrite(A1, LOW);

  MAX3107_RST_Process();
  MAX3107_I2C_Init(false);

  Wire.begin(SDA_PIN, SCL_PIN); // Wire comm. begin
  while (!Serial);
  Wire.beginTransmission(byte(0x2C)); // Datasheet + I2C scan
  Wire.write(0x00);
  byte error;
  error = Wire.endTransmission();
  if (error != 0)
  {
    Serial.println("I2C communication failed at #1");
  }
}

void loop() {
  //MAX3107_I2C_Write(0x09,2);   // RX tiltása
  MAX3107_I2C_Read(0x00); // Ezt úgy kéne, hogy csak akkor, ha van FIFO-ban adat
  //MAX3107_I2C_Write(0x09,0);    // RX engedélyezése
  delay(10);
}
