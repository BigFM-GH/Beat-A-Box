#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>


// Pin definitions
#define LED_PIN     2
#define BUZZER_PIN  8

// 
#define LED_COUNT  7  // NeoPixel count
#define BRIGHTNESS 200  // NeoPixel brightness, 0 (min) to 255 (max)
#define GOAL_POSITION 1 // Position of the light to reach

Adafruit_MPU6050 mpu; // MPU6050 instance
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800); // NeoPixel instance


void setup() 
{
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println("Beat-A-Box starting!");

  // Try to initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  // Setup motion detection
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  mpu.setMotionDetectionThreshold(1);
  mpu.setMotionDetectionDuration(20);
  mpu.setInterruptPinLatch(true);	// Keep it latched.  Will turn off when reinitialized.
  mpu.setInterruptPinPolarity(true);
  mpu.setMotionInterrupt(true);

  Serial.println("Init buzzer");
  delay(100);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(50);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.println("Init led");

  strip.begin();           
  strip.show();            
  strip.setBrightness(BRIGHTNESS);
  delay(100);
  showPausedRainbow();
}

static int gameState = 0; // State of the game


void loop() 
{
  bool gameWon = false;

  switch(gameState)
  {
    case 0: // Wait for the first punch
      if(mpu.getMotionInterruptStatus()) 
      {
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);
        if(checkForPunch(a.acceleration.x, a.acceleration.y))
        {
          gameState = 1;
          digitalWrite(BUZZER_PIN, HIGH);
          delay(100);
          digitalWrite(BUZZER_PIN, LOW);
          blinkPlayfiled();
        }
      }
      delay(5);
      break;

    case 1:
      //Game stage 1
      gameWon = playGame(200); // 1 second between drops
      if(gameWon)
      {
        gameState = 2;
        showWonGame();
      }
      else
      { 
        gameState = 0;
        showLostGame();
        showPausedRainbow();
      }
      //delay(5);
      break;

    case 2:
      //Game stage 2
      gameWon = playGame(150); // 0,75 second between drops
      if(gameWon)
      {
        gameState = 3;
        showWonGame();
      }
      else
      { 
        gameState = 0;
        showLostGame();
        showPausedRainbow();
      }
      //delay(5);
      break;

    case 3:
      //Game stage 3
      gameWon = playGame(100); // 0,5 second between drops
      if(gameWon)
      {
        gameState = 4;
        showWonGame();
      }
      else
      { 
        gameState = 0;
        showLostGame();
        showPausedRainbow();
      }
      //delay(5);
      break;

    case 4:
      //Game stage 3
      gameWon = playGame(50); // 0,25 second between drops
      if(gameWon)
      {
        gameState = 5;
        showWonGame();
      }
      else
      { 
        gameState = 0;
        showLostGame();
        showPausedRainbow();
      }
      //delay(5);
      break;

    case 5:
      //Game stage 3
      gameWon = playGame(25); // 0,125 second between drops
      if(gameWon)
      {
        gameState = 6;
        showWonGame();
      }
      else
      { 
        gameState = 0;
        showLostGame();
        showPausedRainbow();
      }
      //delay(5);
      break;

    case 6:
      //Game stage 3
      gameWon = playGame(10); // 0,125 second between drops
      if(gameWon)
      {
        gameState = 0;
        showWonGameSuper();
        showPausedRainbow();
      }
      else
      { 
        gameState = 0;
        showLostGame();
        showPausedRainbow();
      }
      //delay(5);
      break;

    default:
      gameState = 0;
      showPausedRainbow();
      delay(5);
      break;
  }
}

// 200 loops for 1 second between drops
bool playGame(int delayLoops)
{
  bool goalHit = false;
  bool stopLoop = false;

  for(int x=0;x<10;x++)
  {
    mpu.getMotionInterruptStatus();
    delay(5);
  }

  for(int i=7;i>=0;i--)
  {
    showPlayfiled();
    strip.setPixelColor(i, strip.Color(  0,   255, 0));
    strip.show();
  
    digitalWrite(BUZZER_PIN, HIGH);
    delay(50);
    digitalWrite(BUZZER_PIN, LOW);

    for(int j=0;j<delayLoops;j++)
    {
      if(mpu.getMotionInterruptStatus()) 
      {
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);
        if(checkForPunch(a.acceleration.x, a.acceleration.y))
        {
          //Serial.print("i: "); Serial.print(i);Serial.print(" # Goal: "); Serial.println(GOAL_POSITION);
          if(i == GOAL_POSITION) goalHit = true;
          else goalHit = false;
          stopLoop = true;
          break;
        }
      }
      delay(5);
    }
    if(stopLoop) break;
  }

  return goalHit;
}


void showLostGame()
{
 for(int i=0;i<3;i++)
  {
    digitalWrite(BUZZER_PIN, HIGH);
    colorWipe(strip.Color(  255,   0, 0),0);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(500);
    digitalWrite(BUZZER_PIN, LOW);
    colorWipe(strip.Color(  0,   0, 0),0);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(500);
  }
}


void showWonGame()
{
 for(int i=0;i<3;i++)
  {
    colorWipe(strip.Color(  0,   255, 0), 0);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(500);
    colorWipe(strip.Color(  0,   0, 0), 0);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(500);
  }
}


void showWonGameSuper()
{
 for(int i=0;i<3;i++)
  {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(50);
    digitalWrite(BUZZER_PIN, LOW);
    colorStripe(strip.Color(0,0,0));
    strip.setPixelColor(0, strip.Color(  255,   255, 255)); 
    strip.setPixelColor(2, strip.Color(  255,   255, 255));
    strip.setPixelColor(4, strip.Color(  255,   255, 255));
    strip.setPixelColor(6, strip.Color(  255,   255, 255));
    strip.show();                          //  Update strip to match
    delay(500);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(50);
    digitalWrite(BUZZER_PIN, LOW);
    colorStripe(strip.Color(0,0,0));
    strip.setPixelColor(1, strip.Color(  255,   255, 255)); 
    strip.setPixelColor(3, strip.Color(  255,   255, 255));
    strip.setPixelColor(5, strip.Color(  255,   255, 255));
    strip.show();                          //  Update strip to match
    delay(500);
  }
}


void showEndGame()
{
  for(int i=0;i<2;i++)
  {
    colorWipe(strip.Color(  255,   0, 0), 0);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(500);
     colorWipe(strip.Color(  255,   255, 0), 0);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(500);
     colorWipe(strip.Color(  0,   255, 0), 0);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(500);
    colorWipe(strip.Color(  0,   255, 255), 0);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(500);
     colorWipe(strip.Color(  0,   0, 255), 0);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(500);
     colorWipe(strip.Color(  255,   0, 255), 0);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(500);
  }  
}


void showPlayfiled()
{
  colorStripe(strip.Color(  0,   0, 0));
  strip.setPixelColor(GOAL_POSITION, strip.Color(  255,   0, 0));         //  Set pixel's color (in RAM)
  strip.show();                          //  Update strip to match
}


void blinkPlayfiled()
{
  colorStripe(strip.Color(  0,   0, 0));
  for(int i=0;i<5;i++)
  {
    strip.setPixelColor(GOAL_POSITION, strip.Color(  255,   0, 0));         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(250);
    strip.setPixelColor(GOAL_POSITION, strip.Color(  0,   0, 0));         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(250);
  }
  strip.setPixelColor(GOAL_POSITION, strip.Color(  255,   0, 0));         //  Set pixel's color (in RAM)
  strip.show();                          //  Update strip to match
  delay(500);                           //  Pause for a moment
}


void showPausedRainbow()
{
  strip.setPixelColor(0, strip.Color(  255,   0, 255)); // Lila
  strip.setPixelColor(1, strip.Color(  0,   255, 255)); // Hellblau
  strip.setPixelColor(2, strip.Color(  0,   0, 255)); // Blau
  strip.setPixelColor(3, strip.Color(  0,   255, 0)); // Grün 
  strip.setPixelColor(4, strip.Color(  255,   255, 0)); // Gelb
  strip.setPixelColor(5, strip.Color(  255,   100, 0)); // Orange
  strip.setPixelColor(6, strip.Color(  255,   0, 0)); // Rot
  strip.show();
}


bool checkForPunch(float valueX, float valueY)
{
  if(((valueX > 2.0f) | (valueX < -2.0f)) | ((valueY > 2.0f) | (valueY < -2.0f)))
  {
    return true;
  }
  else
  {
    return false;
  }

}


void colorStripe(uint32_t color) 
{
  for(int i=0; i<strip.numPixels(); i++) 
  {
    strip.setPixelColor(i, color);
  }
  strip.show();
}


void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}
