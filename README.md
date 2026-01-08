# ESP32-C3 0.42" OLED Часы с WiFi, NTP, будильником и таймером
# ESP32-C3 0.42" OLED Clock with WiFi, NTP, Alarm and Timer

![ESP32-C3 0.42" OLED](https://github.com/CheshirCa/ESP32-C3-Clock/blob/main/ESP32-C3-Clock.jpg)
![ESP32-C3 0.42" OLED with buzzer HCM1203X pinout](https://github.com/CheshirCa/ESP32-C3-Clock/blob/main/ESP32-C3-Buzzer.jpg)
![ESP32-C3 clock web interface](https://github.com/CheshirCa/ESP32-C3-Clock/blob/main/ESP32-C3-Clock_Web.jpg)

[English version below](#english-version) | [Русская версия](#Русская-версия)

---

## Русская версия

## 🆕 Версия 2.0

### Что нового в версии 2.0:

- 🌐 **Веб-интерфейс** - полнофункциональное управление через браузер
- 🔵 **Улучшенная LED индикация:**
  - Постоянное свечение = будильник активен
  - Мигание (500мс) = таймер работает
- 🔊 **Новый паттерн звукового сигнала:** три коротких пи (150мс) с паузами, затем длинная пауза
- ⏲️ **Высокоточный таймер** на базе `esp_timer` (точность до микросекунд)
- 📜 **История команд** в Serial терминале с навигацией стрелками ↑/↓ (10 команд)
- 🎨 **Улучшенный веб-интерфейс:**
  - Автообновление данных каждые 2 секунды
  - Адаптивный дизайн для всех устройств
  - Полная поддержка UTF-8 (русский язык)
  - Подтверждение критических операций
- 🔧 **Расширенное управление:**
  - Отображение оставшегося времени таймера в реальном времени
  - Статус синхронизации времени в веб-интерфейсе
  - Информация о свободной памяти

### 📋 Описание

Проект часов на базе **ESP32-C3 SuperMini с встроенным OLED-дисплеем 0.42" (72x40 пикселей)**, поддержкой WiFi, NTP-синхронизацией времени, будильником и таймером. Устройство сохраняет настройки в энергонезависимой памяти (NVS) и управляется через серийный терминал, встроенную кнопку BOOT и **веб-интерфейс**.

### ✨ Возможности

- ⏰ **Часы реального времени** с синхронизацией через NTP
- 📡 **WiFi подключение** с сохранением настроек
- 🌐 **Веб-интерфейс** для удалённого управления
- 🔔 **Гибкий будильник:**
  - Ежедневный
  - По дням недели (Пн-Вс, любая комбинация)
  - На конкретную дату
  - С текстовым сообщением (до 30 байт, ~15 символов кириллицы)
  - Повторяющийся или одноразовый
  - Сохранение в NVS
- ⏲️ **Таймер обратного отсчета:**
  - От 1 секунды до 24 часов
  - С текстовым сообщением
  - Форматы: HH:MM:SS, MM:SS или SS
  - Высокоточный счёт на базе esp_timer (точность до микросекунд)
- 💾 **Энергонезависимое хранение** настроек WiFi, часового пояса и будильника
- 🔵 **Светодиодная индикация:**
  - Постоянное свечение = будильник активен
  - Мигание (500мс) = таймер работает
- 🔊 **Звуковые сигналы** пьезоизлучателем (паттерн "пи-пи-пи")
- 📟 **Информационные экраны** с детальной статистикой
- 🔤 **Поддержка UTF-8** (кириллица и латиница)
- 📜 **История команд** в Serial терминале (стрелки вверх/вниз, 10 команд)

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
3. **WebServer** (встроенная в ESP32)
4. **Preferences** (встроенная в ESP32)

### 🚀 Быстрый старт

1. **Настройте WiFi в коде** (или через Serial/Web после загрузки):
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

4. **Откройте Serial Monitor** (115200 baud, NL&CR) или **веб-браузер**

5. **Для веб-интерфейса:**
   - Узнайте IP адрес из Serial Monitor
   - Откройте `http://192.168.x.x` в браузере
   - Управляйте часами через удобный интерфейс!

### 🌐 Веб-интерфейс (НОВОЕ в v2.0!)

После подключения к WiFi откройте IP адрес устройства в браузере для доступа к полнофункциональному веб-интерфейсу.

#### Возможности веб-интерфейса:

**📊 Главная страница:**
- Текущее время (крупным шрифтом)
- Дата и день недели
- WiFi SSID и статус подключения
- IP адрес
- Статус синхронизации времени
- Объём свободной памяти

**🔔 Управление будильником:**
- Выбор типа: ежедневный / по дням недели / на конкретную дату
- Установка времени
- Текстовое сообщение (поддержка кириллицы!)
- Флаги повтора и сохранения в NVS
- Отображение текущего активного будильника

**⏲️ Управление таймером:**
- Установка длительности (HH:MM:SS)
- Текстовое сообщение
- Отображение оставшегося времени в реальном времени

**⚙️ Системные настройки:**
- Изменение NTP сервера
- Настройка часового пояса (GMT offset)
- Настройка летнего времени (DST offset)
- Кнопка принудительной синхронизации
- Ручная установка времени
- Сохранение/восстановление настроек из NVS
- Очистка NVS
- Перезагрузка устройства

**🎨 Особенности интерфейса:**
- Адаптивный дизайн (работает на телефоне, планшете, ПК)
- Автообновление данных каждые 2 секунды
- Современный градиентный дизайн
- Подтверждение деструктивных операций
- UTF-8 поддержка (вводите текст на русском!)

### 💻 Команды Serial-терминала

**Подключение:** 115200 baud, Newline & Carriage Return (NL&CR)

**История команд (НОВОЕ в v2.0!):** Используйте стрелки ↑/↓ для навигации по истории (10 последних команд)

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
- `ТЕКСТ` - текст сообщения до 30 байт (~15 символов кириллицы) (опционально)
- `R` - повторять после срабатывания (опционально)
- `S` - сохранить в NVS (опционально)

**Примеры:**

```bash
# Ежедневный будильник в 07:00
ALARM 07:00

# Будильник в 08:30 с текстом "Работа"
ALARM 08:30 Работа

# Будильник на конкретную дату
ALARM 2025-12-31 23:59 НовыйГод

# Будильник по будням (Пн-Пт) в 07:00, повторяющийся, с сохранением
ALARM 12345 07:00 Подъём R S

# Будильник на выходные (Сб-Вс)
ALARM 67 10:00 Выходной

# Только в понедельник
ALARM 1 09:00 Понедельник

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
TIMER 3:00:00 Встреча

# 45 секунд
TIMER 45

# С текстом на русском
TIMER 3:00 Чай

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
  │   Подъём     │  ← текст будильника/таймера (поддержка кириллицы!)
  │              │
  │  BOOT-STOP   │  ← Нажмите BOOT для остановки
  └──────────────┘
```

### 🎮 Управление кнопкой BOOT

| Экран | Действие | Результат |
|-------|----------|-----------|
| Главный экран | 1x нажатие | Info Screen 1 |
| Info Screen 1 | 1x нажатие | Info Screen 2 |
| Info Screen 2 | 1x нажатие | Возврат на главный экран |
| Будильник/Таймер | 1x нажатие | Остановить сигнал |

### 💡 Индикация синим светодиодом (УЛУЧШЕНО в v2.0!)

- **ПОСТОЯННО ГОРИТ** (active LOW) - будильник установлен и активен
- **МИГАЕТ (500мс)** - таймер работает
- **ВЫКЛЮЧЕН** - будильник отключён или сработал

### 🔊 Работа зуммера (УЛУЧШЕНО в v2.0!)

**Паттерн сигнала при срабатывании:**
- Первый пи: 150мс
- Пауза: 150мс
- Второй пи: 150мс
- Пауза: 150мс
- Третий пи: 150мс
- Длинная пауза: 1250мс
- Повтор...

**Остановка:** Нажать кнопку BOOT

### 💾 Хранение данных в NVS

**Автоматически сохраняются:**
- Учётные данные WiFi (при использовании команды SAVE)
- Настройки часового пояса (при использовании команды SAVE)
- Настройки NTP сервера (при использовании команды SAVE)

**Сохранение будильника:**
- Используйте флаг `S` при установке будильника
- Или сохраните текущую конфигурацию командой SAVE

**Примеры:**
```bash
# Установить будильник и сохранить
ALARM 07:00 Работа R S

# Настроить WiFi и сохранить
WIFI MyNetwork password123
SAVE

# Очистить все настройки
ERASE
```

### 📊 Примеры использования

#### Рабочий будильник (Пн-Пт, 7:00, повторяющийся)
```bash
# Подключитесь к Serial Monitor (115200 baud)
> ALARM 12345 07:00 Работа R S
Alarm set for Weekdays: Mon Tue Wed Thu Fri 07:00 [R] [S] 'Работа'
Alarm saved to NVS

> STATUS
=== SYSTEM STATUS ===
...
Alarm: Weekdays: MonTueWedThuFri 07:00 [R] [S] 'Работа'
...
```

#### Таймер для чая (3 минуты)
```bash
> TIMER 3:00 Чай
Timer set for 03:00 seconds, text='Чай'
```

#### Новогодний будильник
```bash
> ALARM 2025-12-31 23:59 НовыйГод S
Alarm set for 2025-12-31 23:59 [S] 'НовыйГод'
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

### 🐛 Устранение неполадок

#### Проблема: Зуммер не работает
**Решение:**
1. Проверьте полярность HCM1203X
2. GPIO10 должен быть подключён к "+" зуммера
3. GND должен быть подключён к "-" зуммера
4. Если не работает - попробуйте поменять контакты местами
5. Убедитесь, что используете активный зуммер (со встроенным генератором)

#### Проблема: Дисплей не работает или показывает мусор
**Решение:**
1. Проверьте I2C соединения (SDA=GPIO5, SCL=GPIO6)
2. Убедитесь, что установлена библиотека U8g2
3. Проверьте напряжение питания (должно быть 3.3V)

#### Проблема: Не могу подключиться к WiFi
**Решение:**
1. Проверьте SSID и пароль
2. Убедитесь, что используете WiFi 2.4 ГГц (ESP32-C3 не поддерживает 5 ГГц)
3. Используйте команду `WIFI SSID пароль` для переконфигурации
4. Проверьте доступность сети командой `STATUS`

#### Проблема: Время не синхронизируется
**Решение:**
1. Проверьте интернет-соединение
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
3. Проверьте, не повреждён ли NVS: `ERASE`, затем переконфигурируйте

### 📝 Известные ограничения

1. **WiFi 5 ГГц не поддерживается** - ESP32-C3 работает только с 2.4 ГГц
2. **Максимальная длина текста** для будильника/таймера - 30 байт (~15 символов кириллицы)
3. **Один активный будильник** - можно установить только один будильник одновременно
4. **Один активный таймер** - можно установить только один таймер одновременно
5. **Автовозврат** с информационных экранов через 10 секунд
6. **Синхронизация времени** происходит при запуске и может быть вызвана командой `SYNC`

### 🎯 Советы по продвинутому использованию

#### Создание сложных расписаний будильников

Поскольку активен только один будильник, используйте эти стратегии:

**Утро в будни + Выходные:**
```bash
# Утро понедельник - пятница
ALARM 12345 07:00 Работа R S

# Когда наступят выходные, установите вручную:
ALARM 67 10:00 Выходной
```

**Ежемесячные напоминания:**
```bash
# Установить будильник на конкретную дату
ALARM 2025-02-01 09:00 Квартплата S

# После срабатывания установить на следующий месяц
ALARM 2025-03-01 09:00 Квартплата S
```

#### Использование таймера для техники Pomodoro

```bash
# Рабочая сессия (25 минут)
TIMER 25:00 Работа

# После работы установить перерыв (5 минут)
TIMER 5:00 Перерыв

# Длинный перерыв (15 минут)
TIMER 15:00 ДлинныйПерерыв
```

#### Несколько NTP серверов для надёжности

Если основной NTP сервер недоступен:
```bash
NTP pool.ntp.org        # Основной
NTP time.google.com     # Резервный
NTP time.cloudflare.com # Альтернативный
```

### 🔐 Соображения безопасности

1. **Учётные данные WiFi** хранятся в открытом виде в NVS
2. **Serial консоль** не защищена - любой с доступом к USB может управлять устройством
3. Для промышленного использования рассмотрите:
   - Добавление аутентификации для serial команд
   - Шифрование чувствительных данных в NVS
   - Отключение serial консоли в финальных сборках

### 📚 Технические детали

#### Использование памяти

Типичный объём памяти:
- **Flash:** ~1.2 MB (программный код)
- **SRAM:** ~115 KB использовано, ~285 KB свободно
- **NVS:** ~1-2 KB (хранение настроек)

#### Энергопотребление

Приблизительные значения:
- **В покое с WiFi:** ~80 мА @ 3.3V
- **Активный дисплей:** +20 мА
- **Активный зуммер:** +30 мА
- **Глубокий сон:** Не реализован (устройство всегда включено)

#### Скорость шины I2C

- Настроена на 400 кГц (быстрый режим) для OLED дисплея
- Может быть снижена до 100 кГц при возникновении проблем

### 📖 Структура кода

```
ESP32-C3-Clock_new.ino (v2.0)
├── Setup & Configuration
│   ├── Hardware initialization
│   ├── WiFi connection
│   ├── Web server initialization
│   └── NTP synchronization
│
├── Web Server Functions (НОВОЕ в v2.0!)
│   ├── setupWebServer()
│   ├── handleRoot()
│   ├── handleStatus()
│   ├── handleAlarm()
│   └── handleTimer()
│
├── NVS Functions
│   ├── loadConfigFromNVS()
│   ├── saveConfigToNVS()
│   ├── loadAlarmFromNVS()
│   └── saveAlarmToNVS()
│
├── Serial Command Handler
│   ├── handleSerial()
│   ├── Command history (НОВОЕ в v2.0!)
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
│   ├── updateAlarmIndicator() (УЛУЧШЕНО в v2.0!)
│   └── High-precision timer (esp_timer) (НОВОЕ в v2.0!)
│
└── Main Loop
    ├── Web server handling (НОВОЕ в v2.0!)
    ├── Serial handling
    ├── Button handling
    ├── Time updates
    ├── LED blinking for timer (НОВОЕ в v2.0!)
    ├── Alarm/Timer checks
    └── Display updates
```

### 📧 Контакты

- GitHub: [@CheshirCa](https://github.com/CheshirCa)
- Ссылка на проект: [https://github.com/CheshirCa/ESP32-C3-Clock](https://github.com/CheshirCa/ESP32-C3-Clock)

### ⭐ История звёзд

Если этот проект вам помог, пожалуйста, поставьте звезду на GitHub!

---

## English version

## 🆕 Version 2.0

### What's new in version 2.0:

- 🌐 **Web Interface** - full-featured browser-based control
- 🔵 **Improved LED indication:**
  - Solid ON = alarm is active
  - Blinking (500ms) = timer is running
- 🔊 **New buzzer pattern:** three short beeps (150ms) with pauses, then long pause
- ⏲️ **High-precision timer** based on `esp_timer` (microsecond accuracy)
- 📜 **Command history** in Serial terminal with ↑/↓ arrow navigation (10 commands)
- 🎨 **Enhanced web interface:**
  - Auto-refresh data every 2 seconds
  - Responsive design for all devices
  - Full UTF-8 support (Cyrillic characters)
  - Confirmation for critical operations
- 🔧 **Extended control:**
  - Real-time timer countdown display
  - Time sync status in web interface
  - Free memory information

### 📋 Description

ESP32-C3 SuperMini clock project with **built-in 0.42" OLED display (72x40 pixels)**, WiFi connectivity, NTP time synchronization, alarm and timer. The device stores settings in non-volatile storage (NVS) and is controlled via serial terminal, built-in BOOT button, and **web interface**.

### ✨ Features

- ⏰ **Real-time clock** with NTP synchronization
- 📡 **WiFi connectivity** with persistent settings
- 🌐 **Web interface** for remote control
- 🔔 **Flexible alarm:**
  - Daily
  - Weekdays (Mon-Sun, any combination)
  - Specific date
  - Text message (up to 30 bytes, ~10 characters in English)
  - Repeating or one-time
  - Save to NVS
- ⏲️ **Countdown timer:**
  - From 1 second to 24 hours
  - Text message
  - Formats: HH:MM:SS, MM:SS or SS
  - High-precision counting based on esp_timer (microsecond accuracy)
- 💾 **Non-volatile storage** of WiFi, timezone and alarm settings
- 🔵 **LED indication:**
  - Solid ON = alarm is active
  - Blinking (500ms) = timer is running
- 🔊 **Buzzer alerts** with piezo buzzer (beep-beep-beep pattern)
- 📟 **Information screens** with detailed statistics
- 🔤 **UTF-8 support** (Cyrillic and Latin)
- 📜 **Command history** in Serial terminal (up/down arrows, 10 commands)

### 🔧 Hardware

**Board:** ESP32-C3 SuperMini with built-in 0.42" OLED (72x40)

**Board specifications:**
- Processor: ESP32-C3 RISC-V @ 160MHz
- RAM: 400 KB SRAM
- Flash: 4 MB
- Built-in OLED display 0.42" (72x40 pixels)
- Built-in BOOT button (GPIO9)
- Built-in blue LED (GPIO8)
- USB Type-C for power and programming

**Built-in components pinout:**

| Component | GPIO | Note |
|-----------|------|------|
| OLED SDA | GPIO5 | I2C Data |
| OLED SCL | GPIO6 | I2C Clock |
| BOOT button | GPIO9 | With internal pull-up resistor |
| Blue LED | GPIO8 | Active low |
| Buzzer HCM1203X | GPIO10 | Active piezo buzzer |

**⚠️ IMPORTANT: HCM1203X buzzer connection**
- This is an **active** piezo buzzer (with built-in oscillator)
- Connection: GPIO10 (pin 16) → "+" buzzer, GND (pin 2) → "-" buzzer
- Voltage: 3.3V
- Current: ~30 mA
- **Polarity is critical!** If not working - swap the connections

### 📚 Required Libraries

Install via Arduino Library Manager:

1. **U8g2** by olikraus (for OLED display)
2. **WiFi** (built-in ESP32)
3. **WebServer** (built-in ESP32)
4. **Preferences** (built-in ESP32)

### 🚀 Quick Start

1. **Configure WiFi in code** (or via Serial/Web after upload):
   ```cpp
   String defSSID = "your_SSID";        // Your WiFi SSID
   String defPASS = "your_PASSWORD";    // Your WiFi password
   ```

2. **Configure timezone** (default GMT+3):
   ```cpp
   long defGMTOffset = 3 * 3600;        // GMT+3 (Moscow)
   long defDaylightOffset = 0;          // DST (0 = off)
   ```

3. **Upload sketch** to ESP32-C3

4. **Open Serial Monitor** (115200 baud, NL&CR) or **web browser**

5. **For web interface:**
   - Get IP address from Serial Monitor
   - Open `http://192.168.x.x` in browser
   - Control your clock through convenient interface!

### 🌐 Web Interface (NEW in v2.0!)

After connecting to WiFi, open the device's IP address in a browser for full-featured web interface access.

#### Web interface features:

**📊 Main page:**
- Current time (large font)
- Date and day of week
- WiFi SSID and connection status
- IP address
- Time sync status
- Free memory information

**🔔 Alarm control:**
- Type selection: daily / weekdays / specific date
- Time setting
- Text message (Cyrillic support!)
- Repeat and save to NVS flags
- Display of current active alarm

**⏲️ Timer control:**
- Duration setting (HH:MM:SS)
- Text message
- Real-time remaining time display

**⚙️ System settings:**
- Change NTP server
- Configure timezone (GMT offset)
- Configure daylight saving time (DST offset)
- Force sync button
- Manual time setting
- Save/restore settings from NVS
- Clear NVS
- Reboot device

**🎨 Interface features:**
- Responsive design (works on phone, tablet, PC)
- Auto-refresh data every 2 seconds
- Modern gradient design
- Confirmation for destructive operations
- UTF-8 support (type text in any language!)

### 💻 Serial Terminal Commands

**Connection:** 115200 baud, Newline & Carriage Return (NL&CR)

**Command history (NEW in v2.0!):** Use ↑/↓ arrows to navigate history (last 10 commands)

#### Help and status
```
HELP                    - Show all available commands
STATUS                  - Show detailed system status
SYNC                    - Force time synchronization with NTP
REBOOT                  - Reboot device
```

#### WiFi setup
```
WIFI <SSID> <password>  - Set WiFi parameters
SAVE                    - Save settings to NVS
RESTORE                 - Load settings from NVS
ERASE                   - Clear all settings in NVS
```

**Examples:**
```
WIFI MyHomeWiFi password123
SAVE
```

#### Time setup
```
TIME YYYY-MM-DD HH:MM:SS - Set time manually
NTP <server>             - Change NTP server (default: pool.ntp.org)
TZ <±hours>              - Set timezone
DST <±hours>             - Set daylight saving time offset
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
- `TEXT` - message text up to 30 bytes (~10 characters) (optional)
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

# Weekday alarm (Mon-Fri) at 07:00, repeating, with save
ALARM 12345 07:00 WakeUp R S

# Weekend alarm (Sat-Sun)
ALARM 67 10:00 Weekend

# Monday only
ALARM 1 09:00 Monday

# Clear alarm
ALARM CLEAR
```

**Important:**
- If starts with digits 1-7 WITHOUT colon, these are weekdays
- If starts with time (e.g., 17:00), this is a daily alarm
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

# With text
TIMER 3:00 Tea

# Clear timer
TIMER CLEAR
```

**Limitations:**
- Minimum: 1 second
- Maximum: 24 hours (86400 seconds)

### 🖥️ Display Operation

#### Main screen
```
  ┌──────────────┐
  │   14:30      │  ← Time (blinking colon)
  │ 15.01.2025 * │  ← Date, * = alarm active
  └──────────────┘
```

**Indicators:**
- `*` - alarm active
- `#` - timer active

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

### 💡 Blue LED Indication (IMPROVED in v2.0!)

- **SOLID ON** (active LOW) - alarm is set and active
- **BLINKING (500ms)** - timer is running
- **OFF** - alarm is disabled or triggered

### 🔊 Buzzer Operation (IMPROVED in v2.0!)

**Signal pattern when triggered:**
- First beep: 150ms
- Pause: 150ms
- Second beep: 150ms
- Pause: 150ms
- Third beep: 150ms
- Long pause: 1250ms
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
2. **Maximum text length** for alarm/timer - 30 bytes (~10 characters)
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
ESP32-C3-Clock_new.ino (v2.0)
├── Setup & Configuration
│   ├── Hardware initialization
│   ├── WiFi connection
│   ├── Web server initialization (NEW in v2.0!)
│   └── NTP synchronization
│
├── Web Server Functions (NEW in v2.0!)
│   ├── setupWebServer()
│   ├── handleRoot()
│   ├── handleStatus()
│   ├── handleAlarm()
│   └── handleTimer()
│
├── NVS Functions
│   ├── loadConfigFromNVS()
│   ├── saveConfigToNVS()
│   ├── loadAlarmFromNVS()
│   └── saveAlarmToNVS()
│
├── Serial Command Handler
│   ├── handleSerial()
│   ├── Command history (NEW in v2.0!)
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
│   ├── updateAlarmIndicator() (IMPROVED in v2.0!)
│   └── High-precision timer (esp_timer) (NEW in v2.0!)
│
└── Main Loop
    ├── Web server handling (NEW in v2.0!)
    ├── Serial handling
    ├── Button handling
    ├── Time updates
    ├── LED blinking for timer (NEW in v2.0!)
    ├── Alarm/Timer checks
    └── Display updates
```

### 📧 Contact

- GitHub: [@CheshirCa](https://github.com/CheshirCa)
- Project Link: [https://github.com/CheshirCa/ESP32-C3-Clock](https://github.com/CheshirCa/ESP32-C3-Clock)

### ⭐ Star History

If you find this project helpful, please consider giving it a star on GitHub!

