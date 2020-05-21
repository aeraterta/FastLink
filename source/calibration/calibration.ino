
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

//Emergency Details
String Details = "Name: Anthony E. Raterta / 09171550845 C/O: Milcah E. Raterta / 09167760609";     //Syntax Patient/Contact Number Relative/Contact Number
String EmergencyNumber = "09263139731";                                                             //Emergency Contact Number (i.e. CDRRMO Officer)
String Number2 = "09171550845";
//Emergency Details 

//Pulse Rate Settings
const int PulseWire = A0;
const int Threshold = 650;   // Adjust this number to avoid noise when idle
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
int count = 0;
double Lat = 0.00;
double Long = 0.00;
char latitude[15];
char longitude[15];
int pulsecount = 1;

char mess[160];
int Pulse;
int Pulse1;
int Pulse2;
int Pulse3;
int Pulse4;
int Pulse5;
int Pulse6;
int Pulse7;
int Pulse8;
int Pulse9;
int Pulse10;
String pulseStream;
String Site = "www.google.com.ph/maps/place/";
String Link = "NO GPS RECEPTION";
int start;
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
}

void loop() 
{    
  if(start==0)
  {
    pulseSensor.blinkOnPulse(pulseBlinkLED);
    start++;
  }
 if (pulsecount <11)
  {
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
            Serial.println(count);
            pulseSensor.outputBeat();
          }
        }
    }
  }
  if(pulsecount == 11)
  {
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
          pulsecount++;
          digitalWrite(blue, LOW);
          digitalWrite(red, LOW);
          digitalWrite(green, LOW);
  }
}

void Disp()
{
  Pulse = count;
  switch (pulsecount)
  {
  case 1:
    Pulse1 = Pulse;
    break;
  case 2:
    Pulse2 = Pulse;
    break;
  case 3:
    Pulse3 = Pulse;
    break;
  case 4:
    Pulse4 = Pulse;
    break;
  case 5:
    Pulse5 = Pulse;
    break;
  case 6:
    Pulse6 = Pulse;
    break;
  case 7:
    Pulse7 = Pulse;
    break;
  case 8:
    Pulse8 = Pulse;
    break;
  case 9:
    Pulse9 = Pulse;
    break;
  case 10:
    Pulse10 = Pulse;
    break;  
  }
  pulsecount++;
  count = 0;
  if (Pulse <= 60) {
      // Too low
      pulseSensor.blinkOnPulse(blue);
    } else if (Pulse <=100 && Pulse > 60) {
       pulseSensor.blinkOnPulse(green);
    } else if (Pulse > 100) {
       pulseSensor.blinkOnPulse(red );
    }
  String pulseStream = String(Pulse1)  + "," + String(Pulse2) + "," + String(Pulse3) +  "," + String(Pulse4)  + "," + String(Pulse5)  + "," + String(Pulse6)  + "," + String(Pulse7)  + "," + String(Pulse8)  + "," + String(Pulse9)  + "," + String(Pulse10);
  String out ="Pulse: " + pulseStream + "\n" + Link;
  out.toCharArray(mess, 160);
  Serial.println(mess);
}
