#pragma once
#ifdef MIW_MAIN
#define EXTERN
#else
#define EXTERN extern
#endif

struct BUILTIN_INFO {
	uint8_t gHue = 0; // rotating "base color" used by many of the patterns
    // bouncing balls
	int nBouncingBallsCount = 4;
	int nBouncingBallsDecay = 1000;
	int nBouncingBallsFirstColor;   // first color, wraps to get all 32
	int nBouncingBallsChangeColors; // how many 100 count cycles to wait for change
	// bpm
	int nBpmBeatsPerMinute = 62;
	bool bBpmCycleHue;
    // rainbow pulse settings
	int nRainbowPulseColorScale = 10;
	int nRainbowPulsePause = 5;
	int nRainbowPulseSaturation = 255;
	int nRainbowPulseStartColor = 0;
    // checkerboard/bars
	int nCheckerboardHoldframes = 10;
	int nCheckboardBlackWidth = 12;
	int nCheckboardWhiteWidth = 12;
	bool bCheckerBoardAlternate = true;
	int nCheckerboardAddPixels = 0;
	// confetti
	bool bConfettiCycleHue = false;
    // cylon eye
	int nCylonEyeSize = 10;
	int nCylonEyeRed = 255;
	int nCylonEyeGreen = 0;
	int nCylonEyeBlue = 0;
	// black and white lines
	int nLinesWhite = 5;
	int nLinesBlack = 5;
    // meteor
	int nMeteorSize = 10;
	int nMeteorRed = 255;
	int nMeteorGreen = 255;
	int nMeteorBlue = 255;
	// rainbow
	int nRainbowHueDelta = 4;
	int nRainbowInitialHue = 0;
	int nRainbowFadeTime = 10;       // fade in out 0.1 Sec
	bool bRainbowAddGlitter = 0;
	bool bRainbowCycleHue = 0;
    // random bars
	bool bRandomBarsBlacks = true;
	int nRandomBarsHoldframes = 10;
	// sine
	int nSineStartingHue = 0;
	bool bSineCycleHue = 0;
	int nSineSpeed = 13;
	// display all color and lightbar
	bool bDisplayAllRGB = false;    // true for RGB, else HSV
	int nDisplayAllRed = 255;
	int nDisplayAllGreen = 255;
	int nDisplayAllBlue = 255;
	int nDisplayAllHue = 0;
	int nDisplayAllSaturation = 255;
	int nDisplayAllBrightness = 255;
	int nDisplayAllPixelCount = 288;
	bool bDisplayAllFromMiddle = true;
	// twinkle
	bool bTwinkleOnlyOne;
	// wedge data
	bool bWedgeFill = false;
	int nWedgeRed = 255;
	int nWedgeGreen = 255;
	int nWedgeBlue = 255;
};
typedef BUILTIN_INFO BUILTIN_INFO;
EXTERN RTC_DATA_ATTR BUILTIN_INFO BuiltinInfo;

struct BI_MENU {
    char* title;
    void* pData;
    int min, max, decimals; // all 0 for booleans
    char* yes;              // null for integers
    char* no;               // null for integers
};
typedef BI_MENU BiMenu;
#define MAX_BI_MENUS 20
EXTERN BiMenu BpmMenu[MAX_BI_MENUS]
#ifdef MIW_MAIN
=
{
    {"Beats per minute",&BuiltinInfo.nBpmBeatsPerMinute,1,300},
    {"Cycle Hue",&BuiltinInfo.bBpmCycleHue,0,0,0,"Yes","No"},
}
#endif
;

EXTERN BiMenu BouncingBallsMenu[MAX_BI_MENUS]
#ifdef MIW_MAIN
=
{
    {"Ball Count",&BuiltinInfo.nBouncingBallsCount,1,32},
    {"Decay",&BuiltinInfo.nBouncingBallsDecay,500,10000},
    {"First Color",&BuiltinInfo.nBouncingBallsFirstColor,0,31},
    {"Change Color Rate",&BuiltinInfo.nBouncingBallsChangeColors,0,10,0},
}
#endif
;

EXTERN BiMenu CheckerBoardMenu[MAX_BI_MENUS]
#ifdef MIW_MAIN
=
{
    {"Hold Frames",&BuiltinInfo.nCheckerboardHoldframes,1,100},
    {"Black Width (pixels)",&BuiltinInfo.nCheckboardBlackWidth,1,288},
    {"White Width (pixels)",&BuiltinInfo.nCheckboardWhiteWidth,1,288},
    {"Add Pixels per Cycle",&BuiltinInfo.nCheckerboardAddPixels,0,144},
    {"Alternate per Cycle",&BuiltinInfo.bCheckerBoardAlternate,0,0,0,"Yes","No"},
}
#endif
;
EXTERN BiMenu ConfettiMenu[MAX_BI_MENUS]
#ifdef MIW_MAIN
=
{
    {"Cycle Hue",&BuiltinInfo.bConfettiCycleHue,0,0,0,"Yes","No"},
}
#endif
;
EXTERN BiMenu CylonEyeMenu[MAX_BI_MENUS]
#ifdef MIW_MAIN
=
{
    {"Eye Size",&BuiltinInfo.nCylonEyeSize,1,100},
    {"Eye Red",&BuiltinInfo.nCylonEyeRed,0,255},
    {"Eye Green",&BuiltinInfo.nCylonEyeGreen,0,255},
    {"Eye Blue",&BuiltinInfo.nCylonEyeBlue,0,255},
}
#endif
;
EXTERN BiMenu LinesMenu[MAX_BI_MENUS]
#ifdef MIW_MAIN
=
{
    {"White Pixels",&BuiltinInfo.nLinesWhite,0,288},
    {"Black Pixels",&BuiltinInfo.nLinesBlack,0,288},
}
#endif
;
EXTERN BiMenu MeteorMenu[MAX_BI_MENUS]
#ifdef MIW_MAIN
=
{
    {"Meteor Size",&BuiltinInfo.nMeteorSize,1,100},
    {"Meteor Red",&BuiltinInfo.nMeteorRed,0,255},
    {"Meteor Green",&BuiltinInfo.nMeteorGreen,0,255},
    {"Meteor Blue",&BuiltinInfo.nMeteorBlue,0,255},
}
#endif
;
EXTERN BiMenu RainbowMenu[MAX_BI_MENUS]
#ifdef MIW_MAIN
=
{
    {"Fade Time (S)",&BuiltinInfo.nRainbowFadeTime, 0, 100, 1},
    {"Starting Hue",&BuiltinInfo.nRainbowInitialHue,0,255 },
    {"Cycle Hue",&BuiltinInfo.bRainbowCycleHue,0,0,0,"Yes","No" },
    {"Hue Delta Size",&BuiltinInfo.nRainbowHueDelta,1,255 },
    {"Add Glitter",&BuiltinInfo.bRainbowAddGlitter,0,0,0,"Yes","No" },
}
#endif
;
EXTERN BiMenu RandomBarsMenu[MAX_BI_MENUS]
#ifdef MIW_MAIN
=
{
    {"Hold Frames",&BuiltinInfo.nRandomBarsHoldframes, 1, 100},
    {"Alternating Black",&BuiltinInfo.bRandomBarsBlacks,0,0,0,"Yes","No" },
}
#endif
;
EXTERN BiMenu RainbowPulseMenu[MAX_BI_MENUS]
#ifdef MIW_MAIN
=
{
    {"Step Pause",&BuiltinInfo.nRainbowPulsePause,0,1000},
    {"Color Rate Scale",&BuiltinInfo.nRainbowPulseColorScale,0,256},
    {"Start Color",&BuiltinInfo.nRainbowPulseStartColor,0,255},
    {"Color Saturation",&BuiltinInfo.nRainbowPulseSaturation,0,255},
}
#endif
;
EXTERN BiMenu SineMenu[MAX_BI_MENUS]
#ifdef MIW_MAIN
=
{
    {"Starting Hue",&BuiltinInfo.nSineStartingHue,0,255},
    {"Cycle Hue",&BuiltinInfo.bSineCycleHue,0,0,0,"Yes","No"},
    {"Speed",&BuiltinInfo.nSineSpeed,1,500},
}
#endif
;
// to allow menu items in menu builder BuiltInMenu()
// for eIfEqual etc, title is NULL, min contains the enum, and max the test value
// enum must start at 0 since we use it for loop control in BuiltInMenu()
// decimal indicates boolean, 0 for int
enum VALID_MENU_ITEMS { eIfEqual = 1, eElse, eEndif };
EXTERN BiMenu LedLightBarMenu[MAX_BI_MENUS]
#ifdef MIW_MAIN
=
{
    {"Color Mode",&BuiltinInfo.bDisplayAllRGB,0,0,0,"RGB","HSL"},
    {NULL,&BuiltinInfo.bDisplayAllRGB,eIfEqual,true,true},
        {"Red",&BuiltinInfo.nDisplayAllRed,0,255},
        {"Green",&BuiltinInfo.nDisplayAllGreen,0,255},
        {"Blue",&BuiltinInfo.nDisplayAllBlue,0,255},
    {NULL,NULL,eElse,0},
        {"Hue",&BuiltinInfo.nDisplayAllHue,0,255},
        {"Saturation",&BuiltinInfo.nDisplayAllSaturation,0,255},
        {"Brightness",&BuiltinInfo.nDisplayAllBrightness,0,255},
    {NULL,NULL,eEndif,0},
    {"Pixels",&BuiltinInfo.nDisplayAllPixelCount,1,288},
    {"From",&BuiltinInfo.bDisplayAllFromMiddle,0,0,0,"Middle","End"},
}
#endif
;
EXTERN BiMenu TwinkleMenu[MAX_BI_MENUS]
#ifdef MIW_MAIN
=
{
    {"One or Many",&BuiltinInfo.bTwinkleOnlyOne,0,0,0,"One","Many"},
}
#endif
;
EXTERN BiMenu WedgeMenu[MAX_BI_MENUS]
#ifdef MIW_MAIN
=
{
    {"Fill Wedge",&BuiltinInfo.bWedgeFill,0,0,0,"Solid","<"},
    {"Red: %d",&BuiltinInfo.nWedgeRed,0,255},
    {"Green: %d",&BuiltinInfo.nWedgeGreen,0,255},
    {"Blue: %d",&BuiltinInfo.nWedgeBlue,0,255},
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
EXTERN BuiltInItem BuiltInFiles[]
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
    {"Sine Trails",TestSine,SineMenu},
    {"Solid Color",DisplayLedLightBar,LedLightBarMenu},
    {"Stripes",TestStripes},
    {"Twinkle",TestTwinkle,TwinkleMenu},
    {"Two Dots",OppositeRunningDots},
    {"Wedge",TestWedge,WedgeMenu},
}
#endif
;
