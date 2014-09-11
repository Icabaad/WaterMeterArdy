/*
 
 */
#include <LiquidCrystal.h>
#include <Wire.h>
#define MAG_ADDR  0x0E //7-bit address for the MAG3110, doesn't change
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


  int newVal = 0; //New Reading from MAX3110
  int prevVal = 0; //Previous Reading from MAX3110
  int maxVal = 0; //minumum magnetic field measured
  int minVal = 0; //minumum magnetic field measured
  int newRising = 0; //newVal rising? true or false?
  int oldRising = 0; //previous rising state
  int revs = 0; //revolutions of water meter totalled for interval time
 long previousMillis = 0;
 long interval = 60000; //Send data via xbee every 60 Seconds
  int revTick = 0.25; //For converting revolutions(revs) of meter to millilitres. You have to figure this out
  int waterUse = 0;
  
  
void setup() {
  // declare the ledPin as an OUTPUT:

  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  config();            // turn the MAG3110 on
  lcd.begin(16, 2);
  lcd.clear();
}

void loop() {

  newVal = readx();

  if(newVal > maxVal) {
    maxVal = newVal;
  }
  if(newVal < minVal) {
    minVal = newVal;
  }  
  if(newVal >= (maxVal - 100)) {
    newRising = 1;
  }
  else if(newVal <= (minVal + 100)) {
    newRising = 0;
  } 
  
  if((oldRising == 0) && (newRising == 1)) {
    revs ++;
    oldRising = newRising;
  }

  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;  
    waterUse = (revs * revTick);
 }
  
  print_values();
  delay(5);
}


void config(void)
{
  Wire.beginTransmission(MAG_ADDR); // transmit to device 0x0E
  Wire.write(0x11);              // cntrl register2
  Wire.write(0x80);              // send 0x80, enable auto resets
  Wire.endTransmission();       // stop transmitting
  
  delay(15);
  
  Wire.beginTransmission(MAG_ADDR); // transmit to device 0x0E
  Wire.write(0x10);              // cntrl register1
  Wire.write(1);                 // send 0x01, active mode
  Wire.endTransmission();       // stop transmitting
}

void print_values(void)
{

  Serial.print("x=");
  lcd.setCursor(0,0);
  lcd.print("x="); 
  Serial.print(readx()); 
  lcd.print(readx()); 
  Serial.print(",");  
  lcd.setCursor(8,0);
  Serial.print("y=");   
  lcd.print("y=");  
  Serial.print(ready());
  lcd.print(ready()); 
  Serial.print(",");  
  lcd.setCursor(0,1);
  Serial.print("z=");  
  lcd.print("z=");   
  Serial.println(readz()); 
  lcd.print(readz());   
  lcd.setCursor(8,1);
  lcd.print(revs); 
}

int readx(void)
{
  int xl, xh;  //define the MSB and LSB
  
  Wire.beginTransmission(MAG_ADDR); // transmit to device 0x0E
  Wire.write(0x01);              // x MSB reg
  Wire.endTransmission();       // stop transmitting
 
  delayMicroseconds(2); //needs at least 1.3us free time between start and stop
  
  Wire.requestFrom(MAG_ADDR, 1); // request 1 byte
  while(Wire.available())    // slave may send less than requested
  { 
    xh = Wire.read(); // receive the byte
  }
  
  delayMicroseconds(2); //needs at least 1.3us free time between start and stop
  
  Wire.beginTransmission(MAG_ADDR); // transmit to device 0x0E
  Wire.write(0x02);              // x LSB reg
  Wire.endTransmission();       // stop transmitting
 
  delayMicroseconds(2); //needs at least 1.3us free time between start and stop
  
  Wire.requestFrom(MAG_ADDR, 1); // request 1 byte
  while(Wire.available())    // slave may send less than requested
  { 
    xl = Wire.read(); // receive the byte
  }
  
  int xout = (xl|(xh << 8)); //concatenate the MSB and LSB
  return xout;
}

int ready(void)
{
  int yl, yh;  //define the MSB and LSB
  
  Wire.beginTransmission(MAG_ADDR); // transmit to device 0x0E
  Wire.write(0x03);              // y MSB reg
  Wire.endTransmission();       // stop transmitting
 
  delayMicroseconds(2); //needs at least 1.3us free time between start and stop
  
  Wire.requestFrom(MAG_ADDR, 1); // request 1 byte
  while(Wire.available())    // slave may send less than requested
  { 
    yh = Wire.read(); // receive the byte
  }
  
  delayMicroseconds(2); //needs at least 1.3us free time between start and stop
  
  Wire.beginTransmission(MAG_ADDR); // transmit to device 0x0E
  Wire.write(0x04);              // y LSB reg
  Wire.endTransmission();       // stop transmitting
 
  delayMicroseconds(2); //needs at least 1.3us free time between start and stop
  
  Wire.requestFrom(MAG_ADDR, 1); // request 1 byte
  while(Wire.available())    // slave may send less than requested
  { 
    yl = Wire.read(); // receive the byte
  }
  
  int yout = (yl|(yh << 8)); //concatenate the MSB and LSB
  return yout;
}

int readz(void)
{
  int zl, zh;  //define the MSB and LSB
  
  Wire.beginTransmission(MAG_ADDR); // transmit to device 0x0E
  Wire.write(0x05);              // z MSB reg
  Wire.endTransmission();       // stop transmitting
 
  delayMicroseconds(2); //needs at least 1.3us free time between start and stop
  
  Wire.requestFrom(MAG_ADDR, 1); // request 1 byte
  while(Wire.available())    // slave may send less than requested
  { 
    zh = Wire.read(); // receive the byte
  }
  
  delayMicroseconds(2); //needs at least 1.3us free time between start and stop
  
  Wire.beginTransmission(MAG_ADDR); // transmit to device 0x0E
  Wire.write(0x06);              // z LSB reg
  Wire.endTransmission();       // stop transmitting
 
  delayMicroseconds(2); //needs at least 1.3us free time between start and stop
  
  Wire.requestFrom(MAG_ADDR, 1); // request 1 byte
  while(Wire.available())    // slave may send less than requested
  { 
    zl = Wire.read(); // receive the byte
  }
  
  int zout = (zl|(zh << 8)); //concatenate the MSB and LSB
  return zout;
}
