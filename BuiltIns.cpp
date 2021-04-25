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
