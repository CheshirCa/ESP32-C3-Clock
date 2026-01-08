#include <U8g2lib.h>
#include <WiFi.h>
#include <time.h>
#include <Wire.h>
#include <Preferences.h>
#include <esp_timer.h>

// OLED Display (70x40)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, 6, 5);
const int DISP_W = 72;
const int DISP_H = 40;
const int X_OFF = 30;
const int Y_OFF = 20;

const int INFO_Y_OFFSET = 10;

// Default Configuration
String defSSID = "your_SSID";
String defPASS = "your_PASSWORD";
String defNTP = "pool.ntp.org";
long defGMTOffset = 3 * 3600;
long defDaylightOffset = 0;

// Active Configuration
String wifiSSID;
String wifiPASS;
String ntpServer;
long gmtOffset_sec;
long daylightOffset_sec;

// NVS Storage
Preferences prefs;
const char* PREF_NS = "clockcfg";
const char* ALARM_PREF_NS = "alarms";

// Time
struct tm timeinfo;
bool timeValid = false;

// UI States
enum ScreenMode {
  SCREEN_CLOCK,
  SCREEN_INFO1,
  SCREEN_INFO2,
  SCREEN_ALARM,
  SCREEN_TIMER
};
ScreenMode currentScreen = SCREEN_CLOCK;
int infoScreenPage = 1;

// Auto Return
const unsigned long INFO_TIMEOUT = 10000;
unsigned long infoStartTime = 0;

// Blinking Colon
bool colonVisible = true;
unsigned long lastBlink = 0;

// Button
#define BOOT_PIN 9
const unsigned long BTN_DEBOUNCE = 300;
unsigned long lastBtnTime = 0;

// Buzzer
#define BUZZER_PIN 10
bool buzzerActive = false;

// Time Strings
char hhStr[3];
char mmStr[3];
char dateStr[11];
char weekdayStr[15];

// Serial Input
String serialInput = "";
bool promptShown = false;

// Command History
const int HISTORY_SIZE = 10;
String commandHistory[HISTORY_SIZE];
int historyIndex = 0;      // Current position in history
int historyCount = 0;      // Total commands in history
int historyBrowseIndex = -1; // Index while browsing (-1 = not browsing)
String tempInput = "";     // Temporary storage while browsing

// Alarm Structure
struct Alarm {
  bool active = false;
  int year = 0;         // 0 = daily, specific year if set
  int month = 0;        // 0 = daily, 1-12 if specific date
  int day = 0;          // 0 = daily, 1-31 if specific date
  int weekdays = 0;     // Bitmask: 0x01=Mon, 0x02=Tue, 0x04=Wed, 0x08=Thu, 0x10=Fri, 0x20=Sat, 0x40=Sun
  int hour = 0;         // 0-23
  int minute = 0;       // 0-59
  bool repeat = false;  // Repeat after trigger
  bool saved = false;   // Save to NVS
  char text[11] = "";   // Alarm text (max 10 chars)
};
Alarm myAlarm;

// Timer
bool timerActive = false;
uint64_t timerStartUs = 0;
uint64_t timerDurationUs = 0;
char timerText[11] = "";

// Trigger Flags
bool timerTriggered = false;
bool alarmTriggered = false;

// Weekdays in Russian (for display)
const char* weekdaysRU[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

// Blue LED for Alarm Indicator
#define BLUE_LED_PIN 8
#define LED_ACTIVE_LOW true

// =================================================
void setup() {
  pinMode(BOOT_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Blue LED setup
  pinMode(BLUE_LED_PIN, OUTPUT);
  digitalWrite(BLUE_LED_PIN, HIGH);

  Serial.begin(115200);
  unsigned long start = millis();
  while (!Serial && millis() - start < 2000) delay(10);

  Serial.println("\n=== ESP32-C3 Clock ===");
  Serial.println("Type HELP for commands");

  // OLED Initialization
  u8g2.begin();
  u8g2.setContrast(255);
  u8g2.setBusClock(400000);
  u8g2.enableUTF8Print();

  // Display Test
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_7x14_tf);
  u8g2.setCursor(30, 30);
  u8g2.print("TEST OK");
  u8g2.sendBuffer();
  delay(500);

  // Load configuration and connect
  loadConfigFromNVS();
  loadAlarmFromNVS();
  showSplash();
  connectWiFi();
  syncTime();
  updateClockStrings();
}

// ================= NVS FUNCTIONS =================
void loadConfigFromNVS() {
  prefs.begin(PREF_NS, true);

  if (prefs.isKey("ssid") && !prefs.getString("ssid").isEmpty()) {
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

void loadAlarmFromNVS() {
  prefs.begin(ALARM_PREF_NS, true);

  if (prefs.isKey("active")) {
    myAlarm.active = prefs.getBool("active", false);
    myAlarm.year = prefs.getInt("year", 0);
    myAlarm.month = prefs.getInt("month", 0);
    myAlarm.day = prefs.getInt("day", 0);
    myAlarm.weekdays = prefs.getInt("weekdays", 0);
    myAlarm.hour = prefs.getInt("hour", 0);
    myAlarm.minute = prefs.getInt("minute", 0);
    myAlarm.repeat = prefs.getBool("repeat", false);
    myAlarm.saved = true;

    String text = prefs.getString("text", "");
    text.toCharArray(myAlarm.text, sizeof(myAlarm.text));

    Serial.println("Alarm loaded from NVS");
  } else {
    memset(&myAlarm, 0, sizeof(myAlarm));
  }

  prefs.end();
  updateAlarmIndicator();
}

void saveAlarmToNVS() {
  prefs.begin(ALARM_PREF_NS, false);
  prefs.putBool("active", myAlarm.active);
  prefs.putInt("year", myAlarm.year);
  prefs.putInt("month", myAlarm.month);
  prefs.putInt("day", myAlarm.day);
  prefs.putInt("weekdays", myAlarm.weekdays);
  prefs.putInt("hour", myAlarm.hour);
  prefs.putInt("minute", myAlarm.minute);
  prefs.putBool("repeat", myAlarm.repeat);
  prefs.putString("text", String(myAlarm.text));
  prefs.end();

  myAlarm.saved = true;
  Serial.println("Alarm saved to NVS");
  updateAlarmIndicator();
}

void clearAlarmFromNVS() {
  prefs.begin(ALARM_PREF_NS, false);
  prefs.clear();
  prefs.end();

  myAlarm.saved = false;
  Serial.println("Alarm cleared from NVS");
  updateAlarmIndicator();
}

void eraseNVS() {
  prefs.begin(PREF_NS, false);
  prefs.clear();
  prefs.end();

  prefs.begin(ALARM_PREF_NS, false);
  prefs.clear();
  prefs.end();

  Serial.println("NVS erased");
}

void updateAlarmIndicator() {
  // For active LOW: LOW = ON, HIGH = OFF
  bool shouldBeOn = myAlarm.active && !alarmTriggered;
  digitalWrite(BLUE_LED_PIN, shouldBeOn ? LOW : HIGH);
}

// ================= COMMAND HISTORY FUNCTIONS =================
void addToHistory(String cmd) {
  if (cmd.length() == 0) return;
  
  // Don't add if same as last command
  if (historyCount > 0 && commandHistory[(historyIndex - 1 + HISTORY_SIZE) % HISTORY_SIZE] == cmd) {
    return;
  }
  
  commandHistory[historyIndex] = cmd;
  historyIndex = (historyIndex + 1) % HISTORY_SIZE;
  if (historyCount < HISTORY_SIZE) historyCount++;
}

String getHistoryUp() {
  if (historyCount == 0) return "";
  
  // First time pressing up - save current input
  if (historyBrowseIndex == -1) {
    tempInput = serialInput;
    historyBrowseIndex = (historyIndex - 1 + HISTORY_SIZE) % HISTORY_SIZE;
  } else {
    // Move back in history
    int prevIndex = (historyBrowseIndex - 1 + HISTORY_SIZE) % HISTORY_SIZE;
    // Check if we've reached the oldest command
    int oldestIndex = (historyIndex - historyCount + HISTORY_SIZE) % HISTORY_SIZE;
    if (historyBrowseIndex != oldestIndex) {
      historyBrowseIndex = prevIndex;
    }
  }
  
  return commandHistory[historyBrowseIndex];
}

String getHistoryDown() {
  if (historyBrowseIndex == -1) return serialInput; // Not browsing
  
  int nextIndex = (historyBrowseIndex + 1) % HISTORY_SIZE;
  
  // If moving forward to current position, restore temp input
  if (nextIndex == historyIndex) {
    historyBrowseIndex = -1;
    return tempInput;
  }
  
  historyBrowseIndex = nextIndex;
  return commandHistory[historyBrowseIndex];
}

void clearCurrentLine() {
  // Move cursor to start of input and clear it
  for (int i = 0; i < serialInput.length(); i++) {
    Serial.print("\b \b");
  }
}

// ================= SERIAL COMMAND HANDLER =================
void handleSerial() {
  if (!promptShown) {
    Serial.print("> ");
    promptShown = true;
  }

  while (Serial.available()) {
    char c = Serial.read();

    // Handle ANSI escape sequences (arrow keys)
    static bool escapeMode = false;
    static bool bracketMode = false;
    
    if (c == 27) { // ESC
      escapeMode = true;
      continue;
    }
    
    if (escapeMode) {
      if (c == '[') {
        bracketMode = true;
        continue;
      }
      
      if (bracketMode) {
        if (c == 'A') { // Up arrow
          String histCmd = getHistoryUp();
          if (histCmd.length() > 0) {
            clearCurrentLine();
            serialInput = histCmd;
            Serial.print(serialInput);
          }
          escapeMode = false;
          bracketMode = false;
          continue;
        } else if (c == 'B') { // Down arrow
          String histCmd = getHistoryDown();
          clearCurrentLine();
          serialInput = histCmd;
          Serial.print(serialInput);
          escapeMode = false;
          bracketMode = false;
          continue;
        } else if (c == 'C' || c == 'D') { // Right/Left arrow - ignore for now
          escapeMode = false;
          bracketMode = false;
          continue;
        }
      }
      escapeMode = false;
      bracketMode = false;
      continue;
    }

    // Handle Backspace
    if (c == 8 || c == 127) {
      if (serialInput.length() > 0) {
        serialInput.remove(serialInput.length() - 1);
        Serial.print("\b \b");
      }
      continue;  // Skip further processing for backspace
    }

    Serial.print(c);  // echo

    if (c == '\r' || c == '\n') {
      if (serialInput.length() == 0) {
        Serial.print("> ");
        return;
      }

      String cmd = serialInput;
      addToHistory(cmd); // Add to history
      historyBrowseIndex = -1; // Reset browse mode
      serialInput = "";
      cmd.trim();
      cmd.toUpperCase();

      // HELP command
      if (cmd.equals("HELP")) {
        Serial.println("=== Clock Commands ===");
        Serial.println("TIME YYYY-MM-DD HH:MM:SS");
        Serial.println("WIFI <ssid> <pass>");
        Serial.println("NTP <server>");
        Serial.println("TZ <+/-offset_hours>");
        Serial.println("DST <+/-offset_hours>");
        Serial.println("SAVE | RESTORE | ERASE");
        Serial.println("STATUS | SYNC | REBOOT");
        Serial.println("=== Alarm Commands ===");
        Serial.println("ALARM [YYYY-MM-DD|1234567] HH:MM [TEXT] [R] [S]");
        Serial.println("  YYYY-MM-DD = specific date");
        Serial.println("  1234567 = weekdays (1=Mon,7=Sun)");
        Serial.println("  R = repeat after trigger");
        Serial.println("  S = save to NVS");
        Serial.println("ALARM CLEAR");
        Serial.println("=== Timer Commands ===");
        Serial.println("TIMER HH:MM:SS [TEXT]");
        Serial.println("TIMER MM:SS [TEXT]");
        Serial.println("TIMER SS [TEXT]");
        Serial.println("TIMER CLEAR");
      }
      // TIME command
      else if (cmd.startsWith("TIME ")) {
        setManualTime(cmd.substring(5));
        updateClockStrings();
      }
      // WIFI command
      else if (cmd.startsWith("WIFI ")) {
        int sp = cmd.indexOf(' ', 5);
        if (sp > 0) {
          wifiSSID = cmd.substring(5, sp);
          wifiPASS = cmd.substring(sp + 1);
          connectWiFi();
        }
      }
      // NTP command
      else if (cmd.startsWith("NTP ")) {
        ntpServer = cmd.substring(4);
        syncTime();
        updateClockStrings();
      }
      // TZ command
      else if (cmd.startsWith("TZ ")) {
        String tzStr = cmd.substring(3);
        tzStr.trim();
        if (tzStr.length() > 0) {
          bool negative = false;
          int startIndex = 0;

          if (tzStr[0] == '+') {
            startIndex = 1;
          } else if (tzStr[0] == '-') {
            negative = true;
            startIndex = 1;
          }

          String numStr = tzStr.substring(startIndex);
          long value = numStr.toInt() * 3600;

          if (negative) value = -value;
          gmtOffset_sec = value;

          Serial.print("GMT offset set to ");
          Serial.print(value >= 0 ? "+" : "-");
          Serial.print(abs(value / 3600));
          Serial.println(" hours");
          configTime(gmtOffset_sec, daylightOffset_sec, ntpServer.c_str());
        }
      }
      // DST command
      else if (cmd.startsWith("DST ")) {
        String dstStr = cmd.substring(4);
        dstStr.trim();
        if (dstStr.length() > 0) {
          bool negative = false;
          int startIndex = 0;

          if (dstStr[0] == '+') {
            startIndex = 1;
          } else if (dstStr[0] == '-') {
            negative = true;
            startIndex = 1;
          }

          String numStr = dstStr.substring(startIndex);
          long value = numStr.toInt() * 3600;

          if (negative) value = -value;
          daylightOffset_sec = value;

          Serial.print("Daylight offset set to ");
          Serial.print(value >= 0 ? "+" : "-");
          Serial.print(abs(value / 3600));
          Serial.println(" hours");
          configTime(gmtOffset_sec, daylightOffset_sec, ntpServer.c_str());
        }
      }
      // SAVE command
      else if (cmd.equals("SAVE")) {
        saveConfigToNVS();
      }
      // RESTORE command
      else if (cmd.equals("RESTORE")) {
        loadConfigFromNVS();
        connectWiFi();
        syncTime();
        updateClockStrings();
      }
      // ERASE command
      else if (cmd.equals("ERASE")) {
        eraseNVS();
      }
      // STATUS command
      else if (cmd.equals("STATUS")) {
        Serial.println("=== SYSTEM STATUS ===");
        Serial.println("SSID: " + wifiSSID);
        Serial.println("WiFi: " + String(WiFi.status() == WL_CONNECTED ? "CONNECTED" : "DISCONNECTED"));
        if (WiFi.status() == WL_CONNECTED) {
          Serial.println("IP: " + WiFi.localIP().toString());
        }
        Serial.println("NTP Server: " + ntpServer);
        Serial.println("NTP Status: " + String(timeValid ? "SYNCED" : "NOT SYNCED"));

        // Current time
        if (getLocalTime(&timeinfo) && timeinfo.tm_year > 120) {
          char timeStr[50];
          sprintf(timeStr, "Current Time: %02d:%02d:%02d",
                  timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
          Serial.println(timeStr);

          sprintf(timeStr, "Current Date: %02d.%02d.%04d",
                  timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
          Serial.println(timeStr);
        } else {
          Serial.println("Current Time: NOT AVAILABLE");
        }

        // Alarm status
        if (myAlarm.active) {
          Serial.print("Alarm: ");
          if (myAlarm.year > 0) {
            Serial.printf("%04d-%02d-%02d ", myAlarm.year, myAlarm.month, myAlarm.day);
          } else if (myAlarm.weekdays > 0) {
            Serial.print("Weekdays: ");
            if (myAlarm.weekdays & 0x01) Serial.print("Mon");
            if (myAlarm.weekdays & 0x02) Serial.print("Tue");
            if (myAlarm.weekdays & 0x04) Serial.print("Wed");
            if (myAlarm.weekdays & 0x08) Serial.print("Thu");
            if (myAlarm.weekdays & 0x10) Serial.print("Fri");
            if (myAlarm.weekdays & 0x20) Serial.print("Sat");
            if (myAlarm.weekdays & 0x40) Serial.print("Sun");
            Serial.print(" ");
          } else {
            Serial.print("Daily ");
          }
          Serial.printf("%02d:%02d", myAlarm.hour, myAlarm.minute);
          if (myAlarm.repeat) Serial.print(" [R]");
          if (myAlarm.saved) Serial.print(" [S]");
          if (myAlarm.text[0]) Serial.printf(" '%s'", myAlarm.text);
          Serial.println();
        } else {
          Serial.println("Alarm: OFF");
        }

        // Timer status
        if (timerActive) {
          uint64_t elapsed = esp_timer_get_time() - timerStartUs;
          uint64_t remaining = (elapsed >= timerDurationUs) ? 0 : (timerDurationUs - elapsed);
          int secRemaining = (remaining + 500000) / 1000000;
          int minRemaining = secRemaining / 60;
          secRemaining = secRemaining % 60;
          int hourRemaining = minRemaining / 60;
          minRemaining = minRemaining % 60;

          char timerStr[60];
          sprintf(timerStr, "Timer: %02d:%02d:%02d remaining",
                  hourRemaining, minRemaining, secRemaining);
          if (strcmp(timerText, "TIMER") != 0) {
            sprintf(timerStr + strlen(timerStr), ", text='%s'", timerText);
          }
          Serial.println(timerStr);
        } else {
          Serial.println("Timer: OFF");
        }

        Serial.println("Free RAM: " + String(esp_get_free_heap_size() / 1024) + " KB");
      }
      // SYNC command
      else if (cmd.equals("SYNC")) {
        Serial.println("Forcing NTP synchronization...");
        syncTime();
        if (timeValid) {
          Serial.println("Time synchronized successfully");
          updateClockStrings();
        } else {
          Serial.println("NTP synchronization failed");
        }
      }
      // REBOOT command
      else if (cmd.equals("REBOOT")) {
        ESP.restart();
      }
      // ALARM CLEAR command
      else if (cmd.equals("ALARM CLEAR")) {
        memset(&myAlarm, 0, sizeof(myAlarm));
        alarmTriggered = false;
        buzzerActive = false;
        digitalWrite(BUZZER_PIN, LOW);
        clearAlarmFromNVS();
        updateAlarmIndicator();
        Serial.println("Alarm cleared");
      }
      // SET ALARM command (improved format)
      else if (cmd.startsWith("ALARM ")) {
        String s = cmd.substring(6);
        s.trim();

        // Parse date/weekdays
        int dateType = 0;  // 0=daily, 1=specific date, 2=weekdays
        int year = 0, month = 0, day = 0;
        int weekdaysMask = 0;

        // Check for specific date (YYYY-MM-DD) - must have exactly 2 dashes at positions 4 and 7
        if (s.length() >= 10 && s.charAt(4) == '-' && s.charAt(7) == '-') {
          dateType = 1;
          year = s.substring(0, 4).toInt();
          month = s.substring(5, 7).toInt();
          day = s.substring(8, 10).toInt();
          s = s.substring(11);  // Remove date part
          s.trim();
        }
        // Check for weekdays (only digits 1-7 without colons)
        else if (s.length() > 0 && s[0] >= '1' && s[0] <= '7') {
          // Check if this is weekdays or time by looking for colon
          int firstColon = s.indexOf(':');
          int firstSpace = s.indexOf(' ');
          
          // If colon comes before any weekday digits end, it's time not weekdays
          // Example: "17:00" - colon at position 2, so it's time
          // Example: "12345 17:00" - space at position 5, colon at 8, so 12345 are weekdays
          
          bool isWeekdays = false;
          if (firstColon == -1) {
            // No colon at all - can't be valid, but treat first part as weekdays
            isWeekdays = true;
          } else if (firstSpace != -1 && firstSpace < firstColon) {
            // Space before colon - digits before space are weekdays
            isWeekdays = true;
          } else {
            // Colon comes first - this is time, not weekdays
            isWeekdays = false;
          }
          
          if (isWeekdays) {
            dateType = 2;
            String digits = "";
            int i = 0;
            while (i < s.length() && s[i] >= '1' && s[i] <= '7') {
              digits += s[i];
              i++;
            }
            s = s.substring(i);
            s.trim();

            // Convert digits to bitmask
            for (int j = 0; j < digits.length(); j++) {
              char d = digits[j];
              if (d >= '1' && d <= '7') {
                int dayNum = d - '1';  // 0=Mon, 6=Sun
                weekdaysMask |= (1 << dayNum);
              }
            }
          }
        }

        // Parse time (HH:MM)
        int colonPos = s.indexOf(':');
        if (colonPos == -1 || colonPos >= 3) {
          Serial.println("Format: ALARM [YYYY-MM-DD|1234567] HH:MM [TEXT] [R] [S]");
          Serial.print("> ");
          return;
        }

        int spacePos = s.indexOf(' ', colonPos + 1);
        String timePart = spacePos == -1 ? s : s.substring(0, spacePos);
        String rest = spacePos == -1 ? "" : s.substring(spacePos + 1);

        int hour = timePart.substring(0, colonPos).toInt();
        int minute = timePart.substring(colonPos + 1).toInt();

        if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
          Serial.println("Hours: 0-23, Minutes: 0-59");
          Serial.print("> ");
          return;
        }

        if (!timeValid) {
          Serial.println("Wait for time sync!");
          Serial.print("> ");
          return;
        }

        // Parse options (R, S, TEXT)
        // Strategy: first collect all single-letter flags (R, S), then rest is TEXT
        bool repeat = false;
        bool save = false;
        String textContent = "";
        
        rest.trim();
        
        while (rest.length() > 0) {
          // Check if starts with R or S followed by space or end
          if ((rest[0] == 'R' || rest[0] == 'r') && 
              (rest.length() == 1 || rest[1] == ' ')) {
            repeat = true;
            rest = rest.substring(1);
            rest.trim();
          } 
          else if ((rest[0] == 'S' || rest[0] == 's') && 
                   (rest.length() == 1 || rest[1] == ' ')) {
            save = true;
            rest = rest.substring(1);
            rest.trim();
          } 
          else {
            // Everything else is TEXT
            textContent = rest;
            break;
          }
        }
        
        // Convert text to char array
        char text[11] = "";
        if (textContent.length() > 0) {
          int len = min((int)textContent.length(), 10);
          textContent.substring(0, len).toCharArray(text, sizeof(text));
        }

        // Set alarm
        myAlarm.active = true;
        myAlarm.hour = hour;
        myAlarm.minute = minute;
        myAlarm.repeat = repeat;
        alarmTriggered = false;
        strcpy(myAlarm.text, text);

        if (dateType == 1) {
          myAlarm.year = year;
          myAlarm.month = month;
          myAlarm.day = day;
          myAlarm.weekdays = 0;
        } else if (dateType == 2) {
          myAlarm.year = 0;
          myAlarm.month = 0;
          myAlarm.day = 0;
          myAlarm.weekdays = weekdaysMask;
        } else {
          myAlarm.year = 0;
          myAlarm.month = 0;
          myAlarm.day = 0;
          myAlarm.weekdays = 0;
        }

        if (save) {
          saveAlarmToNVS();
        } else {
          myAlarm.saved = false;
          updateAlarmIndicator();
        }

        // Print confirmation
        Serial.print("Alarm set for ");
        if (dateType == 1) {
          Serial.printf("%04d-%02d-%02d ", year, month, day);
        } else if (dateType == 2) {
          Serial.print("Weekdays: ");
          if (weekdaysMask & 0x01) Serial.print("Mon ");
          if (weekdaysMask & 0x02) Serial.print("Tue ");
          if (weekdaysMask & 0x04) Serial.print("Wed ");
          if (weekdaysMask & 0x08) Serial.print("Thu ");
          if (weekdaysMask & 0x10) Serial.print("Fri ");
          if (weekdaysMask & 0x20) Serial.print("Sat ");
          if (weekdaysMask & 0x40) Serial.print("Sun ");
        } else {
          Serial.print("Daily ");
        }
        Serial.printf("%02d:%02d", hour, minute);
        if (repeat) Serial.print(" [R]");
        if (save) Serial.print(" [S]");
        if (text[0]) Serial.printf(" '%s'", text);
        Serial.println();
      }
      // TIMER CLEAR command
      else if (cmd.equals("TIMER CLEAR")) {
        timerActive = false;
        timerTriggered = false;
        buzzerActive = false;
        digitalWrite(BUZZER_PIN, LOW);
        Serial.println("Timer cleared");
      }
      // SET TIMER command (improved format - removed 1 hour limit)
      else if (cmd.startsWith("TIMER ")) {
        String s = cmd.substring(6);
        s.trim();

        // Find where time part ends
        int firstSpace = s.indexOf(' ');
        String timePart;
        String rest = "";

        if (firstSpace != -1) {
          timePart = s.substring(0, firstSpace);
          rest = s.substring(firstSpace + 1);
          rest.trim();
        } else {
          timePart = s;
        }

        // Parse time in different formats
        int hour = 0, minute = 0, second = 0;
        int colonCount = 0;

        for (int i = 0; i < timePart.length(); i++) {
          if (timePart[i] == ':') colonCount++;
        }

        if (colonCount == 2) {
          // HH:MM:SS format
          int c1 = timePart.indexOf(':');
          int c2 = timePart.indexOf(':', c1 + 1);
          hour = timePart.substring(0, c1).toInt();
          minute = timePart.substring(c1 + 1, c2).toInt();
          second = timePart.substring(c2 + 1).toInt();
        } else if (colonCount == 1) {
          // MM:SS format
          int c1 = timePart.indexOf(':');
          minute = timePart.substring(0, c1).toInt();
          second = timePart.substring(c1 + 1).toInt();
        } else if (colonCount == 0) {
          // SS format
          second = timePart.toInt();
        } else {
          Serial.println("Format: TIMER [HH:]MM:SS [TEXT] or TIMER SS [TEXT]");
          Serial.print("> ");
          return;
        }

        // Validate
        if (hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59) {
          Serial.println("Invalid time values (Hours: 0-23, Minutes: 0-59, Seconds: 0-59)");
          Serial.print("> ");
          return;
        }

        unsigned long totalSec = (unsigned long)hour * 3600 + (unsigned long)minute * 60 + (unsigned long)second;

        if (totalSec == 0) {
          Serial.println("Timer must be at least 1 second");
          Serial.print("> ");
          return;
        }

        // Maximum ~24 hours (86400 seconds)
        if (totalSec > 86400) {
          Serial.println("Timer maximum is 24 hours");
          Serial.print("> ");
          return;
        }

        // Default text
        strcpy(timerText, "TIMER");

        // Parse TEXT
        if (rest.length() > 0) {
          int len = min((int)rest.length(), 10);
          rest.substring(0, len).toCharArray(timerText, sizeof(timerText));
        }

        timerActive = true;
        timerTriggered = false;
        timerStartUs = esp_timer_get_time();
        timerDurationUs = (uint64_t)totalSec * 1000000;

        Serial.print("Timer set for ");
        if (hour > 0) Serial.printf("%02d:", hour);
        if (minute > 0 || hour > 0) Serial.printf("%02d:", minute);
        Serial.printf("%02d", second);
        Serial.print(" seconds");

        if (strcmp(timerText, "TIMER") != 0) {
          Serial.print(", text='");
          Serial.print(timerText);
          Serial.print("'");
        }
        Serial.println();
      }
      // Hidden debug commands (not shown in HELP)
      else if (cmd.equals("BUZZER ON")) {
        buzzerActive = true;
        Serial.println("Buzzer ON");
      } else if (cmd.equals("BUZZER OFF")) {
        buzzerActive = false;
        digitalWrite(BUZZER_PIN, LOW);
        Serial.println("Buzzer OFF");
      } else {
        Serial.println("Unknown command. Type HELP for commands.");
      }

      Serial.print("> ");
    } else {
      serialInput += c;
    }
  }
}

// ================= TIME FUNCTIONS =================
void updateClockStrings() {
  if (getLocalTime(&timeinfo) && timeinfo.tm_year > 120) {
    snprintf(hhStr, sizeof(hhStr), "%02d", timeinfo.tm_hour);
    snprintf(mmStr, sizeof(mmStr), "%02d", timeinfo.tm_min);
    snprintf(dateStr, sizeof(dateStr), "%02d.%02d.%04d",
             timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);

    int wday = timeinfo.tm_wday;
    if (wday >= 0 && wday < 7) {
      strcpy(weekdayStr, weekdaysRU[wday]);
    } else {
      strcpy(weekdayStr, "---");
    }
  }
}

void drawClock() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_logisoso24_tn);

  // Center "88:88"
  const char* ref = "88:88";
  int refW = u8g2.getStrWidth(ref);
  int refX = X_OFF + (DISP_W - refW) / 2;
  int refY = Y_OFF + 30;

  int colonOffset = u8g2.getStrWidth("88");
  int hhX = refX;
  int colonX = refX + colonOffset;
  int mmX = colonX + u8g2.getStrWidth(":");

  u8g2.setCursor(hhX, refY);
  u8g2.print(hhStr);

  u8g2.setCursor(colonX, refY);
  u8g2.print(colonVisible ? ":" : " ");

  u8g2.setCursor(mmX, refY);
  u8g2.print(mmStr);

  // Date and indicators
  String ds = String(dateStr);
  if (myAlarm.active) ds += " *";
  if (timerActive) ds += " #";

  u8g2.setFont(u8g2_font_5x7_t_cyrillic);
  int dw = u8g2.getStrWidth(ds.c_str());
  int dx = X_OFF + (DISP_W - dw) / 2;
  u8g2.setCursor(dx, Y_OFF + DISP_H - 1);
  u8g2.print(ds);

  u8g2.sendBuffer();
}

void drawAlarmOrTimer(const char* txt) {
  u8g2.clearBuffer();

  String s = (txt && txt[0]) ? String(txt) : "ALARM";
  if (s.length() > 10) s = s.substring(0, 10);

  u8g2.setFont(u8g2_font_7x14_tf);

  int w = u8g2.getStrWidth(s.c_str());
  int x = X_OFF + (DISP_W - w) / 2;
  int y = Y_OFF + 24;

  u8g2.setCursor(x, y);
  u8g2.print(s);

  u8g2.setFont(u8g2_font_5x7_t_cyrillic);
  u8g2.setCursor(X_OFF, Y_OFF + DISP_H - 1);
  u8g2.print("BOOT to STOP");

  u8g2.sendBuffer();
}

void drawInfoScreen1() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_5x7_t_cyrillic);

  int y = Y_OFF + INFO_Y_OFFSET;

  u8g2.setCursor(X_OFF, y);
  u8g2.print("INFO 1/2");
  y += 8;

  u8g2.setCursor(X_OFF, y);
  u8g2.printf("Day: %s", weekdayStr);
  y += 8;

  u8g2.setCursor(X_OFF, y);
  if (myAlarm.active) {
    u8g2.printf("Alarm: %02d:%02d", myAlarm.hour, myAlarm.minute);
  } else {
    u8g2.print("Alarm: OFF");
  }
  y += 8;

  u8g2.setCursor(X_OFF, y);
  if (timerActive) {
    uint64_t elapsed = esp_timer_get_time() - timerStartUs;
    uint64_t remaining = (elapsed >= timerDurationUs) ? 0 : (timerDurationUs - elapsed);
    int secRemaining = (remaining + 500000) / 1000000;
    u8g2.printf("Timer: %02d sec", secRemaining);
  } else {
    u8g2.print("Timer: OFF");
  }
  y += 8;

  u8g2.setCursor(X_OFF, y);
  u8g2.printf("WiFi: %s", WiFi.status() == WL_CONNECTED ? "ON" : "OFF");

  u8g2.sendBuffer();
}

void drawInfoScreen2() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_4x6_t_cyrillic);

  int y = Y_OFF + INFO_Y_OFFSET;

  u8g2.setCursor(X_OFF, y);
  u8g2.print("INFO 2/2");
  y += 7;

  u8g2.setCursor(X_OFF, y);
  String ssidDisplay = wifiSSID;
  if (ssidDisplay.length() > 14) {
    ssidDisplay = ssidDisplay.substring(0, 14) + "...";
  }
  u8g2.printf("SSID: %s", ssidDisplay.c_str());
  y += 7;

  u8g2.setCursor(X_OFF, y);
  if (WiFi.status() == WL_CONNECTED) {
    IPAddress ip = WiFi.localIP();
    u8g2.printf("IP: %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  } else {
    u8g2.print("IP: No connection");
  }
  y += 7;

  u8g2.setCursor(X_OFF, y);
  u8g2.printf("Time: %s", timeValid ? "SYNC" : "NO SYNC");
  y += 7;

  u8g2.setCursor(X_OFF, y);
  u8g2.printf("RAM: %d KB", esp_get_free_heap_size() / 1024);

  u8g2.sendBuffer();
}

// ================= SYSTEM FUNCTIONS =================
void showSplash() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_t_cyrillic);
  u8g2.setCursor(X_OFF + 6, Y_OFF + 20);
  u8g2.print("ESP32-C3");
  u8g2.setCursor(X_OFF + 20, Y_OFF + 36);
  u8g2.print("CLOCK");
  u8g2.sendBuffer();
  delay(1500);
}

void connectWiFi() {
  Serial.println("Connecting to WiFi...");
  Serial.println("SSID: " + wifiSSID);

  WiFi.disconnect(true);
  WiFi.begin(wifiSSID.c_str(), wifiPASS.c_str());

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start > 10000) {
      Serial.println("WiFi timeout");
      return;
    }
    delay(200);
  }

  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());
}

void syncTime() {
  timeValid = false;
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer.c_str());
  for (int i = 0; i < 30; i++) {
    if (getLocalTime(&timeinfo) && timeinfo.tm_year > 120) {
      timeValid = true;
      Serial.println("Time synced");
      return;
    }
    delay(500);
  }
  Serial.println("Time sync failed");
}

void setManualTime(String s) {
  struct tm t {};
  sscanf(s.c_str(), "%d-%d-%d %d:%d:%d",
         &t.tm_year, &t.tm_mon, &t.tm_mday,
         &t.tm_hour, &t.tm_min, &t.tm_sec);
  t.tm_year -= 1900;
  t.tm_mon -= 1;

  time_t tt = mktime(&t);
  struct timeval now = { tt, 0 };
  settimeofday(&now, nullptr);
  timeValid = true;
  updateClockStrings();
  Serial.println("Manual time set");
}

// ================= BUTTON HANDLER =================
void handleButton() {
  static bool btnPrev = HIGH;
  bool btnNow = digitalRead(BOOT_PIN);

  if (btnPrev == HIGH && btnNow == LOW) {
    if (millis() - lastBtnTime > BTN_DEBOUNCE) {
      if (alarmTriggered || timerTriggered) {
        // Stop alarm/timer
        if (alarmTriggered && !myAlarm.repeat) {
          myAlarm.active = false;
          if (myAlarm.saved) clearAlarmFromNVS();
        }
        if (timerTriggered) timerActive = false;

        alarmTriggered = false;
        timerTriggered = false;
        buzzerActive = false;
        digitalWrite(BUZZER_PIN, LOW);
        currentScreen = SCREEN_CLOCK;
        infoScreenPage = 1;
        updateAlarmIndicator();
        Serial.println("Signal stopped");
      } else if (currentScreen == SCREEN_CLOCK) {
        currentScreen = SCREEN_INFO1;
        infoScreenPage = 1;
        infoStartTime = millis();
        Serial.println("Showing Info Screen 1");
      } else if (currentScreen == SCREEN_INFO1) {
        currentScreen = SCREEN_INFO2;
        infoScreenPage = 2;
        infoStartTime = millis();
        Serial.println("Showing Info Screen 2");
      } else if (currentScreen == SCREEN_INFO2) {
        currentScreen = SCREEN_CLOCK;
        infoScreenPage = 1;
        Serial.println("Returning to Clock");
      }
      lastBtnTime = millis();
    }
  }
  btnPrev = btnNow;
}

// ================= ALARM CHECK =================
bool checkAlarmMatch() {
  if (!myAlarm.active || alarmTriggered) return false;

  // Check specific date
  if (myAlarm.year > 0) {
    if (timeinfo.tm_year + 1900 != myAlarm.year || timeinfo.tm_mon + 1 != myAlarm.month || timeinfo.tm_mday != myAlarm.day) {
      return false;
    }
  }
  // Check weekdays
  else if (myAlarm.weekdays > 0) {
    // Convert tm_wday (0=Sun,6=Sat) to our bitmask (0=Mon,6=Sun)
    int wday = timeinfo.tm_wday;
    if (wday == 0) wday = 6;  // Sunday becomes 6
    else wday -= 1;           // Mon=0, Tue=1, etc.

    if (!(myAlarm.weekdays & (1 << wday))) {
      return false;
    }
  }
  // Daily alarm - always match date-wise

  // Check time
  if (timeinfo.tm_hour != myAlarm.hour || timeinfo.tm_min != myAlarm.minute || timeinfo.tm_sec != 0) {
    return false;
  }

  return true;
}

void handleAutoReturn() {
  if ((currentScreen == SCREEN_INFO1 || currentScreen == SCREEN_INFO2) && millis() - infoStartTime > INFO_TIMEOUT) {
    currentScreen = SCREEN_CLOCK;
    infoScreenPage = 1;
  }
}

// ================= MAIN LOOP =================
void loop() {
  handleSerial();
  handleButton();
  handleAutoReturn();

  // Blinking colon
  if (millis() - lastBlink >= 500) {
    colonVisible = !colonVisible;
    lastBlink = millis();
  }

  // Update time
  if (getLocalTime(&timeinfo) && timeinfo.tm_year > 120) {
    timeValid = true;
  }

  // Alarm check (once per second)
  static unsigned long lastSec = 0;
  if (timeValid && millis() - lastSec >= 1000) {
    lastSec = millis();
    updateClockStrings();

    if (checkAlarmMatch()) {
      alarmTriggered = true;
      buzzerActive = true;
      currentScreen = SCREEN_ALARM;
      updateAlarmIndicator();
      Serial.println("ALARM TRIGGERED!");
    }
  }

  // Timer check
  if (timerActive && !timerTriggered) {
    uint64_t elapsed = esp_timer_get_time() - timerStartUs;
    if (elapsed >= timerDurationUs) {
      timerTriggered = true;
      buzzerActive = true;
      currentScreen = SCREEN_TIMER;
      Serial.println("TIMER TRIGGERED!");
    }
  }

  // Buzzer control - alarm pattern: beep-pause-beep-pause-beep-long pause
  if (buzzerActive) {
    uint64_t phase = esp_timer_get_time() % 2000000;  // 2 second cycle
    bool on = false;
    
    // First beep: 0-150ms
    if (phase < 150000) {
      on = true;
    }
    // Pause: 150-300ms
    // Second beep: 300-450ms
    else if (phase >= 300000 && phase < 450000) {
      on = true;
    }
    // Pause: 450-600ms
    // Third beep: 600-750ms
    else if (phase >= 600000 && phase < 750000) {
      on = true;
    }
    // Long pause: 750-2000ms
    
    digitalWrite(BUZZER_PIN, on ? HIGH : LOW);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  // Display
  switch (currentScreen) {
    case SCREEN_INFO1:
      drawInfoScreen1();
      delay(100);
      break;

    case SCREEN_INFO2:
      drawInfoScreen2();
      delay(100);
      break;

    case SCREEN_ALARM:
      if (alarmTriggered) {
        drawAlarmOrTimer(myAlarm.text);
        delay(20);
      } else {
        currentScreen = SCREEN_CLOCK;
      }
      break;

    case SCREEN_TIMER:
      if (timerTriggered) {
        drawAlarmOrTimer(timerText);
        delay(20);
      } else {
        currentScreen = SCREEN_CLOCK;
      }
      break;

    case SCREEN_CLOCK:
    default:
      if (timeValid) {
        drawClock();
      } else {
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_5x7_t_cyrillic);
        u8g2.setCursor(X_OFF, Y_OFF + 20);
        u8g2.print("NO TIME SYNC");
        u8g2.sendBuffer();
      }
      delay(50);
      break;
  }
}
