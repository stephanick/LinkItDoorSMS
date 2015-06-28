#include <LGSM.h>

/*
  SMS sending door sensor
  
  Using the LinkIt development board along with the light sensor and
  buzzer available in the Grove Starter Kit, the following code
  will detect a light beam being interrupted (like when a door is
  opened) and send an SMS.  Additionally, it will continue to beep/buzz
  as long as the light beam is interrupted.
  
  modified 27 June 2015
  by James Stephanick
 */

const int mainLedPin = 13;      // LED on the main board
const int lightSensorPin = A0;  // Grove Light Sensor connect to A0
const int speakerPin = 3;       // Grove Buzzer connect to D3
const int threshold = 700;      // Threshold, if light level is lower, alarm

int cnt=0;
int phase=0;
int openFlag= 0;
int gsmReady = 0;

void playTone(int tone, int duration) {
    for (long i = 0; i < duration * 1000L; i += tone * 2) {
        digitalWrite(speakerPin, HIGH);
        delayMicroseconds(tone);
        digitalWrite(speakerPin, LOW);
        delayMicroseconds(tone);
    }
}

void playNote(char note, int duration) {
    char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
    int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };

    // play the tone corresponding to the note name
    for (int i = 0; i < 8; i++) {
        if (names[i] == note) {
            playTone(tones[i], duration);
        }
    }
}

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize LED and buzzer pins as an output.
  pinMode(mainLedPin, OUTPUT);
  pinMode(speakerPin, OUTPUT);

  // setup the 'MediaTek USB Modem Port' to output debug data at 115200 baud
  Serial.begin(115200);
}

// the loop function runs over and over again forever
void loop() {
  
    // debug info, homw many times through loop
    Serial.print("cnt is: ");
    Serial.print( cnt );
    cnt++;
    
    // debug info, is SMS on line?
    // if it doesn't come up, check the orientation of the SIM card
    Serial.print("  gsm: ");
    Serial.print( LSMS.ready() );
  
    // Read the value of the light sensor. The light sensor is an analog sensor.
    int sensorValue = analogRead(lightSensorPin);
    Serial.print("  sensor: ");
    Serial.println( sensorValue );

    // if the light beam is interrupted, assume that the door is opened
    if ( sensorValue < threshold )
    {
      if ( openFlag == 0 )
      {
        // first detection, different tone and send SMS
        playNote('g', 300 );
        
        if ( LSMS.ready() )
        {
          LSMS.beginSMS("2065551198");
          LSMS.print("Score!  Door open?");
          LSMS.endSMS();
        }
        
        // indicate that SMS sent to keep us from continually sending SMSs
        openFlag = 1;
      }
      else
      {
        // already sent an SMS, so simply make noise this time
        playNote('c', 300 );
      }
    }
    else
    {
      // indicate the door has been close so that when it is
      // opened again, we will send another SMS
      openFlag = 0;
    }
    
    // blink/toggle the LED on the main board.
    if ( phase == 0 )
    {
        phase = 1;
        digitalWrite(mainLedPin, HIGH);
    }
    else
    {
        phase = 0;
        digitalWrite(mainLedPin, LOW );
    }
    
    // wait a half second before repeating
    delay( 500 );
}
