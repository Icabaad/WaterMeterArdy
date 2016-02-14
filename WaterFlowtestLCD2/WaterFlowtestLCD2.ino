/*
  Hook up to meter.
  Turn on water tap until it Runs
  Turn on Arduino Water Meter
*/

#include <XBee.h>
#include <Wire.h>
#define MAG_ADDR  0x0E //7-bit address for the MAG3110, doesn't change

//XBEE
XBee xbee = XBee();
// This is the XBee broadcast address.  You can use the address
// of any device you have also.
XBeeAddress64 Broadcast = XBeeAddress64(0x00000000, 0x0000ffff);

char Buffer[128];  // this needs to be longer than your longest packet.

int newVal = 0; //New Reading from MAX3110
int prevVal = 0; //Previous Reading from MAX3110
int avgVal = 0;
int maxVal = 0; //minumum magnetic field measured
int minVal = 32000; //minumum magnetic field measured
int newRising = 0; //newVal rising? true or false?
int oldRising = 0; //previous rising state
int revs = 0; //revolutions of water meter totalled for interval time
long previousMillis = 0;
long interval = 60000; //Send data via xbee every 60 Seconds
float revTick = 57.1428; //For converting revolutions(revs) of meter to millilitres. You have to figure this out
float waterUseTotal = 0;
float waterUseMinute = 0;
int test = 0;
int test2 = 0;
int upTime = 0; //In minutes
int batteryVPin = A0;
float batteryV = 0;
int ledPin = 9;
int ledValue = 0;
int calcVal = 0;
int valVal = 0;

void setup() {

  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  config();            // turn the MAG3110 on
  pinMode(13, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  digitalWrite(13, LOW);
  delay(1000);
  digitalWrite(13, HIGH);
  digitalWrite(9, LOW);
  delay(1000);
  digitalWrite(13, LOW);
  digitalWrite(9, HIGH);
  delay(1000);
  digitalWrite(9, LOW);



}
void loop() {
  //  delay(1000);
  prevVal = newVal;
  newVal = readz();
  /*
    if (revs <= 10) {
      delay(500);
      minVal = min(minVal, newVal);
      maxVal = max(maxVal, newVal);
      if ((newVal != minVal)) {
        Serial.print("Z: "); Serial.print(readz());Serial.print(" Min: "); Serial.print(minVal); Serial.print(" Max: "); Serial.println(maxVal);
      }
      else if ((newVal == maxVal )) {
        revs++;
        Serial.print("Min: "); Serial.print(minVal); Serial.print(" Max: "); Serial.println(maxVal);
      }
    }
  */
  // record the maximum sensor value
  if (newVal > maxVal) {
    maxVal = newVal;
    // Serial.print("Min: "); Serial.print(minVal); Serial.print(" Max: "); Serial.println(maxVal);
  }

  // record the minimum sensor value
  if (newVal < minVal) {
    minVal = newVal;
    //Serial.print("Min: "); Serial.print(minVal); Serial.print(" Max: "); Serial.println(maxVal);
  }
  valVal = maxVal - minVal;

  //calcVal = constrain(newVal, 0, 100);
  //calcVal = map(newVal, minVal, maxVal, 0, valVal);
  //  ledValue = map(calcVal, 0, 100, 0, 255);
  //delay(200);

  //  Serial.print("Min: "); Serial.print(minVal); Serial.print(" Max: "); Serial.println(maxVal);
  // Serial.print(newVal); Serial.print("-----"); Serial.println(calcVal);

  if ((newVal > prevVal) && (newVal >= (maxVal - 100))) {
    newRising = 1;
    //    Serial.println(newRising);
    analogWrite(ledPin, 40);
  }

  else if ((newVal < prevVal) && (newVal <= (minVal + 100))) {
    newRising = 0;
    //  Serial.println(newRising);
    analogWrite(ledPin, 0);
  }

  if ((oldRising == 0) && (newRising == 1)) {
    revs ++;

  }

  oldRising = newRising;

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    digitalWrite(9, LOW);
    batteryV = analogRead(batteryVPin) * 2;
    waterUseMinute = 0;
    previousMillis = currentMillis;
    waterUseMinute = waterUseMinute + (revs * revTick);
    waterUseTotal = waterUseTotal + waterUseMinute;
    revs = 0;
    upTime ++;
    char Buffer2[20];
    char buffer[20];
    dtostrf((waterUseMinute / 1000), 5, 2, Buffer);//
    strcpy(Buffer2, Buffer);
    strcat(Buffer2, ",");
    strcat(Buffer2, dtostrf(batteryV, 5, 2, Buffer));

    ZBTxRequest zbtx = ZBTxRequest(Broadcast, (uint8_t *)Buffer2, strlen(Buffer2));
    xbee.send(zbtx);

    //    digitalWrite(9, HIGH);

  }
  test = (readx());
  test2 = (ready());

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

int readx(void)
{
  int xl, xh;  //define the MSB and LSB

  Wire.beginTransmission(MAG_ADDR); // transmit to device 0x0E
  Wire.write(0x01);              // x MSB reg
  Wire.endTransmission();       // stop transmitting

  delayMicroseconds(2); //needs at least 1.3us free time between start and stop

  Wire.requestFrom(MAG_ADDR, 1); // request 1 byte
  while (Wire.available())   // slave may send less than requested
  {
    xh = Wire.read(); // receive the byte
  }

  delayMicroseconds(2); //needs at least 1.3us free time between start and stop

  Wire.beginTransmission(MAG_ADDR); // transmit to device 0x0E
  Wire.write(0x02);              // x LSB reg
  Wire.endTransmission();       // stop transmitting

  delayMicroseconds(2); //needs at least 1.3us free time between start and stop

  Wire.requestFrom(MAG_ADDR, 1); // request 1 byte
  while (Wire.available())   // slave may send less than requested
  {
    xl = Wire.read(); // receive the byte
  }

  int xout = (xl | (xh << 8)); //concatenate the MSB and LSB
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
  while (Wire.available())   // slave may send less than requested
  {
    yh = Wire.read(); // receive the byte
  }

  delayMicroseconds(2); //needs at least 1.3us free time between start and stop

  Wire.beginTransmission(MAG_ADDR); // transmit to device 0x0E
  Wire.write(0x04);              // y LSB reg
  Wire.endTransmission();       // stop transmitting

  delayMicroseconds(2); //needs at least 1.3us free time between start and stop

  Wire.requestFrom(MAG_ADDR, 1); // request 1 byte
  while (Wire.available())   // slave may send less than requested
  {
    yl = Wire.read(); // receive the byte
  }

  int yout = (yl | (yh << 8)); //concatenate the MSB and LSB
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
  while (Wire.available())   // slave may send less than requested
  {
    zh = Wire.read(); // receive the byte
  }

  delayMicroseconds(2); //needs at least 1.3us free time between start and stop

  Wire.beginTransmission(MAG_ADDR); // transmit to device 0x0E
  Wire.write(0x06);              // z LSB reg
  Wire.endTransmission();       // stop transmitting

  delayMicroseconds(2); //needs at least 1.3us free time between start and stop

  Wire.requestFrom(MAG_ADDR, 1); // request 1 byte
  while (Wire.available())   // slave may send less than requested
  {
    zl = Wire.read(); // receive the byte
  }

  int zout = (zl | (zh << 8)); //concatenate the MSB and LSB
  return zout;
}




