# ESP32-C3 0.42" OLED Часы с WiFi, NTP, будильником и таймером
# ESP32-C3 0.42" OLED Clock with WiFi, NTP, Alarm and Timer

![ESP32-C3 0.42" OLED](https://github.com/CheshirCa/ESP32-C3-Clock/blob/main/ESP32-C3-Clock.jpg)
![ESP32-C3 0.42" OLED with buzzer HCM1203X pinout](https://github.com/CheshirCa/ESP32-C3-Clock/blob/main/ESP32-C3-Buzzer.jpg)

[English version below](#english-version) | [Русская версия](#Русская-версия)

---

## Русская версия

### 📋 Описание

Проект часов на базе **ESP32-C3 SuperMini с встроенным OLED-дисплеем 0.42" (72x40 пикселей)**, поддержкой WiFi, NTP-синхронизацией времени, будильником и таймером. Устройство сохраняет настройки в энергонезависимой памяти (NVS) и управляется через серийный терминал и встроенную кнопку BOOT.

### ✨ Возможности

- ⏰ **Часы реального времени** с синхронизацией через NTP
- 📡 **WiFi подключение** с сохранением настроек
- 🔔 **Гибкий будильник:**
  - Ежедневный
  - По дням недели (Пн-Вс, любая комбинация)
  - На конкретную дату
  - С текстовым сообщением (до 10 символов)
  - Повторяющийся или одноразовый
  - Сохранение в NVS
- ⏲️ **Таймер обратного отсчета:**
  - От 1 секунды до 24 часов
  - С текстовым сообщением
  - Форматы: HH:MM:SS, MM:SS или SS
- 💾 **Энергонезависимое хранение** настроек WiFi, часового пояса и будильника
- 🔵 **Светодиодная индикация** активного будильника
- 🔊 **Звуковые сигналы** пьезоизлучателем
- 📟 **Информационные экраны** с детальной статистикой

### 🔧 Аппаратная часть

**Плата:** ESP32-C3 SuperMini с встроенным OLED 0.42" (72x40)

**Характеристики платы:**
- Процессор: ESP32-C3 RISC-V @ 160MHz
- ОЗУ: 400 KB SRAM
- Flash: 4 MB
- Встроенный OLED дисплей 0.42" (72x40 пикселей)
- Встроенная кнопка BOOT (GPIO9)
- Встроенный синий светодиод (GPIO8)
- USB Type-C для питания и программирования

**Распиновка встроенных компонентов:**

| Компонент | GPIO | Примечание |
|-----------|------|-----------|
| OLED SDA | GPIO5 | I2C Data |
| OLED SCL | GPIO6 | I2C Clock |
| Кнопка BOOT | GPIO9 | С внутренним подтягивающим резистором |
| Синий LED | GPIO8 | Активный низкий уровень |
| Зуммер HCM1203X | GPIO10 | Активный пьезоизлучатель |

**⚠️ ВАЖНО: Подключение зуммера HCM1203X**
- Это **активный** пьезоизлучатель (со встроенным генератором)
- Подключение: GPIO10 (pin 16) → "+" зуммера, GND (pin 2) → "-" зуммера
- Напряжение: 3.3V
- Потребление: ~30 мА
- **Полярность критична!** Если не работает - поменяйте местами контакты

### 📚 Необходимые библиотеки

Установите через Arduino Library Manager:

1. **U8g2** by olikraus (для OLED дисплея)
2. **WiFi** (встроенная в ESP32)
3. **Preferences** (встроенная в ESP32)

### 🚀 Быстрый старт

1. **Настройте WiFi в коде** (или через Serial после загрузки):
   ```cpp
   String defSSID = "your_SSID";        // Ваш WiFi SSID
   String defPASS = "your_PASSWORD";    // Ваш WiFi пароль
   ```

2. **Настройте часовой пояс** (по умолчанию GMT+3):
   ```cpp
   long defGMTOffset = 3 * 3600;        // GMT+3 (Москва)
   long defDaylightOffset = 0;          // Летнее время (0 = выкл)
   ```

3. **Загрузите скетч** на ESP32-C3

4. **Откройте Serial Monitor** (115200 baud, NL&CR)

5. **Проверьте подключение:**
   ```
   STATUS
   ```

### 💻 Команды Serial-терминала

**Подключение:** 115200 baud, Newline & Carriage Return (NL&CR)

#### Справка и статус
```
HELP                    - Показать все доступные команды
STATUS                  - Показать детальный статус системы
SYNC                    - Принудительная синхронизация времени с NTP
REBOOT                  - Перезагрузить устройство
```

#### Настройка WiFi
```
WIFI <SSID> <пароль>    - Установить параметры WiFi
SAVE                    - Сохранить настройки в NVS
RESTORE                 - Загрузить настройки из NVS
ERASE                   - Очистить все настройки в NVS
```

**Примеры:**
```
WIFI MyHomeWiFi password123
SAVE
```

#### Настройка времени
```
TIME YYYY-MM-DD HH:MM:SS - Установить время вручную
NTP <сервер>             - Изменить NTP-сервер (по умолчанию: pool.ntp.org)
TZ <±часы>               - Установить часовой пояс
DST <±часы>              - Установить смещение летнего времени
```

**Примеры:**
```
TIME 2025-01-15 14:30:00
NTP time.google.com
TZ +3                     # GMT+3 (Москва)
TZ -5                     # GMT-5 (Нью-Йорк)
DST +1                    # Летнее время +1 час
```

#### Будильник

**Формат команды:**
```
ALARM [дата/дни] HH:MM [ТЕКСТ] [R] [S]
```

**Параметры:**
- `HH:MM` - время срабатывания (обязательно)
- `YYYY-MM-DD` - конкретная дата (опционально)
- `1234567` - дни недели: 1=Пн, 2=Вт, 3=Ср, 4=Чт, 5=Пт, 6=Сб, 7=Вс (опционально)
- `ТЕКСТ` - текст сообщения до 10 символов (опционально)
- `R` - повторять после срабатывания (опционально)
- `S` - сохранить в NVS (опционально)

**Примеры:**

```bash
# Ежедневный будильник в 07:00
ALARM 07:00

# Будильник в 08:30 с текстом "Work"
ALARM 08:30 Work

# Будильник на конкретную дату
ALARM 2025-12-31 23:59 NewYear

# Будильник по будням (Пн-Пт) в 07:00, повторяющийся, с сохранением
ALARM 12345 07:00 Wake R S

# Будильник на выходные (Сб-Вс)
ALARM 67 10:00 Weekend

# Только в понедельник
ALARM 1 09:00 Monday

# Очистить будильник
ALARM CLEAR
```

**Важно:**
- Если начинается с цифр 1-7 БЕЗ двоеточия, то это дни недели
- Если начинается с времени (например, 17:00), то это ежедневный будильник
- Флаги `R` и `S` - отдельные, не регистрозависимые буквы
- Текст не должен начинаться с букв `R` или `S` (иначе будет воспринят как флаг)

#### Таймер

**Форматы команды:**
```
TIMER HH:MM:SS [ТЕКСТ]   - Часы:Минуты:Секунды
TIMER MM:SS [ТЕКСТ]      - Минуты:Секунды
TIMER SS [ТЕКСТ]         - Секунды
TIMER CLEAR              - Очистить таймер
```

**Примеры:**
```bash
# 5 минут
TIMER 5:00
TIMER 300

# 1 час 30 минут
TIMER 1:30:00
TIMER 90:00

# 3 часа с текстом
TIMER 3:00:00 Meeting

# 45 секунд
TIMER 45

# Очистить таймер
TIMER CLEAR
```

**Ограничения:**
- Минимум: 1 секунда
- Максимум: 24 часа (86400 секунд)

### 🖥️ Работа с дисплеем

#### Главный экран
```
  ┌──────────────┐
  │   14:30      │  ← Время (двоеточие мигает)
  │ 15.01.2025 * │  ← Дата, * = будильник активен
  └──────────────┘
```

**Индикаторы:**
- `*` - активен будильник
- `#` - активен таймер

#### Info Screen 1 (нажмите BOOT 1 раз)
```
INFO 1/2
Day: Wed
Alarm: 07:00
Timer: 45 sec
WiFi: ON
```

#### Info Screen 2 (нажмите BOOT 2 раза)
```
INFO 2/2
SSID: MyHomeWiFi
IP: 192.168.1.100
Time: SYNC
RAM: 285 KB
```

**Автовозврат:** Через 10 секунд автоматически возвращается к главному экрану

#### Экран срабатывания

Когда срабатывает будильник или таймер:
```
  ┌──────────────┐
  │    ALARM     │  ← или текст будильника/таймера
  │              │
  │ BOOT to STOP │  ← Нажмите BOOT для остановки
  └──────────────┘
```

### 🎮 Управление кнопкой BOOT

| Экран | Действие | Результат |
|-------|----------|-----------|
| Главный экран | 1x нажатие | Info Screen 1 |
| Info Screen 1 | 1x нажатие | Info Screen 2 |
| Info Screen 2 | 1x нажатие | Возврат на главный экран |
| Будильник/Таймер | 1x нажатие | Остановить сигнал |

### 💡 Индикация синим светодиодом

- **Горит** (активный LOW) - будильник установлен и активен
- **Не горит** - будильник выключен или сработал

### 🔊 Работа зуммера

**Паттерн сигнала при срабатывании:**
- 1 секунда ВКЛ
- 1 секунда ВЫКЛ
- 1 секунда ВКЛ
- 2 секунды ВЫКЛ
- Повтор...

**Остановка:** Нажмите кнопку BOOT

### 💾 Хранение данных в NVS

**Автоматически сохраняются:**
- Параметры WiFi (при использовании команды SAVE)
- Настройки часового пояса (при использовании команды SAVE)
- Настройки NTP сервера (при использовании команды SAVE)

**Сохранение будильника:**
- Используйте флаг `S` при установке будильника
- Или сохраните текущую конфигурацию командой SAVE

**Примеры:**
```bash
# Установить будильник и сохранить
ALARM 07:00 Work R S

# Настроить WiFi и сохранить
WIFI MyNetwork password123
SAVE

# Очистить все настройки
ERASE
```

### 📊 Примеры использования

#### Будильник на работу (Пн-Пт, 7:00, повторяющийся)
```bash
# Подключение к Serial Monitor (115200 baud)
> ALARM 12345 07:00 Work R S
Alarm set for Weekdays: Mon Tue Wed Thu Fri 07:00 [R] [S] 'Work'
Alarm saved to NVS

> STATUS
=== SYSTEM STATUS ===
...
Alarm: Weekdays: MonTueWedThuFri 07:00 [R] [S] 'Work'
...
```

#### Таймер для заварки чая (3 минуты)
```bash
> TIMER 3:00 Tea
Timer set for 03:00 seconds, text='Tea'
```

#### Будильник на Новый Год
```bash
> ALARM 2025-12-31 23:59 NewYear S
Alarm set for 2025-12-31 23:59 [S] 'NewYear'
Alarm saved to NVS
```

#### Настройка для другого часового пояса
```bash
# Лондон (GMT+0)
> TZ 0
GMT offset set to +0 hours

# Нью-Йорк (GMT-5)
> TZ -5
GMT offset set to -5 hours

# Токио (GMT+9)
> TZ +9
GMT offset set to +9 hours

# Сохранить настройки
> SAVE
Config saved
```

### 🐛 Устранение неисправностей

#### Проблема: Зуммер не работает
**Решение:**
1. Проверьте полярность подключения HCM1203X
2. GPIO10 должен быть подключен к "+" зуммера
3. GND должен быть подключен к "-" зуммера
4. Если не работает - попробуйте поменять контакты местами
5. Проверьте, что используется активный зуммер (со встроенным генератором)

#### Проблема: Дисплей не работает или показывает мусор
**Решение:**
1. Проверьте подключение I2C (SDA=GPIO5, SCL=GPIO6)
2. Убедитесь, что библиотека U8g2 установлена
3. Проверьте напряжение питания (должно быть 3.3V)

#### Проблема: Не подключается к WiFi
**Решение:**
1. Проверьте SSID и пароль
2. Убедитесь, что используется WiFi 2.4 GHz (ESP32-C3 не поддерживает 5 GHz)
3. Используйте команду `WIFI SSID password` для перенастройки
4. Проверьте доступность сети командой `STATUS`

#### Проблема: Время не синхронизируется
**Решение:**
1. Проверьте подключение к интернету
2. Используйте команду `SYNC` для принудительной синхронизации
3. Попробуйте другой NTP сервер: `NTP time.google.com`
4. Проверьте настройки часового пояса: `TZ +3`

#### Проблема: Будильник срабатывает в неправильное время
**Решение:**
1. Проверьте часовой пояс командой `STATUS`
2. Установите правильный часовой пояс: `TZ +3`
3. Проверьте настройку летнего времени: `DST 0`
4. Сохраните настройки: `SAVE`

#### Проблема: Настройки не сохраняются после перезагрузки
**Решение:**
1. Используйте команду `SAVE` после изменения настроек
2. Для будильника используйте флаг `S`: `ALARM 07:00 S`
3. Проверьте, что NVS не повреждена: `ERASE`, затем заново настройте

### 📝 Известные особенности

1. **WiFi 5 GHz не поддерживается** - ESP32-C3 работает только с 2.4 GHz
2. **Максимальная длина текста** будильника/таймера - 10 символов
3. **Один активный будильник** - можно установить только один будильник одновременно
4. **Один активный таймер** - можно установить только один таймер одновременно
5. **Автовозврат** с информационных экранов через 10 секунд
6. **Синхронизация времени** происходит при старте и может быть вызвана командой `SYNC`

### 📧 Контакты

GitHub: [@CheshirCa](https://github.com/CheshirCa)

---

## English Version

### 📋 Description

Clock project based on **ESP32-C3 SuperMini with built-in 0.42" OLED display (72x40 pixels)**, featuring WiFi support, NTP time synchronization, alarm, and timer functions. The device stores settings in non-volatile storage (NVS) and can be controlled via serial terminal and built-in BOOT button.

### ✨ Features

- ⏰ **Real-time clock** with NTP synchronization
- 📡 **WiFi connection** with persistent settings
- 🔔 **Flexible alarm:**
  - Daily
  - By weekdays (Mon-Sun, any combination)
  - Specific date
  - With text message (up to 10 characters)
  - Repeating or one-time
  - NVS storage
- ⏲️ **Countdown timer:**
  - From 1 second to 24 hours
  - With text message
  - Formats: HH:MM:SS, MM:SS or SS
- 💾 **Non-volatile storage** of WiFi, timezone and alarm settings
- 🔵 **LED indicator** for active alarm
- 🔊 **Sound alerts** via piezo buzzer
- 📟 **Info screens** with detailed statistics

### 🔧 Hardware

**Board:** ESP32-C3 SuperMini with built-in 0.42" OLED (72x40)

**Board specifications:**
- Processor: ESP32-C3 RISC-V @ 160MHz
- RAM: 400 KB SRAM
- Flash: 4 MB
- Built-in 0.42" OLED display (72x40 pixels)
- Built-in BOOT button (GPIO9)
- Built-in blue LED (GPIO8)
- USB Type-C for power and programming

**Built-in components pinout:**

| Component | GPIO | Note |
|-----------|------|------|
| OLED SDA | GPIO5 | I2C Data |
| OLED SCL | GPIO6 | I2C Clock |
| BOOT button | GPIO9 | Internal pull-up |
| Blue LED | GPIO8 | Active low |
| HCM1203X Buzzer | GPIO10 | Active piezo buzzer |

**⚠️ IMPORTANT: HCM1203X Buzzer Connection**
- This is an **active** piezo buzzer (with built-in generator)
- Connection: GPIO10 (pin 16) → "+" buzzer, GND (pin 2) → "-" buzzer
- Voltage: 3.3V
- Current: ~30 mA
- **Polarity is critical!** If not working - reverse the connections

### 📚 Required Libraries

Install via Arduino Library Manager:

1. **U8g2** by olikraus (for OLED display)
2. **WiFi** (built-in for ESP32)
3. **Preferences** (built-in for ESP32)

### 🚀 Quick Start

1. **Configure WiFi in code** (or via Serial after upload):
   ```cpp
   String defSSID = "your_SSID";        // Your WiFi SSID
   String defPASS = "your_PASSWORD";    // Your WiFi password
   ```

2. **Configure timezone** (default GMT+3):
   ```cpp
   long defGMTOffset = 3 * 3600;        // GMT+3 (Moscow)
   long defDaylightOffset = 0;          // Daylight saving (0 = off)
   ```

3. **Upload sketch** to ESP32-C3

4. **Open Serial Monitor** (115200 baud, NL&CR)

5. **Check connection:**
   ```
   STATUS
   ```

### 💻 Serial Terminal Commands

**Connection:** 115200 baud, Newline & Carriage Return (NL&CR)

#### Help and Status
```
HELP                    - Show all available commands
STATUS                  - Show detailed system status
SYNC                    - Force NTP time synchronization
REBOOT                  - Reboot device
```

#### WiFi Settings
```
WIFI <SSID> <password>  - Set WiFi credentials
SAVE                    - Save settings to NVS
RESTORE                 - Restore settings from NVS
ERASE                   - Erase all settings from NVS
```

**Examples:**
```
WIFI MyHomeWiFi password123
SAVE
```

#### Time Settings
```
TIME YYYY-MM-DD HH:MM:SS - Set time manually
NTP <server>             - Change NTP server (default: pool.ntp.org)
TZ <±hours>              - Set timezone
DST <±hours>             - Set daylight saving offset
```

**Examples:**
```
TIME 2025-01-15 14:30:00
NTP time.google.com
TZ +3                     # GMT+3 (Moscow)
TZ -5                     # GMT-5 (New York)
DST +1                    # Daylight saving +1 hour
```

#### Alarm

**Command format:**
```
ALARM [date/days] HH:MM [TEXT] [R] [S]
```

**Parameters:**
- `HH:MM` - trigger time (required)
- `YYYY-MM-DD` - specific date (optional)
- `1234567` - weekdays: 1=Mon, 2=Tue, 3=Wed, 4=Thu, 5=Fri, 6=Sat, 7=Sun (optional)
- `TEXT` - message text up to 10 characters (optional)
- `R` - repeat after trigger (optional)
- `S` - save to NVS (optional)

**Examples:**

```bash
# Daily alarm at 07:00
ALARM 07:00

# Alarm at 08:30 with text "Work"
ALARM 08:30 Work

# Alarm on specific date
ALARM 2025-12-31 23:59 NewYear

# Weekday alarm (Mon-Fri) at 07:00, repeating, saved
ALARM 12345 07:00 Wake R S

# Weekend alarm (Sat-Sun)
ALARM 67 10:00 Weekend

# Monday only
ALARM 1 09:00 Monday

# Clear alarm
ALARM CLEAR
```

**Important:**
- If starts with digits 1-7 WITHOUT colon, it's weekdays
- If starts with time (e.g., 17:00), it's daily alarm
- Flags `R` and `S` are separate, case-insensitive letters
- Text should not start with letters `R` or `S` (will be treated as flag)

#### Timer

**Command formats:**
```
TIMER HH:MM:SS [TEXT]   - Hours:Minutes:Seconds
TIMER MM:SS [TEXT]      - Minutes:Seconds
TIMER SS [TEXT]         - Seconds
TIMER CLEAR             - Clear timer
```

**Examples:**
```bash
# 5 minutes
TIMER 5:00
TIMER 300

# 1 hour 30 minutes
TIMER 1:30:00
TIMER 90:00

# 3 hours with text
TIMER 3:00:00 Meeting

# 45 seconds
TIMER 45

# Clear timer
TIMER CLEAR
```

**Limits:**
- Minimum: 1 second
- Maximum: 24 hours (86400 seconds)

### 🖥️ Display Operation

#### Main Screen
```
  ┌──────────────┐
  │   14:30      │  ← Time (colon blinks)
  │ 15.01.2025 * │  ← Date, * = alarm active
  └──────────────┘
```

**Indicators:**
- `*` - alarm is active
- `#` - timer is active

#### Info Screen 1 (press BOOT once)
```
INFO 1/2
Day: Wed
Alarm: 07:00
Timer: 45 sec
WiFi: ON
```

#### Info Screen 2 (press BOOT twice)
```
INFO 2/2
SSID: MyHomeWiFi
IP: 192.168.1.100
Time: SYNC
RAM: 285 KB
```

**Auto-return:** Returns to main screen after 10 seconds automatically

#### Trigger Screen

When alarm or timer triggers:
```
  ┌──────────────┐
  │    ALARM     │  ← or alarm/timer text
  │              │
  │ BOOT to STOP │  ← Press BOOT to stop
  └──────────────┘
```

### 🎮 BOOT Button Control

| Screen | Action | Result |
|--------|--------|--------|
| Main screen | 1x press | Info Screen 1 |
| Info Screen 1 | 1x press | Info Screen 2 |
| Info Screen 2 | 1x press | Return to main screen |
| Alarm/Timer | 1x press | Stop signal |

### 💡 Blue LED Indication

- **ON** (active LOW) - alarm is set and active
- **OFF** - alarm is disabled or triggered

### 🔊 Buzzer Operation

**Signal pattern when triggered:**
- 1 second ON
- 1 second OFF
- 1 second ON
- 2 seconds OFF
- Repeat...

**Stop:** Press BOOT button

### 💾 NVS Data Storage

**Automatically saved:**
- WiFi credentials (when using SAVE command)
- Timezone settings (when using SAVE command)
- NTP server settings (when using SAVE command)

**Saving alarm:**
- Use flag `S` when setting alarm
- Or save current configuration with SAVE command

**Examples:**
```bash
# Set alarm and save
ALARM 07:00 Work R S

# Configure WiFi and save
WIFI MyNetwork password123
SAVE

# Clear all settings
ERASE
```

### 📊 Usage Examples

#### Work alarm (Mon-Fri, 7:00, repeating)
```bash
# Connect to Serial Monitor (115200 baud)
> ALARM 12345 07:00 Work R S
Alarm set for Weekdays: Mon Tue Wed Thu Fri 07:00 [R] [S] 'Work'
Alarm saved to NVS

> STATUS
=== SYSTEM STATUS ===
...
Alarm: Weekdays: MonTueWedThuFri 07:00 [R] [S] 'Work'
...
```

#### Tea timer (3 minutes)
```bash
> TIMER 3:00 Tea
Timer set for 03:00 seconds, text='Tea'
```

#### New Year alarm
```bash
> ALARM 2025-12-31 23:59 NewYear S
Alarm set for 2025-12-31 23:59 [S] 'NewYear'
Alarm saved to NVS
```

#### Configure for different timezone
```bash
# London (GMT+0)
> TZ 0
GMT offset set to +0 hours

# New York (GMT-5)
> TZ -5
GMT offset set to -5 hours

# Tokyo (GMT+9)
> TZ +9
GMT offset set to +9 hours

# Save settings
> SAVE
Config saved
```

### 🐛 Troubleshooting

#### Issue: Buzzer not working
**Solution:**
1. Check HCM1203X polarity
2. GPIO10 should be connected to "+" of buzzer
3. GND should be connected to "-" of buzzer
4. If not working - try reversing the connections
5. Verify you're using active buzzer (with built-in generator)

#### Issue: Display not working or shows garbage
**Solution:**
1. Check I2C connections (SDA=GPIO5, SCL=GPIO6)
2. Ensure U8g2 library is installed
3. Check power supply voltage (should be 3.3V)

#### Issue: Cannot connect to WiFi
**Solution:**
1. Verify SSID and password
2. Ensure you're using 2.4 GHz WiFi (ESP32-C3 doesn't support 5 GHz)
3. Use `WIFI SSID password` command to reconfigure
4. Check network availability with `STATUS` command

#### Issue: Time not synchronizing
**Solution:**
1. Check internet connection
2. Use `SYNC` command to force synchronization
3. Try different NTP server: `NTP time.google.com`
4. Check timezone settings: `TZ +3`

#### Issue: Alarm triggers at wrong time
**Solution:**
1. Check timezone with `STATUS` command
2. Set correct timezone: `TZ +3`
3. Check daylight saving setting: `DST 0`
4. Save settings: `SAVE`

#### Issue: Settings not persisting after reboot
**Solution:**
1. Use `SAVE` command after changing settings
2. For alarm use `S` flag: `ALARM 07:00 S`
3. Check NVS is not corrupted: `ERASE`, then reconfigure

### 📝 Known Limitations

1. **WiFi 5 GHz not supported** - ESP32-C3 only works with 2.4 GHz
2. **Maximum text length** for alarm/timer - 10 characters
3. **Single active alarm** - only one alarm can be set at a time
4. **Single active timer** - only one timer can be set at a time
5. **Auto-return** from info screens after 10 seconds
6. **Time synchronization** occurs at startup and can be triggered with `SYNC` command

### 🎯 Advanced Usage Tips

#### Creating Complex Alarm Schedules

Since only one alarm can be active, use these strategies:

**Weekday morning + Weekend:**
```bash
# Monday morning - Friday morning
ALARM 12345 07:00 Work R S

# When weekend comes, manually set:
ALARM 67 10:00 Weekend
```

**Monthly reminders:**
```bash
# Set specific date alarm
ALARM 2025-02-01 09:00 Rent S

# After trigger, set next month
ALARM 2025-03-01 09:00 Rent S
```

#### Using Timer for Pomodoro Technique

```bash
# Work session (25 minutes)
TIMER 25:00 Work

# After work, set break (5 minutes)
TIMER 5:00 Break

# Long break (15 minutes)
TIMER 15:00 LongBreak
```

#### Multiple NTP Servers for Reliability

If primary NTP server fails:
```bash
NTP pool.ntp.org        # Primary
NTP time.google.com     # Backup
NTP time.cloudflare.com # Alternative
```

### 🔐 Security Considerations

1. **WiFi credentials** are stored in plain text in NVS
2. **Serial console** is unprotected - anyone with USB access can control the device
3. For production use, consider:
   - Adding authentication to serial commands
   - Encrypting sensitive data in NVS
   - Disabling serial console in final builds

### 📚 Technical Details

#### Memory Usage

Typical memory footprint:
- **Flash:** ~1.2 MB (program code)
- **SRAM:** ~115 KB used, ~285 KB free
- **NVS:** ~1-2 KB (settings storage)

#### Power Consumption

Approximate values:
- **Idle with WiFi:** ~80 mA @ 3.3V
- **Display active:** +20 mA
- **Buzzer active:** +30 mA
- **Deep sleep:** Not implemented (device always on)

#### I2C Bus Speed

- Configured at 400 kHz (fast mode) for OLED display
- Can be reduced to 100 kHz if experiencing issues

### 📖 Code Structure

```
ESP32-C3-Clock.ino
├── Setup & Configuration
│   ├── Hardware initialization
│   ├── WiFi connection
│   └── NTP synchronization
│
├── NVS Functions
│   ├── loadConfigFromNVS()
│   ├── saveConfigToNVS()
│   ├── loadAlarmFromNVS()
│   └── saveAlarmToNVS()
│
├── Serial Command Handler
│   ├── handleSerial()
│   └── Command parsing
│
├── Time Functions
│   ├── updateClockStrings()
│   ├── syncTime()
│   └── setManualTime()
│
├── Display Functions
│   ├── drawClock()
│   ├── drawAlarmOrTimer()
│   ├── drawInfoScreen1()
│   └── drawInfoScreen2()
│
├── Alarm & Timer Logic
│   ├── checkAlarmMatch()
│   └── Timer countdown
│
└── Main Loop
    ├── Serial handling
    ├── Button handling
    ├── Time updates
    ├── Alarm/Timer checks
    └── Display updates
```

### ⭐ Star History

If you find this project helpful, please consider giving it a star on GitHub!

---
