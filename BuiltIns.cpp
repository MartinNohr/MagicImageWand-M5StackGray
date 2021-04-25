#include "MagicImageWand-M5StackGray.h"
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
void TestWedge() {};


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

struct BI_MENU {
    char* title;
    void* pData;
    int min, max, decimals; // all 0 for booleans
    char* yes;              // null for integers
    char* no;               // null for integers
};
typedef BI_MENU BiMenu;
BiMenu BpmMenuList[] =
{
    {"Beats per minute: ",&nBpmBeatsPerMinute,1,300,0},
    {"Cycle Hue:",&bBpmCycleHue,0,0,0,"Yes","No"},
};

void BpmMenu()
{
    ezMenu* pSettings;
    int16_t ix = 1;
    while (ix != 0) {
        pSettings = new ezMenu("Beats Settings");
        pSettings->txtSmall();
        pSettings->buttons("up # # Go # Back # down # ");
        pSettings->addItem("Beats per minute:", &nBpmBeatsPerMinute, 1, 300, 0, HandleMenuInteger);
        pSettings->addItem("Cycle Hue:", &bBpmCycleHue, "Yes", "No", ToggleBool);
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
