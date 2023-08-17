#include <Arduino.h>
#include <Wire.h>
#include <WireData.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <LowPower.h>

// I2C slave address
const int i2cSlaveAddress = 0x08;

// bucket tips counter
volatile long tipsCount = 0;

// previous bucket state
volatile bool stateEven = false;
volatile bool stateOdd = false;

// interrupt routine to detect rain gauge tips with debounce
void bucketTipIsrEven()
{
  if (!stateEven)
  {
    stateEven = true;
    stateOdd = false;
    tipsCount++;
  }
}

// interrupt routine to detect rain gauge tips with debounce
void bucketTipIsrOdd()
{
  if (!stateOdd)
  {
    stateOdd = true;
    stateEven = false;
    tipsCount++;
  }
}

// i2c routine to report tips counter status
void i2cRequestIsr()
{
  wireWriteData(tipsCount);
  Serial.print("Telemetry request.  ");
  Serial.print(tipsCount);
  Serial.println(" reported to  Master.");
}

void setup()
{

  clock_prescale_set(clock_div_2); // reduce clock speed to 8 MHz

  // set input pin modes
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

  // init i2c as slave unit
  Wire.begin(i2cSlaveAddress);
  Wire.onRequest(i2cRequestIsr);

  // enable external interrupts to count bucket tips
  attachInterrupt(digitalPinToInterrupt(2), bucketTipIsrEven, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), bucketTipIsrOdd, FALLING);

  // initialize serial line
  Serial.begin(115200); // monitor speed is 57600 bps due to reduced clock speed
}

void loop()
{
  Serial.print("Tick count: ");
  Serial.println(tipsCount);
  Serial.println("Spink!");
  delay(200);
  Serial.flush();
  // enter sleep monde
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}