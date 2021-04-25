/*
 Name:		MagicImageWant_M5CoreGray.ino
 Created:	3/29/2021 4:08:35 PM
 Author:	Martin
*/
#define MIW_MAIN 1
#include "MagicImageWand-M5StackGray.h"

#define MAIN_DECLARED
String exit_button = "Exit";

ezMenu builtinMenu("Built-Ins");

float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;

float gyroX = 0.0F;
float gyroY = 0.0F;
float gyroZ = 0.0F;

float pitch = 0.0F;
float roll = 0.0F;
float yaw = 0.0F;

float temp = 0.0F;

//static const char* TAG = "lightwand";
//esp_timer_cb_t oneshot_timer_callback(void* arg)
void IRAM_ATTR oneshot_LED_timer_callback(void* arg)
{
    bStripWaiting = false;
    //int64_t time_since_boot = esp_timer_get_time();
    //Serial.println("in isr");
    //ESP_LOGI(TAG, "One-shot timer called, time since boot: %lld us", time_since_boot);
}

void setup() {
#include <themes/default.h>
#include <themes/dark.h>
    ezt::setDebug(INFO);
    ez.begin();
    Wire.begin();
    oneshot_LED_timer_args = {
                oneshot_LED_timer_callback,
                /* argument specified here will be passed to timer callback function */
                (void*)0,
                ESP_TIMER_TASK,
                "one-shotLED"
    };
    esp_timer_create(&oneshot_LED_timer_args, &oneshot_LED_timer);
    builtinMenu.txtSmall();
    ez.msgBox("Initializing", "LED test", "", false);
    // don't sort, we need the list in the same order as the one in the .h list of built-ins
    //builtinMenu.setSortFunction(CompareNames);
    for (BuiltInItem bi : BuiltInFiles) {
        if (bi.text == NULL)
            break;
        builtinMenu.addItem(bi.text);
    }
    builtinMenu.buttons("up # View # Go # Menu # down # SD");
    M5.IMU.Init();
    leds = (CRGB*)calloc(LedInfo.nPixelCount, sizeof(CRGB));
    FastLED.addLeds<NEOPIXEL, DATA_PIN1>(leds, 0, LedInfo.nPixelCount);
    FastLED.setBrightness(LedInfo.nLEDBrightness);
	FastLED.setMaxPowerInVoltsAndMilliamps(5, LedInfo.nStripMaxCurrent);
    //   for (int ix = 0; ix < LedInfo.nPixelCount; ++ix) {
 //       // note that SetPixel protects out of range locations
	//	SetPixel(ix, CRGB::Red);
	//	//SetPixel(ix + 1, CRGB::Green);
	//	//SetPixel(ix + 2, CRGB::Blue);
	//	FastLED.show();
	//	SetPixel(ix, CRGB::Black);
	//	//SetPixel(ix + 1, CRGB::Black);
	//	//SetPixel(ix + 2, CRGB::Black);
	//	delayMicroseconds(50);
 //   }
	//for (int ix = LedInfo.nPixelCount - 1; ix >= 0; --ix) {
 //       // note that SetPixel protects out of range locations
 //       SetPixel(ix, CRGB::Red);
 //       //SetPixel(ix + 1, CRGB::Green);
 //       //SetPixel(ix + 2, CRGB::Blue);
 //       FastLED.show();
 //       SetPixel(ix, CRGB::Black);
 //       //SetPixel(ix + 1, CRGB::Black);
 //       //SetPixel(ix + 2, CRGB::Black);
 //       delayMicroseconds(50);
 //   }
    RainbowPulse();
    //fill_rainbow(leds, LedInfo.nPixelCount, 0);
    //FastLED.show();
    //delay(1500);
    FastLED.clear(true);
}

ezMenu* pFileMenu = NULL;

ezMenu* activeMenu;

void loop() {
    static std::stack<int> selectionStack;
    static bool bReloadSD = true;
    bool bRetry = false;
    if (bShowBuiltInTests) {
        builtinMenu.buttons("up # Settings # Go # Menu # down # SD");
        activeMenu = &builtinMenu;
    }
    else {
        builtinMenu.buttons("up # View # Go # Menu # down # SD");
        if (bReloadSD) {
            if (pFileMenu != NULL)
                delete pFileMenu;
			pFileMenu = new ezMenu(currentFolder);
            pFileMenu->setSortFunction(CompareNames);
            if (!GetFileNames(currentFolder, pFileMenu)) {
                bRetry = true;
            }
            pFileMenu->buttons("up # View # Go # Menu # down # Internal");
            pFileMenu->txtSmall();
            if (!selectionStack.empty()) {
                Serial.println("setting: " + String(selectionStack.top()));
                pFileMenu->setItem(selectionStack.top());
                selectionStack.pop();
            }
            bReloadSD = false;
        }
        activeMenu = pFileMenu;
    }
	while (!bRetry && true) {
        int retNum = activeMenu->runOnce();
		String btnpressed = activeMenu->pickButton();
        currentFile = activeMenu->pickName();
        if (btnpressed == "Go") {
			// run the file or change the folder here
            String tmp = activeMenu->pickName();
            if (tmp[0] == NEXT_FOLDER_CHAR) {
                currentFolder = currentFolder + tmp.substring(1) + "/";
                currentFile = "";
                bReloadSD = true;
                Serial.println("index: " + String(retNum));
                selectionStack.push(retNum);
                // set the next location to the start of the menu
                selectionStack.push(1);
                break;
            }
            else if (tmp[0] == PREVIOUS_FOLDER_CHAR) {
                // remove the ending /
                currentFolder = currentFolder.substring(0, currentFolder.length() - 1);
                // remove after the last /
				currentFolder = currentFolder.substring(0, currentFolder.lastIndexOf('/') + 1);
                bReloadSD = true;
                break;
            }
            else {
                currentFile = tmp;
				// run the file
				ProcessFileOrTest();
            }
		}
		else if (btnpressed == "SD" || btnpressed == "Internal") {
            bShowBuiltInTests = !bShowBuiltInTests;
            if (!bSdCardValid) {
                bShowBuiltInTests = true;
            }
			break;
		}
		else if (btnpressed == "Menu") {
			SettingsMenu();
		}
		else if (btnpressed == "View") {
			// preview the file
            ShowBmp();
			//ez.msgBox("preview: ", String(activeMenu->pickName()));
		}
        else if (btnpressed == "Settings") {
            // internal settings
            int ix = activeMenu->pick() - 1;
            if (BuiltInFiles[ix].menuList) {
                // build and run the menu
				String hdr = activeMenu->getItemName() + " Settings";
                BuiltInMenu(hdr, BuiltInFiles[ix].menuList);
            }
            else {
                ez.msgBox("Built-In Settings", "No settings available", "OK", true, 0, TFT_WHITE);
            }
        }
		else {
			ez.msgBox("How did we get here?", btnpressed);
		}
	}
    //mainmenu.addItem("Flexible text menus", mainmenu_menus);
    ////mainmenu.addItem("Image menus", mainmenu_image);
    ////mainmenu.addItem("Neat messages", mainmenu_msgs);
    //mainmenu.addItem("Multi-function buttons", mainmenu_buttons);
    ////mainmenu.addItem("3-button text entry", mainmenu_entry);
    //mainmenu.addItem("Built-in wifi & other settings", ez.settings.menu);
    ////mainmenu.addItem("Updates via https", mainmenu_ota);
    //mainmenu.upOnFirst("last|up");
    //mainmenu.downOnLast("first|down");
    //mainmenu.run();
}

// compare strings for sort ignoring case
bool CompareNames(const char* a, const char* b)
{
    String a1 = a, b1 = b;
    a1.toUpperCase();
    b1.toUpperCase();
    // force folders to sort last
    if (a1[0] == NEXT_FOLDER_CHAR)
        a1[0] = '0x7e';
    if (b1[0] == NEXT_FOLDER_CHAR)
        b1[0] = '0x7e';
    // force previous folder to sort first
    if (a1[0] == PREVIOUS_FOLDER_CHAR)
        a1[0] = '0' - 1;
    if (b1[0] == PREVIOUS_FOLDER_CHAR)
        b1[0] = '0' - 1;
    return a1.compareTo(b1) < 0;
}

// maintain the array by subtracting the old value from the sum, adding the new value and putting the new value in the array
// increment and wrap the index
// then return the new sum divided by the size
float GetAverage(int& ix, int size, float val, float* fary, float& sum)
{
    sum -= fary[ix];
    sum += val;
    fary[ix] = val;
    ++ix;
    ix = ix % size;
    return sum / size;
}

#define SAMPLES 10
float tempSum;
float tempArray[SAMPLES];
int tempIx = 0;

float XSum;
float XArray[SAMPLES];
int XIx = 0;

float YSum;
float YArray[SAMPLES];
int YIx = 0;

float ZSum;
float ZArray[SAMPLES];
int ZIx = 0;
int ZOffset = -10;
int XOffset = 0;
void LevelDisplay()
{
    ez.screen.clear();
    ez.canvas.font(&FreeSans12pt7b);
    ez.buttons.show("V Offset#" + exit_button + "#H Offset");
    // the "box" displayed, height comes from z, tilt from x
    // x2y2 --- x3y3
    // |           |
    // x0y0 --- x2y2
    int x[4] = { 0 };
    int y[4] = { 0 };
    while (true) {
        String btn = ez.buttons.poll();
        if (btn == "Exit") 
			break;
        //M5.IMU.getGyroData(&gyroX, &gyroY, &gyroZ);
        M5.IMU.getAccelData(&accX, &accY, &accZ);
        //M5.IMU.getAhrsData(&pitch, &roll, &yaw);
        M5.IMU.getTempData(&temp);
        temp = GetAverage(tempIx, SAMPLES, temp, tempArray, tempSum);
        ez.canvas.pos(0, 20);
        accX = GetAverage(XIx, SAMPLES, accX, XArray, XSum) + (float)XOffset / 100;
        accY = GetAverage(YIx, SAMPLES, accY, YArray, YSum);
		accZ = GetAverage(ZIx, SAMPLES, accZ, ZArray, ZSum) + (float)ZOffset / 100;
        //ez.canvas.printf(" %5.2f   %5.2f   %5.2f   ", accX, accY, accZ);
		// erase previous lines, only if drawn
        //m5.Lcd.fillTriangle(x[0], y[0], x[1], y[1], x[2], y[2], BLACK);
        //m5.Lcd.fillTriangle(x[1], y[1], x[2], y[2], x[3], y[3], BLACK);
        if (x[0]) {
            for (int ix = 0; ix < 4; ++ix) {
                int pos1 = ix < 2 ? 0 : 3;
                int pos2 = (ix % 2) + 1;
                m5.Lcd.drawLine(x[pos1], y[pos1], x[pos2], y[pos2], BLACK);
            }
        }
        // calculate the box height based on z
        int h = 100 * accZ;
        // get the rotation
        float rotate = 1.2 * accX;
        // set box color
		int color = abs(h) < 3 && abs(rotate) < 0.05 ? GREEN : RED;
        // set box coordinates
        x[0] = 50;
        y[0] = 120;
        x[1] = 320 - x[0];
        y[1] = 120;
		x[2] = x[0] + abs(h);
        y[2] = 120 + h;
		x[3] = x[1] - abs(h);
        y[3] = 120 + h;
        // rotate the points depending on accX using the rotation matrix
        int xrel, yrel;
        for (int ix = 0; ix < 4; ++ix) {
            // put origin in middle
			xrel = x[ix] - 160;
			yrel = y[ix] - 120;
            // rotate
            xrel = xrel * cos(rotate) - yrel * sin(rotate);
            yrel = xrel * sin(rotate) + yrel * cos(rotate);
            // restore origin
            x[ix] = xrel + 160;
            y[ix] = yrel + 120;
        }
        // draw the box
        //m5.Lcd.fillTriangle(x[0], y[0], x[1], y[1], x[2], y[2], color);
        //m5.Lcd.fillTriangle(x[1], y[1], x[2], y[2], x[3], y[3], color);
        for (int ix = 0; ix < 4; ++ix) {
            int pos1 = ix < 2 ? 0 : 3;
            int pos2 = (ix % 2) + 1;
            m5.Lcd.drawLine(x[pos1], y[pos1], x[pos2], y[pos2], color);
        }
        // draw the reference circle
        M5.Lcd.drawCircle(160, 120, 80, BLUE);
        m5.Lcd.fillCircle(160, 120, 4, BLUE);
        M5.Lcd.drawLine(65, 120, 85, 120, CYAN);
        M5.Lcd.drawLine(235, 120, 255, 120, CYAN);
        delay(20);
    }
}

// handle the settings menu
void SettingsMenu()
{
    ezMenu settings("Settings");
    settings.txtSmall();
    settings.buttons("up # # Go # Back # down # ");
	settings.addItem("Image Settings", ImageSettings);
    settings.addItem("Repeat and Chain Settings", RepeatSettings);
    settings.addItem("LED Strip Settings", LEDStripSettings);
    settings.addItem("Light Bar", DisplayLedLightBar);
    settings.addItem("Level", LevelDisplay);
    settings.addItem("System Settings", ez.settings.menu);
    settings.addItem("Power Off", powerOff);
    settings.addItem("Reboot", reboot);
    settings.addItem("SysInfo", sysInfo);
    //settings.addItem("Exit | Go back to main menu");
    settings.run();
}

void reboot()
{
    if (ez.msgBox("Restart", "This will restart the system", "Cancel#OK#") == "OK") {
        ESP.restart();
    }
}

void powerOff()
{
    m5.powerOFF(); 
}

void sysInfo() {
    sysInfoPage1();
    while (true) {
        String btn = ez.buttons.poll();
        if (btn == "up") sysInfoPage1();
        if (btn == "down") sysInfoPage2();
        if (btn == "Exit") break;
    }
}

void sysInfoPage1() {
    const byte tab = 120;
    ez.screen.clear();
    ez.header.show("System Info  (1/2)");
    ez.buttons.show("#" + exit_button + "#down");
    ez.canvas.font(&FreeSans9pt7b);
    ez.canvas.lmargin(10);
    ez.canvas.println("");
    ez.canvas.print("CPU freq:");  ez.canvas.x(tab); ez.canvas.println(String(ESP.getCpuFreqMHz()) + " MHz");
    ez.canvas.print("CPU cores:");  ez.canvas.x(tab); ez.canvas.println("2");    //   :)
    ez.canvas.print("Chip rev.:");  ez.canvas.x(tab); ez.canvas.println(String(ESP.getChipRevision()));
    ez.canvas.print("Flash speed:");  ez.canvas.x(tab); ez.canvas.println(String(ESP.getFlashChipSpeed() / 1000000) + " MHz");
    ez.canvas.print("Flash size:");  ez.canvas.x(tab); ez.canvas.println(String(ESP.getFlashChipSize() / 1000000) + " MB");
    ez.canvas.print("ESP SDK:");  ez.canvas.x(tab); ez.canvas.println(String(ESP.getSdkVersion()));
    ez.canvas.print("M5ez:");  ez.canvas.x(tab); ez.canvas.println(String(ez.version()));
}

void sysInfoPage2() {
    const String SD_Type[5] = { "NONE", "MMC", "SD", "SDHC", "UNKNOWN" };
    const byte tab = 140;
    ez.screen.clear();
    ez.header.show("System Info  (2/2)");
    ez.buttons.show("up#" + exit_button + "#");
    ez.canvas.font(&FreeSans9pt7b);
    ez.canvas.lmargin(10);
    ez.canvas.println("");
    ez.canvas.print("Free RAM:");  ez.canvas.x(tab);  ez.canvas.println(String((long)ESP.getFreeHeap()) + " bytes");
    ez.canvas.print("Min. free seen:");  ez.canvas.x(tab); ez.canvas.println(String((long)esp_get_minimum_free_heap_size()) + " bytes");
    const int sd_type = SD.cardType();

    SPIFFS.begin();
    ez.canvas.print("SPIFFS size:"); ez.canvas.x(tab); ez.canvas.println(String((long)SPIFFS.totalBytes()) + " bytes");
    ez.canvas.print("SPIFFS used:"); ez.canvas.x(tab); ez.canvas.println(String((long)SPIFFS.usedBytes()) + " bytes");
    ez.canvas.print("SD type:"); ez.canvas.x(tab); ez.canvas.println(SD_Type[sd_type]);
    if (sd_type != 0) {
        ez.canvas.print("SD size:"); ez.canvas.x(tab); ez.canvas.println(String((long)(SD.cardSize() / 1000000)) + " MB");
        ez.canvas.print("SD used:"); ez.canvas.x(tab); ez.canvas.println(String((long)(SD.usedBytes() / 1000000)) + " MB");
    }
}

// read the files from the card or list the built-ins
// look for start.MIW, and process it, but don't add it to the list
bool GetFileNames(String dir, ezMenu* menu) {
	//if (nBootCount == 0)
	//	CurrentFileIndex = 0;
    uint8_t cardType = SD.cardType();
	bSdCardValid = true;
	if (cardType == CARD_NONE) {
		ez.msgBox("SD Card Error", "No SD card was found.", "OK", true, 0, TFT_RED);
		//Serial.println("No SD card attached");
		bSdCardValid = false;
		bShowBuiltInTests = true;
		return false;
	}
	String startfile;
	if (dir.length() > 1)
		dir = dir.substring(0, dir.length() - 1);
	File root = SD.open(dir);
	File file;
	String CurrentFilename = "";
	if (!root) {
		Serial.println("Failed to open directory: " + dir);
		//Serial.println("error: " + String(root.getError()));
		//SD.errorPrint("fail");
		return false;
	}
	if (!root.isDirectory()) {
		//Serial.println("Not a directory: " + dir);
		return false;
	}

	file = root.openNextFile();
	if (dir != "/") {
		// add an arrow to go back
		String sdir = currentFolder.substring(0, currentFolder.length() - 1);
		sdir = sdir.substring(0, sdir.lastIndexOf("/"));
		if (sdir.length() == 0)
			sdir = "/";
        menu->addItem(String(PREVIOUS_FOLDER_CHAR));
	}
	while (file) {
		CurrentFilename = file.name();
		// strip path
		CurrentFilename = CurrentFilename.substring(CurrentFilename.lastIndexOf('/') + 1);
		//Serial.println("name: " + CurrentFilename);
		if (CurrentFilename != "System Volume Information") {
			if (file.isDirectory()) {
                menu->addItem(String(NEXT_FOLDER_CHAR) + CurrentFilename);
			}
			else {
				String uppername = CurrentFilename;
				uppername.toUpperCase();
				if (uppername.endsWith(".BMP")) { //find files with our extension only
					//Serial.println("name: " + CurrentFilename);
                    menu->addItem(CurrentFilename);
				}
				else if (uppername == "START.MIW") {
					startfile = CurrentFilename;
				}
			}
		}
		file.close();
		file = root.openNextFile();
	}
	root.close();
	// see if we need to process the auto start file
	if (startfile.length())
		ProcessConfigFile(startfile);
	return true;
}

bool ProcessConfigFile(String filename)
{
    return false;
}

void DisplayLine(int line, String text, int indent, int16_t color)
{
    //if (bPauseDisplay)
    //    return;
    M5.Lcd.textdatum = TL_DATUM;
    int charHeight = M5.Lcd.fontHeight();
    int y = line * charHeight;
    M5.Lcd.fillRect(indent, y, M5.Lcd.width(), charHeight, TFT_BLACK);
    M5.Lcd.setTextColor(color);
    M5.Lcd.drawString(text, indent, y);
}

// some useful BMP constants
#define MYBMP_BF_TYPE           0x4D42	// "BM"
#define MYBMP_BI_RGB            0L
//#define MYBMP_BI_RLE8           1L
//#define MYBMP_BI_RLE4           2L
//#define MYBMP_BI_BITFIELDS      3L

//put the current file on the display
//Note that menu is not used, it is called with NULL sometimes
void ShowBmp()
{
    if (bShowBuiltInTests)
        return;
    String fn = currentFolder + currentFile;
    // make sure this is a bmp file, if not just quietly go away
    String tmp = fn.substring(fn.length() - 3);
    tmp.toLowerCase();
    if (tmp.compareTo("bmp")) {
        return;
    }
    uint16_t* scrBuf;
    scrBuf = (uint16_t*)calloc(320 * 144, sizeof(uint16_t));
    if (scrBuf == NULL) {
        ez.msgBox("Error", "Not enough memory");
        return;
    }
    bool bOldGamma = LedInfo.bGammaCorrection;
    LedInfo.bGammaCorrection = false;
    dataFile = SD.open(fn);
    // if the file is available send it to the LED's
    if (!dataFile.available()) {
        free(scrBuf);
		ez.msgBox("Error", "failed to open:\n" + currentFolder + currentFile);
        return;
    }
    M5.Lcd.fillScreen(TFT_BLACK);
    // clear the file cache buffer
    readByte(true);
    uint16_t bmpType = readInt();
    uint32_t bmpSize = readLong();
    uint16_t bmpReserved1 = readInt();
    uint16_t bmpReserved2 = readInt();
    uint32_t bmpOffBits = readLong();

    /* Check file header */
    if (bmpType != MYBMP_BF_TYPE) {
        free(scrBuf);
		ez.msgBox("Error", String("Invalid BMP:\n") + currentFolder + currentFile);
        return;
    }

    /* Read info header */
    uint32_t imgSize = readLong();
    uint32_t imgWidth = readLong();
    uint32_t imgHeight = readLong();
    uint16_t imgPlanes = readInt();
    uint16_t imgBitCount = readInt();
    uint32_t imgCompression = readLong();
    uint32_t imgSizeImage = readLong();
    uint32_t imgXPelsPerMeter = readLong();
    uint32_t imgYPelsPerMeter = readLong();
    uint32_t imgClrUsed = readLong();
    uint32_t imgClrImportant = readLong();

    /* Check info header */
    if (imgWidth <= 0 || imgHeight <= 0 || imgPlanes != 1 ||
        imgBitCount != 24 || imgCompression != MYBMP_BI_RGB || imgSizeImage == 0)
    {
        free(scrBuf);
		ez.msgBox("Error", String("Unsupported, must be 24bpp:\n") + currentFolder + currentFile);
        return;
    }

    int displayWidth = imgWidth;
    if (imgWidth > LedInfo.nPixelCount) {
        displayWidth = LedInfo.nPixelCount;           //only display the number of led's we have
    }

    /* compute the line length */
    uint32_t lineLength = imgWidth * 3;
    // fix for padding to 4 byte words
    if ((lineLength % 4) != 0)
        lineLength = (lineLength / 4 + 1) * 4;
    bool done = false;
    bool redraw = true;
    bool allowScroll = imgHeight > 320;
    // offset for showing the image
    int imgOffset = 0;
    int oldImgOffset;
    bool bShowingSize = false;
    // show some info
    float walk = (float)imgHeight / (float)imgWidth;
	DisplayLine(7, "Size: " + String(imgWidth) + " x " + String(imgHeight) + " Pixels");
    DisplayLine(8, "" + String(walk, 2) + " meters " + String(walk * 3.28084, 1) + " feet");
    // calculate display time
    float dspTime = ImgInfo.bFixedTime ? ImgInfo.nFixedImageTime : (imgHeight * ImgInfo.nColumnHoldTime / 1000.0 + imgHeight * .008);
    DisplayLine(9, "About " + String((int)round(dspTime)) + " Seconds");
    while (!done) {
        if (redraw) {
            // loop through the image, y is the image width, and x is the image height
            for (int y = imgOffset; y < (imgHeight > 320 ? 320 : imgHeight) + imgOffset; ++y) {
                int bufpos = 0;
                CRGB pixel;
                // get to start of pixel data for this column
                FileSeekBuf((uint32_t)bmpOffBits + (y * lineLength));
                for (int row = displayWidth - 1; row >= 0; --row) {
                    // this reads three bytes
                    pixel = getRGBwithGamma();
                    // add to the display memory
                    if (row >= 0 && row < 144) {
						uint16_t color = m5.Lcd.color565(pixel.r, pixel.g, pixel.b);
                        uint16_t sbcolor;
                        // the memory image colors are byte swapped
                        swab(&color, &sbcolor, 2);
                        scrBuf[(143 - row) * 320 + (y - imgOffset)] = sbcolor;
                    }
                }
            }
            oldImgOffset = imgOffset;
            // got it all, go show it
            m5.Lcd.pushRect(0, 0, 320, 144, scrBuf);
            redraw = false;
        }
        String btns;
        if (imgHeight > 320) {
			if (imgOffset == 0)
				btns = "# # Exit # # right # End";
			else if (imgOffset >= imgHeight - 320)
                btns = "left # Start # Exit # # #";
            else
				btns = "left # Start # Exit # # right # End";
        }
        else {
            btns = "Exit";
        }
        ez.buttons.show(btns);
        String str = ez.buttons.poll();
        if (str == "Exit") {
            done = true;
        }
        else if (str == "left") {
			if (allowScroll) {
				imgOffset -= 160;
				imgOffset = max(0, imgOffset);
			}
		}
        else if (str == "right") {
			if (allowScroll) {
				imgOffset += 160;
				imgOffset = min(imgHeight - 320, imgOffset);
			}
        }
        else if (str == "Start") {
            if (allowScroll) {
                imgOffset = 0;
            }
        }
        else if (str == "End") {
            if (allowScroll) {
                imgOffset = imgHeight - 320;
            }
        }
        if (oldImgOffset != imgOffset) {
            redraw = true;
        }
        delay(2);
    }
    // all done
    free(scrBuf);
    dataFile.close();
    readByte(true);
    LedInfo.bGammaCorrection = bOldGamma;
    M5.Lcd.fillScreen(TFT_BLACK);
}

String FormatInteger(int num, int decimals)
{
    String str;
    if (decimals) {
		str = String(num / (int)pow10(decimals)) + "." + String(num % (int)pow10(decimals));
    }
    else {
        str = String(num);
    }
    return str;
}

bool HandleMenuInteger(ezMenu* menu)
{
    int minVal = menu->getIntMinVal();
    int maxVal = menu->getIntMaxVal();
    int decimals = menu->getIntDecimals();
    int value = *menu->getIntValue();
    int inc = 1;
    int originalVal = value;
    ezProgressBar bl(menu->pickCaption(), "From " + FormatInteger(minVal, decimals) + " to " + FormatInteger(maxVal, decimals), "left # - # OK # Cancel # right # +");
    ez.canvas.font(&FreeSans12pt7b);
    int lastVal = value;
    int lastInc = inc;
    ez.canvas.x(140);
    ez.canvas.y(180);
    ez.canvas.print("value: " + FormatInteger(value, decimals) + "   ");
    while (true) {
        String b = ez.buttons.poll();
        if (b == "right")
            value += inc;
        else if (b == "left")
            value -= inc;
        else if (b == "+") {
            inc *= 10;
        }
        else if (b == "-") {
            inc /= 10;
        }
        else if (b == "OK") {
            break;
        }
        else if (b == "Cancel") {
            if (ez.msgBox("Restore original", "Cancel?", "Cancel # OK #") == "OK") {
                value = originalVal;
                break;
            }
            ez.buttons.show("left # - # OK # Cancel # right # +");
            ez.canvas.font(&FreeSans12pt7b);
        }
        inc = constrain(inc, 1, maxVal);
        value = constrain(value, minVal, maxVal);
        bl.value(((float)value / ((float)(maxVal - minVal) / 100.0)));
        if (lastInc != inc) {
            ez.canvas.x(0);
            ez.canvas.y(180);
            uint16_t oldcolor = ez.canvas.color();
            ez.canvas.color(TFT_CYAN);
            ez.canvas.print("+/- " + FormatInteger(inc, decimals) + "   ");
            ez.canvas.color(oldcolor);
            lastInc = inc;
        }
        if (lastVal != value) {
            ez.canvas.x(140);
            ez.canvas.y(180);
            ez.canvas.print("value: " + FormatInteger(value, decimals) + "   ");
            lastVal = value;
        }
    }
    String caption = menu->pickCaption();
    caption = caption.substring(0, caption.lastIndexOf('\t') + 1);
    menu->setCaption(menu->pickName(), caption + FormatInteger(value, decimals));
    // store the new value
    *menu->getIntValue() = value;
    return true;
}

// handle boolean toggles
bool ToggleBool(ezMenu* menu)
{
    *menu->getBoolValue() = !*menu->getBoolValue();
    String caption = menu->pickCaption();
    caption = caption.substring(0, caption.lastIndexOf('\t') + 1);
    menu->setCaption(menu->pickName(), caption + (*menu->getBoolValue() ? menu->getBoolTrue() : menu->getBoolFalse()));
    return true;
}

// Strip settings
void LEDStripSettings()
{
    ezMenu settings("LED Strip Settings");
    settings.txtSmall();
    settings.buttons("up # # Go # Back # down # ");
	settings.addItem("LED Brightness", &LedInfo.nLEDBrightness, 1, 255, 0, HandleMenuInteger);
	settings.addItem("Second Controller", &LedInfo.bSecondController, "On", "Off", ToggleBool);
	settings.addItem("Total Pixel Count", &LedInfo.nPixelCount, 1, 512, 0, HandleMenuInteger);
    settings.addItem("Maximum Current (mA)", &LedInfo.nStripMaxCurrent, 100, 20000, 0, HandleMenuInteger);
	settings.addItem("Gamma Correction", &LedInfo.bGammaCorrection, "On", "Off", ToggleBool);
    settings.addItem("Strip Wiring Mode", &LedInfo.stripsMode, 0, 2, 0, HandleMenuInteger);
    settings.addItem("White Balance Red", &LedInfo.whiteBalance.r, 0, 255, 0, HandleMenuInteger);
    settings.addItem("White Balance Green", &LedInfo.whiteBalance.g, 0, 255, 0, HandleMenuInteger);
    settings.addItem("White Balance Blue", &LedInfo.whiteBalance.b, 0, 255, 0, HandleMenuInteger);
    while (settings.runOnce()) {
	}
}

void RepeatSettings()
{
    ezMenu* pSettings;
    int16_t ix = 1;
	while (ix != 0) {
		pSettings = new ezMenu("Repeat & Chain Settings");
		pSettings->txtSmall();
		pSettings->buttons("up # # Go # Back # down # ");
		pSettings->addItem("Repeat Count", &ImgInfo.repeatCount, 1, 100, 0, HandleMenuInteger);
        if (ImgInfo.repeatCount > 1) {
            pSettings->addItem("Repeat Delay", &ImgInfo.repeatDelay, 0, 100, 1, HandleMenuInteger);
        }
		pSettings->addItem("Chain Files", &ImgInfo.bChainFiles, "Yes", "No", ToggleBool);
        if (ImgInfo.bChainFiles) {
            pSettings->addItem("Chain Repeats", &ImgInfo.nChainRepeats, 0, 100, 0, HandleMenuInteger);
            pSettings->addItem("Chain Delay", &ImgInfo.nChainDelay, 0, 100, 1, HandleMenuInteger);
            pSettings->addItem("Chain Wait for Key", &ImgInfo.bChainWaitKey, "Yes", "No", ToggleBool);
        }
		pSettings->setItem(ix);
		ix = pSettings->runOnce();
        delete pSettings;
	}
}

// Image settings
void ImageSettings()
{
    ezMenu* pSettings;
    int16_t ix = 1;
    while (ix != 0) {
        pSettings = new ezMenu("Image Settings");
        pSettings->txtSmall();
        pSettings->buttons("up # # Go # Back # down # ");
        pSettings->addItem("Fixed Column Time", &ImgInfo.bFixedTime, "Yes", "No", ToggleBool);
        if (ImgInfo.bFixedTime) {
            pSettings->addItem("Fixed Image Time (S)", &ImgInfo.nFixedImageTime, 0, 30, 0, HandleMenuInteger);
        }
        else {
            pSettings->addItem("Column Hold Time (mS)", &ImgInfo.nColumnHoldTime, 0, 100, 0, HandleMenuInteger);
        }
        pSettings->addItem("Fade In/Out Frames", &ImgInfo.nFadeInOutFrames, 0, 50, 0, HandleMenuInteger);
        pSettings->addItem("Start Delay", &ImgInfo.startDelay, 0, 1000, 1, HandleMenuInteger);
        pSettings->addItem("Walk Direction", &ImgInfo.bReverseImage, "Right to Left", "Left to Right", ToggleBool);
        pSettings->addItem("Upside Down", &ImgInfo.bUpsideDown, "Yes", "No", ToggleBool);
        pSettings->addItem("Double Pixels", &ImgInfo.bDoublePixels, "Yes", "No", ToggleBool);
        pSettings->addItem("Mirror Image", &ImgInfo.bMirrorPlayImage, "Yes", "No", ToggleBool);
        if (ImgInfo.bMirrorPlayImage) {
            pSettings->addItem("Mirror Delay", &ImgInfo.nMirrorDelay, 0, 100, 1, HandleMenuInteger);
        }
        pSettings->addItem("Divide Height by 2", &ImgInfo.bScaleHeight, "Yes", "No", ToggleBool);
        pSettings->addItem("Manual Frame Advance", &ImgInfo.bManualFrameAdvance, "Yes", "No", ToggleBool);
        if (ImgInfo.bManualFrameAdvance) {
            pSettings->addItem("Manual Frame Clicks", &ImgInfo.nFramePulseCount, 0, 10, 0, HandleMenuInteger);
        }
        pSettings->setItem(ix);
        ix = pSettings->runOnce();
        delete pSettings;
    }
}

uint32_t IRAM_ATTR readLong() {
    uint32_t retValue;
    byte incomingbyte;

    incomingbyte = readByte(false);
    retValue = (uint32_t)((byte)incomingbyte);

    incomingbyte = readByte(false);
    retValue += (uint32_t)((byte)incomingbyte) << 8;

    incomingbyte = readByte(false);
    retValue += (uint32_t)((byte)incomingbyte) << 16;

    incomingbyte = readByte(false);
    retValue += (uint32_t)((byte)incomingbyte) << 24;

    return retValue;
}

uint16_t IRAM_ATTR readInt() {
    byte incomingbyte;
    uint16_t retValue;

    incomingbyte = readByte(false);
    retValue += (uint16_t)((byte)incomingbyte);

    incomingbyte = readByte(false);
    retValue += (uint16_t)((byte)incomingbyte) << 8;

    return retValue;
}
byte filebuf[512];
int fileindex = 0;
int filebufsize = 0;
uint32_t filePosition = 0;

int IRAM_ATTR readByte(bool clear) {
    //int retbyte = -1;
    if (clear) {
        filebufsize = 0;
        fileindex = 0;
        return 0;
    }
    // TODO: this needs to align with 512 byte boundaries, maybe
    if (filebufsize == 0 || fileindex >= sizeof(filebuf)) {
        filePosition = dataFile.position();
        //// if not on 512 boundary yet, just return a byte
        //if ((filePosition % 512) && filebufsize == 0) {
        //    //Serial.println("not on 512");
        //    return dataFile.read();
        //}
        // read a block
//        Serial.println("block read");
        do {
            filebufsize = dataFile.read(filebuf, sizeof(filebuf));
        } while (filebufsize < 0);
        fileindex = 0;
    }
    return filebuf[fileindex++];
    //while (retbyte < 0) 
    //    retbyte = dataFile.read();
    //return retbyte;
}


// make sure we are the right place
void IRAM_ATTR FileSeekBuf(uint32_t place)
{
    if (place < filePosition || place >= filePosition + filebufsize) {
        // we need to read some more
        filebufsize = 0;
        dataFile.seek(place);
    }
}

// return true if current file is folder
bool IsFolder(int index)
{
    return pFileMenu->getItemName(index)[0] == NEXT_FOLDER_CHAR
        || pFileMenu->getItemName(index)[0] == PREVIOUS_FOLDER_CHAR;
}

// count the actual files, at a given starting point
int FileCountOnly(int start)
{
    int count = 0;
    // ignore folders, at the end
    for (int files = start; files < pFileMenu->getItemCount(); ++files) {
        if (!IsFolder(files))
            ++count;
    }
    return count;
}

// return the pixel
CRGB IRAM_ATTR getRGBwithGamma() {
    if (LedInfo.bGammaCorrection) {
        b = gammaB[readByte(false)];
        g = gammaG[readByte(false)];
        r = gammaR[readByte(false)];
    }
    else {
        b = readByte(false);
        g = readByte(false);
        r = readByte(false);
    }
    return CRGB(r, g, b);
}

void fixRGBwithGamma(byte* rp, byte* gp, byte* bp)
{
    if (LedInfo.bGammaCorrection) {
        *gp = gammaG[*gp];
        *bp = gammaB[*bp];
        *rp = gammaR[*rp];
    }
}

// reverse the strip index order for the lower strip, the upper strip is normal
// also check to make sure it isn't out of range
int AdjustStripIndex(int ix)
{
    switch (LedInfo.stripsMode) {
    case 0:	// bottom reversed, top normal, both wired in the middle
        if (ix < LedInfo.nPixelCount) {
            ix = (LedInfo.nPixelCount - 1 - ix);
        }
        break;
    case 1:	// bottom and top normal, chained, so nothing to do
        break;
    case 2:	// top reversed, bottom normal, no connection in the middle
        if (ix >= LedInfo.nPixelCount) {
            ix = (LedInfo.nPixelCount - 1 - ix);
        }
        break;
    }
    // make sure it isn't too big or too small
    ix = constrain(ix, 0, LedInfo.nPixelCount - 1);
    return ix;
}

// write a pixel to the correct location
// pixel doubling is handled here
// e.g. pixel 0 will be 0 and 1, 1 will be 2 and 3, etc
// if upside down n will be n and n-1, n-1 will be n-1 and n-2
// column = -1 to init fade in/out values
void IRAM_ATTR SetPixel(int ix, CRGB pixel, int column, int totalColumns)
{
    static int fadeStep;
    static int fadeColumns;
    static int lastColumn;
    static int maxColumn;
    static int fade;
    if (ImgInfo.nFadeInOutFrames) {
        // handle fading
        if (column == -1) {
            fadeColumns = min(totalColumns / 2, ImgInfo.nFadeInOutFrames);
            maxColumn = totalColumns;
            fadeStep = 255 / fadeColumns;
            //Serial.println("fadeStep: " + String(fadeStep) + " fadeColumns: " + String(fadeColumns) + " maxColumn: " + String(maxColumn));
            lastColumn = -1;
            fade = 255;
            return;
        }
        // when the column changes check if we are in the fade areas
        if (column != lastColumn) {
            int realColumn = ImgInfo.bReverseImage ? maxColumn - 1 - column : column;
            if (realColumn <= fadeColumns) {
                // calculate the fade amount
                fade = realColumn * fadeStep;
                fade = constrain(fade, 0, 255);
                // fading up
                //Serial.println("UP col: " + String(realColumn) + " fade: " + String(fade));
            }
            else if (realColumn >= maxColumn - 1 - fadeColumns) {
                // calculate the fade amount
                fade = (maxColumn - 1 - realColumn) * fadeStep;
                fade = constrain(fade, 0, 255);
                // fading down
                //Serial.println("DOWN col: " + String(realColumn) + " fade: " + String(fade));
            }
            else
                fade = 255;
            lastColumn = column;
        }
    }
    else {
        // no fade
        fade = 255;
    }
    int ix1, ix2;
    if (ImgInfo.bUpsideDown) {
        if (ImgInfo.bDoublePixels) {
            ix1 = AdjustStripIndex(LedInfo.nPixelCount - 1 - 2 * ix);
            ix2 = AdjustStripIndex(LedInfo.nPixelCount - 2 - 2 * ix);
        }
        else {
            ix1 = AdjustStripIndex(LedInfo.nPixelCount - 1 - ix);
        }
    }
    else {
        if (ImgInfo.bDoublePixels) {
            ix1 = AdjustStripIndex(2 * ix);
            ix2 = AdjustStripIndex(2 * ix + 1);
        }
        else {
            ix1 = AdjustStripIndex(ix);
        }
    }
    if (fade != 255) {
        pixel = pixel.nscale8_video(fade);
        //Serial.println("col: " + String(column) + " fade: " + String(fade));
    }
    leds[ix1] = pixel;
    if (ImgInfo.bDoublePixels)
        leds[ix2] = pixel;
}

// check for cancel button
bool CheckCancel()
{
    if (ez.buttons.poll() == "Cancel") {
        bCancelRun = true;
        return true;
    }
    return false;
}

void IRAM_ATTR ReadAndDisplayFile(bool doingFirstHalf) {
    static int totalSeconds;
    if (doingFirstHalf)
        totalSeconds = -1;

    // clear the file cache buffer
    readByte(true);
    uint16_t bmpType = readInt();
    uint32_t bmpSize = readLong();
    uint16_t bmpReserved1 = readInt();
    uint16_t bmpReserved2 = readInt();
    uint32_t bmpOffBits = readLong();
    //Serial.println("\nBMPtype: " + String(bmpType) + " offset: " + String(bmpOffBits));

    /* Check file header */
    if (bmpType != MYBMP_BF_TYPE) {
		ez.msgBox("Error", String("Invalid BMP: ") + currentFolder + currentFile);
        return;
    }

    /* Read info header */
    uint32_t imgSize = readLong();
    uint32_t imgWidth = readLong();
    uint32_t imgHeight = readLong();
    uint16_t imgPlanes = readInt();
    uint16_t imgBitCount = readInt();
    uint32_t imgCompression = readLong();
    uint32_t imgSizeImage = readLong();
    uint32_t imgXPelsPerMeter = readLong();
    uint32_t imgYPelsPerMeter = readLong();
    uint32_t imgClrUsed = readLong();
    uint32_t imgClrImportant = readLong();

    //Serial.println("imgSize: " + String(imgSize));
    //Serial.println("imgWidth: " + String(imgWidth));
    //Serial.println("imgHeight: " + String(imgHeight));
    //Serial.println("imgPlanes: " + String(imgPlanes));
    //Serial.println("imgBitCount: " + String(imgBitCount));
    //Serial.println("imgCompression: " + String(imgCompression));
    //Serial.println("imgSizeImage: " + String(imgSizeImage));
    /* Check info header */
    if (imgWidth <= 0 || imgHeight <= 0 || imgPlanes != 1 ||
        imgBitCount != 24 || imgCompression != MYBMP_BI_RGB || imgSizeImage == 0)
    {
        ez.msgBox("Error", String("Must be 24bpp: ") + currentFolder + currentFile);
        return;
    }

    int displayWidth = imgWidth;
    if (imgWidth > LedInfo.nPixelCount) {
        displayWidth = LedInfo.nPixelCount;           //only display the number of led's we have
    }

    /* compute the line length */
    uint32_t lineLength = imgWidth * 3;
    // fix for padding to 4 byte words
    if ((lineLength % 4) != 0)
        lineLength = (lineLength / 4 + 1) * 4;

    // Note:  
    // The x,r,b,g sequence below might need to be changed if your strip is displaying
    // incorrect colors.  Some strips use an x,r,b,g sequence and some use x,r,g,b
    // Change the order if needed to make the colors correct.
    // init the fade settings in SetPixel
    SetPixel(0, TFT_BLACK, -1, (int)imgHeight);
    long secondsLeft = 0, lastSeconds = 0;
    char num[50];
    int percent;
    unsigned minLoopTime = 0; // the minimum time it takes to process a line
    bool bLoopTimed = false;
	ez.buttons.show(ImgInfo.bManualFrameAdvance ? "-Column # # # Cancel # +Column #" : "# # # Cancel # #");
    // also remember that height and width are effectively swapped since we rotated the BMP image CCW for ease of reading and displaying here
    for (int y = ImgInfo.bReverseImage ? imgHeight - 1 : 0; ImgInfo.bReverseImage ? y >= 0 : y < imgHeight; ImgInfo.bReverseImage ? --y : ++y) {
        // approximate time left
        if (ImgInfo.bReverseImage)
            secondsLeft = ((long)y * (ImgInfo.nColumnHoldTime + minLoopTime) / 1000L) + 1;
        else
            secondsLeft = ((long)(imgHeight - y) * (ImgInfo.nColumnHoldTime + minLoopTime) / 1000L) + 1;
        // mark the time for timing the loop
        if (!bLoopTimed) {
            minLoopTime = millis();
        }
        if (ImgInfo.bMirrorPlayImage) {
            if (totalSeconds == -1)
                totalSeconds = secondsLeft;
            if (doingFirstHalf) {
                secondsLeft += totalSeconds;
            }
        }
        if (secondsLeft != lastSeconds) {
            lastSeconds = secondsLeft;
            sprintf(num, "File Seconds: %d", secondsLeft);
            DisplayLine(2, num);
        }
        percent = map(ImgInfo.bReverseImage ? imgHeight - y : y, 0, imgHeight, 0, 100);
        if (ImgInfo.bMirrorPlayImage) {
            percent /= 2;
            if (!doingFirstHalf) {
                percent += 50;
            }
        }
        if (((percent % 5) == 0) || percent > 90) {
            ShowProgressBar(percent);
        }
        int bufpos = 0;
        CRGB pixel;
        FileSeekBuf((uint32_t)bmpOffBits + (y * lineLength));
        //uint32_t offset = (bmpOffBits + (y * lineLength));
        //dataFile.seekSet(offset);
        for (int x = displayWidth - 1; x >= 0; --x) {
            // this reads three bytes
            pixel = getRGBwithGamma();
            // see if we want this one
            if (ImgInfo.bScaleHeight && (x * displayWidth) % imgWidth) {
                continue;
            }
            SetPixel(x, pixel, y);
        }
        // see how long it took to get here
        if (!bLoopTimed) {
            minLoopTime = millis() - minLoopTime;
            bLoopTimed = true;
            // if fixed time then we need to calculate the framehold value
            if (ImgInfo.bFixedTime) {
                // divide the time by the number of frames
                ImgInfo.nColumnHoldTime = 1000 * ImgInfo.nFixedImageTime / imgHeight;
                ImgInfo.nColumnHoldTime -= minLoopTime;
                ImgInfo.nColumnHoldTime = max(ImgInfo.nColumnHoldTime, 0);
            }
        }
        // wait for timer to expire before we show the next frame
        while (bStripWaiting) {
            //delayMicroseconds(100);
            //yield();
            // we should maybe check the cancel key here to handle slow frame rates?
            if (CheckCancel())
                break;
        }
        // now show the lights
        FastLED.show();
        // set a timer while we go ahead and load the next frame
        bStripWaiting = true;
        esp_timer_start_once(oneshot_LED_timer, ImgInfo.nColumnHoldTime * 1000);
        // check keys
        if (CheckCancel())
            break;
        if (ImgInfo.bManualFrameAdvance) {
            // check if frame advance button requested
            if (ImgInfo.nFramePulseCount) {
                for (int ix = ImgInfo.nFramePulseCount; ix; --ix) {
                    // wait for signal
                    while (digitalRead(FRAMEBUTTON)) {
                        if (CheckCancel())
                            break;
                        delay(10);
                    }
                    // wait for release
                    while (!digitalRead(FRAMEBUTTON)) {
                        if (CheckCancel())
                            break;
                        delay(10);
                    }
                }
            }
            else {
                // by button click or rotate
                for (;;) {
                    String str = ez.buttons.poll();
					if (str == "-Column") {
                        // backup a line, use 2 because the for loop does one when we're done here
                        if (ImgInfo.bReverseImage) {
                            y += 2;
                            if (y > imgHeight)
                                y = imgHeight;
                        }
                        else {
                            y -= 2;
                            if (y < -1)
                                y = -1;
                        }
                        break;
                    }
					else if (str == "+Column")
                        break;
                    if (CheckCancel())
                        break;
                    delay(10);
                }
            }
        }
        if (bCancelRun)
            break;
    }
    // all done
    readByte(true);
}

// run file or built-in
void ProcessFileOrTest()
{
    int nRepeatsLeft;                         // countdown while repeating
    String line;
    //if (bRecordingMacro) {
    //    strcpy(FileToShow, FileNames[CurrentFileIndex].c_str());
    //    WriteOrDeleteConfigFile(String(nCurrentMacro), false, false);
    //}
    bIsRunning = true;
    if (ImgInfo.startDelay) {
        // set a timer
        nTimerSeconds = ImgInfo.startDelay;
        while (nTimerSeconds && !CheckCancel()) {
            line = "Start Delay: " + String(nTimerSeconds / 10) + "." + String(nTimerSeconds % 10);
            DisplayLine(3, line);
            delay(100);
            --nTimerSeconds;
        }
        DisplayLine(3, "");
    }
    int currentFileIndex = activeMenu->pick();
    int chainCount = ImgInfo.bChainFiles ? FileCountOnly(currentFileIndex) : 1;
    int chainRepeatCount = ImgInfo.bChainFiles ? ImgInfo.nChainRepeats : 1;
    // don't allow chaining for built-ins, although maybe we should
    if (bShowBuiltInTests) {
        chainCount = 1;
        chainRepeatCount = 1;
    }
    // set the basic LED info
    FastLED.setTemperature(CRGB(LedInfo.whiteBalance.r, LedInfo.whiteBalance.g, LedInfo.whiteBalance.b));
    FastLED.setBrightness(LedInfo.nLEDBrightness);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, LedInfo.nStripMaxCurrent);
    line = "";
    while (chainRepeatCount-- > 0) {
        while (chainCount-- > 0) {
            DisplayLine(1, activeMenu->getItemName(currentFileIndex));
            if (ImgInfo.bChainFiles && !bShowBuiltInTests) {
                line = "Files: " + String(chainCount + 1);
                DisplayLine(5, line);
                if (chainCount) {
					DisplayLine(6, "Next: " + activeMenu->getItemName(currentFileIndex + 1));
                }
                else {
                    DisplayLine(6, "");
                }
                line = "";
            }
            // process the repeats and waits for each file in the list
            for (nRepeatsLeft = ImgInfo.repeatCount; nRepeatsLeft > 0; nRepeatsLeft--) {
                // fill the progress bar
                if (!bShowBuiltInTests)
                    ShowProgressBar(0);
                if (ImgInfo.repeatCount > 1) {
                    line = "Repeats: " + String(nRepeatsLeft) + " ";
                }
                if (!bShowBuiltInTests && ImgInfo.nChainRepeats > 1) {
                    line += "Chains: " + String(chainRepeatCount + 1);
                }
                DisplayLine(4, line);
                if (bShowBuiltInTests) {
                    ez.buttons.show("# # # Cancel # #");
                    // run the test
					if (BuiltInFiles[currentFileIndex - 1].function)
						(*BuiltInFiles[currentFileIndex - 1].function)();
					else
						ez.msgBox("Built-In Files", "Not Available", "OK", true, 0, TFT_WHITE);
                }
                else {
                    if (nRepeatCountMacro > 1 && bRunningMacro) {
                        DisplayLine(5, String("Macro Repeats: ") + String(nMacroRepeatsLeft));
                    }
                    // output the file
					SendFile(activeMenu->getItemName(currentFileIndex));
                }
                if (bCancelRun) {
                    break;
                }
                if (!bShowBuiltInTests)
                    ShowProgressBar(0);
                if (nRepeatsLeft > 1) {
                    if (ImgInfo.repeatDelay) {
                        FastLED.clear(true);
                        // start timer
                        nTimerSeconds = ImgInfo.repeatDelay;
                        while (nTimerSeconds > 0 && !CheckCancel()) {
                            line = "Repeat Delay: " + String(nTimerSeconds / 10) + "." + String(nTimerSeconds % 10);
                            DisplayLine(3, line);
                            line = "";
                            delay(100);
                            --nTimerSeconds;
                        }
                        DisplayLine(3, "");
                    }
                }
            }
            if (bCancelRun) {
                chainCount = 0;
                break;
            }
            if (bShowBuiltInTests)
                break;
            // see if we are chaining, if so, get the next file, if a folder we're done
            if (ImgInfo.bChainFiles) {
                // grab the next file
                if (currentFileIndex < activeMenu->getItemCount() - 1)
                    ++currentFileIndex;
                if (IsFolder(currentFileIndex))
                    break;
                // handle any chain delay
                for (int dly = ImgInfo.nChainDelay; dly > 0 && !CheckCancel(); --dly) {
                    line = "Chain Delay: " + String(dly / 10) + "." + String(dly % 10);
                    DisplayLine(3, line);
                    delay(100);
                }
                // check for chain wait for keypress
                if (chainCount && ImgInfo.bChainWaitKey) {
                    ez.msgBox("Chain", "Waiting for OK");
                    ez.buttons.show("Next");
                    //DisplayLine(2, "Click: " + FileNames[CurrentFileIndex]);
                    bool waitNext = true;
                    while (waitNext) {
                        String str;
                        delay(10);
                        str = ez.buttons.poll();
						if (str == "Next" || CheckCancel()) {
                            waitNext = false;
                        }
                    }
                }
            }
            line = "";
            // clear
            FastLED.clear(true);
        }
        if (bCancelRun) {
            chainCount = 0;
            chainRepeatCount = 0;
            bCancelRun = false;
            break;
        }
        // start again
        currentFileIndex = activeMenu->pick();
        chainCount = ImgInfo.bChainFiles ? FileCountOnly(currentFileIndex) : 1;
        if (ImgInfo.repeatDelay && (nRepeatsLeft > 1) || chainRepeatCount >= 1) {
            FastLED.clear(true);
            // start timer
            nTimerSeconds = ImgInfo.repeatDelay;
            while (nTimerSeconds > 0 && !CheckCancel()) {
                line = "Repeat Delay: " + String(nTimerSeconds / 10) + "." + String(nTimerSeconds % 10);
                DisplayLine(3, line);
                line = "";
                delay(100);
                --nTimerSeconds;
            }
        }
    }
    FastLED.clear(true);
    m5.Lcd.fillScreen(TFT_BLACK);
    bIsRunning = false;
}

void SendFile(String Filename) {
    // see if there is an associated config file
    //String cfFile = MakeMIWFilename(Filename, true);
    //SettingsSaveRestore(true, 0);
    IMG_INFO savedImgInfo = ImgInfo;
    //ProcessConfigFile(cfFile);
    String fn = currentFolder + Filename;
    dataFile = SD.open(fn);
    // if the file is available send it to the LED's
    if (dataFile.available()) {
        for (int cnt = 0; cnt < (ImgInfo.bMirrorPlayImage ? 2 : 1); ++cnt) {
            ReadAndDisplayFile(cnt == 0);
            ImgInfo.bReverseImage = !ImgInfo.bReverseImage; // note this will be restored by SettingsSaveRestore
            dataFile.seek(0);
            FastLED.clear(true);
            int wait = ImgInfo.nMirrorDelay;
            while (wait-- > 0) {
                delay(100);
            }
            if (CheckCancel())
                break;
        }
        dataFile.close();
    }
    else {
        ez.msgBox("Error","open fail: " + fn);
        return;
    }
    ShowProgressBar(100);
    ImgInfo = savedImgInfo;
    //SettingsSaveRestore(false, 0);
}

void ShowProgressBar(int percent)
{
    //if (!bShowProgress || bPauseDisplay)
    //    return;
    static int lastpercent;
    if (lastpercent && (lastpercent == percent))
        return;
    if (percent == 0) {
        m5.Lcd.fillRect(0, 0, m5.Lcd.width() - 1, 8, TFT_BLACK);
    }
    DrawProgressBar(0, 0, m5.Lcd.width() - 1, 8, percent);
    lastpercent = percent;
}

// draw a progress bar
void DrawProgressBar(int x, int y, int dx, int dy, int percent)
{
    m5.Lcd.drawRoundRect(x, y, dx, dy, 2, TFT_BLUE);
    int fill = (dx - 2) * percent / 100;
    // fill the filled part
    m5.Lcd.fillRect(x + 1, y + 1, fill, dy - 2, TFT_GREEN);
    // blank the empty part
    m5.Lcd.fillRect(x + 1 + fill, y + 1, dx - 2 - fill, dy - 2, TFT_BLACK);
}
