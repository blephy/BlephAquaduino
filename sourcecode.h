
// Please download all the library below and add it to your Arduino project.
// I apologize about this code. I was young ^^

//LIBRARIES
#include <ITDB02_Graph16.h>
#include <avr/pgmspace.h>
#include <ITDB02_Touch.h>
#include <Wire.h>
#include <EEPROM.h>
#include "writeAnything.h"
#include "readAnything.h"
#include <DS1307.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//Default Controller Settings
boolean RECOM_RCD = true; //For Mean Well drivers change "true" to "false"
boolean CLOCK_SCREENSAVER = true; //For a Clock Screensaver "true" / Blank Screen "false"
//You can turn the Screensaver ON/OFF in the pogram

//TOUCH PANEL and ITDB02 MEGA SHIELD
//(Mega Shield utilizes pins 5V, 3V3, GND, 2-6, 20-41, & (50-53 for SD Card))
ITDB02 myGLCD(38,39,40,41,ITDB32S); //May need to add "ITDB32S" depending on LCD controller
ITDB02_Touch myTouch(6,5,4,3,2);

//Declare which fonts to be utilized
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];
//Images Alarme
extern unsigned int //image alarm
imagegreen[4096],
imagehot[4096],
imagecold[4096];

#define LARGE true
#define SMALL false

//Define the PWM PINS for the LEDs
const int ledPinWhite1 = 7;
const int ledPinWhite2 = 8;
const int ledPinRed = 9;
const int ledPinRoyalBlue = 10;

// Define the other DIGITAL and/or PWM PINS being used
const int relayPin1 = 42;
const int relayPin2 = 43;
const int relayPin3 = 44;
const int relayPin4 = 45;
const int alarmePin = 46; //Buzzer Alarm
const int engrais1Pin = 47;
const int engrais2Pin = 48;

float autoco2 = 1;
float manuelco2 = 0;
float autoalimled = 1;
float manuelalimled = 0;
float etat_alarme = 0 ;
float etat_relay1 = 0;
float etat_relay2 = 0;
float etat_relay3 = 0;
float etat_relay4 = 0;


// DS18B20 Temperature sensors plugged into pin 51 (Water, Hood, & Sump)
OneWire OneWireBus(51); //Choose a digital pin
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&OneWireBus);
// Assign the addresses of temperature sensors. Add/Change addresses as needed.
DeviceAddress waterThermometer = {
0x28, 0x5C, 0x56, 0x59, 0x03, 0x00, 0x00, 0xEB };
DeviceAddress pieceThermometer = {
0x28, 0xC8, 0x4E, 0x59, 0x03, 0x00, 0x00, 0xC4 };

float tempW = 0; //Water temperature values
float tempP = 0; //Ambiant temperature
int setTempScale = 0; //Celsius=0 || Fahrenheit=1 (change in prog)
float setTempC = 0.0; //Desired Water Temperature (User input in program)
float setTempF = 0.0;
float fanTempC = 0.0; //Desired Water Temp. Offsets for Heater & Chiller (User input in program)
float fanTempF = 0.0;
float alarmTempC = 0.0; //Temperature the Alarm will sound (User input in program)
float alarmTempF = 0.0;
long intervalAlarm = 1000 * 30; //Interval to beep the Alarm (1000 * seconds)
float temp2beS; //Temporary Temperature Values
float temp2beO; //Temporary Temperature Values
float temp2beA; //Temporary Temperature Values
String degC_F;

//RTC
int setCalendarFormat = 0; //DD/MM/YYYY=0 || Month DD, YYYY=1 (change in prog)
int setTimeFormat = 0; //24HR=0 || 12HR=1 (change in prog)
int rtc[7], rtcSet[7]; //Clock arrays
int rtcSet2, AM_PM, yTime; //Setting clock stuff
int timeDispH, timeDispM,
xTimeH, xTimeM10, xTimeM1,
xTimeAMPM, xColon;
byte data[56];
String time, date, day;

int dispScreen=0; //0-Main Screen, 1-Menu, 2-Clock Setup, 3-Temp Control,
//4-LED Test Options, 5-Test LED Arrays, 6-Test Individual
//LED Colors, 7-Choose LED Color, 8-View Selected Color
//Values, 9-Change Selected Color Array Values
//10-Wavemaker, 11-Wavemaker Settings, 12-General
//Settings, 13-Automatic Feeder, 14-Set Feeder Timers,
//15-About

int x, y; //touch coordinates

long previousMillisLED = 0; //Used in the Test LED Array Function
long previousMillisFive = 0; //Used in the Main Loop (Checks Time,Temp,LEDs,Screen)
long previousMillisAlarm = 0; //Used in the Alarm

int setScreensaver = 1; //ON=1 || OFF=2 (change in prog)
int screenSaverTimer = 0; //counter for Screen Saver
int setScreenSaverTimer = (15)*12; //how long in (minutes) before Screensaver comes

boolean SCREEN_RETURN = true; //Auto Return to mainScreen() after so long of inactivity
int returnTimer = 0; //counter for Screen Return
int setReturnTimer =
setScreenSaverTimer * .75; //Will return to main screen after 75% of the amount of
//time it takes before the screensaver turns on

int LedChangTime = 0; //LED change page, time and values

int min_cnt; //Used to determine the place in the color arrays

boolean LEDtestTick = false; //for testing LEDs and speed up clock

int white1Led, white2Led, royalblueLed, //previous LED output values
redLed;
int white1led_out, white2led_out, royalblueled_out, //current LED output values
redled_out, colorled_out;

int COLOR=0, WHITE1=1, WHITE2=2, //Used to Determine View/Change Color LED Values
ROYAL=4, RED=3;

boolean colorLEDtest = false; //To test individual color LEDs
int Rgad=0, Ggad=0, Bgad=0, //Used in the Test Ind. Color LEDs Gadget
Rfont=0, Gfont=0, Bfont=0,
Rback=0, Gback=0, Bback=0,
Rline=0, Gline=0, Bline=0;
int CL_1=0, CL_10=0, CL_100=0, //Used in the Test Ind. Color LEDs Gadget
cl_1, cl_10, cl_100;
int w2col_out, w1col_out, rbcol_out, //Current LED output values for Test Ind. Color LEDs
rcol_out;
int x1Bar=0, x2Bar=0, //Used in LED Output Chart on Test Ind. LEDs Screen
xValue=0, yValue=0,
LEDlevel, yBar;

//DIMMING VALUES can be changed below BUT the EEPROM must be cleared first.
//To CLEAR EEPROM, use arduino-0022\libraries\EEPROM\examples\eeprom_clear\eeprom_clear.pde
//and change the 512 to 4096 before Upload. After the LED comes on indicating the EEPROM
//has been cleared, it is now ok to change DIMMING VALUES below & Upload the sketch.
//SUMP Dimming Values 8pm to 8am
//REGULAR BLUE Dimming Values

//WHITE 1 Dimming Values (White LED array in RAM)
byte white1led[96] = {
0, 0, 0, 0, 0, 0, 0, 0, //0 - 1
0, 0, 0, 0, 0, 0, 0, 0, //2 - 3
0, 0, 0, 0, 0, 0, 0, 0, //4 - 5
0, 0, 0, 0, 0, 0, 0, 0, //6 - 7
0, 0, 0, 0, 0, 0, 0, 0, //8 - 9
28, 32, 38, 50, 70, 90, 90, 90, //10 - 11
95, 95, 95, 95, 105, 105, 105, 105, //12 - 13
120, 120, 120, 120, 125, 125, 125, 125, //14 - 15
125, 125, 125, 125, 120, 120, 120, 120, //16 - 17
115, 115, 115, 110, 100, 100, 95, 95, //18 - 19
70, 50, 38, 32, 28, 0, 0, 0, //20 - 21
0, 0, 0, 0, 0, 0, 0, 0 //22 - 23
};

//WHITE 2 Dimming Values (White LED array in RAM)
byte white2led[96] = {
0, 0, 0, 0, 0, 0, 0, 0, //0 - 1
0, 0, 0, 0, 0, 0, 0, 0, //2 - 3
0, 0, 0, 0, 0, 0, 0, 0, //4 - 5
0, 0, 0, 0, 0, 0, 0, 0, //6 - 7
0, 0, 0, 0, 0, 0, 0, 0, //8 - 9
28, 32, 38, 50, 70, 90, 90, 90, //10 - 11
95, 95, 95, 95, 105, 105, 105, 105, //12 - 13
120, 120, 120, 120, 125, 125, 125, 125, //14 - 15
125, 125, 125, 125, 120, 120, 120, 120, //16 - 17
115, 115, 115, 110, 100, 100, 95, 95, //18 - 19
70, 50, 38, 32, 28, 0, 0, 0, //20 - 21
0, 0, 0, 0, 0, 0, 0, 0 //22 - 23
};
//ROYAL BLUE Dimming Values
byte royalblueled[96] = {
0, 0, 0, 0, 0, 0, 0, 0, //0 - 1
0, 0, 0, 0, 0, 0, 0, 0, //2 - 3
0, 0, 0, 0, 0, 0, 0, 0, //4 - 5
0, 0, 0, 0, 0, 0, 35, 40, //6 - 7
40, 43, 47, 55, 65, 75, 80, 85, //8 - 9
90, 95, 95, 100, 110, 110, 115, 120, //10 - 11
125, 130, 135, 145, 145, 145, 150, 155, //12 - 13
160, 165, 170, 175, 180, 180, 180, 180, //14 - 15
180, 180, 180, 180, 175, 170, 165, 160, //16 - 17
155, 150, 145, 145, 145, 135, 130, 125, //18 - 19
120, 115, 110, 110, 100, 75, 65, 50, //20 - 21
40, 35, 33, 28, 0, 0, 0, 0 //22 - 23
};
//RED Dimming Values
byte redled[96] = {
0, 0, 0, 0, 0, 0, 0, 0, //0 - 1
0, 0, 0, 0, 0, 0, 0, 0, //2 - 3
0, 0, 0, 0, 0, 0, 0, 0, //4 - 5
0, 0, 0, 0, 0, 0, 0, 0, //6 - 7
0, 0, 0, 0, 0, 0, 0, 0, //8 - 9
0, 0, 0, 0, 0, 0, 0, 0, //10 - 11
30, 30, 40, 40, 50, 50, 60, 60, //12 - 13
60, 60, 50, 50, 40, 40, 30, 30, //14 - 15
0, 0, 0, 0, 0, 0, 0, 0, //16 - 17
0, 0, 0, 0, 0, 0, 0, 0, //18 - 19
0, 0, 0, 0, 0, 0, 0, 0, //20 - 21
0, 0, 0, 0, 0, 0, 0, 0
};
byte tled[96]; //Temporary Array to Hold changed LED Values



/**************************** CHOOSE OPTION MENU BUTTONS *****************************/
const int tanD[]= {
10, 29, 155, 59}; //"TIME and DATE" settings
const int gSet[]= {
10, 69, 155, 99}; //"GENERAL SETTINGS" page
const int tempSet[]= {
10, 109, 155, 139}; // Parametre temperature et ventilateur
const int co2Set[]= {
10, 149, 155, 179}; //"Parametre CO2 menu
const int engraisSet[]= {
165, 29, 310, 59}; //"Parametre Engrais menu
const int alimledSet[]= {
165, 69, 310, 99}; //"Parametre Alim LED menu
const int tesT[]= {
165, 109, 310, 139}; //"LED TESTING OPTIONS" menu
const int ledChM[]= {
165, 149, 310, 179}; //"CHANGE LED VALUES" menu

/**************************** TIME AND DATE SCREEN BUTTONS ***************************/
const int houU[]= {
110, 22, 135, 47}; //hour up
const int minU[]= {
180, 22, 205, 47}; //min up
const int ampmU[]= {
265, 22, 290, 47}; //AM/PM up
const int houD[]= {
110, 73, 135, 96}; //hour down
const int minD[]= {
180, 73, 205, 96}; //min down
const int ampmD[]= {
265, 73, 290, 96}; //AM/PM down
const int dayU[]= {
110, 112, 135, 137}; //day up
const int monU[]= {
180, 112, 205, 137}; //month up
const int yeaU[]= {
265, 112, 290, 137}; //year up
const int dayD[]= {
110, 162, 135, 187}; //day down
const int monD[]= {
180, 162, 205, 187}; //month down
const int yeaD[]= {
265, 162, 290, 187}; //year down
/*************************** H2O TEMP CONTROL SCREEN BUTTONS *************************/
const int temM[]= {
90, 49, 115, 74}; //temp. minus
const int temP[]= {
205, 49, 230, 74}; //temp. plus
const int offM[]= {
90, 99, 115, 124}; //offset minus
const int offP[]= {
205, 99, 230, 124}; //offset plus
const int almM[]= {
90, 149, 115, 174}; //alarm minus
const int almP[]= {
205, 149, 230, 174}; //alarm plus
/**************************** LED TESTING MENU BUTTONS *******************************/
const int tstLA[] = {
40, 59, 280, 99}; //"Test LED Array Output" settings
const int cntIL[] = {
40, 109, 280, 149}; //"Control Individual LEDs" settings
/********************** TEST LED ARRAY OUTPUT SCREEN BUTTONS *************************/
const int stsT[]= {
110, 105, 200, 175}; //start/stop
const int tenM[]= {
20, 120, 90, 160}; //-10s
const int tenP[]= {
220, 120, 290, 160}; //+10s
/******************** TEST INDIVIDUAL LED VALUES SCREEN BUTTONS **********************/
//These Buttons are made within the function
/****************** CHANGE INDIVIDUAL LED VALUES SCREEN BUTTONS **********************/
//These Buttons are made within the function
/************************* CHANGE LED VALUES MENU BUTTONS ****************************/
const int btCIL[]= {
5, 188, 90, 220}; //back to Change Individual LEDs Screen
const int ledChV[]= {
110, 200, 210, 220}; //LED Change Values
const int eeprom[]= {
215, 200, 315, 220}; //Save to EEPROM (Right Button)
/***************************** MISCELLANEOUS BUTTONS *********************************/
const int back[]= {
5, 200, 105, 220}; //BACK
const int prSAVE[]= {
110, 200, 210, 220}; //SAVE
const int canC[]= {
215, 200, 315, 220}; //CANCEL
/**************************** END OF PRIMARY BUTTONS *********************************/


/********************************* EEPROM FUNCTIONS ***********************************/
struct config_g
{
int calendarFormat;
int timeFormat;
int tempScale;
int SCREENsaver;
}
GENERALsettings;

struct config_t
{
int tempset;
int tempFset;
int tempoff;
int tempFoff;
int tempalarm;
int tempFalarm;
} 
tempSettings;

struct config_c
{
int autoco2set;
int manuelco2set;
}
co2Settings;

struct config_a
{
int autoalimledset;
int manuelalimledset;
}
alimledSettings;

void SaveLEDToEEPROM()
{
EEPROM.write(0, 123); //to determine if data available in EEPROM
for (int i=1; i<97; i++)
{
EEPROM.write(i+(96*0), white1led[i]);
EEPROM.write(i+(96*1), white2led[i]);
EEPROM.write(i+(96*2), royalblueled[i]);
EEPROM.write(i+(96*3), redled[i]);
}
}

void SaveGenSetsToEEPROM()
{
GENERALsettings.calendarFormat = int(setCalendarFormat);
GENERALsettings.timeFormat = int(setTimeFormat);
GENERALsettings.tempScale = int(setTempScale);
GENERALsettings.SCREENsaver = int(setScreensaver);
EEPROM_writeAnything(660, GENERALsettings);
}

void Saveco2SetsToEEPROM()
{
co2Settings.autoco2set = int(autoco2);
co2Settings.manuelco2set = int(manuelco2);
EEPROM_writeAnything(620, co2Settings);
}

void SavealimledSetsToEEPROM()
{
alimledSettings.autoalimledset = int(autoalimled);
alimledSettings.manuelalimledset = int(manuelalimled);
EEPROM_writeAnything(600, alimledSettings);
}

void SaveTempToEEPROM()
{
tempSettings.tempset = int(setTempC*10);
tempSettings.tempFset = int(setTempF*10);
tempSettings.tempoff = int(fanTempC*10);
tempSettings.tempFoff = int(fanTempF*10);
tempSettings.tempalarm = int(alarmTempC*10);
tempSettings.tempFalarm = int(alarmTempF*10);
EEPROM_writeAnything(640, tempSettings);
}

void ReadFromEEPROM()
{
int k = EEPROM.read(0);
char tempString[3];

if (k==123) {
for (int i=1; i<97; i++) {
white1led[i] = EEPROM.read(i+(96*0));
white2led[i] = EEPROM.read(i+(96*1));
royalblueled[i] = EEPROM.read(i+(96*2));
redled[i] = EEPROM.read(i+(96*3));
}
}

EEPROM_readAnything(640, tempSettings);
setTempC = tempSettings.tempset;
setTempC /=10;
setTempF = tempSettings.tempFset;
setTempF /=10;
fanTempC = tempSettings.tempoff;
fanTempC /=10;
fanTempF = tempSettings.tempFoff;
fanTempF /=10;
alarmTempC = tempSettings.tempalarm;
alarmTempC /= 10;
alarmTempF = tempSettings.tempFalarm;
alarmTempF /= 10;

EEPROM_readAnything(660, GENERALsettings);
setCalendarFormat = GENERALsettings.calendarFormat;
setTimeFormat = GENERALsettings.timeFormat;
setTempScale = GENERALsettings.tempScale;
setScreensaver = GENERALsettings.SCREENsaver;

EEPROM_readAnything(620, co2Settings);
autoco2 = co2Settings.autoco2set;
manuelco2 = co2Settings.manuelco2set;

EEPROM_readAnything(600, alimledSettings);
autoalimled = alimledSettings.autoalimledset;
manuelalimled = alimledSettings.manuelalimledset;
}
/***************************** END OF EEPROM FUNCTIONS ********************************/

/********************************** RTC FUNCTIONS *************************************/
void SaveRTC()
{
int year=rtcSet[6] - 2000;

RTC.stop(); //RTC clock setup
RTC.set(DS1307_SEC,1);
RTC.set(DS1307_MIN,rtcSet[1]);
RTC.set(DS1307_HR,rtcSet[2]);
//RTC.set(DS1307_DOW,1);
RTC.set(DS1307_DATE,rtcSet[4]);
RTC.set(DS1307_MTH,rtcSet[5]);
RTC.set(DS1307_YR,year);
delay(10);
RTC.start();
delay(10);
for(int i=0; i<56; i++)
{
RTC.set_sram_byte(65,i);
}
delay(50);
}
/********************************* END OF RTC FUNCTIONS *******************************/

/********************************** TIME AND DATE BAR **********************************/
void TimeDateBar(boolean refreshAll=false)
{
String oldVal, rtc1, rtc2, ampm, month;

if ((rtc[1]>=0) && (rtc[1]<=9))
{
rtc1= '0' + String(rtc[1]);
} //adds 0 to minutes
else {
rtc1= String(rtc[1]);
}

if (setTimeFormat==1)
{
if (rtc[2]==0) {
rtc2= 12;
} //12 HR Format
else {
if (rtc[2]>12) {
rtc2= rtc[2]-12;
}
else {
rtc2= String(rtc[2]);
}
}
}

if(rtc[2] < 12){
ampm= " AM ";
} //Adding the AM/PM sufffix
else {
ampm= " PM ";
}

oldVal = time; //refresh time if different
if (setTimeFormat==1)
{
time= rtc2 + ':' + rtc1 + ampm;
}
else {
time= " " + String(rtc[2]) + ':' + rtc1 +" ";
}
if ((oldVal!=time) || refreshAll)
{
char bufferT[9];
time.toCharArray(bufferT, 9); //String to Char array
setFont(SMALL, 255, 255, 0, 64, 64, 64);
myGLCD.print(bufferT, 215, 227); //Display time
}

if (rtc[5]==1) {
month= "JAN ";
} //Convert the month to its name
if (rtc[5]==2) {
month= "FEB ";
}
if (rtc[5]==3) {
month= "MAR ";
}
if (rtc[5]==4) {
month= "APR ";
}
if (rtc[5]==5) {
month= "MAY ";
}
if (rtc[5]==6) {
month= "JUN ";
}
if (rtc[5]==7) {
month= "JLY ";
}
if (rtc[5]==8) {
month= "AUG ";
}
if (rtc[5]==9) {
month= "SEP ";
}
if (rtc[5]==10) {
month= "OCT ";
}
if (rtc[5]==11) {
month= "NOV ";
}
if (rtc[5]==12) {
month= "DEC ";
}

oldVal = date; //refresh date if different

if (setCalendarFormat==0)
{
date= " " + String(rtc[4]) + '/' + String(rtc[5]) + '/' + String(rtc[6]) + " ";
}
else
{
date= " " + month + String(rtc[4]) + ',' + ' ' + String(rtc[6]);
}
if ((oldVal!=date) || refreshAll)
{
char bufferD[15];
date.toCharArray(bufferD, 15); //String to Char array
setFont(SMALL, 255, 255, 0, 64, 64, 64);
myGLCD.print(bufferD, 20, 227); //Display date
}
}
/****************************** END OF TIME AND DATE BAR ******************************/

/******************************** TEMPERATURE FUNCTIONS *******************************/
void checkTempC()
{
sensors.requestTemperatures(); // call sensors.requestTemperatures() to issue a global
// temperature request to all devices on the bus
tempW = (sensors.getTempC(waterThermometer)); //read water temperature
tempP = (sensors.getTempC(pieceThermometer)); //read hood's heatsink temperature
if (tempW<(setTempC+alarmTempC) && tempW>(setTempC-alarmTempC)) {
etat_alarme = 0;
digitalWrite(alarmePin, LOW); //turn off alarm
}
if (tempW<(setTempC+fanTempC)) //turn off ventilateur
{
etat_relay4 = 0;
digitalWrite(relayPin4, LOW);
}
if (fanTempC>0)
{
if (tempW >=(setTempC+fanTempC)) //turn on ventilateur turn off chauffage
{
etat_relay4 = 1;
etat_relay2 = 0;
digitalWrite(relayPin2, LOW);
digitalWrite(relayPin4, HIGH);
}
if (tempW < (setTempC+fanTempC)) //turn on chauffage
{
etat_relay2 = 1;
digitalWrite(relayPin2, HIGH);
}
if (tempW <= (setTempC-fanTempC))
{
etat_relay2 = 1;
digitalWrite(relayPin2, HIGH);
}
}
if (alarmTempC>0) //turn on alarm
{
if ((tempW>=(setTempC+alarmTempC)) || (tempW<=(setTempC-alarmTempC)))
{
etat_alarme = 1;
unsigned long cMillis = millis();
if (cMillis - previousMillisAlarm > intervalAlarm)
{
previousMillisAlarm = cMillis;
digitalWrite(alarmePin, HIGH);
delay(1000);
digitalWrite(alarmePin, LOW);
}
}
}

}
/*************************** END OF TEMPERATURE FUNCTIONS *****************************/


/************************************* COMMANDE RELAY *************************************/

void CommandeRelay()
{
if (autoco2 == 1)
{
if ((white1led_out > 0) && (white2led_out > 0)) // commande relay 1 = co2Set
{
etat_relay1 = 1;
digitalWrite(relayPin1, HIGH);
}
else
{
etat_relay1 = 0;
digitalWrite(relayPin1, LOW);
}
}
if (autoco2 == 0)
{
if (manuelco2 == 1)
{
etat_relay1 = 1;
digitalWrite(relayPin1, HIGH);
}
else
{
etat_relay1 = 0;
digitalWrite(relayPin1, LOW);
}
}
if (autoalimled == 1)
{
if ((white1led_out == 0) && (white2led_out == 0) && (redled_out == 0) && (royalblueled_out == 0)) // commande relay 3 = Alimentation LED
{
etat_relay3 = 0;
digitalWrite(relayPin3, LOW);
}
else
{
etat_relay3 = 1;
digitalWrite(relayPin3, HIGH);
}
}
if (autoalimled == 0)
{
if (manuelalimled == 1)
{
etat_relay3 = 1;
digitalWrite(relayPin3, HIGH);
}
else
{
etat_relay3 = 0;
digitalWrite(relayPin3, LOW);
}
}
}

/************************************* LED LEVELS *************************************/
void LED_levels_output()
{
int sector, sstep, t1, t2 ;
int w1_out, w2_out, rb_out, r_out;

if (min_cnt>=1440) {
min_cnt=1;
} // 24 hours of minutes
sector = min_cnt/15; // divided by gives sector -- 15 minute
sstep = min_cnt%15; // remainder gives add on to sector value

t1 =sector;
if (t1==95) {
t2=0;
}
else {
t2 = t1+1;
}


if (colorLEDtest)
{
white1led_out = w1col_out;
white2led_out = w2col_out;
royalblueled_out = rbcol_out;
redled_out = rcol_out;
}
else
{
if (sstep==0)
{
white1led_out = white1led[t1];
white2led_out = white2led[t1];
royalblueled_out = royalblueled[t1];
redled_out = redled[t1];
}
else
{
white1led_out = check(&white1led[t1], &white1led[t2], sstep);
white2led_out = check(&white2led[t1], &white2led[t2], sstep);
royalblueled_out = check(&royalblueled[t1], &royalblueled[t2], sstep);
redled_out = check(&redled[t1], &redled[t2], sstep);
}

}

if (RECOM_RCD) {
w1_out = white1led_out;
w2_out = white2led_out;
rb_out = royalblueled_out;
r_out = redled_out;
}
else {
w1_out = 255 - white1led_out;
w2_out = 255 - white2led_out;
rb_out = 255 - royalblueled_out;
r_out = 255 - redled_out;
}

analogWrite(ledPinWhite1, w1_out);
analogWrite(ledPinWhite2, w2_out);
analogWrite(ledPinRoyalBlue, rb_out);
analogWrite(ledPinRed, r_out);
}

int check( byte *pt1, byte *pt2, int lstep)
{
int result;
float fresult;

if (*pt1==*pt2) {
result = *pt1;
} // No change
else if (*pt1<*pt2) //Increasing brightness
{
fresult = ((float(*pt2-*pt1)/15.0) * float(lstep))+float(*pt1);
result = int(fresult);
}
//Decreasing brightness
else {
fresult = -((float(*pt1-*pt2)/15.0) * float(lstep))+float(*pt1);
result = int(fresult);
}
return result;
}
/********************************* END OF LED LEVELS **********************************/

/********************************* MISC. FUNCTIONS ************************************/
void clearScreen()
{
myGLCD.setColor(0, 0, 0);
myGLCD.fillRect(1, 15, 318, 226);
}


void printButton(char* text, int x1, int y1, int x2, int y2, boolean fontsize = false)
{
int stl = strlen(text);
int fx, fy;

myGLCD.setColor(0, 0, 255);
myGLCD.fillRoundRect (x1, y1, x2, y2);
myGLCD.setColor(255, 255, 255);
myGLCD.drawRoundRect (x1, y1, x2, y2);

myGLCD.setBackColor(0, 0, 255);
if (fontsize) {
myGLCD.setFont(BigFont);
fx = x1+(((x2 - x1+1)-(stl*16))/2);
fy = y1+(((y2 - y1+1)-16)/2);
myGLCD.print(text, fx, fy);
}
else {
myGLCD.setFont(SmallFont);
fx = x1+(((x2 - x1)-(stl*8))/2);
fy = y1+(((y2 - y1-1)-8)/2);
myGLCD.print(text, fx, fy);
}
}

void printCenter(char* text, int x1, int y1, int x2, int y2)
{
int stl = strlen(text);
int fx, fy;
fx = x1+(((x2 - x1)-(stl*8))/2);
fy = y1+(((y2 - y1-1)-8)/2);
myGLCD.print(text, fx, fy);
}

void printLedChangerP(char* text, int x1, int y1, int x2, int y2, boolean fontsize = false)
{
int stl = strlen(text);
int fx, fy;

myGLCD.setColor(255, 255, 255);
myGLCD.fillRect (x1, y1, x2, y2);
myGLCD.setColor(255, 255, 255);
myGLCD.drawRect (x1, y1, x2, y2);

myGLCD.setBackColor(255, 255, 255);
myGLCD.setColor(255, 0, 0);
if (fontsize) {
myGLCD.setFont(BigFont);
fx = x1+(((x2 - x1+1)-(stl*16))/2);
fy = y1+(((y2 - y1+1)-16)/2);
myGLCD.print(text, fx, fy);
}
else {
myGLCD.setFont(SmallFont);
fx = x1+(((x2 - x1)-(stl*8))/2);
fy = y1+(((y2 - y1-1)-8)/2);
myGLCD.print(text, fx, fy);
}
}


void printLedChangerM(char* text, int x1, int y1, int x2, int y2, boolean fontsize = false)
{
int stl = strlen(text);
int fx, fy;

myGLCD.setColor(255, 255, 255);
myGLCD.fillRect (x1, y1, x2, y2);
myGLCD.setColor(255, 255, 255);
myGLCD.drawRect (x1, y1, x2, y2);

myGLCD.setBackColor(255, 255, 255);
myGLCD.setColor(0, 0, 255);
if (fontsize) {
myGLCD.setFont(BigFont);
fx = x1+(((x2 - x1+1)-(stl*16))/2);
fy = y1+(((y2 - y1+1)-16)/2);
myGLCD.print(text, fx, fy);
}
else {
myGLCD.setFont(SmallFont);
fx = x1+(((x2 - x1)-(stl*8))/2);
fy = y1+(((y2 - y1-1)-8)/2);
myGLCD.print(text, fx, fy);
}
}


void printHeader(char* headline)
{
setFont(SMALL, 255, 255, 0, 255, 255, 0);
myGLCD.fillRect (1, 1, 318, 14);
myGLCD.setColor(0, 0, 0);
myGLCD.print(headline, CENTER, 1);
}


void setFont(boolean font, byte cr, byte cg, byte cb, byte br, byte bg, byte bb)
{
myGLCD.setBackColor(br, bg, bb); //font background black
myGLCD.setColor(cr, cg, cb); //font color white
if (font==LARGE)
myGLCD.setFont(BigFont); //font size LARGE
else if (font==SMALL)
myGLCD.setFont(SmallFont);
}


void waitForIt(int x1, int y1, int x2, int y2) // Draw a red frame while a button is touched
{
myGLCD.setColor(255, 0, 0);
myGLCD.drawRoundRect (x1, y1, x2, y2);
while (myTouch.dataAvailable()) {
myTouch.read();
}
myGLCD.setColor(255, 255, 255);
myGLCD.drawRoundRect (x1, y1, x2, y2);
}

void waitForItSq(int x1, int y1, int x2, int y2) // Draw a red frame while a button is touched
{
myGLCD.setColor(255, 0, 0);
myGLCD.drawRect (x1, y1, x2, y2);
while (myTouch.dataAvailable()) {
myTouch.read();
}
myGLCD.setColor(255, 255, 255);
myGLCD.drawRect (x1, y1, x2, y2);
}


int LedToPercent (int Led_out) //returns LED output in %
{
int result;

if (Led_out==0) {
result = 0;
}
else {
result = map(Led_out, 1, 255, 1, 100);
}

return result;
}


void drawBarGraph()
{
myGLCD.setColor(255, 255, 255); //LED Chart
setFont(SMALL, 255, 255, 255, 0, 0, 0);

myGLCD.drawRect(30, 137, 113, 138); //x-line
myGLCD.drawRect(30, 137, 31, 36); //y-line

for (int i=0; i<5; i++) //tick-marks
{
myGLCD.drawLine(31, (i*20)+36, 35, (i*20)+36);
}
myGLCD.print("100", 4, 30);
myGLCD.print("80", 12, 50);
myGLCD.print("60", 12, 70);
myGLCD.print("40", 12, 90);
myGLCD.print("20", 12, 110);
myGLCD.print("0", 20, 130);

myGLCD.setColor(255, 255, 255);
myGLCD.drawRect(40, 136, 52, 135); //white1 %bar place holder
myGLCD.setColor(255, 255, 255);
myGLCD.drawRect(57, 136, 69, 135); //white2 %bar place holder
myGLCD.setColor(255, 0, 0);
myGLCD.drawRect(74, 136, 86, 135); //red %bar place holder
myGLCD.setColor(58, 95, 205);
myGLCD.drawRect(91, 136, 103, 135); //royal %bar place holder
}


void drawBarandColorValue()
{
colorled_out = CL_100 + CL_10 + CL_1;
setFont(SMALL, Rback, Gback, Bback, Rback, Gback, Bback);
myGLCD.print(" ", xValue, yValue); //fill over old
setFont(SMALL, Rfont, Gfont, Bfont, Rback, Gback, Bback);
myGLCD.printNumI(colorled_out, xValue, yValue);

yBar = 136 - colorled_out*.39;
myGLCD.setColor(0, 0, 0);
myGLCD.fillRect(x1Bar, yBar, x2Bar, 36); //hide end of last bar
myGLCD.setColor(Rgad, Ggad, Bgad);
myGLCD.fillRect(x1Bar, 136, x2Bar, yBar); //color percentage bar
if (COLOR==RED) {
rcol_out=colorled_out;
}
if (COLOR==WHITE1) {
w1col_out=colorled_out;
}
if (COLOR==WHITE2) {
w2col_out=colorled_out;
}
if (COLOR==ROYAL) {
rbcol_out=colorled_out;
}
}


void ledChangerGadget()
{
myGLCD.setColor(Rgad, Ggad, Bgad);
myGLCD.fillRoundRect(199, 25, 285, 132); //Gadget Color
myGLCD.setColor(0, 0, 0);
myGLCD.fillRect(204, 50, 280, 74); //Black Background of Numbers
myGLCD.setColor(255, 255, 255);
myGLCD.drawRoundRect(199, 25, 285, 132); //Outline Gadget
myGLCD.setColor(Rline, Gline, Bline); //Line Color
myGLCD.drawLine(199, 46, 285, 46);
setFont(SMALL, Rfont, Gfont, Bfont, Rback, Gback, Bback);
if (COLOR==0){
myGLCD.print("COLOR", 202, 28);
}
if (COLOR==1){
myGLCD.print("BLANC 1", 202, 28);
}
if (COLOR==2){
myGLCD.print("BlANC 2", 202, 28);
}
if (COLOR==4){
myGLCD.print("ROYAL", 202, 28);
}
if (COLOR==3){
myGLCD.print("ROUGE", 202, 28);
}
for (int b=0; b<3; b++)
{
printLedChangerP("+", (b*27)+204, 78, (b*27)+226, 100, LARGE); //Press Increase Number
printLedChangerM("-", (b*27)+204, 105, (b*27)+226, 127, LARGE); //Press Decrease Number
}
for (int c=0; c<3; c++)
{
myGLCD.setColor(Rline, Gline, Bline);
myGLCD.drawRect((c*27)+204, 78, (c*27)+226, 100);
myGLCD.drawRect((c*27)+204, 105, (c*27)+226, 127);
}
setFont(LARGE, 255, 255, 255, 0, 0, 0);
myGLCD.printNumI(cl_100, 214, 54);
myGLCD.printNumI(cl_10, 234, 54);
myGLCD.printNumI(cl_1, 255, 54);
}


void TimeSaver(boolean refreshAll=false)
{
if (setTimeFormat==0) //24HR Format
{
myGLCD.setColor(0, 0, 255);
myGLCD.setBackColor(0, 0, 0);
myGLCD.setFont(SevenSegNumFont);
if ((rtc[2]>=0) && (rtc[2]<=9)) //Display HOUR
{
myGLCD.setColor(0, 0, 0);
myGLCD.fillRect(80, 95, 111, 145);
myGLCD.setColor(0, 0, 255);
myGLCD.printNumI(rtc[2], 112, 95);
}
else {
myGLCD.printNumI(rtc[2], 80, 95);
}
}

if (setTimeFormat==1) //12HR Format
{
myGLCD.setColor(0, 0, 255);
myGLCD.setBackColor(0, 0, 0);
myGLCD.setFont(SevenSegNumFont);
if (rtc[2]==0) //Display HOUR
{
myGLCD.print("12", 80, 95);
}
if ((rtc[2]>=1) && (rtc[2]<=9))
{
myGLCD.setColor(0, 0, 0);
myGLCD.fillRect(80, 95, 111, 145);
myGLCD.setColor(0, 0, 255);
myGLCD.printNumI(rtc[2], 112, 95);
}
if ((rtc[2]>=10) && (rtc[2]<=12))
{
myGLCD.printNumI(rtc[2], 80, 95);
}
if ((rtc[2]>=13) && (rtc[2]<=21))
{
myGLCD.setColor(0, 0, 0);
myGLCD.fillRect(80, 95, 111, 145);
myGLCD.setColor(0, 0, 255);
myGLCD.printNumI(rtc[2]-12, 112, 95);
}
if (rtc[2]>=22)
{
myGLCD.printNumI(rtc[2]-12, 80, 95);
}

if ((rtc[2]>=0) && (rtc[2]<=11)) //Display AM/PM
{
setFont(LARGE, 0, 0, 255, 0, 0, 0);
myGLCD.print("AM", 244, 129);
}
else
{
setFont(LARGE, 0, 0, 255, 0, 0, 0);
myGLCD.print("PM", 244, 129);
}
}

myGLCD.setColor(0, 0, 255);
myGLCD.setBackColor(0, 0, 0);
myGLCD.setFont(SevenSegNumFont);
myGLCD.fillCircle(160, 108, 4);
myGLCD.fillCircle(160, 132, 4);
if ((rtc[1]>=0) && (rtc[1]<=9)) //Display MINUTES
{
myGLCD.print("0", 176, 95);
myGLCD.printNumI(rtc[1], 208, 95);
}
else {
myGLCD.printNumI(rtc[1], 176, 95);
}
}


void screenSaver() //Make the Screen Go Blank after so long
{
if (setScreensaver==1)
{
if (myTouch.dataAvailable())
{
processMyTouch();
}
else {
screenSaverTimer++;
}
if (screenSaverTimer==setScreenSaverTimer)
{
dispScreen=0;
myGLCD.clrScr();
}
if (CLOCK_SCREENSAVER==true)
{
if (screenSaverTimer>setScreenSaverTimer)
{
dispScreen=0;
TimeSaver(true);
}
}
}
}


void genSetSelect()
{
if (setCalendarFormat==0) //Calendar Format Buttons
{
myGLCD.setColor(0, 255, 0);
myGLCD.fillRoundRect(185, 19, 305, 39);
setFont(SMALL, 0, 0, 0, 0, 255, 0);
myGLCD.print("DD/MM/YYYY", 207, 23);
myGLCD.setColor(0, 0, 255);
myGLCD.fillRoundRect(185, 45, 305, 65);
setFont(SMALL, 255, 255, 255, 0, 0, 255);
myGLCD.print("MTH DD, YYYY", 199, 49);
}
else
{
myGLCD.setColor(0, 0, 255);
myGLCD.fillRoundRect(185, 19, 305, 39);
setFont(SMALL, 255, 255, 255, 0, 0, 255);
myGLCD.print("DD/MM/YYYY", 207, 23);
myGLCD.setColor(0, 255, 0);
myGLCD.fillRoundRect(185, 45, 305, 65);
setFont(SMALL, 0, 0, 0, 0, 255, 0);
myGLCD.print("MTH DD, YYYY", 199, 49);
}
if (setTimeFormat==0) //Time Format Buttons
{
myGLCD.setColor(0, 0, 255);
myGLCD.fillRoundRect(195, 76, 235, 96);
setFont(SMALL, 255, 255, 255, 0, 0, 255);
myGLCD.print("12HR", 201, 80);
myGLCD.setColor(0, 255, 0);
myGLCD.fillRoundRect(255, 76, 295, 96);
setFont(SMALL, 0, 0, 0, 0, 255, 0);
myGLCD.print("24HR", 261, 80);
}
else
{
myGLCD.setColor(0, 255, 0);
myGLCD.fillRoundRect(195, 76, 235, 96);
setFont(SMALL, 0, 0, 0, 0, 255, 0);
myGLCD.print("12HR", 201, 80);
myGLCD.setColor(0, 0, 255);
myGLCD.fillRoundRect(255, 76, 295, 96);
setFont(SMALL, 255, 255, 255, 0, 0, 255);
myGLCD.print("24HR", 261, 80);
}
if (setTempScale==0) //Temperature Scale Buttons
{
myGLCD.setColor(0, 255, 0);
myGLCD.fillRoundRect(195, 107, 235, 127);
setFont(SMALL, 0, 0, 0, 0, 255, 0);
myGLCD.print("C", 215, 111);
myGLCD.setColor(0, 0, 255);
myGLCD.fillRoundRect(255, 107, 295, 127);
setFont(SMALL, 255, 255, 255, 0, 0, 255);
myGLCD.print("F", 275, 111);
myGLCD.setColor(0, 0, 0);
myGLCD.drawCircle(210, 113, 1);
myGLCD.setColor(255, 255, 255);
myGLCD.drawCircle(270, 113, 1);
}
else
{
myGLCD.setColor(0, 0, 255);
myGLCD.fillRoundRect(195, 107, 235, 127);
setFont(SMALL, 255, 255, 255, 0, 0, 255);
myGLCD.print("C", 215, 111);
myGLCD.setColor(0, 255, 0);
myGLCD.fillRoundRect(255, 107, 295, 127);
setFont(SMALL, 0, 0, 0, 0, 255, 0);
myGLCD.print("F", 275, 111);
myGLCD.setColor(255, 255, 255);
myGLCD.drawCircle(210, 113, 1);
myGLCD.setColor(0, 0, 0);
myGLCD.drawCircle(270, 113, 1);
}
if (setScreensaver==1) //Screensaver Buttons
{
myGLCD.setColor(0, 255, 0);
myGLCD.fillRoundRect(195, 138, 235, 158);
setFont(SMALL, 0, 0, 0, 0, 255, 0);
myGLCD.print("ON", 209, 142);
myGLCD.setColor(0, 0, 255);
myGLCD.fillRoundRect(255, 138, 295, 158);
setFont(SMALL, 255, 255, 255, 0, 0, 255);
myGLCD.print("OFF", 265, 142);
}
else
{
myGLCD.setColor(0, 0, 255);
myGLCD.fillRoundRect(195, 138, 235, 158);
setFont(SMALL, 255, 255, 255, 0, 0, 255);
myGLCD.print("ON", 209, 142);
myGLCD.setColor(0, 255, 0);
myGLCD.fillRoundRect(255, 138, 295, 158);
setFont(SMALL, 0, 0, 0, 0, 255, 0);
myGLCD.print("OFF", 265, 142);
}
myGLCD.setColor(255, 255, 255);
myGLCD.drawRoundRect(185, 19, 305, 39);
myGLCD.drawRoundRect(185, 45, 305, 65);
for (int x=0; x<2; x++)
{
for (int y=0; y<3; y++)
{
myGLCD.drawRoundRect((x*60)+195, (y*31)+76, (x*60)+235, (y*31)+96);
}
}
}
void co2SetSelect()
{
if (autoco2 == 1) // Auto format bouton
{
myGLCD.setColor(0, 255, 0);
myGLCD.fillRoundRect(220, 19, 305, 39);
setFont(SMALL, 0, 0, 0, 0, 255, 0);
myGLCD.print("Auto", 248, 23);
myGLCD.setColor(0, 0, 255);
myGLCD.fillRoundRect(220, 45, 305, 65);
setFont(SMALL, 255, 255, 255, 0, 0, 255);
myGLCD.print("Manuel", 242, 49);
}
else
{
myGLCD.setColor(0, 0, 255);
myGLCD.fillRoundRect(220, 19, 305, 39);
setFont(SMALL, 255, 255, 255, 0, 0, 255);
myGLCD.print("Auto", 248, 23);
myGLCD.setColor(0, 255, 0);
myGLCD.fillRoundRect(220, 45, 305, 65);
setFont(SMALL, 0, 0, 0, 0, 255, 0);
myGLCD.print("Manuel", 242, 49);
}
if (manuelco2 == 1) // Manuel format bouton
{
myGLCD.setColor(0, 255, 0);
myGLCD.fillRoundRect(220, 94, 305, 114);
setFont(SMALL, 0, 0, 0, 0, 255, 0);
myGLCD.print("ON", 256, 98);
myGLCD.setColor(0, 0, 255);
myGLCD.fillRoundRect(220, 120, 305, 140);
setFont(SMALL, 255, 255, 255, 0, 0, 255);
myGLCD.print("OFF", 252, 124);
}
else
{
myGLCD.setColor(0, 0, 255);
myGLCD.fillRoundRect(220, 94, 305, 114);
setFont(SMALL, 255, 255, 255, 0, 0, 255);
myGLCD.print("ON", 256, 98);
myGLCD.setColor(0, 255, 0);
myGLCD.fillRoundRect(220, 120, 305, 140);
setFont(SMALL, 0, 0, 0, 0, 255, 0);
myGLCD.print("OFF", 252, 124);
}
myGLCD.setColor(255, 255, 255);
myGLCD.drawRoundRect(220, 19, 305, 39);
myGLCD.drawRoundRect(220, 45, 305, 65);
myGLCD.drawRoundRect(220, 94, 305, 114);
myGLCD.drawRoundRect(220, 120, 305, 140);
}

void alimledSetSelect()
{
if (autoalimled == 1) // Auto format bouton
{
myGLCD.setColor(0, 255, 0);
myGLCD.fillRoundRect(220, 19, 305, 39);
setFont(SMALL, 0, 0, 0, 0, 255, 0);
myGLCD.print("Auto", 248, 23);
myGLCD.setColor(0, 0, 255);
myGLCD.fillRoundRect(220, 45, 305, 65);
setFont(SMALL, 255, 255, 255, 0, 0, 255);
myGLCD.print("Manuel", 242, 49);
}
else
{
myGLCD.setColor(0, 0, 255);
myGLCD.fillRoundRect(220, 19, 305, 39);
setFont(SMALL, 255, 255, 255, 0, 0, 255);
myGLCD.print("Auto", 248, 23);
myGLCD.setColor(0, 255, 0);
myGLCD.fillRoundRect(220, 45, 305, 65);
setFont(SMALL, 0, 0, 0, 0, 255, 0);
myGLCD.print("Manuel", 242, 49);
}
if (manuelalimled == 1) // Manuel format bouton
{
myGLCD.setColor(0, 255, 0);
myGLCD.fillRoundRect(220, 94, 305, 114);
setFont(SMALL, 0, 0, 0, 0, 255, 0);
myGLCD.print("ON", 256, 98);
myGLCD.setColor(0, 0, 255);
myGLCD.fillRoundRect(220, 120, 305, 140);
setFont(SMALL, 255, 255, 255, 0, 0, 255);
myGLCD.print("OFF", 252, 124);
}
else
{
myGLCD.setColor(0, 0, 255);
myGLCD.fillRoundRect(220, 94, 305, 114);
setFont(SMALL, 255, 255, 255, 0, 0, 255);
myGLCD.print("ON", 256, 98);
myGLCD.setColor(0, 255, 0);
myGLCD.fillRoundRect(220, 120, 305, 140);
setFont(SMALL, 0, 0, 0, 0, 255, 0);
myGLCD.print("OFF", 252, 124);
}
myGLCD.setColor(255, 255, 255);
myGLCD.drawRoundRect(220, 19, 305, 39);
myGLCD.drawRoundRect(220, 45, 305, 65);
myGLCD.drawRoundRect(220, 94, 305, 114);
myGLCD.drawRoundRect(220, 120, 305, 140);
}
/******************************* END OF MISC. FUNCTIONS *******************************/

/*********************** MAIN SCREEN ********** dispScreen = 0 ************************/
void mainScreen(boolean refreshAll=false)
{
int ledLevel, t;
String oldval, deg;

TimeDateBar(true);

oldval = day; //refresh day if different
day = String(rtc[4]);
if ((oldval!=day) || refreshAll)
{
myGLCD.setColor(64, 64, 64); //Draw Borders & Dividers in Grey
myGLCD.drawRect(0, 0, 319, 239); //Outside Border
myGLCD.drawRect(158, 14, 160, 226); //Vertical Divider
myGLCD.drawRect(160, 87, 319, 89); //Horizontal Divider
myGLCD.fillRect(0, 0, 319, 14); //Top Bar
setFont(SMALL, 255, 255, 0, 64, 64, 64);
myGLCD.print("Allan Aquarium Controler Version 1.0", CENTER, 1);
setFont(SMALL, 255, 255, 255, 0, 0, 0);
myGLCD.print("INTENSITE LED", 35, 20);
myGLCD.print("ETAT RELAYS", 192, 20);
myGLCD.print("TEMPERATURES", 192, 95);
}

drawBarGraph();

if (tempW >= (setTempC+fanTempC))
{
myGLCD.drawBitmap(207, 113, 64, 64, imagehot, 1); //Alarme status temperature
}
if ((tempW > (setTempC-fanTempC)) && (tempW < (setTempC+fanTempC)))
{
myGLCD.drawBitmap(207, 113, 64, 64, imagegreen, 1); //Alarme status temperature
}
if (tempW <= (setTempC-fanTempC))
{
myGLCD.drawBitmap(207, 113, 64, 64, imagecold, 1); //Alarme status temperature
}


if ((white1Led!=white1led_out) || refreshAll) //refresh white led display
{
white1Led = white1led_out;
ledLevel = LedToPercent(white1led_out);
oldval = "Blanc 1: " + String(ledLevel) + "% " + " ";
char bufferW1[13];
oldval.toCharArray(bufferW1, 13);
t= 136 - white1led_out*.39;

myGLCD.setColor(0, 0, 0);
myGLCD.fillRect(40, t, 52, 36); //hide end of last bar
myGLCD.setColor(255, 255, 255);
setFont(SMALL, 255, 255, 255, 0, 0, 0);
myGLCD.print(bufferW1, 40, 159); //display white LEDs % output
myGLCD.drawRect(40, 136, 52, 135); //white %bar place holder
myGLCD.fillRect(40, 136, 52, t); //white percentage bar
}

if ((white2Led!=white2led_out) || refreshAll) //refresh blue led displays
{
white2Led = white2led_out;
ledLevel = LedToPercent(white2led_out);
oldval = "Blanc 2: " + String(ledLevel) + "%" + " ";
char bufferW2[13];
oldval.toCharArray(bufferW2, 13);
t= 136 - white2led_out*.39;

myGLCD.setColor(0, 0, 0);
myGLCD.fillRect(57, t, 69, 36); //hide end of last bar
myGLCD.setColor(255, 255, 255);
setFont(SMALL, 255, 255, 255, 0, 0, 0);
myGLCD.print(bufferW2, 40, 171); //display blue LEDs % output
myGLCD.drawRect(57, 136, 69, 135); //blue %bar place holder
myGLCD.fillRect(57, 136, 69, t); //blue percentage bar
}

if ((redLed!=redled_out) || refreshAll) //refresh red led display
{
redLed = redled_out;
ledLevel = LedToPercent(redled_out);
oldval = "Rouge: " + String(ledLevel) + "% " + " ";
char bufferR[13];
oldval.toCharArray(bufferR, 13);
t= 136 - redled_out*.39;

myGLCD.setColor(0, 0, 0);
myGLCD.fillRect(74, t, 86, 36); //hide end of last bar
myGLCD.setColor(255, 0, 0);
setFont(SMALL, 255, 0, 0, 0, 0, 0);
myGLCD.print(bufferR, 40, 183); //display red LEDs % output
myGLCD.drawRect(74, 136, 86, 135); //red %bar place holder
myGLCD.fillRect(75, 136, 86, t); //red percentage bar
}

if ((royalblueLed!=royalblueled_out) || refreshAll) //refresh royal blue led display
{
royalblueLed = royalblueled_out;
ledLevel = LedToPercent(royalblueled_out);
oldval = "Royal: " + String(ledLevel) + "% " + " ";
char bufferRB[13];
oldval.toCharArray(bufferRB, 13);
t= 136 - royalblueled_out*.39;

myGLCD.setColor(0, 0, 0);
myGLCD.fillRect(91, t, 103, 36); //hide end of last bar
myGLCD.setColor(58, 95, 205);
setFont(SMALL, 58, 95, 205, 0, 0, 0);
myGLCD.print(bufferRB, 40, 195); //display royal blue LEDs % output
myGLCD.drawRect(91, 136, 103, 135); //royal %bar place holder
myGLCD.fillRect(91, 136, 103, t); //royal percentage bar
}

if (setTempScale==1) {
deg ="F";
} //Print deg C or deg F
else {
deg = "C";
}
degC_F=deg;
char bufferDeg[2];
degC_F.toCharArray(bufferDeg,2);

if (refreshAll) //draw static elements
{
setFont(SMALL, 0, 255, 0, 0, 0 , 0);
myGLCD.print("Temp Eau:", 169, 186);
myGLCD.drawCircle(304, 188, 1);
myGLCD.print(bufferDeg, 309, 186);
myGLCD.print("Temp Piece:", 169 , 199);
myGLCD.drawCircle(304, 201, 1);
myGLCD.print(bufferDeg, 309, 199);
}

if ((tempW>50) || (tempW<10)) //range in deg C no matter what
{
setFont(SMALL, 255, 0, 0, 0, 0, 0);
myGLCD.print("Error", 260, 186);
}
else
{
if (setTempScale==1)
{
tempW = ((tempW*1.8) + 32.05);
} //C to F with rounding
setFont(SMALL, 0, 255, 0, 0, 0, 0);
myGLCD.printNumF( tempW, 1, 260, 186); //Sump temperature (No Flags)
if ((tempW<100) && (tempW>=0))
{
myGLCD.setColor(0, 0, 0);
myGLCD.fillRect(292, 186, 300, 198);
}
}
if ((tempP>50) || (tempP<10))
{
setFont(SMALL, 255, 0, 0, 0, 0, 0);
myGLCD.print("Error", 260, 199);
}
else
{
if (setTempScale==1)
{
tempP = ((tempP*1.8) + 32.05);
}
setFont(SMALL, 0, 255, 0, 0, 0, 0);
myGLCD.printNumF( tempP, 1, 260, 199); //Hood temperature (No Flags)
if ((tempP<100) && (tempP>=0))
{
myGLCD.setColor(0, 0, 0);
myGLCD.fillRect(292, 199, 300, 211);
}
}
if (refreshAll)
{
setFont(SMALL, 0, 255, 0, 0, 0, 0);
myGLCD.print("R1:Co2", 169, 35);
myGLCD.print("R2:Chauffe Sol", 169, 47);
myGLCD.print("R3:Alim LED", 169, 59);
myGLCD.print("R4:Ventilateur", 169, 71);
}
if (etat_relay1 == 1)
{
myGLCD.setColor(0, 0, 0);
myGLCD.fillRect(306, 35, 314, 47); //cache OFF après refresh
setFont(SMALL, 0, 255, 0, 0, 0, 0);
myGLCD.print("ON", 290, 35);
}
else
{
setFont(SMALL, 255, 0, 0, 0, 0, 0);
myGLCD.print("OFF", 290, 35);
}
if (etat_relay2 == 1)
{
myGLCD.setColor(0, 0, 0);
myGLCD.fillRect(306, 47, 314, 59); //cache OFF après refresh
setFont(SMALL, 0, 255, 0, 0, 0, 0);
myGLCD.print("ON", 290, 47);
}
else
{
setFont(SMALL, 255, 0, 0, 0, 0, 0);
myGLCD.print("OFF", 290, 47);
}
if (etat_relay3 == 1)
{
myGLCD.setColor(0, 0, 0);
myGLCD.fillRect(306, 59, 314, 71); //cache OFF après refresh
setFont(SMALL, 0, 255, 0, 0, 0, 0);
myGLCD.print("ON", 290, 59);
}
else
{
setFont(SMALL, 255, 0, 0, 0, 0, 0);
myGLCD.print("OFF", 290, 59);
}
if (etat_relay4 == 1)
{
myGLCD.setColor(0, 0, 0);
myGLCD.fillRect(306, 71, 314, 83); //cache OFF après refresh
setFont(SMALL, 0, 255, 0, 0, 0, 0);
myGLCD.print("ON", 290, 71);
}
else
{
setFont(SMALL, 255, 0, 0, 0, 0, 0);
myGLCD.print("OFF", 290, 71);
}

}

void screenReturn() //Auto Return to MainScreen()
{
if (SCREEN_RETURN==true)
{
if (dispScreen!=0)
{
if (myTouch.dataAvailable())
{
processMyTouch();
}
else {
returnTimer++;
}
if (returnTimer>setReturnTimer)
{
returnTimer=0;
LEDtestTick = false;
colorLEDtest = false;
ReadFromEEPROM();
dispScreen=0;
clearScreen();
mainScreen(true);
}
}
}
}
/******************************** END OF MAIN SCREEN **********************************/

/*********************** MENU SCREEN ********** dispScreen = 1 ************************/
void menuScreen()
{
printHeader("Menu Principal");

myGLCD.setColor(64, 64, 64);
myGLCD.drawRect(0, 196, 319, 194);
printButton("ANNULER", canC[0], canC[1], canC[2], canC[3], SMALL);
printButton("Date et Heure", tanD[0], tanD[1], tanD[2], tanD[3]);
printButton("Param Generaux", gSet[0], gSet[1], gSet[2], gSet[3]);
printButton("Param Temp", tempSet[0], tempSet[1], tempSet[2], tempSet[3]);
printButton("Param CO2", co2Set[0], co2Set[1], co2Set[2], co2Set[3]);
printButton("Param Engrais", engraisSet[0], engraisSet[1], engraisSet[2], engraisSet[3]);
printButton("Param Alim LED", alimledSet[0], alimledSet[1], alimledSet[2], alimledSet[3]);
printButton("Tester les LED", tesT[0], tesT[1], tesT[2], tesT[3]);
printButton("Modifier les LED", ledChM[0], ledChM[1], ledChM[2], ledChM[3]);
}
/********************************* END OF MENU SCREEN *********************************/

/************** TIME and DATE SCREEN ********** dispScreen = 2 ************************/
void clockScreen(boolean refreshAll=true)
{
if (refreshAll)
{
for (int i=0; i<7; i++) {
rtcSet[i] = rtc[i];
}

printHeader("Date et Heure");

myGLCD.setColor(64, 64, 64); //Draw Dividers in Grey
myGLCD.drawRect(0, 196, 319, 194); //Bottom Horizontal Divider
myGLCD.drawLine(0, 104, 319, 104); //Middle Horizontal Divider
printButton("RETOUR", back[0], back[1], back[2], back[3], SMALL);
printButton("SAUVER", prSAVE[0], prSAVE[1], prSAVE[2], prSAVE[3], SMALL);
printButton("ANNULER", canC[0], canC[1], canC[2], canC[3], SMALL);

printButton ("+", houU[0], houU[1], houU[2], houU[3], true); //hour up
printButton ("+", minU[0], minU[1], minU[2], minU[3], true); //min up
printButton ("-", houD[0], houD[1], houD[2], houD[3], true); //hour down
printButton ("-", minD[0], minD[1], minD[2], minD[3], true); //min down
if (setTimeFormat==1)
{
printButton ("+", ampmU[0], ampmU[1], ampmU[2], ampmU[3], true); //AM/PM up
printButton ("-", ampmD[0], ampmD[1], ampmD[2], ampmD[3], true);
}//AM/PM down

printButton ("+", monU[0], monU[1], monU[2], monU[3], true); //month up
printButton ("+", dayU[0], dayU[1], dayU[2], dayU[3], true); //day up
printButton ("+", yeaU[0], yeaU[1], yeaU[2], yeaU[3], true); //year up
printButton ("-", monD[0], monD[1], monD[2], monD[3], true); //month down
printButton ("-", dayD[0], dayD[1], dayD[2], dayD[3], true); //day down
printButton ("-", yeaD[0], yeaD[1], yeaD[2], yeaD[3], true); //year down
}

ReadFromEEPROM();
timeDispH=rtcSet[2];
timeDispM=rtcSet[1];
xTimeH=107;
yTime=52;
xColon=xTimeH+42;
xTimeM10=xTimeH+70;
xTimeM1=xTimeH+86;
xTimeAMPM=xTimeH+155;
timeChange();

setFont(LARGE, 255, 255, 255, 0, 0, 0);
myGLCD.print("Date", 20, 142);
myGLCD.print("/", 149, 142);
myGLCD.print("/", 219, 142);
if (setCalendarFormat==0) //DD/MM/YYYY Format
{
setFont(SMALL, 255, 255, 255, 0, 0, 0);
myGLCD.print("(DD/MM/YYYY)", 5, 160);
setFont(LARGE, 255, 255, 255, 0, 0, 0);
if ((rtcSet[4]>=0) && (rtcSet[4]<=9)) //Set DAY
{
myGLCD.print("0", 107, 142);
myGLCD.printNumI(rtcSet[4], 123, 142);
}
else {
myGLCD.printNumI(rtcSet[4], 107, 142);
}
if ((rtcSet[5]>=0) && (rtcSet[5]<=9)) //Set MONTH
{
myGLCD.print("0", 177, 142);
myGLCD.printNumI(rtcSet[5], 193, 142);
}
else {
myGLCD.printNumI(rtcSet[5], 177, 142);
}
}
else
if (setCalendarFormat==1) //MM/DD/YYYY Format
{
setFont(SMALL, 255, 255, 255, 0, 0, 0);
myGLCD.print("(MM/DD/YYYY)", 5, 160);
setFont(LARGE, 255, 255, 255, 0, 0, 0);
if ((rtcSet[5]>=0) && (rtcSet[5]<=9)) //Set MONTH
{
myGLCD.print("0", 107, 142);
myGLCD.printNumI(rtcSet[5], 123, 142);
}
else {
myGLCD.printNumI(rtcSet[5], 107, 142);
}
if ((rtcSet[4]>=0) && (rtcSet[4]<=9)) //Set DAY
{
myGLCD.print("0", 177, 142);
myGLCD.printNumI(rtcSet[4], 193, 142);
}
else {
myGLCD.printNumI(rtcSet[4], 177, 142);
}
}
myGLCD.printNumI(rtcSet[6], 247, 142); //Set YEAR
}

void timeChange()
{
setFont(LARGE, 255, 255, 255, 0, 0, 0);
myGLCD.print("Heure", 20, yTime);

if (setTimeFormat==0) //24HR Format
{
setFont(SMALL, 255, 255, 255, 0, 0, 0);
myGLCD.print("(24HR)", 29, yTime+18);
}

if (setTimeFormat==1) //12HR Format
{
setFont(SMALL, 255, 255, 255, 0, 0, 0);
myGLCD.print("(12HR)", 29, yTime+18);
}

timeCorrectFormat();
}

void timeCorrectFormat()
{
setFont(LARGE, 255, 255, 255, 0, 0, 0);
myGLCD.print(":", xColon, yTime);
if (setTimeFormat==0) //24HR Format
{
setFont(LARGE, 255, 255, 255, 0, 0, 0);
if ((timeDispH>=0) && (timeDispH<=9)) //Set HOUR
{
myGLCD.print("0", xTimeH, yTime);
myGLCD.printNumI(timeDispH, xTimeH+16, yTime);
}
else {
myGLCD.printNumI(timeDispH, xTimeH, yTime);
}
}
if (setTimeFormat==1) //12HR Format
{
setFont(LARGE, 255, 255, 255, 0, 0, 0);
if (timeDispH==0) //Set HOUR
{
myGLCD.print("12", xTimeH, yTime);
}
if ((timeDispH>=1) && (timeDispH<=9))
{
myGLCD.print("0", xTimeH, yTime);
myGLCD.printNumI(timeDispH, xTimeH+16, yTime);
}
if ((timeDispH>=10) && (timeDispH<=12))
{
myGLCD.printNumI(timeDispH, xTimeH, yTime);
}
if ((timeDispH>=13) && (timeDispH<=21))
{
myGLCD.print("0", xTimeH, yTime);
myGLCD.printNumI(timeDispH-12, xTimeH+16, yTime);
}
if (timeDispH>=22)
{
myGLCD.printNumI(timeDispH-12, xTimeH, yTime);
}

if (AM_PM==1)
{
myGLCD.print("AM", xTimeAMPM, yTime);
}
if (AM_PM==2)
{
myGLCD.print("PM", xTimeAMPM, yTime);
}
}
if ((timeDispM>=0) && (timeDispM<=9)) //Set MINUTES
{
myGLCD.print("0", xTimeM10, yTime);
myGLCD.printNumI(timeDispM, xTimeM1, yTime);
}
else {
myGLCD.printNumI(timeDispM, xTimeM10, yTime);
}
}
/**************************** END OF TIME and DATE SCREEN *****************************/

/*********** H2O TEMP CONTROL SCREEN ********** dispScreen = 3 ************************/
void tempScreen(boolean refreshAll=false)
{
String deg;
if (refreshAll)
{
if ((setTempC==0) && (setTempScale==0)) {
setTempC = 26.1;
} //change to 26.1 deg C
if (((setTempF==0) || (setTempF==setTempC)) && (setTempScale==1)) {
setTempF = 79.0;
} //change to 79.0 deg F

if (setTempScale==1) {
temp2beS = setTempF;
temp2beO = fanTempF;
temp2beA = alarmTempF;
}
else {
temp2beS = setTempC;
temp2beO = fanTempC;
temp2beA = alarmTempC;
}

printHeader("Parametres Temperature");

myGLCD.setColor(64, 64, 64); //Draw Dividers in Grey
myGLCD.drawRect(0, 196, 319, 194); //Bottom Horizontal Divider
printButton("RETOUR", back[0], back[1], back[2], back[3], SMALL);
printButton("SAUVER", prSAVE[0], prSAVE[1], prSAVE[2], prSAVE[3], SMALL);
printButton("ANNULER", canC[0], canC[1], canC[2], canC[3], SMALL);

if (setTempScale==1) {
deg ="F";
} //Print deg C or deg F
else {
deg = "C";
}
degC_F=deg;
char bufferDeg[2];
degC_F.toCharArray(bufferDeg,2);

setFont(SMALL, 255, 255, 255, 0, 0, 0);
myGLCD.print("Temperature aquarium", 60, 34);
myGLCD.drawCircle(245, 36, 1);
myGLCD.print(bufferDeg, 250, 34);
myGLCD.print(":",258,34);
myGLCD.print("Ecart ON Ventilateur - OFF Chauffe Sol", CENTER, 84);
myGLCD.print("Ecart ON Alarme:", CENTER, 134);

printButton("-", temM[0], temM[1], temM[2], temM[3], true); //temp minus
printButton("+", temP[0], temP[1], temP[2], temP[3], true); //temp plus
printButton("-", offM[0], offM[1], offM[2], offM[3], true); //offset minus
printButton("+", offP[0], offP[1], offP[2], offP[3], true); //offset plus
printButton("-", almM[0], almM[1], almM[2], almM[3], true); //alarm minus
printButton("+", almP[0], almP[1], almP[2], almP[3], true); //alarm plus
}

setFont(LARGE, 255, 255, 255, 0, 0, 0);
myGLCD.printNumF(temp2beS, 1, CENTER, 54);
myGLCD.printNumF(temp2beO, 1, CENTER, 104);
myGLCD.printNumF(temp2beA, 1, CENTER, 154);
}
/************************** END of H20 TEMP CONTROL SCREEN ****************************/


/********** LED TESTING OPTIONS SCREEN ******** dispScreen = 4 ************************/
void ledTestOptionsScreen()
{
printHeader("Tester led LED");

myGLCD.setColor(64, 64, 64); //Draw Dividers in Grey
myGLCD.drawRect(0, 196, 319, 194); //Bottom Horizontal Divider
printButton("RETOUR", back[0], back[1], back[2], back[3], SMALL);
printButton("ANNULER", canC[0], canC[1], canC[2], canC[3], SMALL);

printButton("Tester la Rampe Entiere", tstLA[0], tstLA[1], tstLA[2], tstLA[3]);
printButton("Tester par Canal", cntIL[0], cntIL[1], cntIL[2], cntIL[3]);
}
/*************************** END OF LED TEST OPTIONS SCREEN ***************************/

/********** TEST LED ARRAY SCREEN ************* dispScreen = 5 ************************/
void testArrayScreen(boolean refreshAll=false)
{
if (refreshAll)
{
printHeader("Tester la rampe entiere");
myGLCD.fillRect (1, 15, 318, 37); //clear "Test in Progress" Banner

myGLCD.setColor(64, 64, 64); //Draw Dividers in Grey
myGLCD.drawRect(0, 196, 319, 194); //Bottom Horizontal Divider
printButton("RETOUR", back[0], back[1], back[2], back[3], SMALL);
printButton("ANNULER", canC[0], canC[1], canC[2], canC[3], SMALL);

printButton ("", stsT[0], stsT[1], stsT[2], stsT[3], true); //start/stop
printButton ("-10s", tenM[0], tenM[1], tenM[2], tenM[3], true); //-10s
printButton ("+10s", tenP[0], tenP[1], tenP[2], tenP[3], true); //+10s
myGLCD.print("START", stsT[0]+6, stsT[1]+15);
myGLCD.print("TEST", stsT[0]+15, stsT[1]+40);
}
else
{
min_cnt=0;

myGLCD.setColor(0, 0, 0);
myGLCD.fillRect (1, 15, 318, 99); //clear test results if any
myGLCD.fillRect (1, 187, 318, 227); //clear the "Back" and "Cancel" Buttons

myGLCD.setColor(0, 0, 255);
myGLCD.fillRect(stsT[0]+5, stsT[1]+5, stsT[2]-5, stsT[3]-40); //clear 'start'
setFont(LARGE, 255, 255, 255, 0, 0, 255);
myGLCD.print("STOP", stsT[0]+15, stsT[1]+15);

myGLCD.setColor(255, 0, 0);
myGLCD.fillRect (1, 15, 318, 37);
myGLCD.drawRoundRect (stsT[0], stsT[1], stsT[2], stsT[3]); //red button during test
setFont(LARGE, 255, 255, 255, 255, 0, 0);
myGLCD.print("Test en cours", CENTER, 16);
setFont(SMALL, 255, 255, 255, 0, 0, 0);
myGLCD.print("HEURE:", 52, 40);
myGLCD.print("LIGHT OUTPUT (0--255):", 140, 40);


while (LEDtestTick) //test LED and speed up time
{
unsigned long currentMillis = millis();

if (myTouch.dataAvailable())
{
processMyTouch();
}

if (currentMillis - previousMillisLED > 500) //change time every 0.5s
{
previousMillisLED = currentMillis;

min_cnt++;
String oldvalue, twelveHR, hrs, HOURS, hrsPM, mins, Minutes, AMPM;
int hours = min_cnt/60;
int minut = min_cnt%60;

if (hours<12){
AMPM="AM";
} //Adding the AM/PM suffix
else{
AMPM="PM";
}

HOURS=hours;
hrsPM=hours-12;
if (hours==0){
hrs=12;
}
else {
if ((hours>=1)&&(hours<=9)){
hrs=" "+HOURS;
} //keep hours in place
else {
if ((hours>=13)&&(hours<=21)){
hrs=" " +hrsPM;
} //convert to 12HR
else {
if ((hours>=22)&&(hours<24)){
hrs=hrsPM;
}
else {
if (hours==24){
hrs=12;
}
else{
hrs=HOURS;
}
}
}
}
}

mins=minut; //add zero to minutes
if ((minut>=0)&&(minut<=9)){
Minutes="0"+mins;
}
else {
Minutes=mins;
}

oldvalue=twelveHR;
twelveHR=hrs+':'+Minutes;
if ((oldvalue!=twelveHR)||refreshAll)
{
char bufferCount[9];
twelveHR.toCharArray(bufferCount,9);
setFont(LARGE, 255, 255, 255, 0, 0, 0);
myGLCD.print(bufferCount, 7, 55);
char bufferAMPM[3];
AMPM.toCharArray(bufferAMPM,3);
myGLCD.print(bufferAMPM, 90, 55);
}

setFont(SMALL, 255, 255, 255, 0, 0, 0);
String white1led = "Blanc1:" + String(white1led_out) + " " + " ";
char bufferW1[11];
white1led.toCharArray(bufferW1, 11);
myGLCD.print(bufferW1, 145, 55);

setFont(SMALL, 255, 255, 255, 0, 0, 0);
String white2led = "Blanc2:" + String(white2led_out) + " " + " ";
char bufferW2[11];
white2led.toCharArray(bufferW2, 11);
myGLCD.print(bufferW2, 145, 67);

setFont(SMALL, 255, 0, 0, 0, 0, 0);
String redled = "Rouge: " + String(redled_out) + " " + " ";
char bufferR[11];
redled.toCharArray(bufferR, 11);
myGLCD.print(bufferR, 235, 55);

setFont(SMALL, 58, 95, 205, 0, 0, 0);
String royalblueled = "Royal: " + String(royalblueled_out) + " " + " ";
char bufferRB[11];
royalblueled.toCharArray(bufferRB, 11);
myGLCD.print(bufferRB, 235, 67);

LED_levels_output();
checkTempC();
CommandeRelay();
TimeDateBar();
}
}
}
}
/*************************** END OF TEST LED ARRAY SCREEN *****************************/

/********* TEST INDIVIDUAL LED(S) SCREEN ****** dispScreen = 6 ************************/
void testIndLedScreen()
{
printHeader("Test Individuel des cannaux (0--255)");

setFont(SMALL, 255, 255, 255, 0, 0, 0);
myGLCD.print("INTENSITE LED", 35, 20);

drawBarGraph();

myGLCD.setColor(255, 255, 255);
myGLCD.fillRoundRect(5, 148, 105, 168);
setFont(SMALL, 0, 0, 0, 255, 255, 255);
myGLCD.print("Blanc1:", 15, 152); //display White LEDs output
myGLCD.print("0", 73, 152);

myGLCD.setColor(255, 255, 255);
myGLCD.fillRoundRect(5, 174, 105, 194);
setFont(SMALL, 0, 0, 0, 255, 255, 255);
myGLCD.print("Blanc2:", 15, 178); //display Blue LEDs output
myGLCD.print("0", 73, 178);

myGLCD.setColor(255, 0, 0);
myGLCD.fillRoundRect(110, 148, 210, 168);
setFont(SMALL, 255, 255, 255, 255, 0, 0);
myGLCD.print("Rouge:", 120, 152); //display Red LEDs output
myGLCD.print("0", 178, 152);

myGLCD.setColor(58, 95, 205);
myGLCD.fillRoundRect(110, 174, 210, 194);
setFont(SMALL, 255, 255, 255, 58, 95, 205);
myGLCD.print("Royal:", 120, 178); //display Royal Blue LEDs output
myGLCD.print("0", 178, 178);

myGLCD.setColor(0, 0, 0);
myGLCD.drawRoundRect(215, 174, 315, 194); //blank button

myGLCD.drawRoundRect(215, 200, 315, 220);
setFont(SMALL, 255, 255, 255, 0, 0, 0);
myGLCD.print("RETOUR", 227, 204); //display "BACK" button

myGLCD.setColor(255, 255, 255); //white Border Around Color Buttons
myGLCD.drawRoundRect(5, 148, 105, 168);
myGLCD.drawRoundRect(5, 174, 105, 194);
myGLCD.setColor(255, 0, 0);
myGLCD.drawRoundRect(110, 148, 210, 168);
myGLCD.setColor(58, 95, 205);
myGLCD.drawRoundRect(110, 174, 210, 194);
printButton("RETOUR", eeprom[0], eeprom[1], eeprom[2], eeprom[3], SMALL);

Rgad=0, Ggad=0, Bgad=0;
Rfont=255, Gfont=255, Bfont=255,
Rback=0, Gback=0, Bback=0,
Rline=255, Gline=255, Bline=255;
COLOR=0;
ledChangerGadget();
myGLCD.setColor(0, 0, 0);
myGLCD.fillRect(204, 50, 280, 74);
setFont(SMALL, 255, 0, 0, 0, 0, 0);
myGLCD.print("SELECT", 220, 50);
myGLCD.print("BELOW", 224, 62);
}
/************************ END OF TEST INDIVIDUAL LED(S) SCREEN ************************/


/******** SHOW LED COLOR CHOICES SCREEN ******* dispScreen = 7 ************************/
void ledColorViewScreen()
{
printHeader("Modifier Intensite des LED");

myGLCD.setColor(64, 64, 64); //Draw Dividers in Grey
myGLCD.drawRect(0, 196, 319, 194); //Bottom Horizontal Divider
printButton("RETOUR", back[0], back[1], back[2], back[3], SMALL);
printButton("ANNULER", canC[0], canC[1], canC[2], canC[3], SMALL);

myGLCD.setColor(255, 255, 255);
myGLCD.fillRoundRect(10, 29, 310, 59);
setFont(SMALL, 0, 0, 0, 255, 255, 255);
printCenter("Blanc 1", 10, 29, 310, 59);

myGLCD.setColor(255, 255, 255);
myGLCD.fillRoundRect(10, 69, 310, 99);
setFont(SMALL, 0, 0, 0, 255, 255, 255);
printCenter("Blanc 2", 10, 69, 310, 99);

myGLCD.setColor(255, 0, 0);
myGLCD.fillRoundRect(10, 109, 310, 139);
setFont(SMALL, 255, 255, 255, 255, 0, 0);
printCenter("Rouge", 10, 109, 310, 139);

myGLCD.setColor(58, 95, 205);
myGLCD.fillRoundRect(10, 149, 310, 179);
setFont(SMALL, 255, 255, 255, 58, 95, 205);
printCenter("Royal Blue", 10, 149, 310, 179);

myGLCD.setColor(255, 255, 255);
for (int x=0; x<2; x++)
{
myGLCD.drawRoundRect(10, (x*40)+29, 310, (x*40)+59);
}
}
/************************** END OF LED COLOR CHOICES SCREEN ***************************/

/****** SHOW LED VALUES FOR CHOICE SCREEN ********** dispScreen = 8 *******************/
void ledValuesScreen()
{
int a;

if (COLOR==1) {
for (int i; i<96; i++)
tled[i] = white1led[i];
printHeader("Tableau des LED Blanc 1");
}
if (COLOR==2) {
for (int i; i<96; i++)
tled[i] = white2led[i];
printHeader("Tableau des LED Blanc 2");
}
if (COLOR==3) {
for (int i; i<96; i++)
tled[i] = redled[i];
printHeader("Tableau des LED Rouge");
}
if (COLOR==4) {
for (int i; i<96; i++)
tled[i] = royalblueled[i];
printHeader("Tableau des LED Royal Blue");
}

if (COLOR!=7) {
setFont(SMALL, 255, 255, 255, 0, 0, 0);
for (int i=0; i<12; i++)
{
myGLCD.setColor(0, 255, 255);
myGLCD.printNumI((i*2), (i*26)+13, 14);
myGLCD.printNumI(((i*2)+1), (i*26)+13, 24);
for (int j=0; j<8; j++)
{
a= (i*8)+j;
myGLCD.setColor(255, 255, 255);
myGLCD.printNumI(tled[a], (i*26)+7, (j*18)+39);
myGLCD.setColor(100, 100, 100);
myGLCD.drawRect((i*26)+4, (j*18)+35, (i*26)+30, (j*18)+53);
}
}
myGLCD.setColor(64, 64, 64);
myGLCD.drawRect(0, 196, 319, 194);
printButton("MORE COLORS", back[0], back[1], back[2], back[3], SMALL);
printButton("CHANGER", ledChV[0], ledChV[1], ledChV[2], ledChV[3], SMALL);
printButton("SAUVER", eeprom[0], eeprom[1], eeprom[2], eeprom[3]);
}
}
/*************************** END OF SHOW LED VALUES SCREEN ****************************/

/********** CHANGE LED VALUES SCREEN ********** dispScreen = 9 ************************/
void ledChangeScreen()
{
if (COLOR==1)
{
printHeader("Changer Intensite des LED Blanc 1");
}
if (COLOR==2)
{
printHeader("Changer Intensite des LED Blanc 2");
}
if (COLOR==3)
{
printHeader("Changer Intensite des LED Rouge");
}
if (COLOR==4)
{
printHeader("Changer Intensite des LED Royal Blue");
}

myGLCD.setColor(64, 64, 64);
myGLCD.drawRect(0, 196, 319, 194);
printButton("MENU", back[0], back[1], back[2], back[3], SMALL);
printButton("OK", prSAVE[0], prSAVE[1], prSAVE[2], prSAVE[3], SMALL);
printButton("ANNULER", canC[0], canC[1], canC[2], canC[3], SMALL);

setFont(SMALL, 0, 255, 255, 0,0,0);
for (int i=0; i<12; i++) {
myGLCD.setColor(0, 255, 255);
myGLCD.printNumI((i*2), (i*26)+10, 22);
myGLCD.printNumI(((i*2)+1), (i*26)+10, 33);
myGLCD.setColor(100, 100, 100);
myGLCD.drawRect((i*26)+4, 20 , (i*26)+30, 45);
}

for (int i=0; i<8; i++)
{
printButton("+", (i*38)+10, 70, (i*38)+35, 95, LARGE);
printButton("-", (i*38)+10, 125, (i*38)+35, 150, LARGE);
}
}
/*************************** END OF CHANGE LED VALUES SCREEN **************************/

/******** GENERAL SETTINGS SCREEN ************* dispScreen = 12 ***********************/
void generalSettingsScreen()
{
printHeader("Parametres generaux");

myGLCD.setColor(64, 64, 64);
myGLCD.drawRect(0, 196, 319, 194);
for (int x=0; x<3; x++)
{
myGLCD.drawLine(0, (x*31)+70, 319, (x*31)+70);
}

printButton("RETOUR", back[0], back[1], back[2], back[3], SMALL);
printButton("SAUVER", prSAVE[0], prSAVE[1], prSAVE[2], prSAVE[3], SMALL);
printButton("ANNULER", canC[0], canC[1], canC[2], canC[3], SMALL);

setFont(SMALL, 0, 255, 0, 0, 0, 0);
myGLCD.print("Format calendrier", 25, 36);
myGLCD.print("Format heure", 25, 80);
myGLCD.print("Unite temperature", 25, 111);
myGLCD.print("Screensaver", 25, 142);

genSetSelect();
}
/*********************** END OF GENERAL SETTINGS SETTINGS SCREEN **********************/

/******** Param CO2 ************* dispScreen = 13 ***********************/
void co2SettingsScreen()
{
printHeader("Parametres CO2");

myGLCD.setColor(64, 64, 64);
myGLCD.drawRect(0, 196, 319, 194);

printButton("RETOUR", back[0], back[1], back[2], back[3], SMALL);
printButton("SAUVER", prSAVE[0], prSAVE[1], prSAVE[2], prSAVE[3], SMALL);
printButton("ANNULER", canC[0], canC[1], canC[2], canC[3], SMALL);

setFont(SMALL, 0, 255, 0, 0, 0, 0);
myGLCD.print("Controle du CO2:", 20, 36);
myGLCD.print("Arret / allumage manuel:", 20, 111);

co2SetSelect();
}
/*********************** END OF GENERAL SETTINGS SETTINGS SCREEN **********************/

/******** Param alim led ************* dispScreen = 10 ***********************/
void alimledSettingsScreen()
{
printHeader("Parametres Alimentation LED");

myGLCD.setColor(64, 64, 64);
myGLCD.drawRect(0, 196, 319, 194);

printButton("RETOUR", back[0], back[1], back[2], back[3], SMALL);
printButton("SAUVER", prSAVE[0], prSAVE[1], prSAVE[2], prSAVE[3], SMALL);
printButton("ANNULER", canC[0], canC[1], canC[2], canC[3], SMALL);

setFont(SMALL, 0, 255, 0, 0, 0, 0);
myGLCD.print("Controle Alim LED:", 20, 36);
myGLCD.print("Arret / allumage manuel:", 20, 111);

alimledSetSelect();
}
/*********************** END OF GENERAL SETTINGS SETTINGS SCREEN **********************/






/************************************ TOUCH SCREEN ************************************/
void processMyTouch()
{
myTouch.read();
x=myTouch.getX();
y=myTouch.getY();

returnTimer=0;
screenSaverTimer=0;

if ((x>=canC[0]) && (x<=canC[2]) && (y>=canC[1]) && (y<=canC[3]) //press cancel
&& (dispScreen!=0) && (dispScreen!=5) && (dispScreen!=6) && (dispScreen!=8)
&& (dispScreen!=11))
{
waitForIt(canC[0], canC[1], canC[2], canC[3]);
LEDtestTick = false;
ReadFromEEPROM();
dispScreen=0;
clearScreen();
mainScreen(true);
}
else
if ((x>=back[0]) && (x<=back[2]) && (y>=back[1]) && (y<=back[3]) //press back
&& (dispScreen!=0) && (dispScreen!=1) && (dispScreen!=5) && (dispScreen!=6)
&& (dispScreen!=8) && (dispScreen!=11) && (dispScreen!=14))
{
waitForIt(back[0], back[1], back[2], back[3]);
LEDtestTick = false;
ReadFromEEPROM();
dispScreen=1;
clearScreen();
menuScreen();
}
else
{
switch (dispScreen) {
case 0: //--------------- MAIN SCREEN (Press Any Key) ---------------
dispScreen=1;
clearScreen();
menuScreen();
break;

case 1: //--------------------- MENU SCREEN -------------------------
if ((x>=tanD[0]) && (x<=tanD[2])) //first column
{
if ((y>=tanD[1]) && (y<=tanD[3])) //press Date & Clock Screen
{
waitForIt(tanD[0], tanD[1], tanD[2], tanD[3]);
if ((timeDispH>=0) && (timeDispH<=11)) {
AM_PM=1;
}
else {
AM_PM=2;
}
dispScreen=2;
clearScreen();
clockScreen();
}
if ((y>=gSet[1]) && (y<=gSet[3])) //press General Settings
{
waitForIt(gSet[0], gSet[1], gSet[2], gSet[3]);
dispScreen=12;
clearScreen();
generalSettingsScreen();
}
if ((y>=tempSet[1]) && (y<=tempSet[3])) //press parametre temperature
{
waitForIt(tempSet[0], tempSet[1], tempSet[2], tempSet[3]);
ReadFromEEPROM();
dispScreen=3;
clearScreen();
tempScreen(true);
}
if ((y>=co2Set[1]) && (y<=co2Set[3])) //press parametre CO2
{
waitForIt(co2Set[0], co2Set[1], co2Set[2], co2Set[3]);
ReadFromEEPROM();
dispScreen=13;
clearScreen();
co2SettingsScreen();
}
}
if ((x>=tesT[0]) && (x<=tesT[2])) //second column
{
if ((y>=tesT[1]) && (y<=tesT[3])) //press LED Testing page
{
waitForIt(tesT[0], tesT[1], tesT[2], tesT[3]);
dispScreen=4;
clearScreen();
ledTestOptionsScreen();
}
if ((y>=engraisSet[1]) && (y<=engraisSet[3])) //press Parametre engrais
{
waitForIt(engraisSet[0], engraisSet[1], engraisSet[2], engraisSet[3]);
dispScreen=4;
clearScreen();
}
if ((y>=alimledSet[1]) && (y<=alimledSet[3])) //press Parametre alimled
{
waitForIt(alimledSet[0], alimledSet[1], alimledSet[2], alimledSet[3]);
dispScreen=10;
clearScreen();
alimledSettingsScreen();
}
if ((y>=ledChM[1]) && (y<=ledChM[3])) //press Change LED values
{
waitForIt(ledChM[0], ledChM[1], ledChM[2], ledChM[3]);
dispScreen=7;
clearScreen();
ledColorViewScreen();
}
}
break;

case 2: //--------------- CLOCK & DATE SETUP SCREEN -----------------
if ((x>=prSAVE[0]) && (x<=prSAVE[2]) && (y>=prSAVE[1]) && (y<=prSAVE[3])) //press SAVE
{
waitForIt(prSAVE[0], prSAVE[1], prSAVE[2], prSAVE[3]);
if (setTimeFormat==1)
{
if ((rtcSet[2]==0) && (AM_PM==2))
{
rtcSet[2]+=12;
}
if (((rtcSet[2]>=1) && (rtcSet[2]<=11)) && (AM_PM==2))
{
rtcSet[2]+=12;
}
if (((rtcSet[2]>=12) && (rtcSet[2]<=23)) && (AM_PM==1))
{
rtcSet[2]-=12;
}
}
SaveRTC();
dispScreen=0;
clearScreen();
mainScreen(true);
}
else
{
if ((y>=houU[1]) && (y<=houU[3])) //FIRST ROW
{
if ((x>=houU[0]) && (x<=houU[2])) //press hour up
{
waitForIt(houU[0], houU[1], houU[2], houU[3]);
rtcSet[2]++;
if (rtcSet[2]>=24)
{
rtcSet[2]=0;
}
}
if ((x>=minU[0]) && (x<=minU[2])) //press min up
{
waitForIt(minU[0], minU[1], minU[2], minU[3]);
rtcSet[1]++;
if (rtcSet[1]>=60) {
rtcSet[1] = 0;
}
}
if ((x>=ampmU[0]) && (x<=ampmU[2])) //press AMPM up
{
waitForIt(ampmU[0], ampmU[1], ampmU[2], ampmU[3]);
if (AM_PM==1) {
AM_PM=2;
}
else {
AM_PM=1;
}
}
}
if ((y>=houD[1]) && (y<=houD[3])) //SECOND ROW
{
if ((x>=houD[0]) && (x<=houD[2])) //press hour down
{
waitForIt(houD[0], houD[1], houD[2], houD[3]);
rtcSet[2]--;
if (rtcSet[2]<0)
{
rtcSet[2]=23;
}
}
if ((x>=minD[0]) && (x<=minD[2])) //press min down
{
waitForIt(minD[0], minD[1], minD[2], minD[3]);
rtcSet[1]--;
if (rtcSet[1]<0) {
rtcSet[1] = 59;
}
}
if ((x>=ampmD[0]) && (x<=ampmD[2])) //press AMPM down
{
waitForIt(ampmD[0], ampmD[1], ampmD[2], ampmD[3]);
if (AM_PM==1) {
AM_PM=2;
}
else {
AM_PM=1;
}
}
}
if ((y>=dayU[1]) && (y<=dayU[3])) //THIRD ROW
{
if (setCalendarFormat==0) //DD/MM/YYYY Format
{
if ((x>=dayU[0]) && (x<=dayU[2])) //press day up
{
waitForIt(dayU[0], dayU[1], dayU[2], dayU[3]);
rtcSet[4]++;
if (rtcSet[4]>31) {
rtcSet[4] = 1;
}
}
if ((x>=monU[0]) && (x<=monU[2])) //press month up
{
waitForIt(monU[0], monU[1], monU[2], monU[3]);
rtcSet[5]++;
if (rtcSet[5]>12) {
rtcSet[5] = 1;
}
}
}
else {
if (setCalendarFormat==1) //MM/DD/YYYY Format
{
if ((x>=dayU[0]) && (x<=dayU[2])) //press month up
{
waitForIt(dayU[0], dayU[1], dayU[2], dayU[3]);
rtcSet[5]++;
if (rtcSet[5]>12) {
rtcSet[5] = 1;
}
}
if ((x>=monU[0]) && (x<=monU[2])) //press day up
{
waitForIt(monU[0], monU[1], monU[2], monU[3]);
rtcSet[4]++;
if (rtcSet[4]>31) {
rtcSet[4] = 1;
}
}
}
}
if ((x>=yeaU[0]) && (x<=yeaU[2])) //press year up
{
waitForIt(yeaU[0], yeaU[1], yeaU[2], yeaU[3]);
rtcSet[6]++;
if (rtcSet[6]>2100) {
rtcSet[6] = 2000;
}
}
}
if ((y>=dayD[1]) && (y<=dayD[3])) //FOURTH ROW
{
if (setCalendarFormat==0) //DD/MM/YYYY Format
{
if ((x>=dayD[0]) && (x<=dayD[2])) //press day down
{
waitForIt(dayD[0], dayD[1], dayD[2], dayD[3]);
rtcSet[4]--;
if (rtcSet[4]<1) {
rtcSet[4] = 31;
}
}
if ((x>=monD[0]) && (x<=monD[2])) //press month down
{
waitForIt(monD[0], monD[1], monD[2], monD[3]);
rtcSet[5]--;
if (rtcSet[5]<1) {
rtcSet[5] = 12;
}
}
}
else {
if (setCalendarFormat==1) //MM/DD/YYYY Format
{
if ((x>=dayD[0]) && (x<=dayD[2])) //press month down
{
waitForIt(dayD[0], dayD[1], dayD[2], dayD[3]);
rtcSet[5]--;
if (rtcSet[5]<1) {
rtcSet[5] = 12;
}
}
if ((x>=monD[0]) && (x<=monD[2])) //press day down
{
waitForIt(monD[0], monD[1], monD[2], monD[3]);
rtcSet[4]--;
if (rtcSet[4]<1) {
rtcSet[4] = 31;
}
}
}
}
if ((x>=yeaD[0]) && (x<=yeaD[2])) //press year down
{
waitForIt(yeaD[0], yeaD[1], yeaD[2], yeaD[3]);
rtcSet[6]--;
if (rtcSet[6]<2000) {
rtcSet[6] = 2100;
}
}
}
clockScreen(false);
}
break;

case 3: //------------------ H20 TEMPERATURE CONTROL ---------------
if ((x>=prSAVE[0]) && (x<=prSAVE[2]) && (y>=prSAVE[1]) && (y<=prSAVE[3])) //press SAVE
{
waitForIt(prSAVE[0], prSAVE[1], prSAVE[2], prSAVE[3]);
setTempC = temp2beS;
setTempF = temp2beS;
fanTempC = temp2beO;
fanTempF = temp2beO;
alarmTempC = temp2beA;
alarmTempF = temp2beA;
if (setTempScale==0) //Celsius to Farenheit (Consistency Conversion)
{
setTempF=((1.8*setTempC)+32.05);
fanTempF=((1.8*fanTempC)+0.05);
alarmTempF=((1.8*alarmTempC)+0.05);
}
if (setTempScale==1) //Farenheit to Celsius (Consistency Conversion)
{
setTempC=((.55556*(setTempF-32))+.05);
fanTempC=(.55556)*fanTempF+.05;
alarmTempC=(.55556)*alarmTempF+.05;
}
dispScreen=0;
SaveTempToEEPROM();
clearScreen();
mainScreen(true);
}
else
setFont(LARGE, 255, 255, 255, 0, 0, 0);
{
if ((x>=temM[0]) && (x<=temM[2])) //first column
{
if ((y>=temM[1]) && (y<=temM[3])) //press temp minus
{
waitForIt(temM[0], temM[1], temM[2], temM[3]);
temp2beS -= 0.1;
if ((setTempScale==1) && (temp2beS <= 50)){
temp2beS = 50;
}
if ((setTempScale==0) && (temp2beS <= 10)) {
temp2beS = 10;
}
tempScreen();
}
if ((y>=offM[1]) && (y<=offM[3])) //press offset minus
{
waitForIt(offM[0], offM[1], offM[2], offM[3]);
temp2beO -= 0.1;
if (temp2beO < 0.1) {
temp2beO = 0.0;
}
tempScreen();
}
if ((y>=almM[1]) && (y<=almM[3])) //press alarm minus
{
waitForIt(almM[0], almM[1], almM[2], almM[3]);
temp2beA -= 0.1;
if (temp2beA < 0.1) {
temp2beA = 0.0;
}
tempScreen();
}
}
if ((x>=temP[0]) && (x<=temP[2])) //second column
{
if ((y>=temP[1]) && (y<=temP[3])) //press temp plus
{
waitForIt(temP[0], temP[1], temP[2], temP[3]);
temp2beS += 0.1;
if ((setTempScale==1) && (temp2beS >= 104)){
temp2beS = 104;
}
if ((setTempScale==0) && (temp2beS >= 40)) {
temp2beS = 40;
}
tempScreen();
}
if ((y>=offP[1]) && (y<=offP[3])) //press offset plus
{
waitForIt(offP[0], offP[1], offP[2], offP[3]);
temp2beO += 0.1;
if (temp2beO >= 10) {
temp2beO = 9.9;
}
tempScreen();
}
if ((y>=almP[1]) && (y<=almP[3])) //press alarm plus
{
waitForIt(almP[0], almP[1], almP[2], almP[3]);
temp2beA += 0.1;
if (temp2beA >= 10) {
temp2beA = 9.9;
}
tempScreen();
}
}
}
break;

case 4: // -------------- LED TEST OPTIONS SCREEN -----------------
if ((x>=tstLA[0]) && (x<=tstLA[2]) && (y>=tstLA[1]) && (y<=tstLA[3])) //Test LED Array Output
{
waitForIt(tstLA[0], tstLA[1], tstLA[2], tstLA[3]);
dispScreen=5;
clearScreen();
testArrayScreen(true);
}
if ((x>=cntIL[0]) && (x<=cntIL[2]) && (y>=cntIL[1]) && (y<=cntIL[3])) //Test Individual LEDs
{
waitForIt(cntIL[0], cntIL[1], cntIL[2], cntIL[3]);
dispScreen=6;
clearScreen();
testIndLedScreen();
colorLEDtest = true;
}
break;

case 5: //---------------- TEST LED ARRAY SCREEN ------------------
if ((x>=back[0]) && (x<=back[2]) && (y>=back[1]) && (y<=back[3]) //press back
&& (LEDtestTick==false))
{
waitForIt(back[0], back[1], back[2], back[3]);
LEDtestTick = false;
ReadFromEEPROM();
dispScreen=4;
clearScreen();
ledTestOptionsScreen();
}
if ((x>=canC[0]) && (x<=canC[2]) && (y>=canC[1]) && (y<=canC[3]) //press CANCEL
&& (LEDtestTick==false))
{
waitForIt(canC[0], canC[1], canC[2], canC[3]);
LEDtestTick = false;
ReadFromEEPROM();
dispScreen=0;
clearScreen();
mainScreen(true);
}
if ((x>=stsT[0]) && (x<=stsT[2]) && (y>=stsT[1]) && (y<=stsT[3])) //press start/stop test
{
waitForIt(stsT[0], stsT[1], stsT[2], stsT[3]);

if (LEDtestTick) {
LEDtestTick = false;
testArrayScreen(true);
}
else {
LEDtestTick = true;
testArrayScreen();
}
}
else
{
if ((x>=tenM[0]) && (x<=tenM[2]) && (y>=tenM[1]) && (y<=tenM[3])) //press -10s
{
min_cnt -= 10;
if (min_cnt<0) {
min_cnt= 0;
}
delay(50);
}
if ((x>=tenP[0]) && (x<=tenP[2]) && (y>=tenP[1]) && (y<=tenP[2])) //press +10s
{
min_cnt += 10;
if (min_cnt>1440) {
min_cnt = 1440;
}
delay(50);
}
}
break;

case 6: // --------------- TEST INDIVIDUAL LED SCREEN --------------
int CL_check, CL_check2;
if ((x>=215) && (x<=315) && (y>=200) && (y<=220)) //press back
{
waitForIt(215, 200, 315, 220);
LEDtestTick = false;
dispScreen=4;
clearScreen();
ledTestOptionsScreen();
colorLEDtest = false;
}
if ((x>=5) && (x<=105)) //First Column of Colors
{
if ((y>=148) && (y<=168)) //Press SUMP
{
waitForIt(5, 148, 105, 168);
COLOR = 1;
Rgad=255, Ggad=255, Bgad=255,
Rfont=0, Gfont=0, Bfont=0,
Rback=255, Gback=255, Bback=255,
Rline=0, Gline=0, Bline=0;
xValue=73 , yValue=152, x1Bar=40, x2Bar=x1Bar+12;
COLOR = WHITE1;
ledChangerGadget();
}
if ((y>=174) && (y<=194)) //press Red
{
waitForIt(5, 174, 105, 194);
COLOR = 2;
Rgad=255, Ggad=255, Bgad=255,
Rfont=0, Gfont=0, Bfont=0,
Rback=255, Gback=255, Bback=255,
Rline=0, Gline=0, Bline=0;
xValue=73 , yValue=178, x1Bar=57, x2Bar=x1Bar+12;
COLOR = WHITE2;
ledChangerGadget();
}
}

if ((x>=110) && (x<=210)) //Second Column of Colors
{
if ((y>=148) && (y<=168)) //press White
{
waitForIt(110, 148, 210, 168);
COLOR = 3;
Rgad=255, Ggad=0, Bgad=0,
Rfont=255, Gfont=255, Bfont=255,
Rback=255, Gback=0, Bback=0,
Rline=0, Gline=0, Bline=0;
xValue=178 , yValue=152, x1Bar=74, x2Bar=x1Bar+12;
COLOR = RED;
ledChangerGadget();
}
if ((y>=174) && (y<=194)) //Press Blue255, 255, 255, 58, 95, 205
{
waitForIt(110, 174, 210, 194);
COLOR = 4;
Rgad=58, Ggad=95, Bgad=205,
Rfont=255, Gfont=255, Bfont=255,
Rback=58, Gback=95, Bback=205,
Rline=0, Gline=0, Bline=0;
xValue=178 , yValue=178, x1Bar=91, x2Bar=x1Bar+12;
COLOR = ROYAL;
ledChangerGadget();
}
}
if ((y>=78) && (y<=100)) //Plus buttons were touched
{
for (int i=0; i<3; i++) {
if ((x>=(i*27)+204) && (x<=(i*27)+226) && (COLOR!=0)) {
waitForItSq((i*27)+204, 78, (i*27)+226, 100);
if (i==0) {
cl_100 += 1;
}
if (i==1) {
cl_10 += 1;
}
if (i==2) {
cl_1 += 1;
}
CL_check = (cl_10*10)+cl_1;
if ((cl_100>2) && (i==0)) {
cl_100=0;
}
if ((cl_100<2) && (cl_10>9) && (i==1)) {
cl_10=0;
}
if ((cl_100>=2) && (cl_10>=5) && (i==1)) {
cl_10=5;
}
if ((cl_100<2) && (cl_1>9) && (i==2)) {
cl_1=0;
}
if ((cl_100>=2) && (cl_10<5) && (cl_1>9) && (i==2)) {
cl_1=0;
}
if ((cl_100>=2) && (cl_10>=5) && (i==2)) {
cl_1=5;
}
if ((CL_check>=55) && ((i==0) || (i==1)) && (cl_100>1))
{
cl_10=5;
cl_1=5;
setFont(LARGE, 255, 255, 255, 0, 0, 0);
myGLCD.printNumI(cl_10, 234, 54);
myGLCD.printNumI(cl_1, 255, 54);
}
setFont(LARGE, 255, 255, 255, 0, 0, 0);
if (i==0) {
myGLCD.printNumI(cl_100, 214, 54);
}
if (i==1) {
myGLCD.printNumI(cl_10, 234, 54);
}
if (i==2) {
myGLCD.printNumI(cl_1, 255, 54);
}
CL_100 = cl_100*100;
CL_10 = cl_10*10;
CL_1 = cl_1;
for (int c=0; c<3; c++)
{
myGLCD.setColor(Rline, Gline, Bline);
myGLCD.drawRect((c*27)+204, 78, (c*27)+226, 100);
myGLCD.drawRect((c*27)+204, 105, (c*27)+226, 127);
}
drawBarandColorValue();
LED_levels_output();
checkTempC();
CommandeRelay();
}
}
}
else if ((y>=105) && (y<=127)) //Minus buttons were touched
{
for (int i=0; i<3; i++) {
if ((x>=(i*27)+204) && (x<=(i*27)+226) && (COLOR!=0)) {
waitForItSq((i*27)+204, 105, (i*27)+226, 127);
if (i==0) {
cl_100 -= 1;
}
if (i==1) {
cl_10 -= 1;
}
if (i==2) {
cl_1 -= 1;
}
CL_check = (cl_10*10)+cl_1;
CL_check2 = (cl_100*100)+cl_1;
if ((cl_100<0) && (i==0)) {
cl_100=2;
}
if ((cl_10<0) && (i==1) && (cl_100<2)) {
cl_10=9;
}
if ((cl_10<0) && (i==1) && (cl_100==2)) {
cl_10=5;
}
if ((cl_1<0) && (i==2) && (cl_100<2)) {
cl_1=9;
}
if ((cl_1<0) && (i==2) && (cl_100==2) && (cl_10<5)) {
cl_1=9;
}
if ((cl_1<0) && (i==2) && (cl_100==2) && (cl_10>=5)) {
cl_1=5;
}
if ((CL_check>=55) && ((i==0) || (i==1)) && (cl_100>1))
{
cl_10=5;
cl_1=5;
setFont(LARGE, 255, 255, 255, 0, 0, 0);
myGLCD.printNumI(cl_10, 234, 54);
myGLCD.printNumI(cl_1, 255, 54);
}
if ((CL_check2>205) && (i==1) && (cl_10<1))
{
cl_10=5;
cl_1=5;
setFont(LARGE, 255, 255, 255, 0, 0, 0);
myGLCD.printNumI(cl_10, 234, 54);
myGLCD.printNumI(cl_1, 255, 54);
}
setFont(LARGE, 255, 255, 255, 0, 0, 0);
if (i==0) {
myGLCD.printNumI(cl_100, 214, 54);
}
if (i==1) {
myGLCD.printNumI(cl_10, 234, 54);
}
if (i==2) {
myGLCD.printNumI(cl_1, 255, 54);
}
CL_100 = cl_100*100;
CL_10 = cl_10*10;
CL_1 = cl_1;
for (int c=0; c<3; c++)
{
myGLCD.setColor(Rline, Gline, Bline);
myGLCD.drawRect((c*27)+204, 78, (c*27)+226, 100);
myGLCD.drawRect((c*27)+204, 105, (c*27)+226, 127);
}
drawBarandColorValue();
LED_levels_output();
checkTempC();
CommandeRelay();
}
}
}
break;

case 7: // ----------- VIEW INDIVIDUAL LED COLORS SCREEN -----------

if ((x>=10) && (x<=310) && (y>=29) && (y<=59)) //View White LEDs Array
{
waitForIt(10, 29, 310, 59);
dispScreen=8;
COLOR = WHITE1;
clearScreen();
ledValuesScreen();
}
if ((x>=10) && (x<=310) && (y>=69) && (y<=99)) //View Royal Blue LEDs Array
{
waitForIt(10, 69, 310, 99);
dispScreen=8;
COLOR = WHITE2;
clearScreen();
ledValuesScreen();
}
if ((x>=10) && (x<=310) && (y>=109) && (y<=139)) //View Red LEDs Array
{
waitForIt(10, 109, 310, 139);
dispScreen=8;
COLOR = RED;
clearScreen();
ledValuesScreen();
}
if ((x>=10) && (x<=310) && (y>=149) && (y<=179)) //View Red LEDs Array
{
waitForIt(10, 149, 310, 179);
dispScreen=8;
COLOR = ROYAL;
clearScreen();
ledValuesScreen();
}
break;

case 8: // -------------- SHOW LED VALUES TABLE SCREEN -------------
if ((x>=back[0]) && (x<=back[2]) && (y>=back[1]) && (y<=back[3])) //press MORE COLORS
{
waitForIt(back[0], back[1], back[2], back[3]);
ReadFromEEPROM();
dispScreen=7;
clearScreen();
ledColorViewScreen();
}
else
if ((x>=ledChV[0]) && (x<=ledChV[2]) && (y>=ledChV[1]) && (y<=ledChV[3]) && (COLOR!=7)) //press CHANGE
{
waitForIt(ledChV[0], ledChV[1], ledChV[2], ledChV[3]);
ReadFromEEPROM();
dispScreen=9;
clearScreen();
ledChangeScreen();
}
else
if ((x>=eeprom[0]) && (x<=eeprom[2]) && (y>=eeprom[1]) && (y<=eeprom[3]) && (COLOR!=7)) //press SAVE
{
waitForIt(eeprom[0], eeprom[1], eeprom[2], eeprom[3]);
SaveLEDToEEPROM();
dispScreen=7;
clearScreen();
ledColorViewScreen();
}
break;

case 9: //---------------- CHANGE LED VALUES SCREEN ---------------
if ((x>=prSAVE[0]) && (x<=prSAVE[2]) && (y>=prSAVE[1]) && (y<=prSAVE[3])) //press SAVE
{
waitForIt(prSAVE[0], prSAVE[1], prSAVE[2], prSAVE[3]);
dispScreen=8;
if (COLOR==1) {
for (int i; i<96; i++) {
white1led[i]=tled[i];
}
}
if (COLOR==2) {
for (int i; i<96; i++) {
white2led[i]=tled[i];
}
}
if (COLOR==3) {
for (int i; i<96; i++) {
redled[i]=tled[i];
}
}
if (COLOR==4) {
for (int i; i<96; i++) {
royalblueled[i]=tled[i];
}
}
clearScreen();
ledValuesScreen();
}

else if ((y>=15) && (y<=40)) //top row with times was touched
{
if ((x>=4) && (x<=316))
{
int oldLCT = LedChangTime;
LedChangTime = map(x, 3, 320, 0, 12);

if (oldLCT != LedChangTime) //highlight touched time
{
myGLCD.setColor(0, 0, 0);
myGLCD.fillRect((oldLCT*26)+5, 21, (oldLCT*26)+29, 45);
setFont(SMALL, 0, 255, 255, 0, 0, 0);
myGLCD.printNumI((oldLCT*2), (oldLCT*26)+10, 22);
myGLCD.printNumI(((oldLCT*2)+1), (oldLCT*26)+10, 33);
myGLCD.setColor(255, 0, 0);
myGLCD.fillRect((LedChangTime*26)+5, 21, (LedChangTime*26)+29, 45);
setFont(SMALL, 255, 255, 255, 255, 0, 0);
myGLCD.printNumI((LedChangTime*2), (LedChangTime*26)+10, 22);
myGLCD.printNumI(((LedChangTime*2)+1), (LedChangTime*26)+10, 33);

for (int i=0; i<8; i++) //print led values for highlighted time
{
int k=(LedChangTime*8)+i;
setFont(SMALL, 255, 255, 255, 0, 0, 0);
myGLCD.print( " ", (i*38)+12, 105);
myGLCD.printNumI( tled[k], (i*38)+12, 105);
}
}
}
}
else if ((y>=70) && (y<=95)) //plus buttons were touched
{
for (int i=0; i<8; i++) {
if ((x>=(i*38)+10) && (x<=(i*38)+35)) {
int k= (LedChangTime*8)+i;
tled[k]++;
if (tled[k]>255) {
tled[k]=255;
}
myGLCD.printNumI( tled[k], (i*38)+12, 105);
}
}
}
else if ((y>=125) && (y<=150)) //minus buttons were touched
{
for (int i=0; i<8; i++) {
if ((x>=(i*38)+10) && (x<=(i*38)+35)) {
int k= (LedChangTime*8)+i;
tled[k]--;
if (tled[k]<0) {
tled[k]=0;
}
myGLCD.print( " ", (i*38)+20, 105);
myGLCD.printNumI( tled[k], (i*38)+12, 105);
}
}
}
break;
case 10: //-------------------- parametre alimled -------------------
if ((x>=prSAVE[0]) && (x<=prSAVE[2]) && (y>=prSAVE[1]) && (y<=prSAVE[3])) //press SAVE
{
waitForIt(prSAVE[0], prSAVE[1], prSAVE[2], prSAVE[3]);
SavealimledSetsToEEPROM();
dispScreen=1;
clearScreen();
menuScreen();
}
if ((x>=220) && (x<=305) && (y>=19) && (y<=39)) //press DD/MM/YYYY Button
{
waitForIt(220, 19, 305, 39);
autoalimled = 1;
alimledSetSelect();
}

if ((x>=220) && (x<=305) && (y>=45) && (y<=65)) //press Month DD, YYYY Button
{
waitForIt(220, 45, 305, 65);
autoalimled = 0;
alimledSetSelect();
}
if ((x>=220) && (x<=305) && (y>=94) && (y<=114)) //press Month DD, YYYY Button
{
waitForIt(220, 94, 305, 114);
manuelalimled = 1;
alimledSetSelect();
}
if ((x>=220) && (x<=305) && (y>=120) && (y<=140)) //press Month DD, YYYY Button
{
waitForIt(220, 120, 305, 140);
manuelalimled = 0;
alimledSetSelect();
}
break;
case 12: //-------------------- GENERAL SETTINGS -------------------
if ((x>=prSAVE[0]) && (x<=prSAVE[2]) && (y>=prSAVE[1]) && (y<=prSAVE[3])) //press SAVE
{
waitForIt(prSAVE[0], prSAVE[1], prSAVE[2], prSAVE[3]);
SaveGenSetsToEEPROM();
dispScreen=1;
clearScreen();
menuScreen();
}
if ((x>=185) && (x<=305) && (y>=19) && (y<=39)) //press DD/MM/YYYY Button
{
waitForIt(185, 19, 305, 39);
setCalendarFormat = 0;
genSetSelect();
}

if ((x>=185) && (x<=305) && (y>=45) && (y<=65)) //press Month DD, YYYY Button
{
waitForIt(185, 45, 305, 65);
setCalendarFormat = 1;
genSetSelect();
}

if ((x>=195) && (x<=235)) //first column
{
if ((y>=76) && (y<=96)) //press 12HR Button
{
waitForIt(195, 76, 235, 96);
setTimeFormat = 1;
genSetSelect();
}
if ((y>=107) && (y<=127)) //press deg C
{
waitForIt(195, 107, 235, 127);
setTempScale = 0;
genSetSelect();
}
if ((y>=138) && (y<=158)) //press Screensaver ON
{
waitForIt(195, 138, 235, 158);
setScreensaver = 1;
genSetSelect();
}
}
if ((x>=255) && (x<=295)) //second column
{
if ((y>=76) && (y<=96)) //press 24HR Button
{
waitForIt(255, 76, 295, 96);
setTimeFormat = 0;
genSetSelect();
}
if ((y>=107) && (y<=127)) //press deg F
{
waitForIt(255, 107, 295, 127);
setTempScale = 1;
genSetSelect();
}
if ((y>=138) && (y<=158)) //press Screensaver OFF
{
waitForIt(255, 138, 295, 158);
setScreensaver = 2;
genSetSelect();
}
}

break;
case 13: //-------------------- parametre CO2 -------------------
if ((x>=prSAVE[0]) && (x<=prSAVE[2]) && (y>=prSAVE[1]) && (y<=prSAVE[3])) //press SAVE
{
waitForIt(prSAVE[0], prSAVE[1], prSAVE[2], prSAVE[3]);
Saveco2SetsToEEPROM();
dispScreen=1;
clearScreen();
menuScreen();
}
if ((x>=220) && (x<=305) && (y>=19) && (y<=39)) //press DD/MM/YYYY Button
{
waitForIt(220, 19, 305, 39);
autoco2 = 1;
co2SetSelect();
}

if ((x>=220) && (x<=305) && (y>=45) && (y<=65)) //press Month DD, YYYY Button
{
waitForIt(220, 45, 305, 65);
autoco2 = 0;
co2SetSelect();
}
if ((x>=220) && (x<=305) && (y>=94) && (y<=114)) //press Month DD, YYYY Button
{
waitForIt(220, 94, 305, 114);
manuelco2 = 1;
co2SetSelect();
}
if ((x>=220) && (x<=305) && (y>=120) && (y<=140)) //press Month DD, YYYY Button
{
waitForIt(220, 120, 305, 140);
manuelco2 = 0;
co2SetSelect();
}
break;
}
}
delay(100);
}
/********************************* END of TOUCH SCREEN ********************************/









/**************************************** SETUP **************************************/
void setup()
{
Serial.begin(9600);

pinMode(ledPinWhite1, OUTPUT);
pinMode(ledPinWhite2, OUTPUT);
pinMode(ledPinRed, OUTPUT);
pinMode(ledPinRoyalBlue, OUTPUT);

myGLCD.InitLCD(LANDSCAPE);
myGLCD.clrScr();

myTouch.InitTouch(LANDSCAPE);
myTouch.setPrecision(PREC_MEDIUM);

sensors.begin(); //start up temperature library
// set the resolution to 9 bit
sensors.setResolution(waterThermometer, 9);
sensors.setResolution(pieceThermometer, 9);

myGLCD.setColor(64, 64, 64);
myGLCD.fillRect(0, 226, 319, 239); //Bottom Bar

RTC.get(rtc,true);
min_cnt= (rtc[2]*60)+rtc[1];
ReadFromEEPROM();
LED_levels_output();
mainScreen(true);
}
/*********************************** END of SETUP ************************************/

/********************************** BEGIN MAIN LOOP **********************************/
void loop()
{
if ((myTouch.dataAvailable()) && (screenSaverTimer>=setScreenSaverTimer))
{
LEDtestTick = false;
myGLCD.setColor(64, 64, 64);
myGLCD.fillRect(0, 226, 319, 239); //Bottom Bar
screenSaverTimer=0;
clearScreen();
mainScreen(true);
delay(1000);
dispScreen=0;
}
else
{
if (myTouch.dataAvailable())
{
processMyTouch();
}
}

unsigned long currentMillis = millis();
if (currentMillis - previousMillisFive > 5000) //check time, temp and LED levels every 5s
{
previousMillisFive = currentMillis;
RTC.get(rtc,true);

if (screenSaverTimer<setScreenSaverTimer)
{
TimeDateBar();
}

checkTempC();
tempW = 24.5;
CommandeRelay();
min_cnt= (rtc[2]*60)+rtc[1];
LED_levels_output();

screenReturn();
screenSaver();

if ((dispScreen == 0) && (screenSaverTimer<setScreenSaverTimer))
{
mainScreen();
}
}
}
