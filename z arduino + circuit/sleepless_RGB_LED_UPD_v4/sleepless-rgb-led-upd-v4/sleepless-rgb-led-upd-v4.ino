#include "neopixel.h"
#include "math.h"

const int TIMEDELAY  = 10;
const int NUMPIXELS  = 192;
const int PIXEL_PIN  = D6;
const int PIXEL_TYPE = WS2812B;

enum OPTIONS 
{ FINISH = 0
, MAXMSP                    // modified
, WIPE_RGB                  // modified
, RAINBOW                   // modified
, THEATERCHASE              // modified
, THEATERCHASERAINBOW       // modified
, CYCLONBOUNCE              // modified
, TWINKLE                   // modified
, SPARKLE                   // modified
, BOUNCINGBALLS
, BOUNCINGCOLORBALLS
, FIRE
, RUNNINGLIGHTS
, WIPE_GOFF                 // modified
, METEORRAIN
, SNOWSPARKLE
, STROBE
, NEWKITT
};

/* 8-bit gamma-correction table.
   Copy & paste this snippet into a Python REPL to regenerate:
   import math
   gamma=2.6
   for x in range(256):
    print("{:3},".format(int(math.pow((x)/255.0,gamma)*255.0+0.5))),
    if x&15 == 15: print
 */
static const uint8_t gamma8[256] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1,   1,   1,   1,   1,
    1,   1,   1,   1,   2,   2,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,
    3,   3,   4,   4,   4,   4,   5,   5,   5,   5,   5,   6,   6,   6,   6,   7,
    7,   7,   8,   8,   8,   9,   9,   9,   10,  10,  10,  11,  11,  11,  12,  12,
    13,  13,  13,  14,  14,  15,  15,  16,  16,  17,  17,  18,  18,  19,  19,  20,
    20,  21,  21,  22,  22,  23,  24,  24,  25,  25,  26,  27,  27,  28,  29,  29,
    30,  31,  31,  32,  33,  34,  34,  35,  36,  37,  38,  38,  39,  40,  41,  42,
    42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,
    58,  59,  60,  61,  62,  63,  64,  65,  66,  68,  69,  70,  71,  72,  73,  75,
    76,  77,  78,  80,  81,  82,  84,  85,  86,  88,  89,  90,  92,  93,  94,  96,
    97,  99,  100, 102, 103, 105, 106, 108, 109, 111, 112, 114, 115, 117, 119, 120,
    122, 124, 125, 127, 129, 130, 132, 134, 136, 137, 139, 141, 143, 145, 146, 148,
    150, 152, 154, 156, 158, 160, 162, 164, 166, 168, 170, 172, 174, 176, 178, 180,
    182, 184, 186, 188, 191, 193, 195, 197, 199, 202, 204, 206, 209, 211, 213, 215,
    218, 220, 223, 225, 227, 230, 232, 235, 237, 240, 242, 245, 247, 250, 252, 255
};

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
// IMPORTANT: Set pixel COUNT, PIN and TYPE
Adafruit_NeoPixel pixels(NUMPIXELS, PIXEL_PIN, PIXEL_TYPE);

const uint32_t localPort = 44444;       // UDP Port used for two way communication
UDP            Udp;                     // A UDP instance to let us send and receive packets over UDP

Timer         timedAnimation(TIMEDELAY, animate); // 
bool          bUpdateAnimation;

struct data_t {
  uint8_t option;
  uint8_t r;
  uint8_t g;
  uint8_t b;
} data;

void setup() {
  Udp.begin(localPort);
  pixels.begin();

  Particle.function("test", testAnimation);
}

void loop() {
  if (sizeof(data) == Udp.receivePacket((uint8_t*)&data, sizeof(data))) {
    bUpdateAnimation = true;
    timedAnimation.start();
  }
} 

int testAnimation(const char* command) {
  if(sscanf(command, "%hhu:%hhu,%hhu,%hhu", &data.option, &data.r, &data.g, &data.b) == 4) {
    bUpdateAnimation = true;
    timedAnimation.start();
    return *((int*)&data);      // return struct as 32bit integer
  }

  return -1;
}

void animate() {
  static uint32_t step = 0;
  static uint32_t msDelay = 0;
  static uint32_t msLastUpdate = 0;
  
  if (bUpdateAnimation) {
    step = 
    msDelay = 
    msLastUpdate = 0;
    
    // do preparation work for animation change
    
    bUpdateAnimation = false;
  }
  
  if (msDelay && millis() - msLastUpdate < msDelay) return;
  msLastUpdate = millis();
  
  switch (data.option) {
    case MAXMSP:
      colorAll(data.r, data.g, data.b);
      break;
        
    case WIPE_RGB: 
      msDelay = 50;
      switch(step / NUMPIXELS) {
        case 0 : pixels.setPixelColor(step++ % NUMPIXELS, 0xff, 0x00, 0x00); break; // red
        case 1 : pixels.setPixelColor(step++ % NUMPIXELS, 0x00, 0xff, 0x00); break; // green
        case 2 : pixels.setPixelColor(step++ % NUMPIXELS, 0x00, 0x00, 0xff); break; // blue
        default: step = 0; return;
      }
      pixels.show();
      break;
        
    case RAINBOW:
      msDelay = 10;
      for(int p = 0; p < NUMPIXELS; pixels.setPixelColor(p, Wheel((step + p) & 255)), p++);
      pixels.show();
      if (++step >= 256) step = 0; 
      break;
        
    case THEATERCHASE:
      msDelay = 50;
      switch(step / 32) {
        case 0: theaterChase(0x7f, 0x7f, 0x7f, step++); break; // White, half brightness
        case 1: theaterChase(0x7f, 0x00, 0x00, step++); break; // Red, half brightness
        case 2: theaterChase(0x00, 0x00, 0x7f, step++); break; // Blue, half brightn  
        default: step = 0; break;
      }
      break;
        
    case THEATERCHASERAINBOW:
      msDelay = 50;
      theaterChaseRainbow(step++);
      break;
        
    case CYCLONBOUNCE:
      msDelay = 10;
      cylonBounce(0xff, 0x00, 0x00, 4, step++); 
      break;
        
    case TWINKLE:
      msDelay = 100;
      twinkle(0xff, 0x00, 0x00, 10, step++);
      break;
        
    case SPARKLE: 
      msDelay = 0;
      sparkle(0xff, 0xff, 0xff); 
      break;
        
    case BOUNCINGBALLS: 
      BouncingBalls(0xff, 0x00, 0x00, 3);
      break;

    case BOUNCINGCOLORBALLS:
      BouncingColoredBalls(3, new byte[3][3]{ {0xff, 0x00, 0x00}, {0xff, 0xff, 0xff}, {0x00, 0x00, 0xff} });
      break;
        
    case FIRE:
      Fire(55, 120, 15);
      break;
        
    case RUNNINGLIGHTS:
      RunningLights(0xff, 0xff, 0x00, 50);
      break;
        
    case WIPE_GOFF: 
      msDelay = 50;
      switch(step / NUMPIXELS) {
        case 0 : pixels.setPixelColor(step++ % NUMPIXELS, 0x00, 0xff, 0x00); break; // green
        case 1 : pixels.setPixelColor(step++ % NUMPIXELS, 0x00, 0x00, 0x00); break; // off
        default: step = 0; break;
      }
      pixels.show();
      break;

    case METEORRAIN:
      meteorRain(0xff, 0xff, 0xff, random(1,20), random(50,250), true, random(1,40));
      break;
        
    case SNOWSPARKLE:
      SnowSparkle(0x0, 0x14, 0x14, 20, 200);
      break;
        
    case STROBE:
      Strobe(0xff, 0xff, 0xff, 10, 50, 1000);
      break;
        
    case NEWKITT:
      NewKITT(0xff, 0x00, 0x00, 8, 10, 50);
      break;

    case FINISH:  // finished
    default: // invalid
      timedAnimation.stop();
      pixels.clear();
      pixels.show();
      step = 
      msDelay = 
      msLastUpdate = 0;
      break;
  }
}

// ----- Some functions of our own for creating animated effects -----------------
// unblocked 
// Theater-marquee-style chasing lights.
void theaterChase(uint8_t r, uint8_t g, uint8_t b, uint32_t step) {
  for(int p = 0; p < pixels.numPixels(); p++) {
    if (!((p+step) % 3))  // every third pixel on
      pixels.setPixelColor(p, r, g, b);
    else                 // others
      pixels.setPixelColor(p, 0);
  }
  pixels.show();
}

/*
// Rainbow-enhanced theater marquee. 
void theaterChaseRainbow(uint32_t step) {
  static int initHue = 0;     // First pixel starts at red (hue 0)
  if (!step) initHue = 0;
  
  for(int p = 0; p < pixels.numPixels(); p++) {
    // hue of pixel 'c' is offset by an amount to make one full
    // revolution of the color wheel (range 65536) along the length
    // of the strip (strip.numPixels() steps):
    int hue = initHue + p * 65536L / pixels.numPixels();
    uint32_t color = gamma32(ColorHSV(hue, 255, 255)); // hue -> RGB
    pixels.setPixelColor(p, !((p - step) % 3) * color); // Set pixel 'c' to value 'color'
  }
  initHue += 65536 / 90; // One cycle of color wheel over 90 frames
  pixels.show();
}
*/

// Rainbow-enhanced theater marquee. 
void theaterChaseRainbow(uint32_t step) {
  for(int p = 0; p < pixels.numPixels(); p++) {
    pixels.setPixelColor(p, !((p - step) % 3) * Wheel((step + p) & 255));
  }
  pixels.show();
}

//Cylon bounce effect
void cylonBounce(uint8_t r, uint8_t g, uint8_t b, int eyeSize, uint32_t step) {
  div_t x = div(step, NUMPIXELS);
  eyeSize = constrain(eyeSize, 2, NUMPIXELS / 10);
  int p = constrain(x.rem, eyeSize / 2, NUMPIXELS - eyeSize / 2);
  if (x.quot % 2) 
    p = NUMPIXELS - p;

  colorAll(0, 0, 0); 
  for (int i = 1; i <= eyeSize/2; i++) {
    pixels.setColorScaled(p+i, r, g, b, 200 * (1 - 1.75*i / eyeSize));
    pixels.setColorScaled(p-i, r, g, b, 200 * (1 - 1.75*i / eyeSize));
  }
  pixels.show();
}

// Twinkle effect
void twinkle(uint8_t r, uint8_t g, uint8_t b, int count, uint32_t step) {
  if (step % count) {
    pixels.setPixelColor(random(NUMPIXELS), r, g, b);
  }
  else {
    colorAll(0, 0, 0);
  }
  pixels.show();
}

// color one pixel - clear it again next time round
// (starts off with previous pattern and eventually leaves all but one off)
void sparkle(uint8_t r, uint8_t g, uint8_t b) {
  colorAll(0, 0, 0);
  static int  pixel = 0;
  pixels.setPixelColor(pixel, 0);
  pixel = random(NUMPIXELS);
  pixels.setPixelColor(pixel, r, g, b);
  pixels.show();
}

// ------------------ need unblocking -------------------------------------------

// bouncing balls
void BouncingBalls(byte r, byte g, byte b, int ballCount) {
  float gravity = -9.81;
  int startHeight = 1;
 
  float height[ballCount];
  float impactVelocityStart = sqrt( -2 * gravity * startHeight );
  float impactVelocity[ballCount];
  float timeSinceLastBounce[ballCount];
  int   position[ballCount];
  long  clockTimeSinceLastBounce[ballCount];
  float dampening[ballCount];
 
  for (int i = 0 ; i < ballCount ; i++) {  
    clockTimeSinceLastBounce[i] = millis();
    height[i] = startHeight;
    position[i] = 0;
    impactVelocity[i] = impactVelocityStart;
    timeSinceLastBounce[i] = 0;
    dampening[i] = 0.90 - float(i)/pow(ballCount,2);
  }

  while (true) {
    for (int i = 0 ; i < ballCount ; i++) {
      timeSinceLastBounce[i] =  millis() - clockTimeSinceLastBounce[i];
      height[i] = 0.5 * gravity * pow( timeSinceLastBounce[i]/1000 , 2.0 ) + impactVelocity[i] * timeSinceLastBounce[i]/1000;
 
      if ( height[i] < 0 ) {                      
        height[i] = 0;
        impactVelocity[i] = dampening[i] * impactVelocity[i];
        clockTimeSinceLastBounce[i] = millis();
 
        if ( impactVelocity[i] < 0.01 ) {
          impactVelocity[i] = impactVelocityStart;
        }
      }
      position[i] = round( height[i] * (NUMPIXELS - 1) / startHeight);
    }
 
    for (int i = 0 ; i < ballCount ; i++) {
      pixels.setPixelColor(position[i], r, g, b);
    }
   
    pixels.show();
    pixels.clear();
// delay(30); ???
  }
}

// meteor rain
void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {  
    pixels.clear();
// delay(30); ???
 
  for(int i = 0; i < NUMPIXELS+NUMPIXELS; i++) {
    // fade brightness all LEDs one step
    for(int j=0; j<NUMPIXELS; j++) {
      if( (!meteorRandomDecay) || (random(10)>5) ) {
        fadeToBlack(j, meteorTrailDecay );        
      }
    }
   
    // draw meteor
    for(int j = 0; j < meteorSize; j++) {
      if( ( i-j <NUMPIXELS) && (i-j>=0) ) {
        pixels.setPixelColor(i-j, red, green, blue);
      }
    }
   
    pixels.show();
    delay(SpeedDelay);
  }
}

void fadeToBlack(int ledNo, byte fadeValue) {
    uint32_t oldColor;
    uint8_t r, g, b;

    oldColor = pixels.getPixelColor(ledNo);
    r = oldColor >> 16;
    g = oldColor >> 8;
    b = oldColor;

    r = (r<=10) ? 0 : (int) r - (r * fadeValue / 255);
    g = (g<=10) ? 0 : (int) g - (g * fadeValue / 255);
    b = (b<=10) ? 0 : (int) b - (b * fadeValue / 255);
   
    pixels.setPixelColor(ledNo, r, g, b);
}

// fire
void Fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[NUMPIXELS];
  int cooldown;
 
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < NUMPIXELS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUMPIXELS) + 2);
   
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
 
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= NUMPIXELS - 1; k >= 2; k--) {
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
      Position++; // = 0; //Position + Rate;
      for(int i=0; i<NUMPIXELS; i++) {
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
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0;
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i)/pow(BallCount,2);
  }

  while (true) {
    for (int i = 0 ; i < BallCount ; i++) {
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
      pixels.setPixelColor(Position[i],colors[i][0],colors[i][1],colors[i][2]);
    }
   
    pixels.show();
    pixels.clear();
// delay(30); ???
  }
}

// snow sparkle
void SnowSparkle(byte r, byte g, byte b, int SparkleDelay, int SpeedDelay) {
  colorAll(r, g, b);

  int p = random(NUMPIXELS);
  pixels.setPixelColor(p, 0xff, 0xff, 0xff);
  pixels.show();
  delay(SparkleDelay);
  pixels.setPixelColor(p, r, g, b);
  pixels.show();
  delay(SpeedDelay);
}

// strobe
void Strobe(byte r, byte g, byte b, int StrobeCount, int FlashDelay, int EndPause){
  for(int j = 0; j < StrobeCount; j++) {
  colorAll(r, g, b);
    pixels.show();
    delay(FlashDelay);
  colorAll(0, 0, 0);
    pixels.show();
    delay(FlashDelay);
  }
 
 delay(EndPause);
}

// new KITT
void NewKITT(byte r, byte g, byte b, int EyeSize, int SpeedDelay, int ReturnDelay){
  RightToLeft(r, g, b, EyeSize, SpeedDelay, ReturnDelay);
  LeftToRight(r, g, b, EyeSize, SpeedDelay, ReturnDelay);
  OutsideToCenter(r, g, b, EyeSize, SpeedDelay, ReturnDelay);
  CenterToOutside(r, g, b, EyeSize, SpeedDelay, ReturnDelay);
  LeftToRight(r, g, b, EyeSize, SpeedDelay, ReturnDelay);
  RightToLeft(r, g, b, EyeSize, SpeedDelay, ReturnDelay);
  OutsideToCenter(r, g, b, EyeSize, SpeedDelay, ReturnDelay);
  CenterToOutside(r, g, b, EyeSize, SpeedDelay, ReturnDelay);
}

void CenterToOutside(byte r, byte g, byte b, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i =((NUMPIXELS-EyeSize)/2); i>=0; i--) {
    colorAll(r, g, b);
// delay(30);??
   
    pixels.setPixelColor(i, r/10, g/10, b/10);
    for(int j = 1; j <= EyeSize; j++) {
      pixels.setPixelColor(i+j, r, g, b);
    }
    pixels.setPixelColor(i+EyeSize+1, r/10, g/10, b/10);
   
    pixels.setPixelColor(NUMPIXELS-i, r/10, g/10, b/10);
    for(int j = 1; j <= EyeSize; j++) {
      pixels.setPixelColor(NUMPIXELS-i-j, r, g, b);
    }
    pixels.setPixelColor(NUMPIXELS-i-EyeSize-1, r/10, g/10, b/10);
   
    pixels.show();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void OutsideToCenter(byte r, byte g, byte b, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i = 0; i<=((NUMPIXELS-EyeSize)/2); i++) {
    colorAll(r, g, b);
// delay(30);??
   
    pixels.setPixelColor(i, r/10, g/10, b/10);
    for(int j = 1; j <= EyeSize; j++) {
      pixels.setPixelColor(i+j, r, g, b);
    }
    pixels.setPixelColor(i+EyeSize+1, r/10, g/10, b/10);
   
    pixels.setPixelColor(NUMPIXELS-i, r/10, g/10, b/10);
    for(int j = 1; j <= EyeSize; j++) {
      pixels.setPixelColor(NUMPIXELS-i-j, r, g, b);
    }
    pixels.setPixelColor(NUMPIXELS-i-EyeSize-1, r/10, g/10, b/10);
   
    pixels.show();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void LeftToRight(byte r, byte g, byte b, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i = 0; i < NUMPIXELS-EyeSize-2; i++) {
    colorAll(r, g, b);
// delay(30);??
    pixels.setPixelColor(i, r/10, g/10, b/10);
    for(int j = 1; j <= EyeSize; j++) {
      pixels.setPixelColor(i+j, r, g, b);
    }
    pixels.setPixelColor(i+EyeSize+1, r/10, g/10, b/10);
    pixels.show();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void RightToLeft(byte r, byte g, byte b, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i = NUMPIXELS-EyeSize-2; i > 0; i--) {
    colorAll(r, g, b);
// delay(30);??
    pixels.setPixelColor(i, r/10, g/10, b/10);
    for(int j = 1; j <= EyeSize; j++) {
      pixels.setPixelColor(i+j, r, g, b);
    }
    pixels.setPixelColor(i+EyeSize+1, r/10, g/10, b/10);
    pixels.show();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

// A 32-bit variant of gamma8[]that applies the same function
// to all components of a packed RGB or WRGB value.
uint32_t gamma32(uint32_t x) {
  uint8_t *y = (uint8_t *)&x;
  // All four bytes of a 32-bit value are filtered even if RGB (not WRGB),
  // to avoid a bunch of shifting and masking that would be necessary for
  // properly handling different endianisms (and each byte is a fairly
  // trivial operation, so it might not even be wasting cycles vs a check
  // and branch for the RGB case). In theory this might cause trouble *if*
  // someone's storing information in the unused most significant byte
  // of an RGB value, but this seems exceedingly rare and if it's
  // encountered in reality they can mask values going in or coming out.
  for (uint8_t i = 0; i < 4; i++)
    y[i] = gamma8[y[i]];
  return x; // Packed 32-bit return
}

uint32_t ColorHSV(uint16_t hue, uint8_t sat, uint8_t val) {
  uint8_t r, g, b;

  // Convert hue to R,G,B (nested ifs faster than divide+mod+switch):
  if (hue < 510) { // Red to Green-1
    b = 0;
    if (hue < 255) { //   Red to Yellow-1
      r = 255;
      g = hue;       //     g = 0 to 254
    } else {         //   Yellow to Green-1
      r = 510 - hue; //     r = 255 to 1
      g = 255;
    }
  } else if (hue < 1020) { // Green to Blue-1
    r = 0;
    if (hue < 765) { //   Green to Cyan-1
      g = 255;
      b = hue - 510;  //     b = 0 to 254
    } else {          //   Cyan to Blue-1
      g = 1020 - hue; //     g = 255 to 1
      b = 255;
    }
  } else if (hue < 1530) { // Blue to Red-1
    g = 0;
    if (hue < 1275) { //   Blue to Magenta-1
      r = hue - 1020; //     r = 0 to 254
      b = 255;
    } else { //   Magenta to Red-1
      r = 255;
      b = 1530 - hue; //     b = 255 to 1
    }
  } else { // Last 0.5 Red (quicker than % operator)
    r = 255;
    g = b = 0;
  }

  // Apply saturation and value to R,G,B, pack into 32-bit result:
  uint32_t v1 = 1 + val;  // 1 to 256; allows >>8 instead of /255
  uint16_t s1 = 1 + sat;  // 1 to 256; same reason
  uint8_t s2 = 255 - sat; // 255 to 0
  return ((((((r * s1) >> 8) + s2) * v1) & 0xff00) << 8) |
         (((((g * s1) >> 8) + s2) * v1) & 0xff00) |
         (((((b * s1) >> 8) + s2) * v1) >> 8);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void fill(uint8_t r, uint8_t g, uint8_t b, uint16_t first, uint16_t count) {
  uint16_t i, end;

  if (first >= NUMPIXELS) {
    return; // If first LED is past end of strip, nothing to do
  }

  // Calculate the index ONE AFTER the last pixel to fill
  if (count == 0) {
    // Fill to end of strip
    end = NUMPIXELS;
  } else {
    // Ensure that the loop won't go past the last pixel
    end = first + count;
    if (end > NUMPIXELS)
      end = NUMPIXELS;
  }

  for (i = first; i < end; i++) {
    pixels.setPixelColor(i, r, g, b);
  }
}

// Set all pixels in the strip to a solid color
void colorAll(uint8_t r, uint8_t g, uint8_t b) {
  for(int p = 0; p < pixels.numPixels(); pixels.setPixelColor(p++, r, g, b));
  pixels.show();
}