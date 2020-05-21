
#define USE_ARDUINO_INTERRUPTS false
#include <PulseSensorPlayground.h>
#include <NeoSWSerial.h>
#include <NMEAGPS.h>
#include <Timer.h>

PulseSensorPlayground pulseSensor;
NMEAGPS gps;//Declaramos el objeto gps
gps_fix fix;
Timer t;

NeoSWSerial serialgps( 10, 11 ); // pin 4 to GPS Tx y 3 to GPS Rx

//Emergency Details & ERROR
String Details = "Name: Aren Dale N. Hataas / 09975917579 S/O: Sheena Cuaton / 09263139731";     //Syntax Patient/Contact Number Relative/Contact Number
String EmergencyNumber = "09263139731";                                                             //Emergency Contact Number (i.e. CDRRMO Officer)
int error = 0;
//Emergency Details 

//Pulse Rate Settings
const int PulseWire = A0;
const int Threshold = 550;   // Adjust this number to avoid noise when idle
byte samplesUntilReport;
const byte SAMPLES_PER_SERIAL_SAMPLE = 10;
//Pulse Rate Settings

//LED Indicator
const int blue = 5;
const int green = 6;
const int red = 7;
const int pulseBlinkLED = 13;    // Pin 13 is the on-board LED
//LED Indicators

//Miscellaneous Variables
const int buzzer = 8;
int count = 0;
double Lat = 0.00;
double Long = 0.00;
char latitude[15];
char longitude[15];
int start = 0;
unsigned long t1;
unsigned long t2;
unsigned long delta;
int Changecount = 0;
const int buttonPin = 2;     // the number of the pushbutton pin
char mess[160];
int pulseflag = 0;
int smsflag = 0;
int Pulse;
String Site = "www.google.com.ph/maps/place/";
String Link = "NO GPS RECEPTION";
//Miscellaneous Variables


void setup() 
{
  Serial.begin(115200);
  serialgps.begin(9600);//GPS serial port begins
  
  pulseSensor.analogInput(PulseWire);
  pulseSensor.setThreshold(Threshold);
  samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
  pinMode(blue, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(buzzer, OUTPUT);
  
  if (!pulseSensor.begin()) 
    {
      for(;;) 
        {
          // Flash the led to show things didn't work.
          digitalWrite(pulseBlinkLED, LOW);
          delay(50);
          digitalWrite(pulseBlinkLED, HIGH);
          delay(50);
        }
    }
  for (int i = 1; i < 16; i++)
  {
    if(i%2 == 0)
      {
        digitalWrite(red, HIGH);
        digitalWrite(green, LOW);
      }
    else
      {
        digitalWrite(red, LOW);
        digitalWrite(green, HIGH);
      }
    delay(1000);
  }
      digitalWrite(red, LOW);
      digitalWrite(green, LOW);
    t.every(60000, Disp);
    attachInterrupt(0, pin_ISR, CHANGE);
}

void loop() 
{
    if(start==0)
    {
       pulseSensor.blinkOnPulse(pulseBlinkLED);
       start++;
    }
    
    //Timer Update
    t.update();
    
    //GPS Routine
    if(gps.available( serialgps ))
    {
      // Save new GPS data when it is available, once per second.
      fix = gps.read();
      if (fix.valid.location) 
          {
            Lat = (fix.latitude());
            Long = (fix.longitude()); 
            dtostrf(Lat, 8 , 6, latitude);
            dtostrf(Long, 8 , 6, longitude);
            Link = Site + latitude + "," + longitude;
          }
      else
          {
            //Serial.println("NO GPS RECEPTION");
            Link = "NO GPS RECEPTION";
          }
    }

    
    //Pulse Sensor Routine   
    if (pulseSensor.sawNewSample()) 
    {
      if (--samplesUntilReport == (byte) 0)
        {
          samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
          if (pulseSensor.sawStartOfBeat()) 
          {
            count++;
            pulseSensor.outputBeat();
          }
        }
    }


  //Push Button Routine 
  if ((delta < 3000 && delta > 50) || pulseflag == 1)         //Short Press, button time is greater than 50ms and lesser than 3s or if pulse rate is lesser than 30 bpm
      {
          digitalWrite( buzzer, HIGH);
          Serial.print("Short Press!  ");
          Serial.print("Interval: ");
          Serial.println(delta);
          Serial.print("AT+CMGF=1\r");
          delay(500);
          Serial.print("AT+CMGS=");
          Serial.print("\"");
          Serial.print(EmergencyNumber);    
          Serial.print("\"");     
          Serial.print("\r");
          delay(500);
          Serial.print(mess);
          delay(500);
          Serial.println((char)26);
          delay(500);
          delta = 0;
          pulseflag = 0;
          smsflag = 1;
      } 
     if ( delta >=3000 && smsflag == 1)                       //Long Press, if button time is greater than 3s
      {
          digitalWrite( buzzer, LOW);
          Serial.print("Long Press!  ");
          Serial.print("Interval: ");
          Serial.println(delta);
          Serial.print("AT+CMGF=1\r");
          delay(500);
          Serial.print("AT+CMGS=");
          Serial.print("\"");
          Serial.print(EmergencyNumber);    
          Serial.print("\"");     
          Serial.print("\r");
          delay(500);
          Serial.print("Disregard Previous Message! Misclick");
          delay(500);
          Serial.println((char)26);
          delay(500);
          delay(500);
          delta = 0;
          smsflag = 0;
      }
}

void Disp()
{
  Pulse = count + error;
  Serial.print("Pulse Rate Count Detected: ");
  Serial.println(Pulse);
 
  if (Pulse <=30)
  {
    pulseflag = 1;
  }
  if (Pulse <= 60) 
    {
      pulseSensor.blinkOnPulse(blue);
    } 
   else if (Pulse <100 && Pulse > 60) 
    {
      pulseSensor.blinkOnPulse(green);
    } 
   else if (Pulse >= 100) 
    {
      pulseSensor.blinkOnPulse(red);
    }
  String SOS = "I NEED HELP!";
  String out = SOS + "\n" + Details + "\n" + "Pulse: " + Pulse + "\n" + Link;
  out.toCharArray(mess, 160);
  Serial.println(mess);
   count = 0;
   Pulse = 0;
}

void pin_ISR() 
{
  Changecount = Changecount + 1;
  if (Changecount % 2 == 1)
    {
      t1 = millis();
    }
  if (Changecount % 2 == 0)
    {
      t2 = millis();
      delta = t2 - t1;
      if (delta > 6000)
        {
          Changecount = 0;
          t1 = 0;
          t2 = 0;
          delta = 0;
        }
    }  
}
