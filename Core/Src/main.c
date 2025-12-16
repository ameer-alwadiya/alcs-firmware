/****************************************************************************/
/* Author            : Ameer Alwadiya                                       */
/* Version           : V3.0.0                                               */
/* Date              : 11 Dec 2025                                          */
/* Description       : MQTT with ESP01 + MQ6 + BMP280 Sensors + IWDG       */
/*                     Publishes to MQTT broker for external dashboard      */
/****************************************************************************/

/****************************************************************************/
/* Include Files                                                            */
/****************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

/* Peripheral drivers */
#include "rcc.h"
#include "gpio.h"
#include "stk.h"
#include "nvic.h"
#include "usart.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "iwdg.h"

/* Device drivers */
#include "esp01.h"
#include "mqtt.h"
#include "mq6.h"
#include "bmp280.h"
#include "tsl2561.h"
#include "smart_light.h"

/****************************************************************************/
/* Configuration                                                            */
/****************************************************************************/
#define USART2_IRQ_NUMBER           38
#define ADC_IRQ_NUMBER              18

/* WiFi Credentials */
#define WIFI_SSID                   "FRITZ!Box 7530 PA"
#define WIFI_PASSWORD               "04648572382621641500"

/* MQTT Broker Configuration */
#define MQTT_BROKER_IP              "broker.hivemq.com"
#define MQTT_BROKER_PORT            1883

/* MQTT Configuration */
#define MQTT_CLIENT_ID              "STM32_ESP01_Multi_Sensor"
#define MQTT_TOPIC_MQ6              "sensors/stm32/mq6"
#define MQTT_TOPIC_BMP280           "sensors/stm32/bmp280"
#define MQTT_TOPIC_TSL2561          "sensors/stm32/tsl2561"
#define MQTT_TOPIC_COMBINED         "sensors/stm32/combined"
#define MQTT_TOPIC_PWM              "sensors/stm32/pwm"
#define MQTT_TOPIC_STATUS           "devices/stm32/status"
#define MQTT_TOPIC_CONTROL          "devices/stm32/control"

/* Timing Configuration */
#define LED_BLINK_INTERVAL          1000
#define ERROR_BLINK_INTERVAL        100
#define MQTT_PUBLISH_INTERVAL       10000
#define MQTT_KEEPALIVE_INTERVAL     30000
#define MQ6_READING_COUNT           10

/* IWDG Configuration */
#define IWDG_TIMEOUT_MS             10000   /* 10 second timeout */
#define IWDG_REFRESH_INTERVAL       2000    /* Refresh every 2 seconds */

/****************************************************************************/
/* Logging Abstraction                                                      */
/*  - All debug/print output goes through AppLog/LOG                        */
/*  - Only ONE place uses printf(), easy to replace with USART/RTOS-safe    */
/****************************************************************************/
static void AppLog(const char *format, ...)
{
    char buffer[256];   /* Adjust size as needed */
    va_list args;
    va_start(args, format);
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    if (len < 0) {
        return;
    }
    if (len >= (int)sizeof(buffer)) {
        len = (int)sizeof(buffer) - 1;
        buffer[len] = '\0';
    }

    /* Single place where printf is actually used */
    printf("%s", buffer);
}

#define LOG(...)   AppLog(__VA_ARGS__)

/****************************************************************************/
/* Application State Machine                                                */
/****************************************************************************/
typedef enum {
    APP_STATE_INIT,
    APP_STATE_ESP01_INIT,
    APP_STATE_WIFI_MODE,
    APP_STATE_WIFI_CONNECT,
    APP_STATE_GET_IP,
    APP_STATE_MQTT_CONNECT_TCP,
    APP_STATE_MQTT_CONNECT_PROTOCOL,
    APP_STATE_MQTT_SUBSCRIBE,
    APP_STATE_MQTT_RUNNING,
    APP_STATE_ERROR
} AppState_t;

typedef enum {
    MQ6_APP_STATE_PREHEATING,
    MQ6_APP_STATE_CALIBRATING,
    MQ6_APP_STATE_READY,
    MQ6_APP_STATE_ERROR
} MQ6AppState_t;

/****************************************************************************/
/* Global Variables                                                         */
/****************************************************************************/
static MQ6AppState_t g_MQ6AppState = MQ6_APP_STATE_PREHEATING;
volatile uint16_t g_u16AdcValue = 0;
volatile uint8_t g_u8ConversionComplete = 0;

static uint32_t g_led_timer = 0;
static uint32_t g_error_led_timer = 0;
static uint32_t g_mqtt_publish_timer = 0;
static uint32_t g_mqtt_keepalive_timer = 0;
static uint32_t g_iwdg_refresh_timer = 0;
static bool g_led_state = false;
static bool g_error_led_state = false;
static uint8_t g_system_healthy = 1;

/* Deferred Light ACK - to avoid blocking during MQTT callback */
static volatile uint8_t g_light_ack_pending = 0;
static volatile SmartLight_Channels_t g_light_ack_channels = { 0, 0, 0 };
static volatile SmartLight_Mode_t g_light_ack_mode = SMART_LIGHT_MODE_MANUAL;

/****************************************************************************/
/* ADC Callback                                                             */
/****************************************************************************/
void ADC_ConversionCallback(void)
{
    ADC_ReadValue(ADC_INSTANCE_1, (uint16_t*)&g_u16AdcValue);
    g_u8ConversionComplete = 1;
}

/****************************************************************************/
/* System Initialization Functions                                          */
/****************************************************************************/
void SystemClock_Init(void)
{
    RCC_InitializeSystemClock();
    RCC_EnableLSI(); //for iwdgt

    RCC_EnablePeripheralClock(RCC_BUS_AHB1, RCC_AHB1_GPIOA_BIT);
    RCC_EnablePeripheralClock(RCC_BUS_AHB1, RCC_AHB1_GPIOB_BIT);
    RCC_EnablePeripheralClock(RCC_BUS_APB2, RCC_APB2_USART1_BIT);
    RCC_EnablePeripheralClock(RCC_BUS_APB1, RCC_APB1_USART2_BIT);
    RCC_EnablePeripheralClock(RCC_BUS_APB2, RCC_APB2_ADC1_BIT);
    RCC_EnablePeripheralClock(RCC_BUS_APB1, RCC_APB1_I2C1_BIT);
    RCC_EnablePeripheralClock(RCC_BUS_APB1, RCC_APB1_TIM2_BIT);
}

void GPIO_Init(void)
{
    /* LED pin (PA0) */
    GPIO_SetPinMode(GPIO_PORT_A, GPIO_PIN_0, GPIO_MODE_OUTPUT);
    GPIO_SetOutputType(GPIO_PORT_A, GPIO_PIN_0, GPIO_OUTPUT_TYPE_PUSH_PULL);
    GPIO_SetOutputSpeed(GPIO_PORT_A, GPIO_PIN_0, GPIO_OUTPUT_SPEED_MEDIUM);

    /* MQ6 ADC pin (PA1) */
    GPIO_SetPinMode(GPIO_PORT_A, GPIO_PIN_1, GPIO_MODE_ANALOG);

    /* USART1 pins (PA9 = TX, PA10 = RX) */
    GPIO_SetPinMode(GPIO_PORT_A, GPIO_PIN_9, GPIO_MODE_ALTF);
    GPIO_SetPinMode(GPIO_PORT_A, GPIO_PIN_10, GPIO_MODE_ALTF);
    GPIO_SetPinAltFunction(GPIO_PORT_A, GPIO_PIN_9, GPIO_ALTFN_7);
    GPIO_SetPinAltFunction(GPIO_PORT_A, GPIO_PIN_10, GPIO_ALTFN_7);

    /* USART2 pins (PA2 = TX, PA3 = RX) */
    GPIO_SetPinMode(GPIO_PORT_A, GPIO_PIN_2, GPIO_MODE_ALTF);
    GPIO_SetPinMode(GPIO_PORT_A, GPIO_PIN_3, GPIO_MODE_ALTF);
    GPIO_SetPinAltFunction(GPIO_PORT_A, GPIO_PIN_2, GPIO_ALTFN_7);
    GPIO_SetPinAltFunction(GPIO_PORT_A, GPIO_PIN_3, GPIO_ALTFN_7);

    /* I2C1 pins (PB8 = SCL, PB9 = SDA) */
    GPIO_SetPinMode(GPIO_PORT_B, GPIO_PIN_8, GPIO_MODE_ALTF);
    GPIO_SetPinAltFunction(GPIO_PORT_B, GPIO_PIN_8, GPIO_ALTFN_4);
    GPIO_SetOutputType(GPIO_PORT_B, GPIO_PIN_8, GPIO_OUTPUT_TYPE_OPEN_DRAIN);
    GPIO_SetOutputSpeed(GPIO_PORT_B, GPIO_PIN_8, GPIO_OUTPUT_SPEED_HIGH);
    GPIO_SetPullState(GPIO_PORT_B, GPIO_PIN_8, GPIO_PULL_UP);

    GPIO_SetPinMode(GPIO_PORT_B, GPIO_PIN_9, GPIO_MODE_ALTF);
    GPIO_SetPinAltFunction(GPIO_PORT_B, GPIO_PIN_9, GPIO_ALTFN_4);
    GPIO_SetOutputType(GPIO_PORT_B, GPIO_PIN_9, GPIO_OUTPUT_TYPE_OPEN_DRAIN);
    GPIO_SetOutputSpeed(GPIO_PORT_B, GPIO_PIN_9, GPIO_OUTPUT_SPEED_HIGH);
    GPIO_SetPullState(GPIO_PORT_B, GPIO_PIN_9, GPIO_PULL_UP);

    /* TIM2 pins for three PWM channels (color temperature LEDs) */
    /* PA5 = TIM2_CH1 = Warm White (3000K) */
    GPIO_SetPinMode(GPIO_PORT_A, GPIO_PIN_5, GPIO_MODE_ALTF);
    GPIO_SetPinAltFunction(GPIO_PORT_A, GPIO_PIN_5, GPIO_ALTFN_1);
    GPIO_SetOutputType(GPIO_PORT_A, GPIO_PIN_5, GPIO_OUTPUT_TYPE_PUSH_PULL);
    GPIO_SetOutputSpeed(GPIO_PORT_A, GPIO_PIN_5, GPIO_OUTPUT_SPEED_HIGH);
    GPIO_SetPullState(GPIO_PORT_A, GPIO_PIN_5, GPIO_PULL_FLOATING);

    /* PA6 = TIM2_CH2 = Neutral White (4000K) */
    GPIO_SetPinMode(GPIO_PORT_A, GPIO_PIN_6, GPIO_MODE_ALTF);
    GPIO_SetPinAltFunction(GPIO_PORT_A, GPIO_PIN_6, GPIO_ALTFN_1);
    GPIO_SetOutputType(GPIO_PORT_A, GPIO_PIN_6, GPIO_OUTPUT_TYPE_PUSH_PULL);
    GPIO_SetOutputSpeed(GPIO_PORT_A, GPIO_PIN_6, GPIO_OUTPUT_SPEED_HIGH);
    GPIO_SetPullState(GPIO_PORT_A, GPIO_PIN_6, GPIO_PULL_FLOATING);

    /* PA7 = TIM2_CH3 = Cool White (6000K) */
    GPIO_SetPinMode(GPIO_PORT_A, GPIO_PIN_7, GPIO_MODE_ALTF);
    GPIO_SetPinAltFunction(GPIO_PORT_A, GPIO_PIN_7, GPIO_ALTFN_1);
    GPIO_SetOutputType(GPIO_PORT_A, GPIO_PIN_7, GPIO_OUTPUT_TYPE_PUSH_PULL);
    GPIO_SetOutputSpeed(GPIO_PORT_A, GPIO_PIN_7, GPIO_OUTPUT_SPEED_HIGH);
    GPIO_SetPullState(GPIO_PORT_A, GPIO_PIN_7, GPIO_PULL_FLOATING);
}

void NVIC_Init(void)
{
    NVIC_SetInterruptGroupMode(NVIC_GROUP_MODE_G16S0);
    NVIC_SetInterruptPriority(USART2_IRQ_NUMBER, NVIC_GROUP_MODE_G16S0, 1, 0);
    NVIC_EnableInterrupt(USART2_IRQ_NUMBER);

    NVIC_SetInterruptPriority(ADC_IRQ_NUMBER, NVIC_GROUP_MODE_G16S0, 2, 0);
    NVIC_EnableInterrupt(ADC_IRQ_NUMBER);
}

uint8_t USART_Init(void)
{
    USART_Status_t status;
    status = USART_Initialize(USART_INSTANCE_1);
    if (status != USART_STATUS_OK)
        return 0;

    USART_Enable(USART_INSTANCE_1);

    USART_Initialize(USART_INSTANCE_2);
    USART_Enable(USART_INSTANCE_2);

    return 1;
}

uint8_t ADC_Init(void)
{
    ADC_Status_t status;
    LOG("Initializing ADC...\r\n");
    status = ADC_Initialize(ADC_INSTANCE_1);
    if (status != ADC_STATUS_OK) {
        LOG("ADC initialization failed! Status: %d\r\n", status);
        return 0;
    }
    status = ADC_Enable(ADC_INSTANCE_1);
    if (status != ADC_STATUS_OK) {
        LOG("ADC enable failed! Status: %d\r\n", status);
        return 0;
    }
    status = ADC_SetCallback(ADC_INSTANCE_1, ADC_ConversionCallback);
    if (status != ADC_STATUS_OK) {
        LOG("ADC callback setup failed! Status: %d\r\n", status);
        return 0;
    }
    return 1;
}

uint8_t I2C_Init(void)
{
    I2C_Status_t status;
    LOG("Initializing I2C1...\r\n");
    status = I2C_Initialize(I2C_INSTANCE_1);
    if (status != I2C_STATUS_OK) {
        LOG("I2C initialization failed! Status: %d\r\n", status);
        return 0;
    }
    LOG("I2C initialization successful.\r\n");
    return 1;
}

uint8_t MQ6_Init(void)
{
    MQ6_Status_t status;
    LOG("Initializing MQ6 gas sensor...\r\n");
    status = MQ6_Initialize();
    if (status != MQ6_STATUS_OK) {
        LOG("MQ6 initialization failed! Status: %d\r\n", status);
        return 0;
    }
    LOG("MQ6 sensor initialized successfully! Starting preheat...\r\n");
    g_MQ6AppState = MQ6_APP_STATE_PREHEATING;
    return 1;
}

uint8_t BMP280_Init(void)
{
    BMP280_Status_t status;
    LOG("Initializing BMP280 sensor...\r\n");
    LOG("I2C Address: 0x%02X\r\n", BMP280_I2C_ADDRESS);
    status = BMP280_Initialize(BMP280_I2C_INSTANCE, BMP280_I2C_ADDRESS);
    if (status != BMP280_STATUS_OK) {
        LOG("BMP280 initialization failed! Status: %d\r\n", status);
        LOG("Check: I2C connections, pull-ups, power supply, address\r\n");
        return 0;
    }
    LOG("BMP280 initialization successful!\r\n");
    return 1;
}

uint8_t TSL2561_Init(void)
{
    TSL2561_Status_t status;
    LOG("Initializing TSL2561 light sensor...\r\n");
    LOG("I2C Address: 0x%02X\r\n", TSL2561_I2C_ADDRESS);
    status = TSL2561_Initialize(TSL2561_I2C_INSTANCE, TSL2561_I2C_ADDRESS);
    if (status != TSL2561_STATUS_OK) {
        LOG("TSL2561 initialization failed! Status: %d\r\n", status);
        LOG("Check: I2C connections, pull-ups, power supply, address\r\n");
        return 0;
    }
    LOG("TSL2561 initialization successful!\r\n");
    return 1;
}

uint8_t IWDG_Init(void)
{
    IWDG_Status_t status;
    uint8_t was_reset = 0;

    /* Configure watchdog manually for ~10s timeout */
    IWDG_Config_t config = IWDG_TIMEOUT_10S;

    /* Check reset cause */
    was_reset = RCC_WasResetByIWDG();
    if (was_reset) {
        LOG("\r\n!!! SYSTEM RESET BY WATCHDOG !!!\r\n");
        LOG("Previous operation hung or exceeded timeout.\r\n\r\n");
        RCC_ClearResetFlags();
    }

    /* Initialize watchdog with manual config */
    LOG("Initializing IWDG (%dms timeout)...\r\n", IWDG_TIMEOUT_MS);
    status = IWDG_Initialize(&config);
    if (status != IWDG_STATUS_OK) {
        LOG("IWDG initialization failed! Status: %d\r\n", status);
        return 0;
    }

    LOG("IWDG enabled - refresh every %dms\r\n\r\n", IWDG_REFRESH_INTERVAL);
    return 1;
}

/****************************************************************************/
/* IWDG Refresh Task                                                        */
/****************************************************************************/
void IWDG_RefreshTask(void)
{
    if (!g_system_healthy) {
        return;
    }

    if (STK_DelayElapsed(&g_iwdg_refresh_timer, IWDG_REFRESH_INTERVAL)) {
        IWDG_Refresh();
    }
}

/****************************************************************************/
/* LED Control Functions                                                    */
/****************************************************************************/
void LED_UpdateNormal(void)
{
    if (STK_DelayElapsed(&g_led_timer, LED_BLINK_INTERVAL)) {
        g_led_state = !g_led_state;
        GPIO_SetPinValue(GPIO_PORT_A, GPIO_PIN_0,
                         g_led_state ? GPIO_PIN_HIGH : GPIO_PIN_LOW);
    }
}

void LED_UpdateError(void)
{
    if (STK_DelayElapsed(&g_error_led_timer, ERROR_BLINK_INTERVAL)) {
        g_error_led_state = !g_error_led_state;
        GPIO_SetPinValue(GPIO_PORT_A, GPIO_PIN_0,
                         g_error_led_state ? GPIO_PIN_HIGH : GPIO_PIN_LOW);
    }
}

/****************************************************************************/
/* Sensor Data Formatting Functions                                         */
/****************************************************************************/
uint8_t FormatMQ6Message(char *buffer, uint16_t buffer_size,
                         MQ6_Reading_t *reading)
{
    int length;
    if (buffer == NULL || reading == NULL || buffer_size < 100) {
        return 0;
    }
    length = snprintf(buffer, buffer_size,
                      "{\"adc_raw\":%u,\"voltage_mv\":%u,"
                      "\"gas_ppm\":%u,\"timestamp\":%lu}",
                      reading->raw_adc, reading->voltage_mv,
                      reading->gas_ppm, reading->timestamp);
    if (length < 0 || length >= buffer_size) {
        return 0;
    }
    return (uint8_t)length;
}

uint8_t FormatBMP280Message(char *buffer, uint16_t buffer_size,
                            BMP280_Data_t *data)
{
    int length;
    if (buffer == NULL || data == NULL || buffer_size < 150) {
        return 0;
    }
    int32_t temp_int = data->temperature / 100;
    int32_t temp_frac = data->temperature % 100;
    if (temp_frac < 0) temp_frac = -temp_frac;

    uint32_t press_hpa  = data->pressure / 100;
    uint32_t press_frac = data->pressure % 100;

    length = snprintf(buffer, buffer_size,
                      "{\"temp_c\":%ld.%02ld,\"pressure_hpa\":%lu.%02lu,"
                      "\"pressure_pa\":%lu,\"timestamp\":%lu}",
                      temp_int, temp_frac, press_hpa, press_frac,
                      data->pressure, data->timestamp);
    if (length < 0 || length >= buffer_size) {
        return 0;
    }
    return (uint8_t)length;
}

uint8_t FormatTSL2561Message(char *buffer, uint16_t buffer_size,
                              TSL2561_Data_t *data)
{
    int length;
    if (buffer == NULL || data == NULL || buffer_size < 100) {
        return 0;
    }
    length = snprintf(buffer, buffer_size,
                      "{\"lux\":%lu,\"ch0\":%u,\"ch1\":%u,\"timestamp\":%lu}",
                      data->lux, data->channel0, data->channel1,
                      data->timestamp);
    if (length < 0 || length >= buffer_size) {
        return 0;
    }
    return (uint8_t)length;
}

uint8_t FormatCombinedMessage(char *buffer, uint16_t buffer_size,
                              MQ6_Reading_t *mq6, BMP280_Data_t *bmp280)
{
    int length;
    if (buffer == NULL || mq6 == NULL || bmp280 == NULL ||
        buffer_size < 256) {
        return 0;
    }
    int32_t temp_int = bmp280->temperature / 100;
    int32_t temp_frac = bmp280->temperature % 100;
    if (temp_frac < 0) temp_frac = -temp_frac;
    uint32_t press_hpa = bmp280->pressure / 100;

    length = snprintf(buffer, buffer_size,
                      "{\"mq6\":{\"gas_ppm\":%u,\"voltage_mv\":%u},"
                      "\"bmp280\":{\"temp_c\":%ld.%02ld,"
                      "\"pressure_hpa\":%lu},"
                      "\"timestamp\":%lu}",
                      mq6->gas_ppm, mq6->voltage_mv,
                      temp_int, temp_frac, press_hpa, STK_GetMillis());
    if (length < 0 || length >= buffer_size) {
        return 0;
    }
    return (uint8_t)length;
}

/****************************************************************************/
/* MQ6 Sensor Handling                                                      */
/****************************************************************************/
void HandleMQ6Sensor(void)
{
    static uint32_t last_state_check = 0;
    uint8_t is_preheated = 0;
    uint8_t calibration_required = 0;
    uint32_t remaining_time;

    if (!STK_DelayElapsed(&last_state_check, 5000)) {
        return;
    }

    if (g_MQ6AppState == MQ6_APP_STATE_PREHEATING) {
        MQ6_IsPreheated(&is_preheated);
        remaining_time = MQ6_GetPreheatTime();
        if (is_preheated) {
            LOG("[MQ6] Preheating complete!\r\n");
            MQ6_IsCalibrationRequired(&calibration_required);
            if (calibration_required) {
                LOG("[MQ6] Calibration required. Starting...\r\n");
                g_MQ6AppState = MQ6_APP_STATE_CALIBRATING;
            } else {
                LOG("[MQ6] Sensor ready!\r\n");
                g_MQ6AppState = MQ6_APP_STATE_READY;
            }
        } else {
            LOG("[MQ6] Preheating... Remaining: %lu ms\r\n", remaining_time);
        }
    } else if (g_MQ6AppState == MQ6_APP_STATE_CALIBRATING) {
        if (PerformMQ6Calibration()) {
            LOG("[MQ6] Calibration successful!\r\n");
            g_MQ6AppState = MQ6_APP_STATE_READY;
        } else {
            LOG("[MQ6] Calibration in progress...\r\n");
        }
    } else if (g_MQ6AppState == MQ6_APP_STATE_ERROR) {
        LOG("[MQ6] Error state. Attempting reset...\r\n");
        if (MQ6_Reset() == MQ6_STATUS_OK) {
            g_MQ6AppState = MQ6_APP_STATE_PREHEATING;
        }
    }
}

/****************************************************************************/
/* Sensor Data Publishing Functions                                         */
/****************************************************************************/
void PublishMQ6Data(void)
{
    char messageBuffer[192];
    uint8_t messageLength;
    uint16_t adcReadings[MQ6_READING_COUNT];
    MQ6_Reading_t mq6Reading;
    MQ6_Status_t status;

    if (g_MQ6AppState != MQ6_APP_STATE_READY) {
        return;
    }

    if (ReadADCValues(adcReadings, MQ6_READING_COUNT)) {
        memset(&mq6Reading, 0, sizeof(MQ6_Reading_t));
        status = MQ6_ReadComplete(adcReadings, &mq6Reading);
        if (status == MQ6_STATUS_OK) {
            messageLength = FormatMQ6Message(messageBuffer,
                                             sizeof(messageBuffer),
                                             &mq6Reading);
            if (messageLength > 0) {
                LOG("[%lu] Publishing MQ6: %s\r\n",
                    STK_GetMillis(), messageBuffer);
                MQTT_Publish(MQTT_TOPIC_MQ6, MQTT_QOS_0,
                             (uint8_t*)messageBuffer, messageLength);
            }
        }
    }
}

void PublishBMP280Data(void)
{
    char messageBuffer[192];
    uint8_t messageLength;
    BMP280_Data_t bmp280Data;
    BMP280_Status_t status;

    status = BMP280_ReadSensorData(&bmp280Data);
    if (status == BMP280_STATUS_OK) {
        messageLength = FormatBMP280Message(messageBuffer,
                                            sizeof(messageBuffer),
                                            &bmp280Data);
        if (messageLength > 0) {
            LOG("[%lu] Publishing BMP280: %s\r\n",
                STK_GetMillis(), messageBuffer);
            MQTT_Publish(MQTT_TOPIC_BMP280, MQTT_QOS_0,
                         (uint8_t*)messageBuffer, messageLength);
        }
    } else {
        LOG("[%lu] BMP280 read failed! Status: %d\r\n",
            STK_GetMillis(), status);
    }
}

void PublishTSL2561Data(void)
{
    char messageBuffer[128];
    uint8_t messageLength;
    TSL2561_Data_t tsl2561Data;
    TSL2561_Status_t status;

    status = TSL2561_ReadSensorData(&tsl2561Data);
    if (status == TSL2561_STATUS_OK || status == TSL2561_STATUS_OVERFLOW) {
        messageLength = FormatTSL2561Message(messageBuffer,
                                              sizeof(messageBuffer),
                                              &tsl2561Data);
        if (messageLength > 0) {
            LOG("[%lu] Publishing TSL2561: %s\r\n",
                STK_GetMillis(), messageBuffer);
            MQTT_Publish(MQTT_TOPIC_TSL2561, MQTT_QOS_0,
                         (uint8_t*)messageBuffer, messageLength);
        }
    } else {
        LOG("[%lu] TSL2561 read failed! Status: %d\r\n",
            STK_GetMillis(), status);
    }
}

void PublishCombinedData(void)
{
    char messageBuffer[320];
    uint8_t messageLength;
    uint16_t adcReadings[MQ6_READING_COUNT];
    MQ6_Reading_t mq6Reading;
    BMP280_Data_t bmp280Data;

    if (g_MQ6AppState != MQ6_APP_STATE_READY) {
        return;
    }
    if (!ReadADCValues(adcReadings, MQ6_READING_COUNT)) {
        return;
    }

    memset(&mq6Reading, 0, sizeof(MQ6_Reading_t));
    if (MQ6_ReadComplete(adcReadings, &mq6Reading) != MQ6_STATUS_OK) {
        return;
    }
    if (BMP280_ReadSensorData(&bmp280Data) != BMP280_STATUS_OK) {
        return;
    }

    messageLength = FormatCombinedMessage(messageBuffer,
                                          sizeof(messageBuffer),
                                          &mq6Reading, &bmp280Data);
    if (messageLength > 0) {
        LOG("[%lu] Publishing Combined: %s\r\n",
            STK_GetMillis(), messageBuffer);
        MQTT_Publish(MQTT_TOPIC_COMBINED, MQTT_QOS_0,
                     (uint8_t*)messageBuffer, messageLength);
    }
}

void PublishSensorStatus(void)
{
    char messageBuffer[192];
    uint8_t messageLength;
    const char *mq6_state = "unknown";

    if (g_MQ6AppState == MQ6_APP_STATE_PREHEATING)
        mq6_state = "preheating";
    else if (g_MQ6AppState == MQ6_APP_STATE_CALIBRATING)
        mq6_state = "calibrating";
    else if (g_MQ6AppState == MQ6_APP_STATE_ERROR)
        mq6_state = "error";
    else
        mq6_state = "ready";

    uint32_t remaining_preheat = MQ6_GetPreheatTime();
    messageLength = (uint8_t)snprintf(
        messageBuffer, sizeof(messageBuffer),
        "{\"mq6_status\":\"%s\",\"preheat_remaining\":%lu,"
        "\"bmp280_status\":\"ready\",\"timestamp\":%lu}",
        mq6_state, remaining_preheat, STK_GetMillis());

    if (messageLength > 0) {
        LOG("[%lu] Publishing Status: %s\r\n",
            STK_GetMillis(), messageBuffer);
        MQTT_Publish(MQTT_TOPIC_STATUS, MQTT_QOS_0,
                     (uint8_t*)messageBuffer, messageLength);
    }
}

void PublishLightStatus(void)
{
    char messageBuffer[256];
    uint8_t messageLength;
    SmartLight_State_t light_state;

    SmartLight_GetState(&light_state);

    messageLength = (uint8_t)snprintf(
        messageBuffer, sizeof(messageBuffer),
        "{\"mode\":\"%s\",\"warm\":%lu,\"neutral\":%lu,\"cool\":%lu,"
        "\"brightness\":%lu,\"lux\":%lu,\"temp_c100\":%ld,"
        "\"timestamp\":%lu}",
        light_state.mode == SMART_LIGHT_MODE_AUTOMATIC ? "auto" : "manual",
        light_state.channels.warm,
        light_state.channels.neutral,
        light_state.channels.cool,
        light_state.brightness,
        light_state.last_lux,
        light_state.last_temp,
        STK_GetMillis());

    if (messageLength > 0) {
        MQTT_Publish(MQTT_TOPIC_PWM, MQTT_QOS_0,
                     (uint8_t*)messageBuffer, messageLength);
    }
}

/****************************************************************************/
/* MQTT State Handlers                                                      */
/****************************************************************************/
void HandleMQTTRunning(void)
{
    LED_UpdateNormal();

    if (STK_DelayElapsed(&g_mqtt_publish_timer, MQTT_PUBLISH_INTERVAL)) {
        if (g_MQ6AppState == MQ6_APP_STATE_READY) {
            PublishMQ6Data();
            PublishBMP280Data();
            PublishTSL2561Data();
        } else {
            PublishSensorStatus();
            PublishTSL2561Data();  /* TSL2561 doesn't need preheat */
        }
    }

    if (STK_DelayElapsed(&g_mqtt_keepalive_timer, MQTT_KEEPALIVE_INTERVAL)) {
        LOG("[%lu] Sending MQTT keep-alive...\r\n", STK_GetMillis());
        MQTT_SendPingRequest();
    }
}

/****************************************************************************/
/* Send Deferred Light ACK                                                  */
/****************************************************************************/
void SendDeferredLightAck(void)
{
    if (!g_light_ack_pending) {
        return;
    }

    SmartLight_Channels_t ch = g_light_ack_channels;
    SmartLight_Mode_t mode = g_light_ack_mode;
    g_light_ack_pending = 0;

    char ack_buffer[160];
    uint8_t ack_len = (uint8_t)snprintf(
        ack_buffer, sizeof(ack_buffer),
        "{\"mode\":\"%s\",\"warm\":%lu,\"neutral\":%lu,\"cool\":%lu,\"status\":\"ok\"}",
        mode == SMART_LIGHT_MODE_AUTOMATIC ? "auto" : "manual",
        ch.warm, ch.neutral, ch.cool);
    LOG("[DEBUG] Sending deferred Light ACK: %s\r\n", ack_buffer);
    MQTT_Publish(MQTT_TOPIC_PWM, MQTT_QOS_0,
                 (uint8_t*)ack_buffer, ack_len);
    LOG("[DEBUG] Deferred Light ACK sent\r\n");
}

/****************************************************************************/
/* JSON Parsing Helpers                                                     */
/****************************************************************************/
static uint32_t ParseJsonNumber(const char *msg, const char *key, uint32_t default_val)
{
    char search[32];
    snprintf(search, sizeof(search), "\"%s\":", key);
    char *ptr = strstr(msg, search);
    if (ptr == NULL) return default_val;

    ptr += strlen(search);
    while (*ptr == ' ' || *ptr == '\t') ptr++;

    uint32_t val = 0;
    while (*ptr >= '0' && *ptr <= '9') {
        val = val * 10 + (*ptr - '0');
        ptr++;
    }
    return val;
}

static int ParseJsonString(const char *msg, const char *key, char *out, int out_size)
{
    char search[32];
    snprintf(search, sizeof(search), "\"%s\":\"", key);
    char *ptr = strstr(msg, search);
    if (ptr == NULL) return 0;

    ptr += strlen(search);
    int i = 0;
    while (*ptr != '"' && *ptr != '\0' && i < out_size - 1) {
        out[i++] = *ptr++;
    }
    out[i] = '\0';
    return i;
}

/****************************************************************************/
/* MQTT Message Callback                                                    */
/****************************************************************************/
void MQTT_MessageCallback(const char *topic, const uint8_t *payload,
                          uint16_t length)
{
    char message[192];

    LOG("\r\n========================================\r\n");
    LOG("[DEBUG] MQTT Callback triggered!\r\n");
    LOG("[DEBUG] Topic: '%s'\r\n", topic);

    if (length >= sizeof(message)) {
        length = sizeof(message) - 1;
    }

    memcpy(message, payload, length);
    message[length] = '\0';

    LOG("[DEBUG] Message: '%s'\r\n", message);

    if (strcmp(topic, MQTT_TOPIC_CONTROL) != 0) {
        LOG("[DEBUG] Topic mismatch, ignoring\r\n");
        LOG("========================================\r\n\r\n");
        return;
    }

    LOG("[DEBUG] Processing light control command...\r\n");

    SmartLight_Channels_t current_channels;
    SmartLight_GetChannels(&current_channels);

    /* Check for mode setting */
    char mode_str[16] = {0};
    if (ParseJsonString(message, "mode", mode_str, sizeof(mode_str)) > 0) {
        if (strcmp(mode_str, "auto") == 0) {
            SmartLight_SetMode(SMART_LIGHT_MODE_AUTOMATIC);
            g_light_ack_mode = SMART_LIGHT_MODE_AUTOMATIC;
            SmartLight_GetChannels(&g_light_ack_channels);
            g_light_ack_pending = 1;
            LOG("[LIGHT] Mode set to AUTOMATIC\r\n");
            LOG("========================================\r\n\r\n");
            return;
        } else if (strcmp(mode_str, "manual") == 0) {
            SmartLight_SetMode(SMART_LIGHT_MODE_MANUAL);
            /* Continue to check for channel values */
        }
    }

    /* Check for three-channel control (warm, neutral, cool) */
    char *warm_ptr = strstr(message, "\"warm\":");
    char *neutral_ptr = strstr(message, "\"neutral\":");
    char *cool_ptr = strstr(message, "\"cool\":");

    if (warm_ptr || neutral_ptr || cool_ptr) {
        /* Multi-channel control mode */
        uint32_t warm = ParseJsonNumber(message, "warm", current_channels.warm);
        uint32_t neutral = ParseJsonNumber(message, "neutral", current_channels.neutral);
        uint32_t cool = ParseJsonNumber(message, "cool", current_channels.cool);

        /* Clamp values */
        if (warm > 1000) warm = 1000;
        if (neutral > 1000) neutral = 1000;
        if (cool > 1000) cool = 1000;

        SmartLight_SetChannels(warm, neutral, cool);

        LOG("[LIGHT] Set W:%lu N:%lu C:%lu\r\n", warm, neutral, cool);

        /* Queue ACK */
        g_light_ack_mode = SMART_LIGHT_MODE_MANUAL;
        SmartLight_GetChannels(&g_light_ack_channels);
        g_light_ack_pending = 1;
    }
    /* Legacy single-duty support for backward compatibility */
    else {
        char *duty_str = strstr(message, "\"duty\":");
        if (duty_str != NULL) {
            uint32_t duty = ParseJsonNumber(message, "duty", 0);
            if (duty <= 1000) {
                /* Apply to all channels equally for legacy support */
                SmartLight_SetChannels(duty, duty, duty);
                LOG("[LIGHT] Legacy duty applied: %lu to all channels\r\n", duty);

                g_light_ack_mode = SMART_LIGHT_MODE_MANUAL;
                SmartLight_GetChannels(&g_light_ack_channels);
                g_light_ack_pending = 1;
            }
        }
    }

    LOG("========================================\r\n\r\n");
}

/****************************************************************************/
/* Application State Handlers                                               */
/****************************************************************************/
AppState_t HandleESP01Init(void)
{
    LED_UpdateNormal();
    LOG("Initializing ESP01...\r\n");
    ESP01_Status_t status = ESP01_Initialize();
    if (status == ESP01_STATUS_OK) {
        LOG("ESP01 initialized successfully!\r\n");
        return APP_STATE_WIFI_MODE;
    } else if (status != ESP01_STATUS_IN_PROGRESS) {
        LOG("ESP01 initialization failed! Status: %d\r\n", status);
        return APP_STATE_ERROR;
    }
    return APP_STATE_ESP01_INIT;
}

AppState_t HandleWiFiMode(void)
{
    LED_UpdateNormal();
    LOG("Setting WiFi mode to Station...\r\n");
    ESP01_Status_t status = ESP01_SetWiFiMode(ESP01_MODE_STATION);
    if (status == ESP01_STATUS_OK) {
        LOG("WiFi mode set successfully\r\n");
        return APP_STATE_WIFI_CONNECT;
    } else {
        LOG("Failed to set WiFi mode! Status: %d\r\n", status);
        return APP_STATE_ERROR;
    }
}

AppState_t HandleWiFiConnect(void)
{
    LED_UpdateNormal();
    LOG("Connecting to WiFi: %s\r\n", WIFI_SSID);
    ESP01_Status_t status = ESP01_ConnectToAP(
        (uint8_t*)WIFI_SSID, (uint8_t*)WIFI_PASSWORD);
    if (status == ESP01_STATUS_OK) {
        LOG("Connected to WiFi!\r\n");
        return APP_STATE_GET_IP;
    } else {
        LOG("Failed to connect! Status: %d\r\n", status);
        return APP_STATE_ERROR;
    }
}

AppState_t HandleGetIP(void)
{
    uint8_t ipBuffer[64];
    LED_UpdateNormal();
    ESP01_Status_t status =
        ESP01_GetIPAddress(ipBuffer, sizeof(ipBuffer));
    if (status == ESP01_STATUS_OK) {
        LOG("IP Info: %s\r\n", ipBuffer);
        return APP_STATE_MQTT_CONNECT_TCP;
    } else if (status != ESP01_STATUS_IN_PROGRESS) {
        LOG("Failed to get IP, continuing...\r\n");
        return APP_STATE_MQTT_CONNECT_TCP;
    }
    return APP_STATE_GET_IP;
}

AppState_t HandleMQTTConnectTCP(void)
{
    LED_UpdateNormal();
    LOG("Connecting to MQTT broker %s:%d...\r\n",
        MQTT_BROKER_IP, MQTT_BROKER_PORT);
    ESP01_Status_t status = ESP01_ConnectTCP(
        (uint8_t*)MQTT_BROKER_IP, MQTT_BROKER_PORT);
    if (status == ESP01_STATUS_OK) {
        LOG("TCP connection established!\r\n");
        return APP_STATE_MQTT_CONNECT_PROTOCOL;
    } else {
        LOG("Failed to connect! Status: %d\r\n", status);
        return APP_STATE_ERROR;
    }
}

AppState_t HandleMQTTConnectProtocol(void)
{
    LED_UpdateNormal();
    LOG("Sending MQTT CONNECT...\r\n");
    MQTT_Status_t status = MQTT_Connect(MQTT_CLIENT_ID);
    if (status == MQTT_STATUS_OK) {
        LOG("MQTT CONNECT sent!\r\n");
        STK_DelayMs(1000);
        return APP_STATE_MQTT_SUBSCRIBE;
    } else {
        LOG("MQTT CONNECT failed! Status: %d\r\n", status);
        return APP_STATE_ERROR;
    }
}

AppState_t HandleMQTTSubscribe(void)
{
    char messageBuffer[128];
    LED_UpdateNormal();
    LOG("Subscribing to: %s\r\n", MQTT_TOPIC_CONTROL);
    MQTT_Status_t status = MQTT_Subscribe(MQTT_TOPIC_CONTROL);
    if (status == MQTT_STATUS_OK) {
        LOG("Subscribed successfully!\r\n");
        LOG("[DEBUG] Registering MQTT callback...\r\n");
        ESP01_RegisterMQTTCallback(MQTT_MessageCallback);
        LOG("[DEBUG] Callback registered successfully!\r\n");

        strcpy(messageBuffer,
               "{\"status\":\"online\",\"device\":\"STM32_Smart_Light\"}");
        uint8_t len = (uint8_t)strlen(messageBuffer);
        MQTT_Publish(MQTT_TOPIC_STATUS, MQTT_QOS_0,
                     (uint8_t*)messageBuffer, len);
        STK_DelayMs(200);
        PublishLightStatus();
        LOG("[DEBUG] Smart Lighting ready on topic: %s\r\n",
            MQTT_TOPIC_CONTROL);

        return APP_STATE_MQTT_RUNNING;
    } else {
        LOG("Subscribe failed! Status: %d\r\n", status);
        return APP_STATE_ERROR;
    }
}

AppState_t HandleError(void)
{
    static uint32_t recovery_timer = 0;
    LED_UpdateError();
    if (STK_DelayElapsed(&recovery_timer, 10000)) {
        LOG("Attempting recovery...\r\n");
        MQ6_Reset();
        g_MQ6AppState = MQ6_APP_STATE_PREHEATING;
        return APP_STATE_ESP01_INIT;
    }
    return APP_STATE_ERROR;
}

/****************************************************************************/
/* Main Function                                                            */
/****************************************************************************/
int main(void)
{
    static AppState_t app_state = APP_STATE_INIT;

    /* System initialization */
    if (app_state == APP_STATE_INIT) {
        SystemClock_Init();
        GPIO_Init();
        STK_Initialize();
        NVIC_Init();

        /* Initialize Smart Lighting driver (initializes PWM channels) */
        SmartLight_Initialize();

        if (!USART_Init()) {
            while (1);
        }

        /* Test print - verify UART is working */
        printf("UART OK\r\n");

        /* Initialize IWDG early - after USART for debug output */
        if (!IWDG_Init()) {
            while (1);
        }

        if (!ADC_Init()) {
            while (1);
        }

        if (!I2C_Init()) {
            while (1);
        }

        if (!MQ6_Init()) {
            while (1);
        }

        if (!BMP280_Init()) {
            while (1);
        }

        if (!TSL2561_Init()) {
            LOG("Warning: TSL2561 init failed, auto mode may not work\r\n");
            /* Don't halt - continue without light sensor */
        }

        LOG("\r\n========================================\r\n");
        LOG("Smart Lighting MQTT System\r\n");
        LOG("IWDG: ENABLED (%dms timeout)\r\n", IWDG_TIMEOUT_MS);
        LOG("========================================\r\n");
        LOG("MQTT Client ID: %s\r\n", MQTT_CLIENT_ID);
        LOG("MQTT Broker: %s:%d\r\n", MQTT_BROKER_IP, MQTT_BROKER_PORT);
        LOG("Sensors: MQ6 (PA1) + BMP280 (I2C) + TSL2561 (I2C)\r\n");
        LOG("LEDs: Warm(PA5) + Neutral(PA6) + Cool(PA7)\r\n");
        LOG("Control Topic: %s\r\n", MQTT_TOPIC_CONTROL);
        LOG("PWM Topic: %s\r\n", MQTT_TOPIC_PWM);
        LOG("========================================\r\n\r\n");

        /* Initialize timers */
        g_led_timer           = STK_GetMillis();
        g_error_led_timer     = STK_GetMillis();
        g_mqtt_publish_timer  = STK_GetMillis();
        g_mqtt_keepalive_timer= STK_GetMillis();
        g_iwdg_refresh_timer  = STK_GetMillis();

        app_state = APP_STATE_ESP01_INIT;
    }

    /* Main application loop */
    while (1) {
        /* CRITICAL: Refresh watchdog FIRST */
        IWDG_RefreshTask();

        /* Process MQTT messages - do this multiple times to catch rapid messages */
        for (int mqtt_iter = 0; mqtt_iter < 3; mqtt_iter++) {
            if (ESP01_HasPendingMQTTMessage()) {
                ESP01_ProcessPendingMQTTMessage();
            }
            ESP01_ProcessMQTTMessages();
        }

        /* Send any deferred Light ACK after processing all pending messages */
        SendDeferredLightAck();

        /* Handle MQ6 sensor state */
        HandleMQ6Sensor();

        /* Handle automatic lighting control */
        SmartLight_AutoTask();

        /* Application state machine */
        if (app_state == APP_STATE_ESP01_INIT) {
            app_state = HandleESP01Init();
        } else if (app_state == APP_STATE_WIFI_MODE) {
            app_state = HandleWiFiMode();
        } else if (app_state == APP_STATE_WIFI_CONNECT) {
            app_state = HandleWiFiConnect();
        } else if (app_state == APP_STATE_GET_IP) {
            app_state = HandleGetIP();
        } else if (app_state == APP_STATE_MQTT_CONNECT_TCP) {
            app_state = HandleMQTTConnectTCP();
        } else if (app_state == APP_STATE_MQTT_CONNECT_PROTOCOL) {
            app_state = HandleMQTTConnectProtocol();
        } else if (app_state == APP_STATE_MQTT_SUBSCRIBE) {
            app_state = HandleMQTTSubscribe();
        } else if (app_state == APP_STATE_MQTT_RUNNING) {
            HandleMQTTRunning();
        } else if (app_state == APP_STATE_ERROR) {
            app_state = HandleError();
        } else {
            app_state = APP_STATE_ERROR;
        }
    }

    return 0;
}
