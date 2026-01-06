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

---

### Аппаратная часть
- **Микроконтроллер:** ESP32-C3  
- **Экран:** встроенный 0.42" OLED 72x40 (SSD1306)  
- **Кнопка BOOT (GPIO9)** — переключение информационных экранов  
- **Питание:** 3.3–5V  

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

---

### Hardware
- **Microcontroller:** ESP32-C3  
- **Display:** built-in 0.42" OLED 72x40 (SSD1306)  
- **BOOT button (GPIO9)** — switch information screens  
- **Power supply:** 3.3–5V  

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
