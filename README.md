# STM32 Smart Lighting System

A bare-metal embedded system for STM32F401xE that implements an IoT smart lighting controller with automatic color temperature adjustment based on ambient light and temperature sensors, featuring MQTT connectivity and a web dashboard.

## System Architecture

```
+----------------------------------------------------------------------------------+
|                              CLOUD / NETWORK                                     |
|  +--------------------------------------------------------------------------+    |
|  |                        MQTT Broker (HiveMQ)                              |    |
|  |                      broker.hivemq.com:1883                              |    |
|  +--------------------------------------------------------------------------+    |
|         ^                         ^                         |                    |
|         | Publish                 | Publish                 | Subscribe          |
|         | sensors/stm32/*         | devices/stm32/status    | devices/stm32/     |
|         |                         |                         | control            |
+---------+-------------------------+-------------------------+--------------------+
          |                         |                         v
+---------+-------------------------+----------------------------------------------+
|                              WiFi MODULE                                         |
|  +--------------------------------------------------------------------------+    |
|  |                      ESP01 (ESP8266)                                     |    |
|  |                   AT Command Interface                                   |    |
|  |                   USART1 @ 115200 baud                                   |    |
|  +--------------------------------------------------------------------------+    |
|                                    |                                             |
|                            USART1 (PA9/PA10)                                     |
+------------------------------------+---------------------------------------------+
                                     |
+------------------------------------+---------------------------------------------+
|                                    v                                             |
|  +--------------------------------------------------------------------------+    |
|  |                     STM32F401xE (Cortex-M4)                              |    |
|  |                         @ 16 MHz (HSI)                                   |    |
|  |  +--------------------------------------------------------------------+  |    |
|  |  |                    APPLICATION LAYER                               |  |    |
|  |  |  +-------------+  +-------------+  +-------------+                 |  |    |
|  |  |  |    State    |  |   Sensor    |  |    MQTT     |                 |  |    |
|  |  |  |   Machine   |  |  Handlers   |  |  Publisher  |                 |  |    |
|  |  |  +-------------+  +-------------+  +-------------+                 |  |    |
|  |  +--------------------------------------------------------------------+  |    |
|  |  +--------------------------------------------------------------------+  |    |
|  |  |                    DEVICE DRIVERS                                  |  |    |
|  |  |  +----------+  +----------+  +----------+  +---------------+       |  |    |
|  |  |  |  ESP01   |  |  BMP280  |  | TSL2561  |  | Smart Light   |       |  |    |
|  |  |  |  Driver  |  |  Driver  |  |  Driver  |  |    Driver     |       |  |    |
|  |  |  |  (WiFi)  |  |(Temp/Prs)|  |  (Lux)   |  | (3-Ch PWM)    |       |  |    |
|  |  |  +----------+  +----------+  +----------+  +---------------+       |  |    |
|  |  +--------------------------------------------------------------------+  |    |
|  |  +--------------------------------------------------------------------+  |    |
|  |  |             MICROCONTROLLER ABSTRACTION LAYER (MCAL)               |  |    |
|  |  |  +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+   |  |    |
|  |  |  | RCC | |GPIO | |USART| | ADC | | I2C | | TIM | | STK | |IWDG |   |  |    |
|  |  |  +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+   |  |    |
|  |  +--------------------------------------------------------------------+  |    |
|  +--------------------------------------------------------------------------+    |
|                                                                                  |
|    PA0 ----------> LED (Status Indicator)                                        |
|    PA5 ----------> PWM CH1 (Warm White 3000K)                                    |
|    PA6 ----------> PWM CH2 (Neutral White 4000K)                                 |
|    PA7 ----------> PWM CH3 (Cool White 6000K)                                    |
|    PA2/PA3 ------> USART2 (Debug Console)                                        |
|                                                                                  |
+----------------------------------------------------------------------------------+
          |                |                              |
          | ADC (PA1)      | I2C1 (PB8/PB9)              | I2C1 (PB8/PB9)
          v                v                              v
+-------------------+ +-------------------+     +-------------------+
|    MQ6 Sensor     | |   BMP280 Sensor   |     |  TSL2561 Sensor   |
|  (Gas Detection)  | | (Temp & Pressure) |     |  (Light Sensor)   |
|                   | |                   |     |                   |
| * LPG Detection   | | * Temperature     |     | * Ambient Light   |
| * Analog Output   | | * Barometric      |     | * Lux Measurement |
| * Preheat Required| |   Pressure        |     | * IR + Visible    |
|                   | | * I2C Interface   |     | * I2C Interface   |
+-------------------+ +-------------------+     +-------------------+
```

## Features

### Smart Lighting Control
- **Three-Channel Color Temperature LEDs**
  - Warm White (3000K) - PA5/TIM2_CH1
  - Neutral White (4000K) - PA6/TIM2_CH2
  - Cool White (6000K) - PA7/TIM2_CH3
  - 0.1% PWM resolution (0-1000 duty cycle)

- **Automatic Mode**
  - Adjusts color temperature based on ambient light (TSL2561)
  - Low light -> Warmer colors (relaxing)
  - Bright light -> Cooler colors (alertness)
  - Temperature compensation via BMP280
  - Hot environment -> Shift to cooler colors
  - Cold environment -> Shift to warmer colors
  - Automatic brightness adjustment based on ambient light

- **Manual Mode**
  - Direct control of individual channels via MQTT/Web Dashboard
  - Quick presets (Warm, Neutral, Cool, Mixed, Off)
  - Real-time slider control

### Multi-Sensor Data Acquisition
- **MQ6** - LPG/combustible gas detection
- **BMP280** - Temperature and barometric pressure
- **TSL2561** - Ambient light level (lux)

### MQTT IoT Connectivity
- WiFi connectivity via ESP01 (ESP8266) module
- Real-time sensor data publishing
- Remote lighting control via MQTT subscription
- JSON-formatted messages

### Robust Operation
- Independent Watchdog (IWDG) for system reliability
- State machine architecture for connection management
- Automatic error recovery with reconnection attempts

## Hardware Requirements

| Component | Description | Interface |
|-----------|-------------|-----------|
| STM32F401xE | Microcontroller (Cortex-M4) | - |
| ESP01 | WiFi module (ESP8266) | USART1 (PA9/PA10) |
| BMP280 | Temperature/Pressure sensor | I2C1 (PB8/PB9) |
| TSL2561 | Ambient light sensor | I2C1 (PB8/PB9) |
| MQ6 | Gas sensor (LPG/Butane/Propane) | ADC1 (PA1) |
| LED | Status indicator | GPIO (PA0) |
| Power LEDs | 3x color temperature LEDs | PWM (PA5/PA6/PA7) |

## Pin Configuration

| Pin | Function | Description |
|-----|----------|-------------|
| PA0 | GPIO Output | Status LED |
| PA1 | ADC1_IN1 | MQ6 analog input |
| PA2 | USART2_TX | Debug output |
| PA3 | USART2_RX | Debug input |
| PA5 | TIM2_CH1 | Warm White LED (3000K) |
| PA6 | TIM2_CH2 | Neutral White LED (4000K) |
| PA7 | TIM2_CH3 | Cool White LED (6000K) |
| PA9 | USART1_TX | ESP01 TX |
| PA10 | USART1_RX | ESP01 RX |
| PB8 | I2C1_SCL | BMP280/TSL2561 clock |
| PB9 | I2C1_SDA | BMP280/TSL2561 data |

## MQTT Topics

| Topic | Direction | Description |
|-------|-----------|-------------|
| `sensors/stm32/mq6` | Publish | MQ6 gas sensor readings |
| `sensors/stm32/bmp280` | Publish | BMP280 temperature/pressure data |
| `sensors/stm32/tsl2561` | Publish | TSL2561 light sensor data |
| `sensors/stm32/pwm` | Publish | Light status (channels, mode, brightness) |
| `devices/stm32/status` | Publish | Device online status |
| `devices/stm32/control` | Subscribe | Light control commands |

### Message Formats

**Light Status (Published):**
```json
{
  "mode": "manual",
  "warm": 500,
  "neutral": 300,
  "cool": 200,
  "brightness": 75,
  "lux": 150,
  "temp_c100": 2350,
  "timestamp": 123456
}
```

**Light Control - Manual Channels:**
```json
{
  "warm": 500,
  "neutral": 300,
  "cool": 200
}
```

**Light Control - Set Mode:**
```json
{"mode": "auto"}
```
or
```json
{"mode": "manual"}
```

**Light Control - Legacy (applies to all channels):**
```json
{"duty": 500}
```

**TSL2561 Sensor Data:**
```json
{
  "lux": 250,
  "ch0": 1024,
  "ch1": 512,
  "timestamp": 123456
}
```

**BMP280 Sensor Data:**
```json
{
  "temp_c": 25.50,
  "pressure_hpa": 1013.25,
  "pressure_pa": 101325,
  "timestamp": 123456
}
```

**MQ6 Sensor Data:**
```json
{
  "adc_raw": 2048,
  "voltage_mv": 1650,
  "gas_ppm": 500,
  "timestamp": 123456
}
```

## Automatic Lighting Algorithm

### Color Temperature Selection (based on ambient light)

| Lux Level | Condition | Color Mix (W/N/C) |
|-----------|-----------|-------------------|
| < 50 | Very dark | 100% / 0% / 0% |
| 50-200 | Dim | 80% / 20% / 0% |
| 200-500 | Twilight | 50% / 50% / 0% |
| 500-1000 | Dusk | 30% / 50% / 20% |
| 1000-5000 | Indoor | 10% / 50% / 40% |
| 5000-10000 | Bright | 0% / 30% / 70% |
| > 10000 | Very bright | 0% / 10% / 90% |

### Temperature Adjustment

| Temperature | Adjustment |
|-------------|------------|
| > 28°C | Shift toward cooler colors |
| < 18°C | Shift toward warmer colors |
| 18-28°C | No adjustment |

### Brightness Calculation (based on ambient light)

| Lux Level | LED Brightness |
|-----------|----------------|
| < 10 | 30% |
| 10-50 | 50% |
| 50-200 | 70% |
| 200-500 | 85% |
| 500-1000 | 95% |
| > 1000 | 100% |

## Smart Light Driver API

### Initialization
```c
SmartLight_Status_t SmartLight_Initialize(void);
```

### Mode Control
```c
SmartLight_Status_t SmartLight_SetMode(SmartLight_Mode_t mode);
SmartLight_Status_t SmartLight_GetMode(SmartLight_Mode_t* mode);
```

### Manual Control
```c
SmartLight_Status_t SmartLight_SetChannels(uint32_t warm, uint32_t neutral, uint32_t cool);
SmartLight_Status_t SmartLight_SetPreset(SmartLight_Preset_t preset, uint32_t brightness);
SmartLight_Status_t SmartLight_TurnOff(void);
```

### Automatic Control
```c
SmartLight_Status_t SmartLight_UpdateAutomatic(void);
void SmartLight_AutoTask(void);  // Call from main loop
```

### Status
```c
SmartLight_Status_t SmartLight_GetState(SmartLight_State_t* state);
SmartLight_Status_t SmartLight_GetChannels(SmartLight_Channels_t* channels);
```

## Project Structure

```
esp_stm32/
+-- Core/
|   +-- Inc/
|   |   +-- drivers/
|   |   |   +-- peripheral/     # MCAL drivers
|   |   |   |   +-- rcc.h       # Clock control
|   |   |   |   +-- gpio.h      # GPIO
|   |   |   |   +-- usart.h     # UART
|   |   |   |   +-- adc.h       # ADC
|   |   |   |   +-- i2c.h       # I2C
|   |   |   |   +-- tim.h       # Timer/PWM
|   |   |   |   +-- stk.h       # SysTick
|   |   |   |   +-- nvic.h      # Interrupts
|   |   |   |   +-- iwdg.h      # Watchdog
|   |   |   +-- device/         # Device drivers
|   |   |       +-- esp01.h     # WiFi module
|   |   |       +-- mqtt.h      # MQTT protocol
|   |   |       +-- bmp280.h    # Temp/pressure sensor
|   |   |       +-- tsl2561.h   # Light sensor
|   |   |       +-- mq6.h       # Gas sensor
|   |   |       +-- smart_light.h  # Smart lighting driver
|   |   +-- common/
|   |       +-- bit_math.h
|   |       +-- std_types.h
|   +-- Src/
|       +-- main.c              # Application entry point
|       +-- drivers/
|           +-- peripheral/     # MCAL implementations
|           +-- device/         # Device implementations
|               +-- smart_light.c  # Smart lighting driver
+-- Debug/                      # Build output
+-- Drivers/                    # STM32 HAL drivers
```

## Configuration

Key configuration parameters in `main.c`:

```c
/* WiFi Credentials */
#define WIFI_SSID           "Your_SSID"
#define WIFI_PASSWORD       "Your_Password"

/* MQTT Broker */
#define MQTT_BROKER_IP      "broker.hivemq.com"
#define MQTT_BROKER_PORT    1883

/* Timing */
#define MQTT_PUBLISH_INTERVAL   10000   // 10 seconds
#define MQTT_KEEPALIVE_INTERVAL 30000   // 30 seconds

/* Watchdog */
#define IWDG_TIMEOUT_MS         10000   // 10 seconds
```

Smart Light configuration in `smart_light.h`:

```c
/* PWM Configuration */
#define SMART_LIGHT_TIMER           TIM2
#define SMART_LIGHT_PRESCALER       16
#define SMART_LIGHT_PERIOD          1000

/* Auto Mode Update Interval */
#define SMART_LIGHT_AUTO_UPDATE_MS  2000

/* Light Thresholds (lux) */
#define SMART_LIGHT_LUX_VERY_DARK   50
#define SMART_LIGHT_LUX_DIM         200
#define SMART_LIGHT_LUX_TWILIGHT    500
#define SMART_LIGHT_LUX_DUSK        1000
#define SMART_LIGHT_LUX_INDOOR      5000
#define SMART_LIGHT_LUX_BRIGHT      10000

/* Temperature Thresholds (Celsius) */
#define SMART_LIGHT_TEMP_COLD       18
#define SMART_LIGHT_TEMP_HOT        28
```

## Building the Project

This project uses STM32CubeIDE with GNU Tools for STM32.

```bash
# Build
make -C Debug all

# Clean
make -C Debug clean

# Full rebuild
make -C Debug clean && make -C Debug all
```

## Web Dashboard

The project includes a web dashboard for monitoring and control. See the `webserver/` directory.

### Dashboard Features
- Real-time sensor data visualization (MQ6, BMP280, TSL2561)
- Three-channel light control sliders (Warm, Neutral, Cool)
- Manual/Automatic mode toggle
- Quick preset buttons (Warm 70%, Neutral 70%, Cool 70%, Mixed 50%, Off)
- Color preview bar
- Live status display

### API Endpoints

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/api/control/light` | POST | Set channel values `{warm, neutral, cool}` |
| `/api/control/mode` | POST | Set mode `{mode: "manual"|"auto"}` |
| `/api/control/preset` | POST | Apply preset `{brightness, colorTemp}` |
| `/api/sensors/latest` | GET | Get latest sensor readings |

## LED Status Indicators

| Pattern | Meaning |
|---------|---------|
| Slow blink (1s) | Normal operation |
| Fast blink (100ms) | Error state |

## Application State Machine

```
+----------+     +-------------+     +-------------+     +-----------------+
|   INIT   |---->| ESP01_INIT  |---->|  WIFI_MODE  |---->|  WIFI_CONNECT   |
+----------+     +-------------+     +-------------+     +--------+--------+
                                                                  |
+----------+     +-------------+     +-----------------+     +----v--------+
|  ERROR   |<----|  MQTT_SUB   |<----| MQTT_PROTOCOL   |<----|   GET_IP    |
+----+-----+     +------+------+     +-----------------+     +-------------+
     |                  |                      ^
     |                  v                      |
     |           +-------------+               |
     +---------->|MQTT_RUNNING |---------------+
      Recovery   +-------------+   (on disconnect)
```

## Author

**Ameer Alwadiya**

Version: V3.0.0
Date: December 2025
