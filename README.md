# ESP32-C3 0.42" OLED Часы с WiFi и NTP / ESP32-C3 0.42" OLED Clock with WiFi and NTP

## Русский / Russian

### Описание
Проект реализует цифровые часы на базе ESP32-C3 с встроенным OLED экраном 0.42" (72x40), подключением к WiFi и синхронизацией времени через NTP. Все настройки сохраняются в энергонезависимой памяти (NVS).  

Функции:  
- Отображение текущего времени и даты  
- Информационные экраны:  
  1. SSID и IP адрес  
  2. Часовой пояс (GMT offset) и летнее время (DST)  
- Настройка через Serial: WiFi, NTP сервер, TZ/DST  
- Автоматический возврат на экран часов через 10 секунд  
- Поддержка Serial терминалов (Arduino IDE, Putty, TeraTerm)  

**Важно:** при старте платы возможен небольшой таймаут, если WiFi не удаётся подключить — в этом случае часы продолжат работать после завершения попыток подключения.  

---

### Аппаратная часть
- **Микроконтроллер:** ESP32-C3  
- **Экран:** встроенный 0.42" OLED 72x40 (SSD1306)  
- **BOOT кнопка (GPIO9)** — переключение информационных экранов  
- **Питание:** 3.3–5V  

---

### Библиотеки
Для сборки проекта необходимо подключить следующие библиотеки:  
- `U8g2` — для работы с OLED  
- `WiFi` — для подключения к сети WiFi  
- `time` — для синхронизации времени через NTP  
- `Preferences` — для хранения настроек в энергонезависимой памяти  

**Arduino IDE:**  
- Выберите плату **ESP32C3 Dev Module**  
- Убедитесь, что выбран **порт USB в режиме CDC (Serial / COM)** для работы с Serial интерфейсом.

---

### Настройка и команды Serial
Скорость Serial: **115200**  
После подключения терминала появляется prompt: `>`  

**Команды:**

| Команда | Описание |
|---------|----------|
| HELP | Список доступных команд |
| TIME YYYY-MM-DD HH:MM:SS | Установка времени вручную |
| WIFI <SSID> <PASS> | Настройка WiFi сети |
| NTP <SERVER> | Установка NTP сервера |
| TZ <OFFSET_SEC> | Установка часового пояса (GMT offset в секундах) |
| DST <OFFSET_SEC> | Установка летнего времени (DST offset в секундах) |
| SAVE | Сохранение текущих настроек в энергонезависимую память |
| RESTORE | Восстановление настроек из энергонезависимой памяти |
| ERASE | Очистка настроек из энергонезависимой памяти |
| STATUS | Отображение текущих настроек |
| REBOOT | Перезагрузка платы |

---

## English

### Description
This project implements a digital clock on ESP32-C3 with a built-in 0.42" OLED screen (72x40), WiFi connectivity, and NTP time synchronization. All settings are stored in non-volatile memory (NVS).  

Features:  
- Display current time and date  
- Information screens:  
  1. WiFi SSID and IP address  
  2. Timezone (GMT offset) and Daylight Saving Time (DST)  
- Configuration via Serial: WiFi, NTP server, TZ/DST  
- Automatic return to clock screen after 10 seconds  
- Serial terminal support (Arduino IDE, Putty, TeraTerm)  

**Note:** On startup, a small delay may occur if the board cannot connect to WiFi — the clock will continue working after timeout.  

---

### Hardware
- **Microcontroller:** ESP32-C3  
- **Display:** built-in 0.42" OLED 72x40 (SSD1306)  
- **BOOT button (GPIO9)** — switch information screens  
- **Power supply:** 3.3–5V  

---

### Required Libraries
- `U8g2` — for OLED display  
- `WiFi` — for WiFi connectivity  
- `time` — for NTP synchronization  
- `Preferences` — for storing settings in non-volatile memory  

**Arduino IDE:**  
- Select board **ESP32C3 Dev Module**  
- Make sure the USB port is in **CDC (Serial / COM) mode** for Serial communication

---

### Serial Setup and Commands
Serial baud rate: **115200**  
After connecting to a terminal, the prompt appears: `>`  

**Commands:**

| Command | Description |
|---------|-------------|
| HELP | List available commands |
| TIME YYYY-MM-DD HH:MM:SS | Set time manually |
| WIFI <SSID> <PASS> | Configure WiFi network |
| NTP <SERVER> | Set NTP server |
| TZ <OFFSET_SEC> | Set timezone (GMT offset in seconds) |
| DST <OFFSET_SEC> | Set daylight saving time (DST offset in seconds) |
| SAVE | Save current settings to non-volatile memory |
| RESTORE | Restore settings from non-volatile memory |
| ERASE | Erase settings from non-volatile memory |
| STATUS | Show current settings |
| REBOOT | Reboot the board |
