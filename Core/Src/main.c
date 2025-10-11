/****************************************************************************/
/* Author            : Ameer Alwadiya                                       */
/* Version           : V2.3.1                                               */
/* Date              : 09 Oct 2025                                          */
/* Description       : MQTT with ESP01 + MQ6 + BMP280 Sensors              */
/*                     Added debug output for MQTT message reception        */
/****************************************************************************/

/****************************************************************************/
/* Include Files                                                            */
/****************************************************************************/
#include "rcc_interface.h"
#include "gpio_interface.h"
#include "stk_interface.h"
#include "nvic_interface.h"
#include "usart_interface.h"
#include "adc_interface.h"
#include "i2c_interface.h"
#include "esp01_interface.h"
#include "mqtt_interface.h"
#include "mq6_interface.h"
#include "bmp280_interface.h"
#include "bmp280_config.h"
#include "tim.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

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
#define MQTT_TOPIC_COMBINED         "sensors/stm32/combined"
#define MQTT_TOPIC_PWM		        "sensors/stm32/pwm"

#define MQTT_TOPIC_STATUS           "devices/stm32/status"
#define MQTT_TOPIC_CONTROL          "devices/stm32/control"

/* Timing Configuration */
#define LED_BLINK_INTERVAL          1000
#define ERROR_BLINK_INTERVAL        100
#define MQTT_PUBLISH_INTERVAL       10000
#define MQTT_KEEPALIVE_INTERVAL     30000
#define MQ6_READING_COUNT           10

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
static bool g_led_state = false;
static bool g_error_led_state = false;
static uint32_t g_current_pwm_duty = 0;

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

    /* MQ6 ADC pin (PA5) */
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

    /* TIM2 pins for channel 1 (PA5 = PWM) */
    GPIO_SetPinMode(GPIO_PORT_A, GPIO_PIN_5, GPIO_MODE_ALTF);
    GPIO_SetPinAltFunction(GPIO_PORT_A, GPIO_PIN_5, GPIO_ALTFN_1);
    GPIO_SetOutputType(GPIO_PORT_A, GPIO_PIN_5, GPIO_OUTPUT_TYPE_PUSH_PULL);
    GPIO_SetOutputSpeed(GPIO_PORT_A, GPIO_PIN_5, GPIO_OUTPUT_SPEED_HIGH);
    GPIO_SetPullState(GPIO_PORT_A, GPIO_PIN_5, GPIO_PULL_FLOATING);

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
    if (status != USART_STATUS_OK) return 0;
    USART_Enable(USART_INSTANCE_1);

    USART_Initialize(USART_INSTANCE_2);
    USART_Enable(USART_INSTANCE_2);

    return 1;
}

uint8_t ADC_Init(void)
{
    ADC_Status_t status;

    printf("Initializing ADC...\r\n");
    status = ADC_Initialize(ADC_INSTANCE_1);
    if (status != ADC_STATUS_OK) {
        printf("ADC initialization failed! Status: %d\r\n", status);
        return 0;
    }

    status = ADC_Enable(ADC_INSTANCE_1);
    if (status != ADC_STATUS_OK) {
        printf("ADC enable failed! Status: %d\r\n", status);
        return 0;
    }

    status = ADC_SetCallback(ADC_INSTANCE_1, ADC_ConversionCallback);
    if (status != ADC_STATUS_OK) {
        printf("ADC callback setup failed! Status: %d\r\n", status);
        return 0;
    }

    return 1;
}

uint8_t I2C_Init(void)
{
    I2C_Status_t status;

    printf("Initializing I2C1...\r\n");
    status = I2C_Initialize(I2C_INSTANCE_1);
    if (status != I2C_STATUS_OK) {
        printf("I2C initialization failed! Status: %d\r\n", status);
        return 0;
    }

    printf("I2C initialization successful.\r\n");
    return 1;
}

uint8_t MQ6_Init(void)
{
    MQ6_Status_t status;

    printf("Initializing MQ6 gas sensor...\r\n");
    status = MQ6_Initialize();
    if (status != MQ6_STATUS_OK) {
        printf("MQ6 initialization failed! Status: %d\r\n", status);
        return 0;
    }

    printf("MQ6 sensor initialized successfully! Starting preheat...\r\n");
    g_MQ6AppState = MQ6_APP_STATE_PREHEATING;
    return 1;
}

uint8_t BMP280_Init(void)
{
    BMP280_Status_t status;

    printf("Initializing BMP280 sensor...\r\n");
    printf("I2C Address: 0x%02X\r\n", BMP280_I2C_ADDRESS);

    status = BMP280_Initialize(BMP280_I2C_INSTANCE, BMP280_I2C_ADDRESS);
    if (status != BMP280_STATUS_OK) {
        printf("BMP280 initialization failed! Status: %d\r\n", status);
        printf("Check: I2C connections, pull-ups, power supply, address\r\n");
        return 0;
    }

    printf("BMP280 initialization successful!\r\n");
    return 1;
}

/****************************************************************************/
/* LED Control Functions                                                    */
/****************************************************************************/
void LED_UpdateNormal(void)
{
    if (STK_DelayElapsed(&g_led_timer, LED_BLINK_INTERVAL)) {
        g_led_state = !g_led_state;
        GPIO_SetPinValue(GPIO_PORT_A, GPIO_PIN_0, g_led_state ? GPIO_PIN_HIGH : GPIO_PIN_LOW);
    }
}

void LED_UpdateError(void)
{
    if (STK_DelayElapsed(&g_error_led_timer, ERROR_BLINK_INTERVAL)) {
        g_error_led_state = !g_error_led_state;
        GPIO_SetPinValue(GPIO_PORT_A, GPIO_PIN_0, g_error_led_state ? GPIO_PIN_HIGH : GPIO_PIN_LOW);
    }
}

/****************************************************************************/
/* Sensor Data Formatting Functions                                        */
/****************************************************************************/
uint8_t FormatMQ6Message(char* buffer, uint16_t buffer_size, MQ6_Reading_t* reading)
{
    int length;

    if (buffer == NULL || reading == NULL || buffer_size < 100) {
        return 0;
    }

    length = snprintf(buffer, buffer_size,
        "{\"adc_raw\":%u,\"voltage_mv\":%u,\"gas_ppm\":%u,\"timestamp\":%lu}",
        reading->raw_adc, reading->voltage_mv, reading->gas_ppm, reading->timestamp);

    if (length < 0 || length >= buffer_size) {
        return 0;
    }

    return (uint8_t)length;
}

uint8_t FormatBMP280Message(char* buffer, uint16_t buffer_size, BMP280_Data_t* data)
{
    int length;

    if (buffer == NULL || data == NULL || buffer_size < 150) {
        return 0;
    }

    int32_t temp_int = data->temperature / 100;
    int32_t temp_frac = data->temperature % 100;
    if (temp_frac < 0) temp_frac = -temp_frac;

    uint32_t press_hpa = data->pressure / 100;
    uint32_t press_frac = data->pressure % 100;

    length = snprintf(buffer, buffer_size,
        "{\"temp_c\":%ld.%02ld,\"pressure_hpa\":%lu.%02lu,\"pressure_pa\":%lu,\"timestamp\":%lu}",
        temp_int, temp_frac, press_hpa, press_frac, data->pressure, data->timestamp);

    if (length < 0 || length >= buffer_size) {
        return 0;
    }

    return (uint8_t)length;
}

uint8_t FormatCombinedMessage(char* buffer, uint16_t buffer_size,
                              MQ6_Reading_t* mq6, BMP280_Data_t* bmp280)
{
    int length;

    if (buffer == NULL || mq6 == NULL || bmp280 == NULL || buffer_size < 256) {
        return 0;
    }

    int32_t temp_int = bmp280->temperature / 100;
    int32_t temp_frac = bmp280->temperature % 100;
    if (temp_frac < 0) temp_frac = -temp_frac;

    uint32_t press_hpa = bmp280->pressure / 100;

    length = snprintf(buffer, buffer_size,
        "{\"mq6\":{\"gas_ppm\":%u,\"voltage_mv\":%u},"
        "\"bmp280\":{\"temp_c\":%ld.%02ld,\"pressure_hpa\":%lu},"
        "\"timestamp\":%lu}",
        mq6->gas_ppm, mq6->voltage_mv, temp_int, temp_frac, press_hpa,
        STK_GetMillis());

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
            printf("[MQ6] Preheating complete!\r\n");
            MQ6_IsCalibrationRequired(&calibration_required);

            if (calibration_required) {
                printf("[MQ6] Calibration required. Starting...\r\n");
                g_MQ6AppState = MQ6_APP_STATE_CALIBRATING;
            } else {
                printf("[MQ6] Sensor ready!\r\n");
                g_MQ6AppState = MQ6_APP_STATE_READY;
            }
        } else {
            printf("[MQ6] Preheating... Remaining: %lu ms\r\n", remaining_time);
        }
    }
    else if (g_MQ6AppState == MQ6_APP_STATE_CALIBRATING) {
        if (PerformMQ6Calibration()) {
            printf("[MQ6] Calibration successful!\r\n");
            g_MQ6AppState = MQ6_APP_STATE_READY;
        } else {
            printf("[MQ6] Calibration in progress...\r\n");
        }
    }
    else if (g_MQ6AppState == MQ6_APP_STATE_ERROR) {
        printf("[MQ6] Error state. Attempting reset...\r\n");
        if (MQ6_Reset() == MQ6_STATUS_OK) {
            g_MQ6AppState = MQ6_APP_STATE_PREHEATING;
        }
    }
}

/****************************************************************************/
/* Sensor Data Publishing Functions                                        */
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
            messageLength = FormatMQ6Message(messageBuffer, sizeof(messageBuffer), &mq6Reading);

            if (messageLength > 0) {
                printf("[%lu] Publishing MQ6: %s\r\n", STK_GetMillis(), messageBuffer);
                MQTT_Publish(MQTT_TOPIC_MQ6, MQTT_QOS_0, (uint8_t*)messageBuffer, messageLength);
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
        messageLength = FormatBMP280Message(messageBuffer, sizeof(messageBuffer), &bmp280Data);

        if (messageLength > 0) {
            printf("[%lu] Publishing BMP280: %s\r\n", STK_GetMillis(), messageBuffer);
            MQTT_Publish(MQTT_TOPIC_BMP280, MQTT_QOS_0, (uint8_t*)messageBuffer, messageLength);
        }
    } else {
        printf("[%lu] BMP280 read failed! Status: %d\r\n", STK_GetMillis(), status);
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

    /* Read both sensors */
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

    /* Format and publish combined message */
    messageLength = FormatCombinedMessage(messageBuffer, sizeof(messageBuffer),
                                         &mq6Reading, &bmp280Data);

    if (messageLength > 0) {
        printf("[%lu] Publishing Combined: %s\r\n", STK_GetMillis(), messageBuffer);
        MQTT_Publish(MQTT_TOPIC_COMBINED, MQTT_QOS_0, (uint8_t*)messageBuffer, messageLength);
    }
}

void PublishSensorStatus(void)
{
    char messageBuffer[192];
    uint8_t messageLength;
    const char* mq6_state = "unknown";

    if (g_MQ6AppState == MQ6_APP_STATE_PREHEATING) mq6_state = "preheating";
    else if (g_MQ6AppState == MQ6_APP_STATE_CALIBRATING) mq6_state = "calibrating";
    else if (g_MQ6AppState == MQ6_APP_STATE_ERROR) mq6_state = "error";
    else mq6_state = "ready";

    uint32_t remaining_preheat = MQ6_GetPreheatTime();

    messageLength = (uint8_t)snprintf(messageBuffer, sizeof(messageBuffer),
        "{\"mq6_status\":\"%s\",\"preheat_remaining\":%lu,\"bmp280_status\":\"ready\",\"timestamp\":%lu}",
        mq6_state, remaining_preheat, STK_GetMillis());

    if (messageLength > 0) {
        printf("[%lu] Publishing Status: %s\r\n", STK_GetMillis(), messageBuffer);
        MQTT_Publish(MQTT_TOPIC_STATUS, MQTT_QOS_0, (uint8_t*)messageBuffer, messageLength);
    }
}

void PublishPWMStatus(void)
{
    char messageBuffer[96];
    uint8_t messageLength;
    uint32_t duty_percent = (g_current_pwm_duty * 100) / 1000;

    messageLength = (uint8_t)snprintf(messageBuffer, sizeof(messageBuffer),
        "{\"duty\":%lu,\"duty_percent\":%lu,\"max\":1000,\"timestamp\":%lu}",
        g_current_pwm_duty, duty_percent, STK_GetMillis());

    if (messageLength > 0) {
        MQTT_Publish(MQTT_TOPIC_PWM, MQTT_QOS_0, (uint8_t*)messageBuffer, messageLength);
    }
}

/****************************************************************************/
/* MQTT State Handlers                                                      */
/****************************************************************************/
void HandleMQTTRunning(void)
{
    LED_UpdateNormal();

    /* Interrupt handles MQTT messages, so we just check here as backup */
    /* The main loop already calls ESP01_ProcessMQTTMessages() */

    /* Check if it's time to publish sensor data */
    if (STK_DelayElapsed(&g_mqtt_publish_timer, MQTT_PUBLISH_INTERVAL)) {
        if (g_MQ6AppState == MQ6_APP_STATE_READY) {

            PublishMQ6Data();
            PublishBMP280Data();
//            PublishPWMStatus();
//        	PublishCombinedData();
        } else {
            PublishSensorStatus();
        }
    }

    if (STK_DelayElapsed(&g_mqtt_keepalive_timer, MQTT_KEEPALIVE_INTERVAL)) {
        printf("[%lu] Sending MQTT keep-alive...\r\n", STK_GetMillis());
        MQTT_SendPingRequest();
    }
}

/****************************************************************************/
/* MQTT Message Callback - WITH DEBUG OUTPUT                               */
/****************************************************************************/
void MQTT_MessageCallback(const char* topic, const uint8_t* payload, uint16_t length)
{
    char message[128];
    uint32_t duty_cycle;

    /* DEBUG: Callback entry */
    printf("\r\n========================================\r\n");
    printf("[DEBUG] MQTT Callback triggered!\r\n");
    printf("[DEBUG] Timestamp: %lu ms\r\n", STK_GetMillis());
    printf("[DEBUG] Topic received: '%s'\r\n", topic);
    printf("[DEBUG] Message length: %u bytes\r\n", length);

    /* Validate and copy message */
    if (length >= sizeof(message)) {
        printf("[DEBUG] Message too long! Truncating from %u to %u bytes\r\n",
               length, sizeof(message) - 1);
        length = sizeof(message) - 1;
    }

    memcpy(message, payload, length);
    message[length] = '\0';

    printf("[DEBUG] Message content: '%s'\r\n", message);
    printf("[DEBUG] Expected topic: '%s'\r\n", MQTT_TOPIC_CONTROL);

    /* Topic comparison */
    int topic_match = strcmp(topic, MQTT_TOPIC_CONTROL);
    printf("[DEBUG] strcmp result: %d (0 = match)\r\n", topic_match);

    if (topic_match == 0) {
        printf("[DEBUG] ✓ Topic matched! Processing command...\r\n");

        /* Search for duty value */
        char* duty_str = strstr(message, "\"duty\":");

        if (duty_str != NULL) {
            printf("[DEBUG] ✓ Found 'duty' key in message\r\n");

            /* Skip past the key */
            duty_str += 7;  // Skip "duty":

            /* Skip whitespace after colon */
            while (*duty_str == ' ' || *duty_str == '\t') {
                duty_str++;
            }

            printf("[DEBUG] Parsing from position: '%s'\r\n", duty_str);

            /* Parse numeric value */
            duty_cycle = 0;
            int digit_count = 0;
            while (*duty_str >= '0' && *duty_str <= '9') {
                duty_cycle = duty_cycle * 10 + (*duty_str - '0');
                duty_str++;
                digit_count++;
            }

            printf("[DEBUG] Parsed value: %lu (from %d digits)\r\n", duty_cycle, digit_count);

            /* Validate range */
            if (duty_cycle <= 1000) {
                printf("[DEBUG] ✓ Value in valid range (0-1000)\r\n");
                printf("[DEBUG] Setting PWM duty cycle...\r\n");

                /* Update global state */
                g_current_pwm_duty = duty_cycle;

                /* Set hardware PWM */
                TIM_PWM_SetDutyCycle(TIM2, TIM_CHANNEL_1, duty_cycle);

                uint32_t duty_percent = (duty_cycle * 100) / 1000;
                printf("[PWM] ✓ Successfully set to: %lu/1000 (%lu%%)\r\n",
                       duty_cycle, duty_percent);

                /* Send acknowledgment */
                char ack_buffer[64];
                uint8_t ack_len = (uint8_t)snprintf(ack_buffer, sizeof(ack_buffer),
                    "{\"duty\":%lu,\"status\":\"ok\"}", duty_cycle);

                printf("[DEBUG] Sending ACK: %s\r\n", ack_buffer);
                STK_DelayMs(100);
                MQTT_Publish(MQTT_TOPIC_PWM, MQTT_QOS_0, (uint8_t*)ack_buffer, ack_len);
                printf("[DEBUG] ✓ ACK sent\r\n");
            } else {
                printf("[DEBUG] ✗ Value out of range: %lu > 1000\r\n", duty_cycle);
            }
        } else {
            printf("[DEBUG] ✗ Could not find '\"duty\":' in message\r\n");
            printf("[DEBUG] Message hex dump:\r\n");
            for (int i = 0; i < length && i < 32; i++) {
                printf("%02X ", (uint8_t)message[i]);
                if ((i + 1) % 16 == 0) printf("\r\n");
            }
            printf("\r\n");
        }
    } else {
        printf("[DEBUG] ✗ Topic did NOT match\r\n");
        printf("[DEBUG] Received topic length: %d\r\n", strlen(topic));
        printf("[DEBUG] Expected topic length: %d\r\n", strlen(MQTT_TOPIC_CONTROL));
    }

    printf("========================================\r\n\r\n");
}

/****************************************************************************/
/* Application State Handlers                                               */
/****************************************************************************/
AppState_t HandleESP01Init(void)
{
    LED_UpdateNormal();

    printf("Initializing ESP01...\r\n");
    ESP01_Status_t status = ESP01_Initialize();

    if (status == ESP01_STATUS_OK) {
        printf("ESP01 initialized successfully!\r\n");
        return APP_STATE_WIFI_MODE;
    } else if (status != ESP01_STATUS_IN_PROGRESS) {
        printf("ESP01 initialization failed! Status: %d\r\n", status);
        return APP_STATE_ERROR;
    }

    return APP_STATE_ESP01_INIT;
}

AppState_t HandleWiFiMode(void)
{
    LED_UpdateNormal();

    printf("Setting WiFi mode to Station...\r\n");
    ESP01_Status_t status = ESP01_SetWiFiMode(ESP01_MODE_STATION);

    if (status == ESP01_STATUS_OK) {
        printf("WiFi mode set successfully\r\n");
        return APP_STATE_WIFI_CONNECT;
    } else {
        printf("Failed to set WiFi mode! Status: %d\r\n", status);
        return APP_STATE_ERROR;
    }
}

AppState_t HandleWiFiConnect(void)
{
    LED_UpdateNormal();

    printf("Connecting to WiFi: %s\r\n", WIFI_SSID);
    ESP01_Status_t status = ESP01_ConnectToAP((uint8_t*)WIFI_SSID, (uint8_t*)WIFI_PASSWORD);

    if (status == ESP01_STATUS_OK) {
        printf("Connected to WiFi!\r\n");
        return APP_STATE_GET_IP;
    } else {
        printf("Failed to connect! Status: %d\r\n", status);
        return APP_STATE_ERROR;
    }
}

AppState_t HandleGetIP(void)
{
    uint8_t ipBuffer[64];
    LED_UpdateNormal();

    ESP01_Status_t status = ESP01_GetIPAddress(ipBuffer, sizeof(ipBuffer));

    if (status == ESP01_STATUS_OK) {
        printf("IP Info: %s\r\n", ipBuffer);
        return APP_STATE_MQTT_CONNECT_TCP;
    } else if (status != ESP01_STATUS_IN_PROGRESS) {
        printf("Failed to get IP, continuing...\r\n");
        return APP_STATE_MQTT_CONNECT_TCP;
    }

    return APP_STATE_GET_IP;
}

AppState_t HandleMQTTConnectTCP(void)
{
    LED_UpdateNormal();

    printf("Connecting to MQTT broker %s:%d...\r\n", MQTT_BROKER_IP, MQTT_BROKER_PORT);
    ESP01_Status_t status = ESP01_ConnectTCP((uint8_t*)MQTT_BROKER_IP, MQTT_BROKER_PORT);

    if (status == ESP01_STATUS_OK) {
        printf("TCP connection established!\r\n");
        return APP_STATE_MQTT_CONNECT_PROTOCOL;
    } else {
        printf("Failed to connect! Status: %d\r\n", status);
        return APP_STATE_ERROR;
    }
}

AppState_t HandleMQTTConnectProtocol(void)
{
    LED_UpdateNormal();

    printf("Sending MQTT CONNECT...\r\n");
    MQTT_Status_t status = MQTT_Connect(MQTT_CLIENT_ID);

    if (status == MQTT_STATUS_OK) {
        printf("MQTT CONNECT sent!\r\n");
        STK_DelayMs(1000);
        return APP_STATE_MQTT_SUBSCRIBE;
    } else {
        printf("MQTT CONNECT failed! Status: %d\r\n", status);
        return APP_STATE_ERROR;
    }
}

AppState_t HandleMQTTSubscribe(void)
{
    char messageBuffer[128];
    LED_UpdateNormal();

    printf("Subscribing to: %s\r\n", MQTT_TOPIC_CONTROL);
    MQTT_Status_t status = MQTT_Subscribe(MQTT_TOPIC_CONTROL);

    if (status == MQTT_STATUS_OK) {
        printf("Subscribed successfully!\r\n");

        /* Register callback BEFORE publishing anything */
        printf("[DEBUG] Registering MQTT callback...\r\n");
        ESP01_RegisterMQTTCallback(MQTT_MessageCallback);
        printf("[DEBUG] Callback registered successfully!\r\n");

        strcpy(messageBuffer, "{\"status\":\"online\",\"device\":\"STM32_Multi_Sensor\"}");
        uint8_t len = (uint8_t)strlen(messageBuffer);
        MQTT_Publish(MQTT_TOPIC_STATUS, MQTT_QOS_0, (uint8_t*)messageBuffer, len);

        STK_DelayMs(200);
        PublishPWMStatus();

        printf("[DEBUG] System ready to receive MQTT messages on topic: %s\r\n", MQTT_TOPIC_CONTROL);
        return APP_STATE_MQTT_RUNNING;
    } else {
        printf("Subscribe failed! Status: %d\r\n", status);
        return APP_STATE_ERROR;
    }
}

AppState_t HandleError(void)
{
    static uint32_t recovery_timer = 0;

    LED_UpdateError();

    if (STK_DelayElapsed(&recovery_timer, 10000)) {
        printf("Attempting recovery...\r\n");
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
    if (app_state == APP_STATE_INIT)
    {
        SystemClock_Init();
        GPIO_Init();
        STK_Initialize();
        NVIC_Init();
        TIM_PWM_Init(TIM2, TIM_CHANNEL_1, 16, 1000); // Prescaler = 16, Period = 1000

        if (!USART_Init()) {
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

        printf("\r\n========================================\r\n");
        printf("Multi-Sensor MQTT System Started\r\n");
        printf("========================================\r\n");
        printf("Client ID: %s\r\n", MQTT_CLIENT_ID);
        printf("Broker: %s:%d\r\n", MQTT_BROKER_IP, MQTT_BROKER_PORT);
        printf("Sensors: MQ6 (PA5) + BMP280 (I2C)\r\n");
        printf("Control Topic: %s\r\n", MQTT_TOPIC_CONTROL);
        printf("PWM Topic: %s\r\n", MQTT_TOPIC_PWM);
        printf("========================================\r\n\r\n");

        /* Initialize timers */
        g_led_timer = STK_GetMillis();
        g_error_led_timer = STK_GetMillis();
        g_mqtt_publish_timer = STK_GetMillis();
        g_mqtt_keepalive_timer = STK_GetMillis();

        app_state = APP_STATE_ESP01_INIT;
    }

    /* Main application loop */
    while (1)
    {
        /* PRIORITY 1: Process any MQTT messages detected in interrupt
         * This gives near-instant response to control commands */
        if (ESP01_HasPendingMQTTMessage()) {
            ESP01_ProcessPendingMQTTMessage();
        }

        /* Process incoming MQTT messages - MUST BE FIRST */
        ESP01_ProcessMQTTMessages();

        /* Handle MQ6 sensor state */
        HandleMQ6Sensor();

        /* Application state machine with function handlers */
        if (app_state == APP_STATE_ESP01_INIT) {
            app_state = HandleESP01Init();
        }
        else if (app_state == APP_STATE_WIFI_MODE) {
            app_state = HandleWiFiMode();
        }
        else if (app_state == APP_STATE_WIFI_CONNECT) {
            app_state = HandleWiFiConnect();
        }
        else if (app_state == APP_STATE_GET_IP) {
            app_state = HandleGetIP();
        }
        else if (app_state == APP_STATE_MQTT_CONNECT_TCP) {
            app_state = HandleMQTTConnectTCP();
        }
        else if (app_state == APP_STATE_MQTT_CONNECT_PROTOCOL) {
            app_state = HandleMQTTConnectProtocol();
        }
        else if (app_state == APP_STATE_MQTT_SUBSCRIBE) {
            app_state = HandleMQTTSubscribe();
        }
        else if (app_state == APP_STATE_MQTT_RUNNING) {
            HandleMQTTRunning();
            /* Stay in this state */
        }
        else if (app_state == APP_STATE_ERROR) {
            app_state = HandleError();
        }
        else {
            /* Unknown state - go to error */
            app_state = APP_STATE_ERROR;
        }
    }

    return 0;
}
