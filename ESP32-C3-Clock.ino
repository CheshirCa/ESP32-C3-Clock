#include <U8g2lib.h>
#include <WiFi.h>
#include <time.h>
#include <Wire.h>
#include <Preferences.h>

/* ===== OLED 70x40 ===== */
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, 6, 5);

const int DISP_W = 72;
const int DISP_H = 40;
const int X_OFF  = 30;
const int Y_OFF  = 20;

/* ===== BUTTON ===== */
#define BOOT_PIN 9
const unsigned long BTN_DEBOUNCE = 300;
unsigned long lastBtnTime = 0;

/* ===== AUTO RETURN ===== */
const unsigned long INFO_TIMEOUT = 10000;
unsigned long infoStartTime = 0;

/* ===== DEFAULT CONFIG (CODE) ===== */
String defSSID = "your_SSID";
String defPASS = "your_PASSWORD";
String defNTP  = "pool.ntp.org";
long defGMTOffset = 3*3600;       // GMT+3
long defDaylightOffset = 0;       // DST

/* ===== ACTIVE CONFIG ===== */
String wifiSSID;
String wifiPASS;
String ntpServer;
long gmtOffset_sec;
long daylightOffset_sec;

/* ===== NVS ===== */
Preferences prefs;
const char* PREF_NS = "clockcfg";

/* ===== TIME ===== */
struct tm timeinfo;
bool timeValid = false;

/* ===== UI ===== */
enum ScreenMode { SCREEN_CLOCK, SCREEN_INFO };
ScreenMode currentScreen = SCREEN_CLOCK;

enum InfoScreen { INFO_NONE, INFO_WIFI, INFO_TZ };
InfoScreen currentInfoScreen = INFO_NONE;

/* ===== BLINK ===== */
bool colonVisible = true;
unsigned long lastBlink = 0;

/* ===== BUFFERS ===== */
char hhStr[3];
char mmStr[3];
char dateStr[11];

/* ===== SERIAL INTERACTIVE ===== */
String serialInput = "";
bool promptShown = false;

/* ================================================= */

void setup() {
  pinMode(BOOT_PIN, INPUT_PULLUP);
  Serial.begin(115200);
  unsigned long start = millis();
  while (!Serial && millis()-start<2000) delay(10); // ждем готовности USB CDC
  Serial.println("=== Serial ready ===");
  Serial.println("Type HELP for commands");

  u8g2.begin();
  u8g2.setContrast(255);
  u8g2.setBusClock(400000);
  u8g2.enableUTF8Print();

  loadConfigFromNVS();
  showSplash();
  connectWiFi();
  syncTime();
  updateClockStrings();
}

/* ================================================= */

/* ================= NVS ================= */

void loadConfigFromNVS() {
  prefs.begin(PREF_NS, true);

  if (prefs.isKey("ssid") && prefs.getString("ssid").length()>0) {
    wifiSSID = prefs.getString("ssid");
    wifiPASS = prefs.getString("pass");
    ntpServer = prefs.getString("ntp");
    gmtOffset_sec = prefs.getInt("gmtOffset", defGMTOffset);
    daylightOffset_sec = prefs.getInt("daylightOffset", defDaylightOffset);
    Serial.println("Config loaded from NVS");
  } else {
    wifiSSID = defSSID;
    wifiPASS = defPASS;
    ntpServer = defNTP;
    gmtOffset_sec = defGMTOffset;
    daylightOffset_sec = defDaylightOffset;
    Serial.println("Using default config");
  }

  prefs.end();
}

void saveConfigToNVS() {
  prefs.begin(PREF_NS, false);
  prefs.putString("ssid", wifiSSID);
  prefs.putString("pass", wifiPASS);
  prefs.putString("ntp", ntpServer);
  prefs.putInt("gmtOffset", gmtOffset_sec);
  prefs.putInt("daylightOffset", daylightOffset_sec);
  prefs.end();
  Serial.println("Config saved");
}

void eraseNVS() {
  prefs.begin(PREF_NS, false);
  prefs.clear();
  prefs.end();
  Serial.println("NVS erased");
}

/* ================= BUTTON ================= */

void handleButton() {
  static bool btnPrev = HIGH;
  bool btnNow = digitalRead(BOOT_PIN);

  if (btnPrev == HIGH && btnNow == LOW) { // нажатие
    if (millis() - lastBtnTime > BTN_DEBOUNCE) {
      if (currentScreen == SCREEN_CLOCK || currentInfoScreen == INFO_NONE) {
        currentScreen = SCREEN_INFO;
        currentInfoScreen = INFO_WIFI;
      }
      else if (currentInfoScreen == INFO_WIFI) {
        currentInfoScreen = INFO_TZ;
      }
      else { // возврат на часы
        currentScreen = SCREEN_CLOCK;
        currentInfoScreen = INFO_NONE;
        updateClockStrings();
      }
      infoStartTime = millis();
      lastBtnTime = millis();
    }
  }
  btnPrev = btnNow;
}

void handleAutoReturn() {
  if (currentScreen == SCREEN_INFO && millis() - infoStartTime > INFO_TIMEOUT) {
    currentScreen = SCREEN_CLOCK;
    currentInfoScreen = INFO_NONE;
    updateClockStrings();
  }
}

/* ================= SERIAL ================= */

void handleSerial() {
  if (!promptShown) { Serial.print("> "); promptShown = true; }

  while (Serial.available()) {
    char c = Serial.read();
    Serial.print(c);  // echo

    if (c == '\r' || c == '\n') {
      if (serialInput.length() == 0) continue;
      String cmd = serialInput;
      serialInput = "";
      cmd.trim();
      if (cmd.length() == 0) { Serial.print("> "); continue; }

      if (cmd.equalsIgnoreCase("HELP")) {
        Serial.println("TIME YYYY-MM-DD HH:MM:SS");
        Serial.println("WIFI <ssid> <pass>");
        Serial.println("NTP <server>");
        Serial.println("TZ <offset_sec>");
        Serial.println("DST <offset_sec>");
        Serial.println("SAVE | RESTORE | ERASE");
        Serial.println("STATUS | REBOOT");
      }
      else if (cmd.startsWith("TIME ")) { setManualTime(cmd.substring(5)); updateClockStrings(); }
      else if (cmd.startsWith("WIFI ")) {
        int sp = cmd.indexOf(' ', 5);
        if(sp>0){ wifiSSID = cmd.substring(5, sp); wifiPASS = cmd.substring(sp+1); connectWiFi(); }
      }
      else if (cmd.startsWith("NTP ")) { ntpServer = cmd.substring(4); syncTime(); updateClockStrings(); }
      else if (cmd.startsWith("TZ ")) { gmtOffset_sec = cmd.substring(3).toInt(); Serial.println("GMT offset set to "+String(gmtOffset_sec)+" seconds"); configTime(gmtOffset_sec, daylightOffset_sec, ntpServer.c_str()); }
      else if (cmd.startsWith("DST ")) { daylightOffset_sec = cmd.substring(4).toInt(); Serial.println("Daylight offset set to "+String(daylightOffset_sec)+" seconds"); configTime(gmtOffset_sec, daylightOffset_sec, ntpServer.c_str()); }
      else if (cmd.equalsIgnoreCase("SAVE")) { saveConfigToNVS(); }
      else if (cmd.equalsIgnoreCase("RESTORE")) { loadConfigFromNVS(); connectWiFi(); syncTime(); updateClockStrings(); }
      else if (cmd.equalsIgnoreCase("ERASE")) { eraseNVS(); }
      else if (cmd.equalsIgnoreCase("STATUS")) {
        Serial.println("SSID: " + wifiSSID);
        Serial.println("IP: " + WiFi.localIP().toString());
        Serial.println("NTP: " + ntpServer);
        Serial.println("GMT offset: " + String(gmtOffset_sec));
        Serial.println("DST offset: " + String(daylightOffset_sec));
        Serial.println(timeValid?"TIME OK":"TIME INVALID");
      }
      else if (cmd.equalsIgnoreCase("REBOOT")) { ESP.restart(); }
      else { Serial.println("Unknown command"); }

      Serial.print("> ");
    }
    else { serialInput += c; }
  }
}

/* ================= CLOCK STRINGS ================= */

void updateClockStrings() {
  if(getLocalTime(&timeinfo) && timeinfo.tm_year>120){
    snprintf(hhStr, sizeof(hhStr), "%02d", timeinfo.tm_hour);
    snprintf(mmStr, sizeof(mmStr), "%02d", timeinfo.tm_min);
    snprintf(dateStr, sizeof(dateStr), "%02d.%02d.%04d",
             timeinfo.tm_mday,
             timeinfo.tm_mon+1,
             timeinfo.tm_year+1900);
  }
}

/* ================= DRAW CLOCK ================= */

void drawClock() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_logisoso24_tn);

  int timeW = u8g2.getStrWidth("88:88");
  int timeX = X_OFF + (DISP_W - timeW)/2;
  int timeY = Y_OFF + 30;

  int wHH = u8g2.getStrWidth("88");
  int wColon = u8g2.getStrWidth(":");

  u8g2.setCursor(timeX, timeY);
  u8g2.print(hhStr);
  if(colonVisible){ u8g2.setCursor(timeX+wHH,timeY); u8g2.print(":"); }
  u8g2.setCursor(timeX+wHH+wColon,timeY);
  u8g2.print(mmStr);

  u8g2.setFont(u8g2_font_5x7_t_cyrillic);
  int dateW = u8g2.getStrWidth(dateStr);
  int dateX = (timeX + timeW/2) - dateW/2;
  u8g2.setCursor(dateX, Y_OFF+DISP_H-1);
  u8g2.print(dateStr);

  u8g2.sendBuffer();
}

/* ================= DRAW INFO ================= */

void drawInfo() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_5x7_t_cyrillic);

  int y = Y_OFF+12;

  if(currentInfoScreen==INFO_WIFI){
    u8g2.setCursor(X_OFF,y); u8g2.print("SSID:"); y+=8;
    u8g2.setCursor(X_OFF,y); u8g2.print(wifiSSID); y+=12;
    u8g2.setCursor(X_OFF,y); u8g2.print("IP:"); y+=8;
    u8g2.setCursor(X_OFF,y); u8g2.print(WiFi.localIP());
  }
  else if(currentInfoScreen==INFO_TZ){
    u8g2.setCursor(X_OFF,y); u8g2.print("GMT:");
    u8g2.setCursor(X_OFF+28,y); u8g2.print(gmtOffset_sec/3600); y+=8;
    u8g2.setCursor(X_OFF,y); u8g2.print("DST:");
    u8g2.setCursor(X_OFF+28,y); u8g2.print(daylightOffset_sec/3600);
  }

  u8g2.sendBuffer();
}

/* ================= SYSTEM ================= */

void showSplash() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_t_cyrillic);
  u8g2.setCursor(X_OFF+6,Y_OFF+20); u8g2.print("ESP32-C3");
  u8g2.setCursor(X_OFF+20,Y_OFF+36); u8g2.print("ЧАСЫ");
  u8g2.sendBuffer();
  delay(2000);
}

void showNoSync() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_5x7_t_cyrillic);
  u8g2.setCursor(X_OFF+4,Y_OFF+22);
  u8g2.print("НЕТ ВРЕМЕНИ");
  u8g2.sendBuffer();
}

/* ================= WIFI / TIME ================= */

void connectWiFi() {
  Serial.println("Connecting to WiFi...");
  Serial.println("SSID: "+wifiSSID);

  WiFi.disconnect(true);
  WiFi.begin(wifiSSID.c_str(),wifiPASS.c_str());

  unsigned long start = millis();
  while(WiFi.status()!=WL_CONNECTED){
    if(millis()-start>10000){ Serial.println("WiFi timeout"); return; }
    delay(200);
  }

  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());
}

void syncTime() {
  timeValid=false;
  configTime(gmtOffset_sec,daylightOffset_sec,ntpServer.c_str());
  for(int i=0;i<30;i++){
    if(getLocalTime(&timeinfo) && timeinfo.tm_year>120){ timeValid=true; updateClockStrings(); return; }
    delay(500);
  }
}

void setManualTime(String s){
  struct tm t{};
  sscanf(s.c_str(),"%d-%d-%d %d:%d:%d",
         &t.tm_year,&t.tm_mon,&t.tm_mday,
         &t.tm_hour,&t.tm_min,&t.tm_sec);
  t.tm_year-=1900; t.tm_mon-=1;

  time_t tt = mktime(&t);
  struct timeval now={tt,0};
  settimeofday(&now,nullptr);
  timeValid=true;
  updateClockStrings();
}

/* ================= LOOP ================= */

void loop() {
  handleSerial();
  handleButton();
  handleAutoReturn();

  if(millis()-lastBlink>=500){ colonVisible=!colonVisible; lastBlink=millis(); }

  if(getLocalTime(&timeinfo) && timeinfo.tm_year>120) timeValid=true;

  if(currentScreen==SCREEN_INFO){ drawInfo(); delay(100); return; }
  if(!timeValid){ showNoSync(); delay(500); return; }

  updateClockStrings();
  drawClock();
  delay(50);
}
