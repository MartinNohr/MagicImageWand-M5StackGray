#pragma once

#define MIW_VERSION "1.00"
// define must first #include <M5Stack.h>
//#define M5STACK_MPU6886 
#define M5STACK_MPU9250 
//#define M5STACK_MPU6050
//#define M5STACK_200Q

#include <stack>
#include <M5ez.h>
#include <M5Stack.h>
#include <EEPROM.h>
#include <ezTime.h>
#include "images.h"
#include <FastLED.h>
//#include <SPIFFS.h>
#include <SD.h>

bool bSdCardValid = false;
#define NEXT_FOLDER_CHAR '>'
#define PREVIOUS_FOLDER_CHAR '<'
String currentFolder = "/";
String currentFile;
fs::File dataFile;
//std::vector<String> FileNames;
RTC_DATA_ATTR bool bShowBuiltInTests = false;           // list the internal file instead of the SD card
RTC_DATA_ATTR int nBootCount = 0;
// image settings
struct IMG_INFO {
    int nColumnHoldTime = 10;           // mSec frame hold time
    bool bFixedTime = false;            // fixed total time or hold time for image
    int nFixedImageTime = 5;            // seconds of time
    int nFadeInOutFrames = 0;           // fading frames
    bool bReverseImage = false;         // show backwards
    bool bUpsideDown = false;           // topsy-turvy
    bool bDoublePixels = false;         // double up to make 144 288 etc
    bool bMirrorPlayImage = false;      // play mirror image trick
    int nMirrorDelay = 0;               // time to delay mirroring
    bool bScaleHeight = false;          // scale height to fit, e.g. 288 to 144
    bool bManualFrameAdvance = false;   // for click advancing of frame or frame wheel counter
    int nFramePulseCount = 0;
    bool bChainFiles = false;           // chain files in the same folder
    int nChainRepeats = 1;              // how many times to repeat the chain
    int nChainDelay = 0;                // 1/10 seconds
    bool bChainWaitKey = false;
    int startDelay = 0;                 // 1/10 seconds
    int repeatCount = 1;
    int repeatDelay = 0;                // 1/10 seconds
};
typedef IMG_INFO IMG_INFO;
RTC_DATA_ATTR IMG_INFO ImgInfo;

#define FRAMEBUTTON 17
// led strip settings
#define DATA_PIN1 2
#define DATA_PIN2 5
CRGB* leds;
struct LED_INFO {
	bool bSecondController = false;
	int nLEDBrightness = 25;
	int nPixelCount = 144;
	bool bGammaCorrection = true;
    int stripsMode = 0;             // 0 feed from center, 1 serial from end, 2 from outsides
// white balance values, really only 8 bits, but menus need ints
    struct {
        int r;
        int g;
        int b;
    } whiteBalance = { 255,255,255 };
    int nStripMaxCurrent = 2000;              // maximum milliamps to allow
};
typedef LED_INFO LED_INFO;
RTC_DATA_ATTR LED_INFO LedInfo;
// set this to the delay time while we get the next frame, also used for delay timers
volatile bool bStripWaiting = false;
volatile int nTimerSeconds;
esp_timer_handle_t oneshot_LED_timer;
esp_timer_create_args_t oneshot_LED_timer_args;
// use this to stop an image run
bool bCancelRun = false;

int g = 0;                                // Variable for the Green Value
int b = 0;                                // Variable for the Blue Value
int r = 0;                                // Variable for the Red Value

// some system settings
bool bIsRunning = false;                // system state, idle or running
bool bRecordingMacro = false;
bool bRunningMacro = false;
RTC_DATA_ATTR int nRepeatCountMacro = 1;                // repeat count for macros
RTC_DATA_ATTR int nMacroRepeatsLeft = 0;

// functions
void DisplayLine(int line, String text, int indent = 0, int16_t color = TFT_WHITE);
void IRAM_ATTR SetPixel(int ix, CRGB pixel, int column = 0, int totalColumns = 1);
bool GetInteger(ezMenu* menu, char* title, int& value, int minval, int maxval, int decimals = 0);

// adjustment values for builtins
RTC_DATA_ATTR uint8_t gHue = 0; // rotating "base color" used by many of the patterns
// bouncing balls
RTC_DATA_ATTR int nBouncingBallsCount = 4;
RTC_DATA_ATTR int nBouncingBallsDecay = 1000;
RTC_DATA_ATTR int nBouncingBallsFirstColor = 0;   // first color, wraps to get all 32
RTC_DATA_ATTR int nBouncingBallsChangeColors = 0; // how many 100 count cycles to wait for change
// bpm
RTC_DATA_ATTR int nBpmBeatsPerMinute = 62;
RTC_DATA_ATTR bool bBpmCycleHue = false;
// rainbow pulse settings
RTC_DATA_ATTR int nRainbowPulseColorScale = 10;
RTC_DATA_ATTR int nRainbowPulsePause = 5;
RTC_DATA_ATTR int nRainbowPulseSaturation = 255;
RTC_DATA_ATTR int nRainbowPulseStartColor = 0;

// built-in "files"
struct BuiltInItem {
    const char* text;
    void(*function)();
	void(*menu)();
};
typedef BuiltInItem BuiltInItem;
void BarberPole();
void TestBpm();
void BpmMenu();
void TestBouncingBalls();
void CheckerBoard() {};
void RandomBars() {};
void RunningDot() {};
void OppositeRunningDots() {};
void TestTwinkle() {};
void TestMeteor() {};
void TestCylon() {};
void TestRainbow() {};
void TestJuggle() {};
void TestSine() {};
void TestConfetti() {};
void DisplayLedLightBar() {};
void TestStripes() {};
void TestLines() {};
void RainbowPulse();
void TestWedge() {};
BuiltInItem BuiltInFiles[] = {
    {"Barber Pole",BarberPole},
    {"Beats",TestBpm,BpmMenu},
    {"Bouncy Balls",TestBouncingBalls/*,BouncingBallsMenu*/},
    {"CheckerBoard",CheckerBoard/*,CheckerBoardMenu*/},
    {"Confetti",TestConfetti/*,ConfettiMenu*/},
    {"Cylon Eye",TestCylon/*,CylonEyeMenu*/},
    {"Juggle",TestJuggle},
    {"Lines",TestLines/*,LinesMenu*/},
    {"Meteor",TestMeteor/*,MeteorMenu*/},
    {"One Dot",RunningDot},
    {"Rainbow",TestRainbow/*,RainbowMenu*/},
    {"Rainbow Pulse",RainbowPulse/*,RainbowPulseMenu*/},
    {"Random Bars",RandomBars/*,RandomBarsMenu*/},
    {"Sine Trails",TestSine/*,SineMenu*/},
    {"Solid Color",DisplayLedLightBar/*,LedLightBarMenu*/},
    {"Stripes",TestStripes},
    {"Twinkle",TestTwinkle/*,TwinkleMenu*/},
    {"Two Dots",OppositeRunningDots},
    {"Wedge",TestWedge/*,WedgeMenu*/},
};

// Gramma Correction (Defalt Gamma = 2.8)
const uint8_t gammaR[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,
    2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,
    5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,
    9,  9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 14, 14, 14,
   15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22,
   23, 24, 24, 25, 25, 26, 27, 27, 28, 29, 29, 30, 31, 31, 32, 33,
   33, 34, 35, 36, 36, 37, 38, 39, 40, 40, 41, 42, 43, 44, 45, 46,
   46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
   62, 63, 65, 66, 67, 68, 69, 70, 71, 73, 74, 75, 76, 78, 79, 80,
   81, 83, 84, 85, 87, 88, 89, 91, 92, 94, 95, 97, 98, 99,101,102,
  104,105,107,109,110,112,113,115,116,118,120,121,123,125,127,128,
  130,132,134,135,137,139,141,143,145,146,148,150,152,154,156,158,
  160,162,164,166,168,170,172,174,177,179,181,183,185,187,190,192,
  194,196,199,201,203,206,208,210,213,215,218,220,223,225,227,230 };

const uint8_t gammaG[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };


const uint8_t gammaB[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,
    4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  7,  7,  7,  8,
    8,  8,  8,  9,  9,  9, 10, 10, 10, 10, 11, 11, 12, 12, 12, 13,
   13, 13, 14, 14, 15, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 19,
   20, 20, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 27, 27, 28, 28,
   29, 30, 30, 31, 32, 32, 33, 34, 34, 35, 36, 37, 37, 38, 39, 40,
   40, 41, 42, 43, 44, 44, 45, 46, 47, 48, 49, 50, 51, 51, 52, 53,
   54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 69, 70,
   71, 72, 73, 74, 75, 77, 78, 79, 80, 81, 83, 84, 85, 86, 88, 89,
   90, 92, 93, 94, 96, 97, 98,100,101,103,104,106,107,109,110,112,
  113,115,116,118,119,121,122,124,126,127,129,131,132,134,136,137,
  139,141,143,144,146,148,150,152,153,155,157,159,161,163,165,167,
  169,171,173,175,177,179,181,183,185,187,189,191,193,196,198,200 };
