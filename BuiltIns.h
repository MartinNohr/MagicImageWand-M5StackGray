#pragma once
#ifdef MIW_MAIN
#define EXTERN
#else
#define EXTERN extern
#endif

// adjustment values for builtins
EXTERN RTC_DATA_ATTR uint8_t gHue; // rotating "base color" used by many of the patterns
// bouncing balls
EXTERN RTC_DATA_ATTR int nBouncingBallsCount
#ifdef MIW_MAIN
= 4
#endif
;
EXTERN RTC_DATA_ATTR int nBouncingBallsDecay
#ifdef MIW_MAIN
= 1000
#endif
;
EXTERN RTC_DATA_ATTR int nBouncingBallsFirstColor;   // first color, wraps to get all 32
EXTERN RTC_DATA_ATTR int nBouncingBallsChangeColors; // how many 100 count cycles to wait for change
// bpm
EXTERN RTC_DATA_ATTR int nBpmBeatsPerMinute
#ifdef MIW_MAIN
= 62
#endif
;
EXTERN RTC_DATA_ATTR bool bBpmCycleHue;
// rainbow pulse settings
EXTERN RTC_DATA_ATTR int nRainbowPulseColorScale
#ifdef MIW_MAIN
= 10
#endif
;
EXTERN RTC_DATA_ATTR int nRainbowPulsePause
#ifdef MIW_MAIN
= 5
#endif
;
EXTERN RTC_DATA_ATTR int nRainbowPulseSaturation
#ifdef MIW_MAIN
= 255
#endif
;
EXTERN RTC_DATA_ATTR int nRainbowPulseStartColor;
// checkerboard/bars
EXTERN RTC_DATA_ATTR int nCheckerboardHoldframes
#ifdef MIW_MAIN
= 10
#endif
;
EXTERN RTC_DATA_ATTR int nCheckboardBlackWidth
#ifdef MIW_MAIN
= 12
#endif
;
EXTERN RTC_DATA_ATTR int nCheckboardWhiteWidth
#ifdef MIW_MAIN
= 12
#endif
;
EXTERN RTC_DATA_ATTR bool bCheckerBoardAlternate
#ifdef MIW_MAIN
= true
#endif
;
EXTERN RTC_DATA_ATTR int nCheckerboardAddPixels;
// confetti
EXTERN RTC_DATA_ATTR bool bConfettiCycleHue;
// cylon eye
EXTERN RTC_DATA_ATTR int nCylonEyeSize
#ifdef MIW_MAIN
= 10
#endif
;
EXTERN RTC_DATA_ATTR int nCylonEyeRed
#ifdef MIW_MAIN
= 255
#endif
;
EXTERN RTC_DATA_ATTR int nCylonEyeGreen;
EXTERN RTC_DATA_ATTR int nCylonEyeBlue;
// black and white lines
EXTERN RTC_DATA_ATTR int nLinesWhite
#ifdef MIW_MAIN
= 5
#endif
;
EXTERN RTC_DATA_ATTR int nLinesBlack
#ifdef MIW_MAIN
= 5
#endif
;
// meteor
EXTERN RTC_DATA_ATTR int nMeteorSize
#ifdef MIW_MAIN
= 10
#endif
;
EXTERN RTC_DATA_ATTR int nMeteorRed
#ifdef MIW_MAIN
= 255
#endif
;
EXTERN RTC_DATA_ATTR int nMeteorGreen
#ifdef MIW_MAIN
= 255
#endif
;
EXTERN RTC_DATA_ATTR int nMeteorBlue
#ifdef MIW_MAIN
= 255
#endif
;
// rainbow
EXTERN RTC_DATA_ATTR int nRainbowHueDelta
#ifdef MIW_MAIN
= 4
#endif
;
EXTERN RTC_DATA_ATTR int nRainbowInitialHue;
EXTERN RTC_DATA_ATTR int nRainbowFadeTime
#ifdef MIW_MAIN
= 10
#endif
;       // fade in out 0.1 Sec
EXTERN RTC_DATA_ATTR bool bRainbowAddGlitter;
EXTERN RTC_DATA_ATTR bool bRainbowCycleHue;
// random bars
EXTERN RTC_DATA_ATTR bool bRandomBarsBlacks
#ifdef MIW_MAIN
= true
#endif
;
EXTERN RTC_DATA_ATTR int nRandomBarsHoldframes
#ifdef MIW_MAIN
= 10
#endif
;

struct BI_MENU {
    char* title;
    void* pData;
    int min, max, decimals; // all 0 for booleans
    char* yes;              // null for integers
    char* no;               // null for integers
};
typedef BI_MENU BiMenu;
#define MAX_BI_MENUS 10
EXTERN BiMenu BpmMenu[MAX_BI_MENUS]
#ifdef MIW_MAIN
=
{
    {"Beats per minute",&nBpmBeatsPerMinute,1,300},
    {"Cycle Hue",&bBpmCycleHue,0,0,0,"Yes","No"},
}
#endif
;

EXTERN BiMenu BouncingBallsMenu[MAX_BI_MENUS]
#ifdef MIW_MAIN
=
{
    {"Ball Count",&nBouncingBallsCount,1,32},
    {"Decay",&nBouncingBallsDecay,500,10000},
    {"First Color",&nBouncingBallsFirstColor,0,31},
    {"Change Color Rate",&nBouncingBallsChangeColors,0,10,0},
}
#endif
;

EXTERN BiMenu CheckerBoardMenu[MAX_BI_MENUS]
#ifdef MIW_MAIN
=
{
    {"Hold Frames",&nCheckerboardHoldframes,1,100},
    {"Black Width (pixels)",&nCheckboardBlackWidth,1,288},
    {"White Width (pixels)",&nCheckboardWhiteWidth,1,288},
    {"Add Pixels per Cycle",&nCheckerboardAddPixels,0,144},
    {"Alternate per Cycle",&bCheckerBoardAlternate,0,0,0,"Yes","No"},
}
#endif
;
EXTERN BiMenu ConfettiMenu[MAX_BI_MENUS]
#ifdef MIW_MAIN
=
{
    {"Cycle Hue",&bConfettiCycleHue,0,0,0,"Yes","No"},
}
#endif
;
EXTERN BiMenu CylonEyeMenu[MAX_BI_MENUS]
#ifdef MIW_MAIN
=
{
    {"Eye Size",&nCylonEyeSize,1,100},
    {"Eye Red",&nCylonEyeRed,0,255},
    {"Eye Green",&nCylonEyeGreen,0,255},
    {"Eye Blue",&nCylonEyeBlue,0,255},
}
#endif
;
EXTERN BiMenu LinesMenu[MAX_BI_MENUS]
#ifdef MIW_MAIN
=
{
    {"White Pixels",&nLinesWhite,0,288},
    {"Black Pixels",&nLinesBlack,0,288},
}
#endif
;
EXTERN BiMenu MeteorMenu[MAX_BI_MENUS]
#ifdef MIW_MAIN
=
{
    {"Meteor Size",&nMeteorSize,1,100},
    {"Meteor Red",&nMeteorRed,0,255},
    {"Meteor Green",&nMeteorGreen,0,255},
    {"Meteor Blue",&nMeteorBlue,0,255},
}
#endif
;
EXTERN BiMenu RainbowMenu[MAX_BI_MENUS]
#ifdef MIW_MAIN
=
{
    {"Fade Time (S)",&nRainbowFadeTime, 0, 100, 1},
    {"Starting Hue",&nRainbowInitialHue,0,255 },
    {"Cycle Hue",&bRainbowCycleHue,0,0,0,"Yes","No" },
    {"Hue Delta Size",&nRainbowHueDelta,1,255 },
    {"Add Glitter",&bRainbowAddGlitter,0,0,0,"Yes","No" },
}
#endif
;
EXTERN BiMenu RandomBarsMenu[MAX_BI_MENUS]
#ifdef MIW_MAIN
=
{
    {"Hold Frames",&nRandomBarsHoldframes, 1, 100},
    {"Alternating Black",&bRandomBarsBlacks,0,0,0,"Yes","No" },
}
#endif
;
EXTERN BiMenu RainbowPulseMenu[MAX_BI_MENUS]
#ifdef MIW_MAIN
=
{
    {"Step Pause",&nRainbowPulsePause,0,1000},
    {"Color Rate Scale",&nRainbowPulseColorScale,0,256},
    {"Start Color",&nRainbowPulseStartColor,0,255},
    {"Color Saturation",&nRainbowPulseSaturation,0,255},
}
#endif
;

// built-in "files"
struct BuiltInItem {
    const char* text;
    void(*function)();
    BiMenu* menuList;
};
typedef BuiltInItem BuiltInItem;
void BarberPole();
void TestBpm();
void TestBouncingBalls();
void CheckerBoard();
void RandomBars();
void RunningDot();
void OppositeRunningDots();
void TestTwinkle();
void TestMeteor();
void TestCylon();
void TestRainbow();
void TestJuggle();
void TestSine();
void TestConfetti();
void DisplayLedLightBar();
void TestStripes();
void TestLines();
void RainbowPulse();
void TestWedge();
#define MAX_BUILTINS 25
EXTERN BuiltInItem BuiltInFiles[MAX_BUILTINS]
#ifdef MIW_MAIN
= {
    {"Barber Pole",BarberPole},
    {"Beats",TestBpm,BpmMenu},
    {"Bouncy Balls",TestBouncingBalls,BouncingBallsMenu},
    {"CheckerBoard",CheckerBoard,CheckerBoardMenu},
    {"Confetti",TestConfetti,ConfettiMenu},
    {"Cylon Eye",TestCylon,CylonEyeMenu},
    {"Juggle",TestJuggle},
    {"Lines",TestLines,LinesMenu},
    {"Meteor",TestMeteor,MeteorMenu},
    {"One Dot",RunningDot},
    {"Rainbow",TestRainbow,RainbowMenu},
    {"Rainbow Pulse",RainbowPulse,RainbowPulseMenu},
    {"Random Bars",RandomBars,RandomBarsMenu},
    {"Sine Trails"/*,TestSine*//*,SineMenu*/},
    {"Solid Color"/*,DisplayLedLightBar*//*,LedLightBarMenu*/},
    {"Stripes"/*,TestStripes*/},
    {"Twinkle"/*,TestTwinkle*//*,TwinkleMenu*/},
    {"Two Dots"/*,OppositeRunningDots*/},
    {"Wedge"/*,TestWedge*//*,WedgeMenu*/},
}
#endif
;