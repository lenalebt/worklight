#include <LightChrono.h>
#include <Chrono.h>

/*
  Serial Event example

 When new serial data arrives, this sketch adds it to a String.
 When a newline is received, the loop prints the string and
 clears it.

 A good test for this is to try it with a GPS receiver
 that sends out NMEA 0183 sentences.

 Created 9 May 2011
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/SerialEvent

 */

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      8

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Chrono chrono(Chrono::SECONDS);

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
long durationOfQuietTime = 0;
int minutesPerPixel = 90 / NUMPIXELS;
uint32_t color = pixels.Color(110,0,0); //RED

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(50);
  pixels.begin();
  pixels.setBrightness(64);
}

void checkSerialTransmissionComplete() {
  if (stringComplete) {
    //TODO: parse string, reset timers
    if (inputString.startsWith("Q ")) {
      durationOfQuietTime = 5;
      chrono.restart();
      String minutes = inputString.substring(2);
      String colorCode = minutes.substring(minutes.indexOf(" ") + 1, minutes.indexOf("\n"));
      minutes = minutes.substring(0, minutes.indexOf(" "));
      durationOfQuietTime = minutes.toInt();
      if (colorCode == ("RED")) {
        color = pixels.Color(255,0,0);
      } else if (colorCode == ("GREEN")) {
        color = pixels.Color(0,255,0);
      } else if (colorCode == ("BLUE")) {
        color = pixels.Color(0,0,255);
      } else if (colorCode == ("PINK")) {
        color = pixels.Color(255,60,140);
      } else if (colorCode == ("ORANGE")) {
        color = pixels.Color(255,135,0);
      } else if (colorCode == ("YELLOW")) {
        color = pixels.Color(255,210,0);
      }
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}
void lightPixels() {
  int minutesToPass = max(0, durationOfQuietTime - (chrono.elapsed() / 60));
  int pixelsToLight = min(NUMPIXELS, minutesToPass / minutesPerPixel);
  for (int i=0; i<pixelsToLight; i++) {
    pixels.setPixelColor(i, color);
  }
  for (int i=pixelsToLight; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, 0);
  }
  pixels.show();
}

void loop() {
  checkSerialTransmissionComplete();
  lightPixels();
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}


