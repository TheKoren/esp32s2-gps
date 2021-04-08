#include <Wire.h>
#define SDA_PIN 33
#define SCL_PIN 26
#define I2C 35
#define A0 21
#define A1 20


void setup() {
  // put your setup code here, to run once:
  pinMode(I2C,OUTPUT);
  pinMode(A0,OUTPUT);
  pinMode(A1,OUTPUT);
  digitalWrite(I2C,LOW);
  digitalWrite(A0,LOW);
  digitalWrite(A1,LOW);
  Wire.begin(SDA_PIN,SCL_PIN); // Wire comm. begin
  Serial.begin(115200); 
  while(!Serial);
  Serial.println("\nI2C Scanner");
}

void loop() {
  // put your main code here, to run repeatedly:
  byte error, address;
  bool device = false;
  Serial.println("Scanning...");
  for (address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();  // Ends a transmission to a slave device that was begun by beginTransmission() and transmits the bytes that were queued by write().
   /*
    * 0:success
    * 1:data too long to fit in transmit buffer
    * 2:received NACK on transmit of address
    * 3:received NACK on transmit of data
    * 4:other error
    */
    if(error == 0)
    {
      //Serial.print(error, DEC);
      Serial.print("I2C device found at address 0x");
      if(address<16)
      {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      device = true;
    }
    else if (error == 4)
    {
      //Serial.print(error, DEC);
      Serial.print("Unknown error at address 0x");
      if(address<16)
      {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }
  }

  if(device==true)
  {
    Serial.println("Device found!");
  }
  else
  {
    Serial.println("No I2C devices found");
  }
Serial.println("Done\n");
delay(5000);
}
