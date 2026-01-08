# ESP32-C3 0.42" OLED Часы с WiFi, NTP, будильником и таймером / ESP32-C3 0.42" OLED Clock with WiFi, NTP, alarm and timer

![ESP32-C3 0.42" OLED](https://github.com/CheshirCa/ESP32-C3-Clock/blob/main/ESP32-C3-Clock.jpg)
![ESP32-C3 0.42" OLED with buzzer HCM1203X pinout](https://github.com/CheshirCa/ESP32-C3-Clock/blob/main/ESP32-C3-Buzzer.jpg)

## Описание / Description

### Русский
Проект часов на базе **ESP32-C3 SuperMini с встроенным OLED-дисплеем 0.42" 72x40 пикселей**, поддержкой WiFi, NTP-синхронизацией, будильником и таймером. Устройство сохраняет настройки в энергонезависимой памяти (NVS) и управляется через серийный терминал и встроенную кнопку BOOT.

**Особенности платы:**
- ESP32-C3 RISC-V процессор
- Встроенный OLED дисплей 0.42" (72x40 пикселей)
- Встроенная кнопка BOOT (GPIO9)
- Пьезоизлучатель HCM1203X (GPIO10)
- Встроенный синий светодиод (GPIO8)
- USB Type-C для питания и программирования

**ВАЖНО:** Пьезоизлучатель HCM1203X подключен напрямую между GND (2-й пин) и GPIO10 (16-й пин). Это активный зуммер, который требует правильной полярности.

### English
Clock project based on **ESP32-C3 SuperMini with built-in 0.42" 72x40 OLED display**, featuring WiFi support, NTP synchronization, alarm, and timer functions. The device stores settings in non-volatile storage (NVS) and can be controlled via serial terminal and built-in BOOT button.

**Board features:**
- ESP32-C3 RISC-V processor
- Built-in 0.42" OLED display (72x40 pixels)
- Built-in BOOT button (GPIO9)
- Piezo buzzer HCM1203X (GPIO10)
- Built-in blue LED (GPIO8)
- USB Type-C for power and programming

**IMPORTANT:** The HCM1203X piezo buzzer is directly connected between GND (pin 2) and GPIO10 (pin 16). This is an active buzzer that requires correct polarity.

## Команды управления через Serial-терминал / Serial Terminal Commands

### Русский
**Подключение:** Serial-терминал 115200 бод, NL&CR

#### Основные команды:
```
HELP                    - Показать все команды
STATUS                  - Показать статус системы
SYNC                    - Принудительная синхронизация времени
REBOOT                  - Перезагрузка устройства
```

#### Настройка времени:
```
TIME YYYY-MM-DD HH:MM:SS  - Установить время вручную
NTP <сервер>              - Установить NTP-сервер (по умолчанию: pool.ntp.org)
TZ <±часы>                - Установить часовой пояс (например: TZ +3)
DST <±часы>               - Установить смещение летнего времени
```

#### Настройка WiFi:
```
WIFI <SSID> <пароль>      - Установить параметры WiFi
```

#### Будильник:
```
ALARM HH:MM [ТЕКСТ] [R] [S]           - Ежедневный будильник
ALARM 1234567 HH:MM [ТЕКСТ] [R] [S]   - Будильник по дням недели (1=Пн,7=Вс)
ALARM YYYY-MM-DD HH:MM [ТЕКСТ] [R] [S] - Будильник на конкретную дату
ALARM CLEAR                           - Очистить будильник

Опции:
  R - Повторять после срабатывания
  S - Сохранить в NVS
  ТЕКСТ - Текст до 10 символов
```

#### Таймер:
```
TIMER HH:MM:SS [ТЕКСТ]    - Таймер на часы:минуты:секунды
TIMER MM:SS [ТЕКСТ]       - Таймер на минуты:секунды
TIMER SS [ТЕКСТ]          - Таймер на секунды
TIMER CLEAR               - Очистить таймер
```

#### Сохранение настроек:
```
SAVE                      - Сохранить настройки в NVS
RESTORE                   - Загрузить настройки из NVS
ERASE                     - Очистить все настройки в NVS
```

### English
**Connection:** Serial terminal 115200 baud, NL&CR

#### Basic Commands:
```
HELP                    - Show all commands
STATUS                  - Show system status
SYNC                    - Force time synchronization
REBOOT                  - Reboot device
```

#### Time Settings:
```
TIME YYYY-MM-DD HH:MM:SS  - Set time manually
NTP <server>              - Set NTP server (default: pool.ntp.org)
TZ <±hours>               - Set timezone (e.g.: TZ +3)
DST <±hours>              - Set daylight saving offset
```

#### WiFi Settings:
```
WIFI <SSID> <password>    - Set WiFi credentials
```

#### Alarm:
```
ALARM HH:MM [TEXT] [R] [S]           - Daily alarm
ALARM 1234567 HH:MM [TEXT] [R] [S]   - Weekday alarm (1=Mon,7=Sun)
ALARM YYYY-MM-DD HH:MM [TEXT] [R] [S] - Specific date alarm
ALARM CLEAR                           - Clear alarm

Options:
  R - Repeat after triggering
  S - Save to NVS
  TEXT - Text up to 10 characters
```

#### Timer:
```
TIMER HH:MM:SS [TEXT]    - Timer for hours:minutes:seconds
TIMER MM:SS [TEXT]       - Timer for minutes:seconds
TIMER SS [TEXT]          - Timer for seconds
TIMER CLEAR              - Clear timer
```

#### Settings Storage:
```
SAVE                      - Save settings to NVS
RESTORE                   - Restore settings from NVS
ERASE                     - Erase all settings from NVS
```

## Индикация на дисплее / Display Indicators

### Русский
**Главный экран:**
- Время с мигающим двоеточием (пример: `14:30`)
- Дата в формате ДД.ММ.ГГГГ
- `*` - активен будильник
- `#` - активен таймер

**Info Screen 1:**
- День недели
- Статус будильника
- Статус таймера
- Статус WiFi

**Info Screen 2:**
- Имя WiFi сети (SSID)
- IP-адрес (если подключен)
- Статус синхронизации времени
- Свободная оперативная память

### English
**Main Screen:**
- Time with blinking colon (e.g., `14:30`)
- Date in DD.MM.YYYY format
- `*` - alarm is active
- `#` - timer is active

**Info Screen 1:**
- Weekday
- Alarm status
- Timer status
- WiFi status

**Info Screen 2:**
- WiFi network name (SSID)
- IP address (if connected)
- Time synchronization status
- Free RAM

## Аппаратная часть / Hardware

### Русский
**Плата:** ESP32-C3 SuperMini с микро OLED 0.42" (72x40)

**Подключение встроенных компонентов:**
- OLED дисплей 0.42" (72x40): I2C, SCL=GPIO6, SDA=GPIO5
- Кнопка BOOT: GPIO9 (внутренний подтягивающий)
- Пьезоизлучатель HCM1203X: GPIO10 → "+", GND → "-"
- Синий светодиод: GPIO8 (активный низкий уровень)

**Важные заметки по зуммеру:**
- HCM1203X - активный пьезоизлучатель
- Подключен напрямую к GPIO10 (pin 16) и GND (pin 2)
- Работает на 3.3V, потребляет ~30мА
- В коде используется активный HIGH сигнал

### English
**Board:** ESP32-C3 SuperMini with 0.42" micro OLED (72x40)

**Built-in components wiring:**
- 0.42" OLED display (72x40): I2C, SCL=GPIO6, SDA=GPIO5
- BOOT button: GPIO9 (internal pull-up)
- HCM1203X Piezo Buzzer: GPIO10 → "+", GND → "-"
- Blue LED: GPIO8 (active low)

**Important buzzer notes:**
- HCM1203X is an active piezo buzzer
- Directly connected to GPIO10 (pin 16) and GND (pin 2)
- Operates at 3.3V, consumes ~30mA
- Uses active HIGH signal in code

## Устранение неисправностей / Troubleshooting

### Русский
**Проблема:** Зуммер не работает
**Решение:** Проверьте полярность. HCM1203X чувствителен к полярности. Если не работает, попробуйте поменять контакты.

**Проблема:** OLED дисплей не показывает изображение
**Решение:** Проверьте соединение I2C. Убедитесь, что библиотека U8g2 установлена.

**Проблема:** Не подключается к WiFi
**Решение:** Проверьте SSID и пароль. Используйте команду `WIFI SSID пароль`.

**Проблема:** Время не синхронизируется
**Решение:** Проверьте интернет-соединение. Используйте команду `SYNC` для принудительной синхронизации.

### English
**Issue:** Buzzer not working
**Solution:** Check polarity. HCM1203X is polarity-sensitive. If not working, try reversing the connections.

**Issue:** OLED display not showing anything
**Solution:** Check I2C connections. Ensure U8g2 library is installed.

**Issue:** Cannot connect to WiFi
**Solution:** Verify SSID and password. Use `WIFI SSID password` command.

**Issue:** Time not synchronizing
**Solution:** Check internet connection. Use `SYNC` command to force synchronization.

