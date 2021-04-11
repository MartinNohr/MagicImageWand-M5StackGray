/*
 Name:		MagicImageWant_M5CoreGray.ino
 Created:	3/29/2021 4:08:35 PM
 Author:	Martin
*/
// define must ahead #include <M5Stack.h>
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
#include "MagicImageWand-M5StackGray.h"
//#include <SPIFFS.h>
#include <SD.h>

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

void setup() {
#include <themes/default.h>
#include <themes/dark.h>
    ezt::setDebug(INFO);
    ez.begin();
    Wire.begin();
    builtinMenu.txtSmall();
    builtinMenu.setSortFunction(CompareNames);
    for (BuiltInItem bi : BuiltInFiles) {
        builtinMenu.addItem(bi.text);
    }
    builtinMenu.buttons("up # View # Go # Menu # down # SD");
    M5.IMU.Init();
}

ezMenu* pFileMenu = NULL;

ezMenu* activeMenu;

void loop() {
    static std::stack<int> selectionStack;
    static bool bReloadSD = true;
    bool bRetry = false;
    if (bShowBuiltInTests) {
        activeMenu = &builtinMenu;
    }
    else {
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
            else
                currentFile = tmp;
			ez.msgBox("run", (bShowBuiltInTests ? "" : String(currentFolder)) + currentFile);
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
			ez.msgBox("preview: ", String(activeMenu->pickName()));
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

void LevelDisplay()
{
    ez.screen.clear();
    ez.canvas.font(&FreeSans12pt7b);
    ez.buttons.show("#" + exit_button + "#");
    int lastX = 0;
    int lastZ = 0;
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
        accX = GetAverage(XIx, SAMPLES, accX, XArray, XSum);
        accY = GetAverage(YIx, SAMPLES, accY, YArray, YSum);
        accZ = GetAverage(ZIx, SAMPLES, accZ, ZArray, ZSum);
        //ez.canvas.printf(" %5.2f   %5.2f   %5.2f   ", accX, accY, accZ);
        // draw the horizon line
		if (lastZ) {
            // erase previous line
            M5.Lcd.drawLine(40, lastZ + lastX / 2, 280, lastZ - lastX / 2, BLACK);
        }
        // this is the reference horizon
        M5.Lcd.drawLine(20, 120, 300, 120, GREEN);
        // draw the new line
        lastZ = 120 - 100 * accZ;
        lastX = -200 * accX;
		M5.Lcd.drawLine(40, lastZ + lastX / 2, 280, lastZ - lastX / 2, ((abs(lastZ - 120) < 2) && (abs(lastX) < 2)) ? GREEN : RED);
        M5.Lcd.drawCircle(160, 120, 80, BLUE);
        delay(100);
    }
}

// handle the settings menu
void SettingsMenu()
{
    ezMenu settings("Settings");
    settings.txtSmall();
    settings.buttons("up # back # Go # # down # ");
    settings.addItem("Image Settings");
    settings.addItem("LED Strip Settings");
    settings.addItem("wifi & other settings", ez.settings.menu);
    settings.addItem("Level", LevelDisplay);
    settings.addItem("SysInfo", sysInfo);
    settings.addItem("Power Off", powerOff);
    settings.addItem("Reboot", reboot);
    //settings.addItem("Exit | Go back to main menu");
    settings.run();
}

void reboot()
{
    if (ez.msgBox("Restart", "This will restart the system", "Cancel#OK#") == "OK") {
        ESP.restart();
    }
}

//void mainmenu_menus() {
//    ezMenu submenu("This is a sub menu");
//    submenu.txtSmall();
//    submenu.buttons("up#Back#select##down#");
//    submenu.addItem("You can make small menus");
//    submenu.addItem("Or big ones");
//    submenu.addItem("(Like the Main menu)");
//    submenu.addItem("In this menu most options");
//    submenu.addItem("Do absolutely nothing");
//    submenu.addItem("They are only here");
//    submenu.addItem("To demonstrate that menus");
//    submenu.addItem("Can run off the screen");
//    submenu.addItem("And will just scroll");
//    submenu.addItem("And scroll");
//    submenu.addItem("And Scroll");
//    submenu.addItem("And Scroll even more");
//    submenu.addItem("more | Learn more about menus", submenu_more);
//    submenu.addItem("Exit | Go back to main menu");
//    submenu.run();
//}

//void submenu_more() {
//    ez.header.show("A simple menu in code...");
//    ez.canvas.lmargin(10);
//    ez.canvas.println("");
//    ez.canvas.println("ezMenu menu(\"Main menu\");");
//    ez.canvas.println("menu.addItem(\"Option 1\");");
//    ez.canvas.println("menu.addItem(\"Option 2\");");
//    ez.canvas.println("menu.addItem(\"Option 3\");");
//    ez.canvas.println("while ( menu.run() ) {");
//    ez.canvas.println("  if (menu.pick == 1) {");
//    ez.canvas.println("    ez.msgBox (\"One!\");");
//    ez.canvas.println("  }");
//    ez.canvas.println("}");
//    ez.buttons.wait("OK");
//
//    ezMenu fontmenu("Menus can change looks");
//    fontmenu.txtFont(&Satisfy_24);
//    fontmenu.addItem("Menus can use");
//    fontmenu.addItem("Various Fonts");
//    fontmenu.runOnce();
//
//    ezMenu delmenu("Menus are dynamic");
//    delmenu.txtSmall();
//    delmenu.addItem("You can delete items");
//    delmenu.addItem("While the menu runs");
//    delmenu.addItem("Delete me!");
//    delmenu.addItem("Delete me!");
//    delmenu.addItem("Delete me!");
//    delmenu.addItem("Exit | Go back");
//    while (delmenu.runOnce()) {
//        if (delmenu.pickName() == "Delete me!") {
//            delmenu.deleteItem(delmenu.pick());
//        }
//    }
//}

//void mainmenu_image() {
//    ezMenu images;
//    images.imgBackground(TFT_BLACK);
//    images.imgFromTop(40);
//    images.imgCaptionColor(TFT_WHITE);
//    images.addItem(sysinfo_jpg, "System Information", sysInfo);
//    images.addItem(wifi_jpg, "WiFi Settings", ez.wifi.menu);
//    images.addItem(about_jpg, "About M5ez", aboutM5ez);
//    images.addItem(sleep_jpg, "Power Off", powerOff);
//    images.addItem(return_jpg, "Back");
//    images.run();
//}

//void mainmenu_msgs() {
//    String cr = (String)char(13);
//    ez.msgBox("You can show messages", "ez.msgBox shows text");
//    ez.msgBox("Looking the way you want", "In any font !", "OK", true, &FreeSerifBold24pt7b, TFT_RED);
//    ez.msgBox("More ez.msgBox", "Even multi-line messages where everything lines up and is kept in the middle of the screen");
//    ez.msgBox("Questions, questions...", "But can it also show any buttons you want?", "No # # Yes");
//    ez.textBox("And there's ez.textBox", "To present or compose longer word-wrapped texts, you can use the ez.textBox function." + cr + cr + "M5ez (pronounced \"M5 easy\") is a complete interface builder library for the M5Stack ESP32 system. It allows even novice programmers to create good looking interfaces. It comes with menus as text or as images, message boxes, very flexible button setup (including different length presses and multi-button functions), 3-button text input (you have to see it to believe it) and built-in Wifi support. Now you can concentrate on what your program does, and let M5ez worry about everything else.", true);
//}

//void mainmenu_buttons() {
//    ez.header.show("Simple buttons...");
//    ez.canvas.font(&FreeSans12pt7b);
//    ez.canvas.lmargin(20);
//    ez.canvas.println("");
//    ez.canvas.println("You can have three buttons");
//    ez.canvas.println("with defined funtions.");
//    ez.buttons.show("One # Two # Done");
//    printButton();
//    ez.canvas.clear();
//    ez.header.show("More functions...");
//    ez.canvas.println("");
//    ez.canvas.println("But why stop there?");
//    ez.canvas.println("If you press a little longer");
//    ez.canvas.println("You access the functions");
//    ez.canvas.println("printed in cyan.");
//    ez.buttons.show("One # Two # Three # Four # Done #");
//    printButton();
//    ez.canvas.clear();
//    ez.header.show("Two keys ...");
//    ez.canvas.y(ez.canvas.top() + 10);
//    ez.canvas.println("It gets even better...");
//    ez.canvas.println("The purple bar shows the");
//    ez.canvas.println("functions for key combis.");
//    ez.canvas.println("See if you can work it out...");
//    ez.buttons.show("One # Two # Three # Four # Five # Six # Seven # Eight # Done");
//    printButton();
//}
//
//void printButton() {
//    while (true) {
//        String btnpressed = ez.buttons.poll();
//        if (btnpressed == "Done")
//            break;
//        if (btnpressed != "") {
//            m5.lcd.fillRect(0, ez.canvas.bottom() - 45, TFT_W, 40, ez.theme->background);
//            ez.canvas.pos(20, ez.canvas.bottom() - 45);
//            ez.canvas.color(TFT_RED);
//            ez.canvas.font(&FreeSansBold18pt7b);
//            ez.canvas.print(btnpressed);
//            ez.canvas.font(&FreeSans12pt7b);
//            ez.canvas.color(TFT_WHITE);
//        }
//    }
//}

//void mainmenu_entry() {
//    if (ez.msgBox("We're gonna enter text ... !", "Have you learned to use the buttons? Go there first if you haven't been there. Or hit 'Go' to see if you can enter your name.", "Back # # Go") == "Go") {
//        String your_name = ez.textInput();
//        ez.msgBox("Pfew...", "Hi " + your_name + "! | | Now that was a pain! But it is good enough for entering, say, a WPA key, or don't you think?");
//        ez.msgBox("Don't worry", "(You do get better with practice...)");
//    }
//}

//void mainmenu_ota() {
//    if (ez.msgBox("Get OTA_https demo", "This will replace the demo with a program that can then load the demo program again.", "Cancel#OK#") == "OK") {
//        ezProgressBar progress_bar("OTA update in progress", "Downloading ...", "Abort");
//#include "raw_githubusercontent_com.h" // the root certificate is now in const char * root_cert
//        if (ez.wifi.update("https://raw.githubusercontent.com/M5ez/M5ez/master/compiled_binaries/OTA_https.bin", root_cert, &progress_bar)) {
//            ez.msgBox("Over The Air updater", "OTA download successful. Reboot to new firmware", "Reboot");
//            ESP.restart();
//        }
//        else {
//            ez.msgBox("OTA error", ez.wifi.updateError(), "OK");
//        }
//    }
//}

void powerOff()
{
    m5.powerOFF(); 
}

//void aboutM5ez() {
//    ez.msgBox("About M5ez", "M5ez was written by | Rop Gonggrijp | | https://github.com/M5ez/M5ez");
//}

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
        ez.canvas.print("SD size:"); ez.canvas.x(tab); ez.canvas.println(String((long)SD.cardSize() / 1000000) + " MB");
        ez.canvas.print("SD used:"); ez.canvas.x(tab); ez.canvas.println(String((long)SD.usedBytes() / 1000000) + " MB");
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

// put the current file on the display
// Note that menu is not used, it is called with NULL sometimes
//void ShowBmp()
//{
//    if (bShowBuiltInTests)
//        return;
//    String fn = currentFolder + FileNames[CurrentFileIndex];
//    // make sure this is a bmp file, if not just quietly go away
//    String tmp = fn.substring(fn.length() - 3);
//    tmp.toLowerCase();
//    if (tmp.compareTo("bmp")) {
//        return;
//    }
//    bool bSawButton0 = !digitalRead(0);
//    uint16_t* scrBuf;
//    scrBuf = (uint16_t*)calloc(320 * 144, sizeof(uint16_t));
//    if (scrBuf == NULL) {
//        //WriteMessage("Not enough memory", true, 5000);
//        return;
//    }
//    bool bOldGamma = bGammaCorrection;
//    bGammaCorrection = false;
//    dataFile = SD.open(fn);
//    // if the file is available send it to the LED's
//    if (!dataFile.available()) {
//        free(scrBuf);
//        WriteMessage("failed to open: " + currentFolder + FileNames[CurrentFileIndex], true);
//        return;
//    }
//    tft.fillScreen(TFT_BLACK);
//    // clear the file cache buffer
//    readByte(true);
//    uint16_t bmpType = readInt();
//    uint32_t bmpSize = readLong();
//    uint16_t bmpReserved1 = readInt();
//    uint16_t bmpReserved2 = readInt();
//    uint32_t bmpOffBits = readLong();
//
//    /* Check file header */
//    if (bmpType != MYBMP_BF_TYPE) {
//        free(scrBuf);
//        WriteMessage(String("Invalid BMP:\n") + currentFolder + FileNames[CurrentFileIndex], true);
//        return;
//    }
//
//    /* Read info header */
//    uint32_t imgSize = readLong();
//    uint32_t imgWidth = readLong();
//    uint32_t imgHeight = readLong();
//    uint16_t imgPlanes = readInt();
//    uint16_t imgBitCount = readInt();
//    uint32_t imgCompression = readLong();
//    uint32_t imgSizeImage = readLong();
//    uint32_t imgXPelsPerMeter = readLong();
//    uint32_t imgYPelsPerMeter = readLong();
//    uint32_t imgClrUsed = readLong();
//    uint32_t imgClrImportant = readLong();
//
//    /* Check info header */
//    if (imgWidth <= 0 || imgHeight <= 0 || imgPlanes != 1 ||
//        imgBitCount != 24 || imgCompression != MYBMP_BI_RGB || imgSizeImage == 0)
//    {
//        free(scrBuf);
//        WriteMessage(String("Unsupported, must be 24bpp:\n") + currentFolder + FileNames[CurrentFileIndex], true);
//        return;
//    }
//
//    int displayWidth = imgWidth;
//    if (imgWidth > STRIPLENGTH) {
//        displayWidth = STRIPLENGTH;           //only display the number of led's we have
//    }
//
//    /* compute the line length */
//    uint32_t lineLength = imgWidth * 3;
//    // fix for padding to 4 byte words
//    if ((lineLength % 4) != 0)
//        lineLength = (lineLength / 4 + 1) * 4;
//    bool done = false;
//    bool redraw = true;
//    bool allowScroll = imgHeight > 320;
//    // offset for showing the image
//    int imgOffset = 0;
//    int oldImgOffset;
//    bool bShowingSize = false;
//    // show some info
//    float walk = (float)imgHeight / (float)imgWidth;
//    DisplayLine(5, "" + String(walk, 2) + " meters " + String(walk * 3.28084, 1) + " feet");
//    DisplayLine(6, "Size: " + String(imgWidth) + " x " + String(imgHeight));
//    // calculate display time
//    float dspTime = bFixedTime ? nFixedImageTime : (imgHeight * nFrameHold / 1000.0 + imgHeight * .008);
//    DisplayLine(7, "About " + String((int)round(dspTime)) + " Seconds");
//    while (!done) {
//        if (redraw) {
//            // loop through the image, y is the image width, and x is the image height
//            for (int y = imgOffset; y < (imgHeight > 320 ? 320 : imgHeight) + imgOffset; ++y) {
//                int bufpos = 0;
//                CRGB pixel;
//                // get to start of pixel data for this column
//                FileSeekBuf((uint32_t)bmpOffBits + (y * lineLength));
//                for (int x = displayWidth - 1; x >= 0; --x) {
//                    // this reads three bytes
//                    pixel = getRGBwithGamma();
//                    // add to the display memory
//                    int row = x - 5;
//                    int col = y - imgOffset;
//                    if (row >= 0 && row < 144) {
//                        uint16_t color = tft.color565(pixel.r, pixel.g, pixel.b);
//                        uint16_t sbcolor;
//                        // the memory image colors are byte swapped
//                        swab(&color, &sbcolor, 2);
//                        scrBuf[(143 - row) * 320 + col] = sbcolor;
//                    }
//                }
//            }
//            oldImgOffset = imgOffset;
//            // got it all, go show it
//            m5.Lcd.pushRect(0, 0, 320, 144, scrBuf);
//        }
//        if (bSawButton0) {
//            while (digitalRead(0) == 0)
//                ;
//            bSawButton0 = false;
//            delay(30);
//        }
//        switch (ReadButton()) {
//        case CRotaryDialButton::BTN_LEFT:
//            if (allowScroll) {
//                imgOffset -= 320;
//                imgOffset = max(0, imgOffset);
//            }
//            break;
//        case CRotaryDialButton::BTN_RIGHT:
//            if (allowScroll) {
//                imgOffset += 320;
//                imgOffset = min((int32_t)imgHeight - 320, imgOffset);
//            }
//            break;
//        case CRotaryDialButton::BTN_LONGPRESS:
//            done = true;
//            break;
//            //case CRotaryDialButton::BTN_CLICK:
//            //	if (bShowingSize) {
//            //		bShowingSize = false;
//            //		redraw = true;
//            //	}
//            //	else {
//            //		tft.fillScreen(TFT_BLACK);
//            //		//DisplayLine(0, currentFolder);
//            //		//DisplayLine(4, FileNames[CurrentFileIndex]);
//            //		float walk = (float)imgHeight / (float)imgWidth;
//            //		DisplayLine(5, "" + String(walk, 2) + " meters " + String(walk * 3.28084, 1) + " feet");
//            //		DisplayLine(6, "Size: " + String(imgWidth) + " x " + String(imgHeight));
//            //		// calculate display time
//            //		float dspTime = bFixedTime ? nFixedImageTime : (imgHeight * nFrameHold / 1000.0 + imgHeight * .008);
//            //		DisplayLine(7, "About " + String((int)round(dspTime)) + " Seconds");
//            //		bShowingSize = true;
//            //		redraw = false;
//            //	}
//            //	break;
//        }
//        if (oldImgOffset != imgOffset) {
//            redraw = true;
//        }
//        // check the 0 button
//        if (digitalRead(0) == 0) {
//            // debounce, don't want this seen again in the main loop
//            delay(30);
//            done = true;
//        }
//        delay(2);
//    }
//    // all done
//    free(scrBuf);
//    dataFile.close();
//    readByte(true);
//    bGammaCorrection = bOldGamma;
//    tft.fillScreen(TFT_BLACK);
//}
