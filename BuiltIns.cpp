#include "MagicImageWand-M5StackGray.h"

#define BARBERSIZE 10
#define BARBERCOUNT 40
void BarberPole()
{
    CRGB:CRGB red, white, blue;
    byte r, g, b;
    r = 255, g = 0, b = 0;
    fixRGBwithGamma(&r, &g, &b);
    red = CRGB(r, g, b);
    r = 255, g = 255, b = 255;
    fixRGBwithGamma(&r, &g, &b);
    white = CRGB(r, g, b);
    r = 0, g = 0, b = 255;
    fixRGBwithGamma(&r, &g, &b);
    blue = CRGB(r, g, b);
    bool done = false;
    for (int loop = 0; !done; ++loop) {
        if (CheckCancel()) {
            done = true;
            break;
        }
        for (int ledIx = 0; ledIx < LedInfo.nPixelCount; ++ledIx) {
            if (CheckCancel()) {
                done = true;
                break;
            }
            // figure out what color
            switch (((ledIx + loop) % BARBERCOUNT) / BARBERSIZE) {
            case 0: // red
                SetPixel(ledIx, red);
                break;
            case 1: // white
            case 3:
                SetPixel(ledIx, white);
                break;
            case 2: // blue
                SetPixel(ledIx, blue);
                break;
            }
        }
        ShowLeds();
        delay(ImgInfo.nColumnHoldTime);
    }
}

void bpm()
{
    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    CRGBPalette16 palette = PartyColors_p;
    uint8_t beat = beatsin8(BuiltinInfo.nBpmBeatsPerMinute, 64, 255);
    for (int i = 0; i < LedInfo.nPixelCount; i++) { //9948
        SetPixel(i, ColorFromPalette(palette, BuiltinInfo.gHue + (i * 2), beat - BuiltinInfo.gHue + (i * 10)));
    }
    if (BuiltinInfo.bBpmCycleHue)
        ++BuiltinInfo.gHue;
}

void TestBpm()
{
    BuiltinInfo.gHue = 0;
    bool done = false;
    while (!done) {
        EVERY_N_MILLISECONDS(ImgInfo.nColumnHoldTime) {
            bpm();
            ShowLeds();
        }
        if (CheckCancel()) {
            done = true;
            break;
        }
    }
}

// create the menu from the list
void BuiltInMenu(String hdr, BiMenu* menuList)
{
    std::stack<bool> condition;
    ezMenu* pSettings;
	int16_t ix = 1;
    bool bAddingMenu = true;
    while (ix != 0) {
        pSettings = new ezMenu(hdr);
        pSettings->txtSmall();
        pSettings->buttons("up # # Go # Back # down # ");
		for (int ix = 0; menuList[ix].title || menuList[ix].min; ++ix) {
            // NULL title means a conditional menu section
			if (menuList[ix].title != NULL) {
                if (bAddingMenu) {
                    // is this bool or int?
                    if (menuList[ix].yes == NULL) {
                        // add int
                        pSettings->addItem(menuList[ix].title, (int*)menuList[ix].pData, menuList[ix].min, menuList[ix].max, menuList[ix].decimals, HandleMenuInteger);
                    }
                    else {
                        // add bool
                        pSettings->addItem(menuList[ix].title, (bool*)menuList[ix].pData, menuList[ix].yes, menuList[ix].no, ToggleBool);
                    }
                }
            }
            else {
                // handle the conditional menu section, nesting is allowed
                switch (menuList[ix].min) {
				case eIfEqual:
                    // save the current on the stack
                    condition.push(bAddingMenu);
                    // test the value
                    if (menuList[ix].decimals == 0) {
                        if (*(int*)(menuList[ix].pData) != menuList[ix].max) {
                            bAddingMenu = false;
                        }
                    }
                    else {
                        if (*(bool*)(menuList[ix].pData) != (bool)(menuList[ix].max)) {
                            bAddingMenu = false;
                        }
                    }
                    break;
				case eElse:
                    bAddingMenu = !bAddingMenu;
                    break;
                case eEndif:
                    // get the stack value back
					bAddingMenu = condition.top();
                    // remove the old one
                    condition.pop();
                    break;
                }
            }
        }
        pSettings->setItem(ix);
        ix = pSettings->runOnce();
        delete pSettings;
    }
}

void BouncingColoredBalls(int balls, CRGB colors[]) {
    time_t startsec = time(NULL);
    float Gravity = -9.81;
    int StartHeight = 1;

    float* Height = (float*)calloc(balls, sizeof(float));
    float* ImpactVelocity = (float*)calloc(balls, sizeof(float));
    float* TimeSinceLastBounce = (float*)calloc(balls, sizeof(float));
    int* Position = (int*)calloc(balls, sizeof(int));
    long* ClockTimeSinceLastBounce = (long*)calloc(balls, sizeof(long));
    float* Dampening = (float*)calloc(balls, sizeof(float));
    float ImpactVelocityStart = sqrt(-2 * Gravity * StartHeight);

    for (int i = 0; i < balls; i++) {
        ClockTimeSinceLastBounce[i] = millis();
        Height[i] = StartHeight;
        Position[i] = 0;
        ImpactVelocity[i] = ImpactVelocityStart;
        TimeSinceLastBounce[i] = 0;
        Dampening[i] = 0.90 - float(i) / pow(balls, 2);
    }

    long percent;
    int colorChangeCounter = 0;
    bool done = false;
    while (!done) {
        if (CheckCancel()) {
            done = true;
            break;
        }
        for (int i = 0; i < balls; i++) {
            if (CheckCancel()) {
                done = true;
                break;
            }
            TimeSinceLastBounce[i] = millis() - ClockTimeSinceLastBounce[i];
            Height[i] = 0.5 * Gravity * pow(TimeSinceLastBounce[i] / BuiltinInfo.nBouncingBallsDecay, 2.0) + ImpactVelocity[i] * TimeSinceLastBounce[i] / BuiltinInfo.nBouncingBallsDecay;

            if (Height[i] < 0) {
                Height[i] = 0;
                ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
                ClockTimeSinceLastBounce[i] = millis();

                if (ImpactVelocity[i] < 0.01) {
                    ImpactVelocity[i] = ImpactVelocityStart;
                }
            }
            Position[i] = round(Height[i] * (LedInfo.nPixelCount - 1) / StartHeight);
        }

        for (int i = 0; i < balls; i++) {
            int ix;
            if (CheckCancel()) {
                done = true;
                break;
            }
            ix = (i + BuiltinInfo.nBouncingBallsFirstColor) % 32;
            SetPixel(Position[i], colors[ix]);
        }
        if (BuiltinInfo.nBouncingBallsChangeColors && colorChangeCounter++ > (BuiltinInfo.nBouncingBallsChangeColors * 100)) {
            ++BuiltinInfo.nBouncingBallsFirstColor;
            colorChangeCounter = 0;
        }
        ShowLeds();
        delayMicroseconds(50);
        FastLED.clear();
    }
    free(Height);
    free(ImpactVelocity);
    free(TimeSinceLastBounce);
    free(Position);
    free(ClockTimeSinceLastBounce);
    free(Dampening);
}

// up to 32 bouncing balls
void TestBouncingBalls() {
    CRGB colors[] = {
        CRGB::White,
        CRGB::Red,
        CRGB::Green,
        CRGB::Blue,
        CRGB::Yellow,
        CRGB::Cyan,
        CRGB::Magenta,
        CRGB::Grey,
        CRGB::RosyBrown,
        CRGB::RoyalBlue,
        CRGB::SaddleBrown,
        CRGB::Salmon,
        CRGB::SandyBrown,
        CRGB::SeaGreen,
        CRGB::Seashell,
        CRGB::Sienna,
        CRGB::Silver,
        CRGB::SkyBlue,
        CRGB::SlateBlue,
        CRGB::SlateGray,
        CRGB::SlateGrey,
        CRGB::Snow,
        CRGB::SpringGreen,
        CRGB::SteelBlue,
        CRGB::Tan,
        CRGB::Teal,
        CRGB::Thistle,
        CRGB::Tomato,
        CRGB::Turquoise,
        CRGB::Violet,
        CRGB::Wheat,
        CRGB::WhiteSmoke,
    };

    BouncingColoredBalls(BuiltinInfo.nBouncingBallsCount, colors);
    FastLED.clear(true);
}

// grow and shrink a rainbow type pattern
#define PI_SCALE 2
#define TWO_HUNDRED_PI (628*PI_SCALE)
void RainbowPulse()
{
    int element = 0;
    int last_element = 0;
    int highest_element = 0;
    //Serial.println("second: " + String(bSecondStrip));
    //Serial.println("Len: " + String(STRIPLENGTH));
    for (int i = 0; i < TWO_HUNDRED_PI; i++) {
        element = round((LedInfo.nPixelCount - 1) / 2 * (-cos(i / (PI_SCALE * 100.0)) + 1));
        //Serial.println("elements: " + String(element) + " " + String(last_element));
        if (element > last_element) {
            SetPixel(element, CHSV(element * BuiltinInfo.nRainbowPulseColorScale + BuiltinInfo.nRainbowPulseStartColor, BuiltinInfo.nRainbowPulseSaturation, 255));
            ShowLeds();
            highest_element = max(highest_element, element);
        }
        if (CheckCancel()) {
            break;
        }
        delayMicroseconds(BuiltinInfo.nRainbowPulsePause * 10);
        if (element < last_element) {
            // cleanup the highest one
            SetPixel(highest_element, CRGB::Black);
            SetPixel(element, CRGB::Black);
            ShowLeds();
        }
        last_element = element;
    }
}

// checkerboard
void CheckerBoard()
{
    int width = BuiltinInfo.nCheckboardBlackWidth + BuiltinInfo.nCheckboardWhiteWidth;
    int times = 0;
    CRGB color1 = CRGB::Black, color2 = CRGB::White;
    int addPixels = 0;
    bool done = false;
    while (!done) {
        for (int y = 0; y < LedInfo.nPixelCount; ++y) {
            SetPixel(y, ((y + addPixels) % width) < BuiltinInfo.nCheckboardBlackWidth ? color1 : color2);
        }
        ShowLeds();
        int count = BuiltinInfo.nCheckerboardHoldframes;
        while (count-- > 0) {
            delay(ImgInfo.nColumnHoldTime);
            if (CheckCancel()) {
                done = true;
                break;
            }
        }
        if (BuiltinInfo.bCheckerBoardAlternate && (times++ % 2)) {
            // swap colors
            CRGB temp = color1;
            color1 = color2;
            color2 = temp;
        }
        addPixels += BuiltinInfo.nCheckerboardAddPixels;
        if (CheckCancel()) {
            done = true;
            break;
        }
    }
}

void confetti()
{
    // random colored speckles that blink in and fade smoothly
    fadeToBlackBy(leds, LedInfo.nPixelCount, 10);
    int pos = random16(LedInfo.nPixelCount);
    leds[pos] += CHSV(BuiltinInfo.gHue + random8(64), 200, 255);
}

void TestConfetti()
{
    time_t start = time(NULL);
    BuiltinInfo.gHue = 0;
    bool done = false;
    while (!done) {
        EVERY_N_MILLISECONDS(ImgInfo.nColumnHoldTime) {
            if (BuiltinInfo.bConfettiCycleHue)
                ++BuiltinInfo.gHue;
            confetti();
            ShowLeds();
        }
        if (CheckCancel()) {
            done = true;
            break;
        }
    }
    // wait for timeout so strip will be blank
    delay(100);
}

void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay)
{
    for (int i = 0; i < LedInfo.nPixelCount - EyeSize - 2; i++) {
        if (CheckCancel()) {
            break;
        }
        FastLED.clear();
        SetPixel(i, CRGB(red / 10, green / 10, blue / 10));
        for (int j = 1; j <= EyeSize; j++) {
            SetPixel(i + j, CRGB(red, green, blue));
        }
        SetPixel(i + EyeSize + 1, CRGB(red / 10, green / 10, blue / 10));
        ShowLeds();
        delay(SpeedDelay);
    }
    delay(ReturnDelay);
    for (int i = LedInfo.nPixelCount - EyeSize - 2; i > 0; i--) {
        if (CheckCancel()) {
            break;
        }
        FastLED.clear();
        SetPixel(i, CRGB(red / 10, green / 10, blue / 10));
        for (int j = 1; j <= EyeSize; j++) {
            SetPixel(i + j, CRGB(red, green, blue));
        }
        SetPixel(i + EyeSize + 1, CRGB(red / 10, green / 10, blue / 10));
        ShowLeds();
        delay(SpeedDelay);
    }
    FastLED.clear(true);
}

void TestCylon()
{
    CylonBounce(BuiltinInfo.nCylonEyeRed, BuiltinInfo.nCylonEyeGreen, BuiltinInfo.nCylonEyeBlue, BuiltinInfo.nCylonEyeSize, ImgInfo.nColumnHoldTime, 50);
}

void juggle()
{
    // eight colored dots, weaving in and out of sync with each other
    fadeToBlackBy(leds, LedInfo.nPixelCount, 20);
    byte dothue = 0;
    uint16_t index;
    for (int i = 0; i < 8; i++) {
        index = beatsin16(i + 7, 0, LedInfo.nPixelCount);
        // use AdjustStripIndex to get the right one
        SetPixel(index, leds[AdjustStripIndex(index)] | CHSV(dothue, 255, 255));
        //leds[beatsin16(i + 7, 0, STRIPLENGTH)] |= CHSV(dothue, 255, 255);
        dothue += 32;
    }
}

void TestJuggle()
{
    bool done = false;
    while (!done) {
        EVERY_N_MILLISECONDS(ImgInfo.nColumnHoldTime) {
            juggle();
            ShowLeds();
        }
        if (CheckCancel()) {
            done = true;
            break;
        }
    }
}

// alternating white and black lines
void TestLines()
{
    FastLED.clear(true);
    bool bWhite = true;
    for (int pix = 0; pix < LedInfo.nPixelCount; ++pix) {
        // fill in each block of pixels
        for (int len = 0; len < (bWhite ? BuiltinInfo.nLinesWhite : BuiltinInfo.nLinesBlack); ++len) {
            SetPixel(pix++, bWhite ? CRGB::White : CRGB::Black);
        }
        bWhite = !bWhite;
    }
    ShowLeds();
    bool done = false;
    while (!done) {
        if (CheckCancel()) {
            done = true;
            break;
        }
        delay(1000);
        // might make this work to toggle blacks and whites eventually
        //for (int ix = 0; ix < STRIPLENGTH; ++ix) {
        //	leds[ix] = (leds[ix] == CRGB::White) ? CRGB::Black : CRGB::White;
        //}
        ShowLeds();
    }
    FastLED.clear(true);
}

void fadeToBlack(int ledNo, byte fadeValue) {
    // FastLED
    leds[ledNo].fadeToBlackBy(fadeValue);
}

void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay)
{
    FastLED.clear(true);

    for (int i = 0; i < LedInfo.nPixelCount + LedInfo.nPixelCount; i++) {
        if (CheckCancel())
            break;;
        // fade brightness all LEDs one step
        for (int j = 0; j < LedInfo.nPixelCount; j++) {
            if (CheckCancel())
                break;
            if ((!meteorRandomDecay) || (random(10) > 5)) {
                fadeToBlack(j, meteorTrailDecay);
            }
        }
        // draw meteor
        for (int j = 0; j < meteorSize; j++) {
            if (CheckCancel())
                break;
            if ((i - j < LedInfo.nPixelCount) && (i - j >= 0)) {
                SetPixel(i - j, CRGB(red, green, blue));
            }
        }
        ShowLeds();
        delay(SpeedDelay);
    }
}

void TestMeteor() {
    meteorRain(BuiltinInfo.nMeteorRed, BuiltinInfo.nMeteorGreen, BuiltinInfo.nMeteorBlue, BuiltinInfo.nMeteorSize, 64, true, 30);
}

// running dot
void RunningDot()
{
    for (int colorvalue = 0; colorvalue <= 3; ++colorvalue) {
        // RGBW
        byte r, g, b;
        switch (colorvalue) {
        case 0: // red
            r = 255;
            g = 0;
            b = 0;
            break;
        case 1: // green
            r = 0;
            g = 255;
            b = 0;
            break;
        case 2: // blue
            r = 0;
            g = 0;
            b = 255;
            break;
        case 3: // white
            r = 255;
            g = 255;
            b = 255;
            break;
        }
        fixRGBwithGamma(&r, &g, &b);
        char line[10];
        for (int ix = 0; ix < LedInfo.nPixelCount; ++ix) {
            if (CheckCancel()) {
                break;
            }
            if (ix > 0) {
                SetPixel(ix - 1, CRGB::Black);
            }
            SetPixel(ix, CRGB(r, g, b));
            ShowLeds();
            delay(ImgInfo.nColumnHoldTime);
        }
        // remember the last one, turn it off
        SetPixel(LedInfo.nPixelCount - 1, CRGB::Black);
        ShowLeds();
    }
    FastLED.clear(true);
}

void FillRainbow(struct CRGB* pFirstLED, int numToFill,
    uint8_t initialhue,
    int deltahue)
{
    CHSV hsv;
    hsv.hue = initialhue;
    hsv.val = 255;
    hsv.sat = 240;
    for (int i = 0; i < numToFill; i++) {
        pFirstLED[AdjustStripIndex(i)] = hsv;
        hsv.hue += deltahue;
    }
}

// time is in mSec
void FadeInOut(int time, bool in)
{
    if (in) {
        for (int i = 0; i <= LedInfo.nLEDBrightness; ++i) {
            FastLED.setBrightness(i);
            ShowLeds();
            delay(time / LedInfo.nLEDBrightness);
        }
    }
    else {
        for (int i = LedInfo.nLEDBrightness; i >= 0; --i) {
            FastLED.setBrightness(i);
            ShowLeds();
            delay(time / LedInfo.nLEDBrightness);
        }
    }
}

void addGlitter(fract8 chanceOfGlitter)
{
    if (random8() < chanceOfGlitter) {
        leds[random16(LedInfo.nPixelCount)] += CRGB::White;
    }
}

void TestRainbow()
{
    BuiltinInfo.gHue = BuiltinInfo.nRainbowInitialHue;
    FillRainbow(leds, LedInfo.nPixelCount, BuiltinInfo.gHue, BuiltinInfo.nRainbowHueDelta);
    FadeInOut(BuiltinInfo.nRainbowFadeTime * 100, true);
    bool done = false;
    while (!done) {
        EVERY_N_MILLISECONDS(ImgInfo.nColumnHoldTime) {
            if (BuiltinInfo.bRainbowCycleHue)
                ++BuiltinInfo.gHue;
            FillRainbow(leds, LedInfo.nPixelCount, BuiltinInfo.gHue, BuiltinInfo.nRainbowHueDelta);
            if (BuiltinInfo.bRainbowAddGlitter)
                addGlitter(80);
            ShowLeds();
        }
        if (CheckCancel()) {
            done = true;
            FastLED.setBrightness(LedInfo.nLEDBrightness);
            break;
        }
    }
    FadeInOut(BuiltinInfo.nRainbowFadeTime * 100, false);
    FastLED.setBrightness(LedInfo.nLEDBrightness);
}

// show random bars of lights with optional blacks between
void ShowRandomBars(bool blacks)
{
    time_t start = time(NULL);
    byte r, g, b;
    srand(millis());
    char line[40];
    bool done = false;
    for (int pass = 0; !done; ++pass) {
        if (blacks && (pass % 2)) {
            // odd numbers, clear
            FastLED.clear(true);
        }
        else {
            // even numbers, show bar
            r = random(0, 255);
            g = random(0, 255);
            b = random(0, 255);
            fixRGBwithGamma(&r, &g, &b);
            // fill the strip color
            FastLED.showColor(CRGB(r, g, b));
        }
        int count = BuiltinInfo.nRandomBarsHoldframes;
        while (count-- > 0) {
            delay(ImgInfo.nColumnHoldTime);
            if (CheckCancel()) {
                done = true;
                break;
            }
        }
    }
}

void RandomBars()
{
    ShowRandomBars(BuiltinInfo.bRandomBarsBlacks);
}

void sinelon()
{
    // a colored dot sweeping back and forth, with fading trails
    fadeToBlackBy(leds, LedInfo.nPixelCount, 20);
    int pos = beatsin16(BuiltinInfo.nSineSpeed, 0, LedInfo.nPixelCount);
    leds[AdjustStripIndex(pos)] += CHSV(BuiltinInfo.gHue, 255, 192);
    if (BuiltinInfo.bSineCycleHue)
        ++BuiltinInfo.gHue;
}

void TestSine()
{
	BuiltinInfo.gHue = BuiltinInfo.nSineStartingHue;
    bool done = false;
    while (!done) {
        EVERY_N_MILLISECONDS(ImgInfo.nColumnHoldTime) {
            sinelon();
            ShowLeds();
        }
        if (CheckCancel()) {
            done = true;
            break;
        }
    }
}

// handle rollover when -ve
// inc 1 gives 255, inc 10 gives 250, inc 100 gives 200
int RollDownRollOver(int inc)
{
    if (inc == 1)
        return 255;
    int retval = 256;
    retval -= retval % inc;
    return retval;
}

// utility for DisplayLedLightBar()
void FillLightBar()
{
    int offset = BuiltinInfo.bDisplayAllFromMiddle ? (LedInfo.nPixelCount - BuiltinInfo.nDisplayAllPixelCount) / 2 : 0;
    if (!BuiltinInfo.bDisplayAllFromMiddle && ImgInfo.bUpsideDown)
        offset = LedInfo.nPixelCount - BuiltinInfo.nDisplayAllPixelCount;
    FastLED.clear();
    for (int ix = 0; ix < BuiltinInfo.nDisplayAllPixelCount; ++ix) {
        SetPixel(ix + offset, BuiltinInfo.bDisplayAllRGB ? CRGB(BuiltinInfo.nDisplayAllRed, BuiltinInfo.nDisplayAllGreen, BuiltinInfo.nDisplayAllBlue) : CHSV(BuiltinInfo.nDisplayAllHue, BuiltinInfo.nDisplayAllSaturation, BuiltinInfo.nDisplayAllBrightness));
    }
    ShowLeds();
}

// Used LEDs as a light bar
void DisplayLedLightBar()
{
    ezMenu* pMenu = NULL;
    FillLightBar();
    int16_t ix = 1;
	while (true) {
		if (pMenu)
			delete pMenu;
		pMenu = new ezMenu("LightBar");
		pMenu->txtSmall();
		pMenu->buttons("up # # Go # Exit # down #");
        //pMenu->addItem("Exit");
        pMenu->addItem("Color Mode", &BuiltinInfo.bDisplayAllRGB, "RGB", "HSL", ToggleBool);
		if (BuiltinInfo.bDisplayAllRGB) {
			pMenu->addItem("Red", &BuiltinInfo.nDisplayAllRed, 0, 255, 0, HandleMenuInteger);
			pMenu->addItem("Green", &BuiltinInfo.nDisplayAllGreen, 0, 255, 0, HandleMenuInteger);
			pMenu->addItem("Blue", &BuiltinInfo.nDisplayAllBlue, 0, 255, 0, HandleMenuInteger);
		}
		else {
			pMenu->addItem("HUE", &BuiltinInfo.nDisplayAllHue, 0, 255, 0, HandleMenuInteger);
			pMenu->addItem("Saturation", &BuiltinInfo.nDisplayAllSaturation, 0, 255, 0, HandleMenuInteger);
			pMenu->addItem("Brightness", &BuiltinInfo.nDisplayAllBrightness, 0, 255, 0, HandleMenuInteger);
		}
		pMenu->addItem("Pixels", &BuiltinInfo.nDisplayAllPixelCount, 1, LedInfo.nPixelCount, 0, HandleMenuInteger);
		pMenu->addItem("From", &BuiltinInfo.bDisplayAllFromMiddle, "Middle", "End", ToggleBool);
        pMenu->setItem(ix);
		ix = pMenu->runOnce();
        String str = pMenu->pickButton();
        if (str == "Exit") {
            break;
        }
		if (CheckCancel()) {
			break;
		}
		FillLightBar();
	}
	if (pMenu)
		delete pMenu;
    FastLED.clear(true);
}

// create a user defined stripe set
// it consists of a list of stripes, each of which have a width and color
// there can be up to 10 of these
#define NUM_STRIPES 20
struct {
    int start;
    int length;
    CHSV color;
} Stripes[NUM_STRIPES];

void TestStripes()
{
    // let's fill in some data
    for (int ix = 0; ix < NUM_STRIPES; ++ix) {
        Stripes[ix].start = ix * 20;
        Stripes[ix].length = 12;
        Stripes[ix].color = CHSV(0, 0, 255);
    }
    int pix = 0;	// pixel address
    FastLED.clear(true);
    for (int ix = 0; ix < NUM_STRIPES; ++ix) {
        pix = Stripes[ix].start;
        // fill in each block of pixels
        for (int len = 0; len < Stripes[ix].length; ++len) {
            SetPixel(pix++, CRGB(Stripes[ix].color));
        }
    }
    ShowLeds();
    bool done = false;
    while (!done) {
        if (CheckCancel()) {
            done = true;
            break;
        }
        delay(1000);
    }
}

void TwinkleRandom(int SpeedDelay, boolean OnlyOne) {
    time_t start = time(NULL);
    bool done = false;
    while (!done) {
        SetPixel(random(LedInfo.nPixelCount), CRGB(random(0, 255), random(0, 255), random(0, 255)));
        ShowLeds();
        delay(SpeedDelay);
        if (OnlyOne) {
            FastLED.clear(true);
        }
        if (CheckCancel()) {
            done = true;
            break;
        }
    }
}

void TestTwinkle() {
    TwinkleRandom(ImgInfo.nColumnHoldTime, BuiltinInfo.bTwinkleOnlyOne);
}

void OppositeRunningDots()
{
    for (int mode = 0; mode <= 3; ++mode) {
        if (CheckCancel())
            break;;
        // RGBW
        byte r, g, b;
        switch (mode) {
        case 0: // red
            r = 255;
            g = 0;
            b = 0;
            break;
        case 1: // green
            r = 0;
            g = 255;
            b = 0;
            break;
        case 2: // blue
            r = 0;
            g = 0;
            b = 255;
            break;
        case 3: // white
            r = 255;
            g = 255;
            b = 255;
            break;
        }
        fixRGBwithGamma(&r, &g, &b);
        for (int ix = 0; ix < LedInfo.nPixelCount; ++ix) {
            if (CheckCancel())
                return;
            if (ix > 0) {
                SetPixel(ix - 1, CRGB::Black);
                SetPixel(LedInfo.nPixelCount - ix + 1, CRGB::Black);
            }
            SetPixel(LedInfo.nPixelCount - ix, CRGB(r, g, b));
            SetPixel(ix, CRGB(r, g, b));
            ShowLeds();
            delay(ImgInfo.nColumnHoldTime);
        }
    }
}

/*
    Write a wedge in time, from the middle out
*/
void TestWedge()
{
    int midPoint = LedInfo.nPixelCount / 2 - 1;
    for (int ix = 0; ix < LedInfo.nPixelCount / 2; ++ix) {
        SetPixel(midPoint + ix, CRGB(BuiltinInfo.nWedgeRed, BuiltinInfo.nWedgeGreen, BuiltinInfo.nWedgeBlue));
        SetPixel(midPoint - ix, CRGB(BuiltinInfo.nWedgeRed, BuiltinInfo.nWedgeGreen, BuiltinInfo.nWedgeBlue));
        if (!BuiltinInfo.bWedgeFill) {
            if (ix > 1) {
                SetPixel(midPoint + ix - 1, CRGB::Black);
                SetPixel(midPoint - ix + 1, CRGB::Black);
            }
            else {
                SetPixel(midPoint, CRGB::Black);
            }
        }
        ShowLeds();
        delay(ImgInfo.nColumnHoldTime);
        if (CheckCancel()) {
            return;
        }
    }
    FastLED.clear(true);
}

// show on leds or display
// mode 0 is normal, mode 1 is prepare for LCD, mode 2 is reset to normal
void ShowLeds(int mode)
{
	static uint16_t* scrBuf;
    static int col;
	if (scrBuf == NULL && mode == 0) {
        FastLED.show();
        return;
    }
    else if (mode == 0) {
        for (int ix = 0; ix < 144; ++ix) {
            uint16_t color = m5.Lcd.color565(leds[ix].r, leds[ix].g, leds[ix].b);
            uint16_t sbcolor;
            // the memory image colors are byte swapped
            swab(&color, &sbcolor, sizeof(uint16_t));
            scrBuf[ix] = sbcolor;
        }
        m5.Lcd.pushRect(col, 0, 1, 144, scrBuf);
        ++col;
        col = col % 320;
    }
	else if (mode == 1) {
        col = 0;
        ez.canvas.clear();
        scrBuf = (uint16_t*)calloc(144, sizeof(uint16_t));
    }
    else if (mode == 2) {
        free(scrBuf);
        delay(2000);
    }
}
