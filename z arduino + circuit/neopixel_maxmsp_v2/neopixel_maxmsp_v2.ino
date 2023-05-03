// MaxMSP WS2812b RGB Lightstrip Controller
//
// This sketch receives RGB values from MaxMSP via the serial bus
// Then it sends those values to WS2812b RGB lightstrips
// The MaxMSP patch allows control of multiple lightstrips
// It also allows for remote control via UDP on multiple computers
//
// david cool 2022
// davidcool.com
//

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        6 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 192

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int r,g,b;
String myString;
char recieved;

void setup() {
  // Start serial listening for MaxMSP values  
  Serial.begin(115200);
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
  #endif
  // END of Trinket-specific code.
  // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.begin();
}

void loop() {

  if(Serial.available()) { // check to see if there's serial data in the buffer  
    
    recieved = Serial.read();
    myString += recieved;  
    
    if (recieved == 'B') {
      //Serial.print('B');
      //Serial.println(myString);
      int commaIndex = myString.indexOf(',');
      int secondCommaIndex = myString.indexOf(',', commaIndex+1);
      String firstValue = myString.substring(0, commaIndex);
      String secondValue = myString.substring(commaIndex+1, secondCommaIndex);
      String thirdValue = myString.substring(secondCommaIndex+1); 

      r = firstValue.toInt();
      g = secondValue.toInt();
      b = thirdValue.toInt(); 

      // The first NeoPixel in a strand is #0, second is 1, all the way up
      // to the count of pixels minus one.
      for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
        // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
        // Here we're using a moderately bright green color:
        pixels.setPixelColor(i, pixels.Color(r, g, b));
      }
      pixels.show();   // Send the updated pixel colors to the hardware.     
      myString ="";
    }

    if (recieved == 'C') {
      //Serial.print('C');
      //Serial.println(myString);
      String func = myString.substring(0);
      int f = func.toInt();
      
      if (f == 2) {
        while(1) {
          if(Serial.available()) { break; }
          colorWipe(pixels.Color(255,   0,   0), 50); // Red
          if(Serial.available()) { break; }
          colorWipe(pixels.Color(  0, 255,   0), 50); // Green
          if(Serial.available()) { break; }
          colorWipe(pixels.Color(  0,   0, 255), 50); // Blue
        }
      }
      if (f == 3) {
        while(1) {
          if(Serial.available()) { break; }
          rainbow(10); 
        } 
      }
      if (f == 4) {
        while(1) {
          if(Serial.available()) { break; }
          theaterChase(pixels.Color(127, 127, 127), 50); // White, half brightness
          if(Serial.available()) { break; }
          theaterChase(pixels.Color(127,   0,   0), 50); // Red, half brightness
          if(Serial.available()) { break; }
          theaterChase(pixels.Color(  0,   0, 127), 50); // Blue, half brightn  
        }
      }
      if (f == 5) {
        while(1) {
          if(Serial.available()) { break; }
          theaterChaseRainbow(50);  
        }
      }
      if (f == 6) {
        while(1) {
          if(Serial.available()) { break; }
          CylonBounce(0xff, 0, 0, 4, 10, 50); 
        }
      }
      if (f == 7) {
        while(1) {
          if(Serial.available()) { break; }
          Twinkle(0xff, 0, 0, 10, 100, false);
        }
      }
      if (f == 8) {
        while(1) {
          if(Serial.available()) { break; }
          Sparkle(0xff, 0xff, 0xff, 0); 
        }
      }
      if (f == 9) {
        while(1) {
          if(Serial.available()) { break; }
          BouncingBalls(0xff,0,0, 3);
        }
      }
      if (f == 10) {
        while(1) {
          if(Serial.available()) { break; }
          byte colors[3][3] = { {0xff, 0,0},
                        {0xff, 0xff, 0xff},
                        {0   , 0   , 0xff} };

          BouncingColoredBalls(3, colors);
        }
      }
      if (f == 11) {
        while(1) {
          if(Serial.available()) { break; }
          Fire(55,120,15);
        }
      }
      if (f == 12) {
        while(1) {
          if(Serial.available()) { break; }
          RunningLights(0xff,0xff,0x00, 50);
        }
      }
      if (f == 13) {
        while(1) {
          if(Serial.available()) { break; }
          colorWipe(0x00,0xff,0x00, 50);
          if(Serial.available()) { break; }
          colorWipe(0x00,0x00,0x00, 50);
        }
      }
      if (f == 14) {
        while(1) {
          if(Serial.available()) { break; }
          meteorRain(0xff,0xff,0xff,10, 64, true, 30);
        }
      }
      if (f == 15) {
        while(1) {
          if(Serial.available()) { break; }
          SnowSparkle(0x10, 0x10, 0x10, 20, 200);
        }
      }
      if (f == 16) {
        while(1) {
          if(Serial.available()) { break; }
          Strobe(0xff, 0xff, 0xff, 10, 50, 1000);
        }
      }
      if (f == 17) {
        while(1) {
          if(Serial.available()) { break; }
          NewKITT(0xff, 0, 0, 8, 10, 50);
        }
      }

      pixels.clear();
      pixels.show();
      myString ="";     
    }
  
  }      

}

// Some functions of our own for creating animated effects -----------------

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<pixels.numPixels(); i++) { // For each pixel in strip...
    if(Serial.available()) {
      break;
    }
    pixels.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    pixels.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    if(Serial.available()) {
      break;
    }
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      if(Serial.available()) {
        break;
      }
      pixels.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<pixels.numPixels(); c += 3) {
        if(Serial.available()) {
          break;
        }
        pixels.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      pixels.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    if(Serial.available()) {
      break;
    }
    // strip.rainbow() can take a single argument (first pixel hue) or
    // optionally a few extras: number of rainbow repetitions (default 1),
    // saturation and value (brightness) (both 0-255, similar to the
    // ColorHSV() function, default 255), and a true/false flag for whether
    // to apply gamma correction to provide 'truer' colors (default true).
    pixels.rainbow(firstPixelHue);
    // Above line is equivalent to:
    // strip.rainbow(firstPixelHue, 1, 255, 255, true);
    pixels.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    if(Serial.available()) {
      break;
    }
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      if(Serial.available()) {
        break;
      }
      pixels.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<pixels.numPixels(); c += 3) {
        if(Serial.available()) {
          break;
        }
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / pixels.numPixels();
        uint32_t color = pixels.gamma32(pixels.ColorHSV(hue)); // hue -> RGB
        pixels.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      pixels.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}

//Cylon bounce effect
void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay){

  for(int i = 0; i < NUMPIXELS-EyeSize-2; i++) {
    if(Serial.available()) { break; }
    pixels.fill(0,0,NUMPIXELS);
    pixels.setPixelColor(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      if(Serial.available()) { break; }
      pixels.setPixelColor(i+j, red, green, blue);
    }
    pixels.setPixelColor(i+EyeSize+1, red/10, green/10, blue/10);
    pixels.show();
    delay(SpeedDelay);
  }

  delay(ReturnDelay);

  for(int i = NUMPIXELS-EyeSize-2; i > 0; i--) {
    if(Serial.available()) { break; }
    pixels.fill(0,0,NUMPIXELS);
    pixels.setPixelColor(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      if(Serial.available()) { break; }
      pixels.setPixelColor(i+j, red, green, blue);
    }
    pixels.setPixelColor(i+EyeSize+1, red/10, green/10, blue/10);
    pixels.show();
    delay(SpeedDelay);
  }
 
  delay(ReturnDelay);
}

// Twinkle effect
void Twinkle(byte red, byte green, byte blue, int Count, int SpeedDelay, boolean OnlyOne) {
  pixels.fill(0,0,NUMPIXELS);
 
  for (int i=0; i<Count; i++) {
     if(Serial.available()) { break; }
     pixels.setPixelColor(random(NUMPIXELS),red,green,blue);
     pixels.show();
     delay(SpeedDelay);
     if(OnlyOne) {
       if(Serial.available()) { break; }
       pixels.fill(0,0,NUMPIXELS);
     }
   }
 
  delay(SpeedDelay);
}

// sparkle
void Sparkle(byte red, byte green, byte blue, int SpeedDelay) {
  int Pixel = random(NUMPIXELS);
  pixels.setPixelColor(Pixel,red,green,blue);
  pixels.show();
  delay(SpeedDelay);
  pixels.setPixelColor(Pixel,0,0,0);
}

// bouncing balls
void BouncingBalls(byte red, byte green, byte blue, int BallCount) {
  float Gravity = -9.81;
  int StartHeight = 1;
 
  float Height[BallCount];
  float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
  float ImpactVelocity[BallCount];
  float TimeSinceLastBounce[BallCount];
  int   Position[BallCount];
  long  ClockTimeSinceLastBounce[BallCount];
  float Dampening[BallCount];
 
  for (int i = 0 ; i < BallCount ; i++) {  
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0;
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i)/pow(BallCount,2);
  }

  while (true) {
    if(Serial.available()) { break; }
    for (int i = 0 ; i < BallCount ; i++) {
      if(Serial.available()) { break; }
      TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
      Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i]/1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i]/1000;
 
      if ( Height[i] < 0 ) {                      
        Height[i] = 0;
        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
        ClockTimeSinceLastBounce[i] = millis();
 
        if ( ImpactVelocity[i] < 0.01 ) {
          ImpactVelocity[i] = ImpactVelocityStart;
        }
      }
      Position[i] = round( Height[i] * (NUMPIXELS - 1) / StartHeight);
    }
 
    for (int i = 0 ; i < BallCount ; i++) {
      if(Serial.available()) { break; }
      pixels.setPixelColor(Position[i],red,green,blue);
    }
   
    pixels.show();
    pixels.fill(0,0,NUMPIXELS);
  }
}

// meteor rain
void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {  
  pixels.fill(0,0,NUMPIXELS);
 
  for(int i = 0; i < NUMPIXELS+NUMPIXELS; i++) {
    if(Serial.available()) { break; }
   
    // fade brightness all LEDs one step
    for(int j=0; j<NUMPIXELS; j++) {
      if(Serial.available()) { break; }
      if( (!meteorRandomDecay) || (random(10)>5) ) {
        fadeToBlack(j, meteorTrailDecay );        
      }
    }
   
    // draw meteor
    for(int j = 0; j < meteorSize; j++) {
      if(Serial.available()) { break; }
      if( ( i-j <NUMPIXELS) && (i-j>=0) ) {
        if(Serial.available()) { break; }
        pixels.setPixelColor(i-j, red, green, blue);
      }
    }
   
    pixels.show();
    delay(SpeedDelay);
  }
}

void fadeToBlack(int ledNo, byte fadeValue) {
 #ifdef ADAFRUIT_NEOPIXEL_H
    // NeoPixel
    uint32_t oldColor;
    uint8_t r, g, b;
    int value;
   
    oldColor = pixels.getPixelColor(ledNo);
    r = (oldColor & 0x00ff0000UL) >> 16;
    g = (oldColor & 0x0000ff00UL) >> 8;
    b = (oldColor & 0x000000ffUL);

    r=(r<=10)? 0 : (int) r-(r*fadeValue/256);
    g=(g<=10)? 0 : (int) g-(g*fadeValue/256);
    b=(b<=10)? 0 : (int) b-(b*fadeValue/256);
   
    pixels.setPixelColor(ledNo, r,g,b);
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   leds[ledNo].fadeToBlackBy( fadeValue );
 #endif  
}

// fire
void Fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[NUMPIXELS];
  int cooldown;
 
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < NUMPIXELS; i++) {
    if(Serial.available()) { break; }
    cooldown = random(0, ((Cooling * 10) / NUMPIXELS) + 2);
   
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
 
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= NUMPIXELS - 1; k >= 2; k--) {
    if(Serial.available()) { break; }
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
   
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < NUMPIXELS; j++) {
    if(Serial.available()) { break; }
    setPixelHeatColor(j, heat[j] );
  }

  pixels.show();
  delay(SpeedDelay);
}

void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
 
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    pixels.setPixelColor(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    pixels.setPixelColor(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    pixels.setPixelColor(Pixel, heatramp, 0, 0);
  }
}

// running lights
void RunningLights(byte red, byte green, byte blue, int WaveDelay) {
  int Position=0;
 
  for(int j=0; j<NUMPIXELS*2; j++)
  {
      if(Serial.available()) { break; }
      Position++; // = 0; //Position + Rate;
      for(int i=0; i<NUMPIXELS; i++) {
        if(Serial.available()) { break; }
        // sine wave, 3 offset waves make a rainbow!
        //float level = sin(i+Position) * 127 + 128;
        //setPixel(i,level,0,0);
        //float level = sin(i+Position) * 127 + 128;
        pixels.setPixelColor(i,((sin(i+Position) * 127 + 128)/255)*red,
                   ((sin(i+Position) * 127 + 128)/255)*green,
                   ((sin(i+Position) * 127 + 128)/255)*blue);
      }
     
      pixels.show();
      delay(WaveDelay);
  }
}

// color wipe
void colorWipe(byte red, byte green, byte blue, int SpeedDelay) {
  for(uint16_t i=0; i<NUMPIXELS; i++) {
      if(Serial.available()) { break; }
      pixels.setPixelColor(i, red, green, blue);
      pixels.show();
      delay(SpeedDelay);
  }
}

// bouncing colored balls
void BouncingColoredBalls(int BallCount, byte colors[][3]) {
  float Gravity = -9.81;
  int StartHeight = 1;
 
  float Height[BallCount];
  float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
  float ImpactVelocity[BallCount];
  float TimeSinceLastBounce[BallCount];
  int   Position[BallCount];
  long  ClockTimeSinceLastBounce[BallCount];
  float Dampening[BallCount];
 
  for (int i = 0 ; i < BallCount ; i++) {
    if(Serial.available()) { break; }  
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0;
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i)/pow(BallCount,2);
  }

  while (true) {
    if(Serial.available()) { break; }
    for (int i = 0 ; i < BallCount ; i++) {
      if(Serial.available()) { break; }
      TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
      Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i]/1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i]/1000;
 
      if ( Height[i] < 0 ) { 
        if(Serial.available()) { break; }                     
        Height[i] = 0;
        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
        ClockTimeSinceLastBounce[i] = millis();
 
        if ( ImpactVelocity[i] < 0.01 ) {
          if(Serial.available()) { break; }
          ImpactVelocity[i] = ImpactVelocityStart;
        }
      }
      Position[i] = round( Height[i] * (NUMPIXELS - 1) / StartHeight);
    }
 
    for (int i = 0 ; i < BallCount ; i++) {
      if(Serial.available()) { break; }
      pixels.setPixelColor(Position[i],colors[i][0],colors[i][1],colors[i][2]);
    }
   
    pixels.show();
    pixels.fill(0,0,NUMPIXELS);
  }
}

// snow sparkle
void SnowSparkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay) {
  int c = (red << 16) + (green << 8) + blue;
  pixels.fill(c,0,NUMPIXELS);
 
  int Pixel = random(NUMPIXELS);
  pixels.setPixelColor(Pixel,0xff,0xff,0xff);
  pixels.show();
  delay(SparkleDelay);
  pixels.setPixelColor(Pixel,red,green,blue);
  pixels.show();
  delay(SpeedDelay);
}

// strobe
void Strobe(byte red, byte green, byte blue, int StrobeCount, int FlashDelay, int EndPause){
  for(int j = 0; j < StrobeCount; j++) {
    if(Serial.available()) { break; }
    int c = (red << 16) + (green << 8) + blue;
    pixels.fill(c,0,NUMPIXELS);
    pixels.show();
    delay(FlashDelay);
    pixels.fill(0,0,NUMPIXELS);
    pixels.show();
    delay(FlashDelay);
  }
 
 delay(EndPause);
}

// new KITT
void NewKITT(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay){
  RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
}

void CenterToOutside(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i =((NUMPIXELS-EyeSize)/2); i>=0; i--) {
    if(Serial.available()) { break; }
    pixels.fill(0,0,NUMPIXELS);
   
    pixels.setPixelColor(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      if(Serial.available()) { break; }
      pixels.setPixelColor(i+j, red, green, blue);
    }
    pixels.setPixelColor(i+EyeSize+1, red/10, green/10, blue/10);
   
    pixels.setPixelColor(NUMPIXELS-i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      if(Serial.available()) { break; }
      pixels.setPixelColor(NUMPIXELS-i-j, red, green, blue);
    }
    pixels.setPixelColor(NUMPIXELS-i-EyeSize-1, red/10, green/10, blue/10);
   
    pixels.show();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void OutsideToCenter(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i = 0; i<=((NUMPIXELS-EyeSize)/2); i++) {
    if(Serial.available()) { break; }
    pixels.fill(0,0,NUMPIXELS);
   
    pixels.setPixelColor(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      if(Serial.available()) { break; }
      pixels.setPixelColor(i+j, red, green, blue);
    }
    pixels.setPixelColor(i+EyeSize+1, red/10, green/10, blue/10);
   
    pixels.setPixelColor(NUMPIXELS-i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      if(Serial.available()) { break; }
      pixels.setPixelColor(NUMPIXELS-i-j, red, green, blue);
    }
    pixels.setPixelColor(NUMPIXELS-i-EyeSize-1, red/10, green/10, blue/10);
   
    pixels.show();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void LeftToRight(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i = 0; i < NUMPIXELS-EyeSize-2; i++) {
    if(Serial.available()) { break; }
    pixels.fill(0,0,NUMPIXELS);
    pixels.setPixelColor(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      if(Serial.available()) { break; }
      pixels.setPixelColor(i+j, red, green, blue);
    }
    pixels.setPixelColor(i+EyeSize+1, red/10, green/10, blue/10);
    pixels.show();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void RightToLeft(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i = NUMPIXELS-EyeSize-2; i > 0; i--) {
    if(Serial.available()) { break; }
    pixels.fill(0,0,NUMPIXELS);
    pixels.setPixelColor(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      if(Serial.available()) { break; }
      pixels.setPixelColor(i+j, red, green, blue);
    }
    pixels.setPixelColor(i+EyeSize+1, red/10, green/10, blue/10);
    pixels.show();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}
