#pragma once
#define MIW_VERSION "1.00"
bool bSdCardValid = false;
#define NEXT_FOLDER_CHAR '>'
#define PREVIOUS_FOLDER_CHAR '<'
String currentFolder = "/";
std::vector<String> FileNames;
RTC_DATA_ATTR bool bShowBuiltInTests = false;           // list the internal file instead of the SD card
RTC_DATA_ATTR int CurrentFileIndex = 0;
RTC_DATA_ATTR int nBootCount = 0;
bool bIsRunning = false;								// system state, idle or running
bool bSettingsMode = false;                             // set true when settings are displayed
// we need to have a pointer reference to this in the MenuItem, the full declaration follows later
struct BuiltInItem;
// built-in "files"
struct BuiltInItem {
    const char* text;
    void(*function)();
    //MenuItem* menu;
};
typedef BuiltInItem BuiltInItem;
void BarberPole() {};
void TestBouncingBalls() {};
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
void TestBpm() {};
void TestConfetti() {};
void DisplayLedLightBar() {};
void TestStripes() {};
void TestLines() {};
void RainbowPulse() {};
void TestWedge() {};
BuiltInItem BuiltInFiles[] = {
    {"Barber Pole",BarberPole},
    {"Beats",TestBpm/*,BpmMenu*/},
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
