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
        FastLED.show();
        delay(ImgInfo.nColumnHoldTime);
    }
}

void bpm()
{
    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    CRGBPalette16 palette = PartyColors_p;
    uint8_t beat = beatsin8(nBpmBeatsPerMinute, 64, 255);
    for (int i = 0; i < LedInfo.nPixelCount; i++) { //9948
        SetPixel(i, ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10)));
    }
    if (bBpmCycleHue)
        ++gHue;
}

void TestBpm()
{
    gHue = 0;
    bool done = false;
    while (!done) {
        EVERY_N_MILLISECONDS(ImgInfo.nColumnHoldTime) {
            bpm();
            FastLED.show();
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
    ezMenu* pSettings;
    int16_t ix = 1;
    while (ix != 0) {
        pSettings = new ezMenu(hdr);
        pSettings->txtSmall();
        pSettings->buttons("up # # Go # Back # down # ");
        for (int ix = 0; menuList[ix].title; ++ix) {
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
            Height[i] = 0.5 * Gravity * pow(TimeSinceLastBounce[i] / nBouncingBallsDecay, 2.0) + ImpactVelocity[i] * TimeSinceLastBounce[i] / nBouncingBallsDecay;

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
            ix = (i + nBouncingBallsFirstColor) % 32;
            SetPixel(Position[i], colors[ix]);
        }
        if (nBouncingBallsChangeColors && colorChangeCounter++ > (nBouncingBallsChangeColors * 100)) {
            ++nBouncingBallsFirstColor;
            colorChangeCounter = 0;
        }
        FastLED.show();
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

    BouncingColoredBalls(nBouncingBallsCount, colors);
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
            SetPixel(element, CHSV(element * nRainbowPulseColorScale + nRainbowPulseStartColor, nRainbowPulseSaturation, 255));
            FastLED.show();
            highest_element = max(highest_element, element);
        }
        if (CheckCancel()) {
            break;
        }
        delayMicroseconds(nRainbowPulsePause * 10);
        if (element < last_element) {
            // cleanup the highest one
            SetPixel(highest_element, CRGB::Black);
            SetPixel(element, CRGB::Black);
            FastLED.show();
        }
        last_element = element;
    }
}

// checkerboard
void CheckerBoard()
{
    int width = nCheckboardBlackWidth + nCheckboardWhiteWidth;
    int times = 0;
    CRGB color1 = CRGB::Black, color2 = CRGB::White;
    int addPixels = 0;
    bool done = false;
    while (!done) {
        for (int y = 0; y < LedInfo.nPixelCount; ++y) {
            SetPixel(y, ((y + addPixels) % width) < nCheckboardBlackWidth ? color1 : color2);
        }
        FastLED.show();
        int count = nCheckerboardHoldframes;
        while (count-- > 0) {
            delay(ImgInfo.nColumnHoldTime);
            if (CheckCancel()) {
                done = true;
                break;
            }
        }
        if (bCheckerBoardAlternate && (times++ % 2)) {
            // swap colors
            CRGB temp = color1;
            color1 = color2;
            color2 = temp;
        }
        addPixels += nCheckerboardAddPixels;
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
    leds[pos] += CHSV(gHue + random8(64), 200, 255);
}

void TestConfetti()
{
    time_t start = time(NULL);
    gHue = 0;
    bool done = false;
    while (!done) {
        EVERY_N_MILLISECONDS(ImgInfo.nColumnHoldTime) {
            if (bConfettiCycleHue)
                ++gHue;
            confetti();
            FastLED.show();
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
        FastLED.show();
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
        FastLED.show();
        delay(SpeedDelay);
    }
    FastLED.clear(true);
}

void TestCylon()
{
    CylonBounce(nCylonEyeRed, nCylonEyeGreen, nCylonEyeBlue, nCylonEyeSize, ImgInfo.nColumnHoldTime, 50);
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
            FastLED.show();
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
        for (int len = 0; len < (bWhite ? nLinesWhite : nLinesBlack); ++len) {
            SetPixel(pix++, bWhite ? CRGB::White : CRGB::Black);
        }
        bWhite = !bWhite;
    }
    FastLED.show();
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
        FastLED.show();
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
        FastLED.show();
        delay(SpeedDelay);
    }
}

void TestMeteor() {
    meteorRain(nMeteorRed, nMeteorGreen, nMeteorBlue, nMeteorSize, 64, true, 30);
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
            FastLED.show();
            delay(ImgInfo.nColumnHoldTime);
        }
        // remember the last one, turn it off
        SetPixel(LedInfo.nPixelCount - 1, CRGB::Black);
        FastLED.show();
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
            FastLED.show();
            delay(time / LedInfo.nLEDBrightness);
        }
    }
    else {
        for (int i = LedInfo.nLEDBrightness; i >= 0; --i) {
            FastLED.setBrightness(i);
            FastLED.show();
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
    gHue = nRainbowInitialHue;
    FillRainbow(leds, LedInfo.nPixelCount, gHue, nRainbowHueDelta);
    FadeInOut(nRainbowFadeTime * 100, true);
    bool done = false;
    while (!done) {
        EVERY_N_MILLISECONDS(ImgInfo.nColumnHoldTime) {
            if (bRainbowCycleHue)
                ++gHue;
            FillRainbow(leds, LedInfo.nPixelCount, gHue, nRainbowHueDelta);
            if (bRainbowAddGlitter)
                addGlitter(80);
            FastLED.show();
        }
        if (CheckCancel()) {
            done = true;
            FastLED.setBrightness(LedInfo.nLEDBrightness);
            break;
        }
    }
    FadeInOut(nRainbowFadeTime * 100, false);
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
        int count = nRandomBarsHoldframes;
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
    ShowRandomBars(bRandomBarsBlacks);
}

void sinelon()
{
    // a colored dot sweeping back and forth, with fading trails
    fadeToBlackBy(leds, LedInfo.nPixelCount, 20);
    int pos = beatsin16(nSineSpeed, 0, LedInfo.nPixelCount);
    leds[AdjustStripIndex(pos)] += CHSV(gHue, 255, 192);
    if (bSineCycleHue)
        ++gHue;
}

void TestSine()
{
    gHue = nSineStartingHue;
    bool done = false;
    while (!done) {
        EVERY_N_MILLISECONDS(ImgInfo.nColumnHoldTime) {
            sinelon();
            FastLED.show();
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
    int offset = bDisplayAllFromMiddle ? (LedInfo.nPixelCount - nDisplayAllPixelCount) / 2 : 0;
    if (!bDisplayAllFromMiddle && ImgInfo.bUpsideDown)
        offset = LedInfo.nPixelCount - nDisplayAllPixelCount;
    FastLED.clear();
    for (int ix = 0; ix < nDisplayAllPixelCount; ++ix) {
        SetPixel(ix + offset, bDisplayAllRGB ? CRGB(nDisplayAllRed, nDisplayAllGreen, nDisplayAllBlue) : CHSV(nDisplayAllHue, nDisplayAllSaturation, nDisplayAllBrightness));
    }
    FastLED.show();
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
		pMenu->buttons("up # # Go # # down #");
        pMenu->addItem("Exit");
        pMenu->addItem("Color Mode", &bDisplayAllRGB, "RGB", "HSL", ToggleBool);
		if (bDisplayAllRGB) {
			pMenu->addItem("Red", &nDisplayAllRed, 0, 255, 0, HandleMenuInteger);
			pMenu->addItem("Green", &nDisplayAllGreen, 0, 255, 0, HandleMenuInteger);
			pMenu->addItem("Blue", &nDisplayAllBlue, 0, 255, 0, HandleMenuInteger);
		}
		else {
			pMenu->addItem("HUE", &nDisplayAllHue, 0, 255, 0, HandleMenuInteger);
			pMenu->addItem("Saturation", &nDisplayAllSaturation, 0, 255, 0, HandleMenuInteger);
			pMenu->addItem("Brightness", &nDisplayAllBrightness, 0, 255, 0, HandleMenuInteger);
		}
		pMenu->addItem("Pixels", &nDisplayAllPixelCount, 1, LedInfo.nPixelCount, 0, HandleMenuInteger);
		pMenu->addItem("From", &bDisplayAllFromMiddle, "Middle", "End", ToggleBool);
        pMenu->setItem(ix);
		ix = pMenu->runOnce();
		//Serial.println("retval: " + String(ix));
		String str = ez.buttons.poll();
		//Serial.println("str: " + str);
		if (ix == 0) {
			delete pMenu;
			return;
		}
		if (CheckCancel()) {
			delete pMenu;
			return;
		}
		FillLightBar();
	}
    delete pMenu;
}

//void LightBar(MenuItem* menu)
//{
//    tft.fillScreen(TFT_BLACK);
//    DisplayLine(0, "LED Light Bar", menuTextColor);
//    DisplayLine(3, "Rotate Dial to Change", menuTextColor);
//    DisplayLine(4, "Click to Set Operation", menuTextColor);
//    DisplayLedLightBar();
//    FastLED.clear(true);
//    // these were set by CheckCancel() in DisplayAllColor() and need to be cleared
//    bCancelMacro = bCancelRun = false;
//}
