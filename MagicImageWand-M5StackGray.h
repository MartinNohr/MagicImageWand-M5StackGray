#pragma once

#ifdef MIW_MAIN
    #define EXTERN
#else
    #define EXTERN extern
#endif

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
#include "BuiltIns.h"

EXTERN bool bSdCardValid;
#define NEXT_FOLDER_CHAR '>'
#define PREVIOUS_FOLDER_CHAR '<'
EXTERN String currentFolder
#ifdef MIW_MAIN
= "/"
#endif
;
EXTERN String currentFile;
EXTERN fs::File dataFile;
//std::vector<String> FileNames;
EXTERN RTC_DATA_ATTR bool bShowBuiltInTests;           // list the internal file instead of the SD card
EXTERN RTC_DATA_ATTR int nBootCount;
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
EXTERN RTC_DATA_ATTR IMG_INFO ImgInfo;

#define FRAMEBUTTON 17
// led strip settings
#define DATA_PIN1 2
#define DATA_PIN2 5
EXTERN  CRGB* leds;
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
EXTERN RTC_DATA_ATTR LED_INFO LedInfo;
// set this to the delay time while we get the next frame, also used for delay timers
EXTERN volatile bool bStripWaiting;
EXTERN volatile int nTimerSeconds;
EXTERN esp_timer_handle_t oneshot_LED_timer;
EXTERN esp_timer_create_args_t oneshot_LED_timer_args;
// use this to stop an image run
EXTERN bool bCancelRun;

EXTERN int g;                                // Variable for the Green Value
EXTERN int b;                                // Variable for the Blue Value
EXTERN int r;                                // Variable for the Red Value

// some system settings
EXTERN bool bValueChanged;              // this is set when an int or boolean has changed value
EXTERN bool bIsRunning;                // system state, idle or running
EXTERN bool bRecordingMacro;
EXTERN bool bRunningMacro;
EXTERN RTC_DATA_ATTR int nRepeatCountMacro
#ifdef MIW_MAIN
= 1
#endif
;                // repeat count for macros
EXTERN RTC_DATA_ATTR int nMacroRepeatsLeft;

// functions
void ShowLeds(int mode = 0);
void DisplayLine(int line, String text, int indent = 0, int16_t color = TFT_WHITE);
void IRAM_ATTR SetPixel(int ix, CRGB pixel, int column = 0, int totalColumns = 1);
bool GetInteger(ezMenu* menu, char* title, int& value, int minval, int maxval, int decimals = 0);
bool CheckCancel();
bool HandleMenuInteger(ezMenu* menu);
bool ToggleBool(ezMenu* menu);
void fixRGBwithGamma(byte* rp, byte* gp, byte* bp);
int AdjustStripIndex(int ix);
// the built-in menu handler
void BuiltInMenu(String hdr, BiMenu* menuList);

// Gramma Correction (Defalt Gamma = 2.8)
EXTERN const uint8_t gammaR[256]
#ifdef MIW_MAIN
= {
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
  194,196,199,201,203,206,208,210,213,215,218,220,223,225,227,230 }
#endif
;

EXTERN const uint8_t gammaG[256]
#ifdef MIW_MAIN
= {
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
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 }
#endif
;

EXTERN const uint8_t gammaB[256]
#ifdef MIW_MAIN
= {
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
  169,171,173,175,177,179,181,183,185,187,189,191,193,196,198,200 }
#endif
;

// save and load variables from MIW files
enum SETVARTYPE {
    vtInt,
    vtBool,
    vtRGB,
    vtShowFile,     // run a file on the display, the file has the path which is used to set the current path
    vtBuiltIn,      // bool for builtins or SD
};
struct SETTINGVAR {
    char* name;
    void* address;
    enum SETVARTYPE type;
    int min, max;
};
struct SETTINGVAR SettingsVarList[] = {
    {"STRIP BRIGHTNESS",&LedInfo.nLEDBrightness,vtInt,1,255},
    {"FADE IN/OUT FRAMES",&ImgInfo.nFadeInOutFrames,vtInt,0,255},
    {"REPEAT COUNT",&ImgInfo.repeatCount,vtInt},
    {"REPEAT DELAY",&ImgInfo.repeatDelay,vtInt},
    {"FRAME TIME",&ImgInfo.nColumnHoldTime,vtInt},
    {"USE FIXED TIME",&ImgInfo.bFixedTime,vtBool},
    {"FIXED IMAGE TIME",&ImgInfo.nFixedImageTime,vtInt},
    {"START DELAY",&ImgInfo.startDelay,vtInt},
    {"REVERSE IMAGE",&ImgInfo.bReverseImage,vtBool},
    {"UPSIDE DOWN IMAGE",&ImgInfo.bUpsideDown,vtBool},
    {"MIRROR PLAY IMAGE",&ImgInfo.bMirrorPlayImage,vtBool},
    {"MIRROR PLAY DELAY",&ImgInfo.nMirrorDelay,vtInt},
    {"CHAIN FILES",&ImgInfo.bChainFiles,vtBool},
    {"CHAIN REPEATS",&ImgInfo.nChainRepeats,vtInt},
    {"CHAIN DELAY",&ImgInfo.nChainDelay,vtInt},
    {"CHAIN WAIT FOR KEY",&ImgInfo.bChainWaitKey,vtBool},
    {"WHITE BALANCE",&LedInfo.whiteBalance,vtRGB},
    {"DISPLAY BRIGHTNESS",&LedInfo.nLEDBrightness,vtInt,0,100},
    //{"DISPLAY MENULINE COLOR",&ImgInfo.menuTextColor,vtInt},
    //{"MENU STAR",&bMenuStar,vtBool},
    //{"HILITE FILE",&bHiLiteCurrentFile,vtBool},
    {"GAMMA CORRECTION",&LedInfo.bGammaCorrection,vtBool},
    {"SELECT BUILTINS",&bShowBuiltInTests,vtBuiltIn},       // this must be before the SHOW FILE command
    //{"SHOW FILE",&FileToShow,vtShowFile},
};

struct saveValues {
    void* val;
    int size;
};
// these values are saved in eeprom, the signature is first
const saveValues saveValueList[] = {
    {signature,sizeof(signature)},                      // this must be first
    {&bSecondController, sizeof(bSecondController)},    // this must be second
    {&TotalLeds, sizeof(TotalLeds)},                    // this must be third
    {&stripsMode, sizeof(stripsMode)},                  // this must be fourth
    {&bAutoLoadSettings, sizeof(bAutoLoadSettings)},    // this must be fifth
    {&nStripBrightness, sizeof(nStripBrightness)},      // all the rest can be in any order
    {&nStripMaxCurrent, sizeof(nStripMaxCurrent)},
    {&nFadeInOutFrames, sizeof(nFadeInOutFrames)},
    {&nFrameHold, sizeof(nFrameHold)},
    {&bFixedTime,sizeof(bFixedTime)},
    {&nFixedImageTime,sizeof(nFixedImageTime)},
    {&nFramePulseCount, sizeof(nFramePulseCount)},
    {&bManualFrameAdvance, sizeof(bManualFrameAdvance)},
    {&startDelay, sizeof(startDelay)},
    //{&bRepeatForever, sizeof(bRepeatForever)},
    {&repeatCount, sizeof(repeatCount)},
    {&repeatDelay, sizeof(repeatDelay)},
    {&bGammaCorrection, sizeof(bGammaCorrection)},
    //{&nBackLightSeconds, sizeof(nBackLightSeconds)},
    //{&nMaxBackLight, sizeof(nMaxBackLight)},
    {&CurrentFileIndex,sizeof(CurrentFileIndex)},
    {&bShowBuiltInTests,sizeof(bShowBuiltInTests)},
    {&bScaleHeight,sizeof(bScaleHeight)},
    {&bChainFiles,sizeof(bChainFiles)},
    {&bReverseImage,sizeof(bReverseImage)},
    {&bMirrorPlayImage,sizeof(bMirrorPlayImage)},
    {&nMirrorDelay,sizeof(nMirrorDelay)},
    {&bUpsideDown,sizeof(bUpsideDown)},
    {&bDoublePixels,sizeof(bDoublePixels)},
    {&nChainRepeats,sizeof(nChainRepeats)},
    {&nChainDelay,sizeof(nChainDelay)},
    {&bChainWaitKey,sizeof(bChainWaitKey)},
    {&whiteBalance,sizeof(whiteBalance)},
    {&bShowProgress,sizeof(bShowProgress)},
    {&bShowFolder,sizeof(bShowFolder)},
    {&bAllowMenuWrap,sizeof(bAllowMenuWrap)},
    {&bShowNextFiles,sizeof(bShowNextFiles)},
    {&CRotaryDialButton::m_bReverseDial,sizeof(CRotaryDialButton::m_bReverseDial)},
    {&CRotaryDialButton::m_nDialSensitivity,sizeof(CRotaryDialButton::m_nDialSensitivity)},
    {&CRotaryDialButton::m_nDialSpeed,sizeof(CRotaryDialButton::m_nDialSpeed)},
    {&CRotaryDialButton::m_nLongPressTimerValue,sizeof(CRotaryDialButton::m_nLongPressTimerValue)},
    {&nDisplayBrightness,sizeof(nDisplayBrightness)},
    {&menuTextColor,sizeof(menuTextColor)},
    {&bMenuStar,sizeof(bMenuStar)},
    {&bHiLiteCurrentFile,sizeof(bHiLiteCurrentFile)},
    {&nPreviewScrollCols,sizeof(nPreviewScrollCols)},
    // the built-in values
    // display all color
    {&bAllowRollover,sizeof(bAllowRollover)},
    {&bDisplayAllRGB,sizeof(bDisplayAllRGB)},
    {&nDisplayAllRed,sizeof(nDisplayAllRed)},
    {&nDisplayAllGreen,sizeof(nDisplayAllGreen)},
    {&nDisplayAllBlue,sizeof(nDisplayAllBlue)},
    {&nDisplayAllHue,sizeof(nDisplayAllHue)},
    {&nDisplayAllSaturation,sizeof(nDisplayAllSaturation)},
    {&nDisplayAllBrightness,sizeof(nDisplayAllBrightness)},
    {&nDisplayAllPixelCount,sizeof(nDisplayAllPixelCount)},
    {&bDisplayAllFromMiddle,sizeof(bDisplayAllFromMiddle)},
    // bouncing balls
    {&nBouncingBallsCount,sizeof(nBouncingBallsCount)},
    {&nBouncingBallsDecay,sizeof(nBouncingBallsDecay)},
    {&nBouncingBallsFirstColor,sizeof(nBouncingBallsFirstColor)},
    {&nBouncingBallsChangeColors,sizeof(nBouncingBallsChangeColors)},
    // cylon eye
    {&nCylonEyeSize,sizeof(nCylonEyeSize)},
    {&nCylonEyeRed,sizeof(nCylonEyeRed)},
    {&nCylonEyeGreen,sizeof(nCylonEyeGreen)},
    {&nCylonEyeBlue,sizeof(nCylonEyeBlue)},
    // random bars
    {&bRandomBarsBlacks,sizeof(bRandomBarsBlacks)},
    {&nRandomBarsHoldframes,sizeof(nRandomBarsHoldframes)},
    // meteor
    {&nMeteorSize,sizeof(nMeteorSize)},
    {&nMeteorRed,sizeof(nMeteorRed)},
    {&nMeteorGreen,sizeof(nMeteorGreen)},
    {&nMeteorBlue,sizeof(nMeteorBlue)},
    // rainbow
    {&nRainbowHueDelta,sizeof(nRainbowHueDelta)},
    {&nRainbowInitialHue,sizeof(nRainbowInitialHue)},
    {&nRainbowFadeTime,sizeof(nRainbowFadeTime)},
    {&bRainbowAddGlitter,sizeof(bRainbowAddGlitter)},
    {&bRainbowCycleHue,sizeof(bRainbowCycleHue)},
    // twinkle
    {&bTwinkleOnlyOne,sizeof(bTwinkleOnlyOne)},
    // confetti
    {&bConfettiCycleHue,sizeof(bConfettiCycleHue)},
    // juggle

    // sine
    {&nSineStartingHue,sizeof(nSineStartingHue)},
    {&bSineCycleHue,sizeof(bSineCycleHue)},
    {&nSineSpeed,sizeof(nSineSpeed)},
    // bpm
    {&nBpmBeatsPerMinute,sizeof(nBpmBeatsPerMinute)},
    {&bBpmCycleHue,sizeof(bBpmCycleHue)},
    // checkerboard/bars
    {&nCheckerboardHoldframes,sizeof(nCheckerboardHoldframes)},
    {&nCheckboardBlackWidth,sizeof(nCheckboardBlackWidth)},
    {&nCheckboardWhiteWidth,sizeof(nCheckboardWhiteWidth)},
    {&bCheckerBoardAlternate,sizeof(bCheckerBoardAlternate)},
    {&nCheckerboardAddPixels,sizeof(nCheckerboardAddPixels)},
    {&nCurrentMacro,sizeof(nCurrentMacro)},
    {&nRepeatCountMacro,sizeof(nRepeatCountMacro)},
    {&nRepeatWaitMacro,sizeof(nRepeatWaitMacro)},
    // lines values
    {&nLinesWhite,sizeof(nLinesWhite)},
    {&nLinesBlack,sizeof(nLinesBlack)},
    // rainbow pulse
    {&nRainbowPulseColorScale,sizeof(nRainbowPulseColorScale)},
    {&nRainbowPulsePause,sizeof(nRainbowPulsePause)},
    {&nRainbowPulseSaturation,sizeof(nRainbowPulseSaturation)},
    {&nRainbowPulseStartColor,sizeof(nRainbowPulseStartColor)},
    // wedge
    {&bWedgeFill,sizeof(bWedgeFill)},
    {&nWedgeBlue,sizeof(nWedgeBlue)},
    {&nWedgeRed,sizeof(nWedgeRed)},
    {&nWedgeGreen,sizeof(nWedgeGreen)},
};
