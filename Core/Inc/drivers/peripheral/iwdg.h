/****************************************************************************/
/* Author            : Ameer Alwadiya                                       */
/* Version           : V1.0.0                                               */
/* Date              : 09 Nov 2025                                          */
/* Description       : IWDG (Independent Watchdog) Interface Header         */
/****************************************************************************/

#ifndef IWDG_H
#define IWDG_H

/****************************************************************************/
/* Include Files                                                            */
/****************************************************************************/
#include <stdint.h>

/****************************************************************************/
/* Register Structure                                                       */
/****************************************************************************/
typedef struct {
    volatile uint32_t KR;    /* Key register */
    volatile uint32_t PR;    /* Prescaler register */
    volatile uint32_t RLR;   /* Reload register */
    volatile uint32_t SR;    /* Status register */
} IWDG_RegDef_t;

/****************************************************************************/
/* Base Address                                                             */
/****************************************************************************/
#define IWDG_BASE_ADDRESS    0x40003000U
#define IWDG                 ((IWDG_RegDef_t *)IWDG_BASE_ADDRESS)

/****************************************************************************/
/* Register Bit Definitions                                                 */
/****************************************************************************/

/* Status Register (SR) */
#define IWDG_SR_PVU          (1U << 0)  /* Prescaler value update */
#define IWDG_SR_RVU          (1U << 1)  /* Reload value update */

/****************************************************************************/
/* Key Values                                                               */
/****************************************************************************/
#define IWDG_KEY_ACCESS    	 0x5555U    /* Key to enable register access */
#define IWDG_KEY_REFRESH     0xAAAAU    /* Key to reload counter */
#define IWDG_KEY_START       0xCCCCU    /* Key to start watchdog */

/****************************************************************************/
/* Private Macros                                                           */
/****************************************************************************/
#define IWDG_MAX_RELOAD     4095U    /* Maximum reload value */
#define IWDG_TIMEOUT_US     50000U   /* Timeout for register updates (50ms) */

#define LSI_FREQUENCY       32000U   /* Typical LSI frequency in Hz */

/****************************************************************************/
/* Status Types                                                             */
/****************************************************************************/
typedef enum {
    IWDG_STATUS_OK = 0,
    IWDG_STATUS_ERROR,
    IWDG_STATUS_TIMEOUT,
    IWDG_STATUS_INVALID_PARAM
} IWDG_Status_t;

/****************************************************************************/
/* Prescaler Configuration                                                  */
/****************************************************************************/
typedef enum {
    IWDG_PRESCALER_4 = 0,      /* Timeout: 0.125ms to 512ms (@ 32kHz LSI) */
    IWDG_PRESCALER_8,          /* Timeout: 0.25ms to 1024ms */
    IWDG_PRESCALER_16,         /* Timeout: 0.5ms to 2048ms */
    IWDG_PRESCALER_32,         /* Timeout: 1ms to 4096ms */
    IWDG_PRESCALER_64,         /* Timeout: 2ms to 8192ms */
    IWDG_PRESCALER_128,        /* Timeout: 4ms to 16384ms */
    IWDG_PRESCALER_256         /* Timeout: 8ms to 32768ms (~32.7 seconds) */
} IWDG_Prescaler_t;

/****************************************************************************/
/* Configuration Structure                                                  */
/****************************************************************************/
typedef struct {
    IWDG_Prescaler_t prescaler;   /* Prescaler value */
    uint16_t reload_value;        /* Reload value (0-4095) */
} IWDG_Config_t;

/****************************************************************************/
/* Predefined Timeout Configurations                                        */
/****************************************************************************/
/* Common timeout values (assuming 32kHz LSI clock) */
#define IWDG_TIMEOUT_100MS      { IWDG_PRESCALER_32, 100 }    /* ~100ms */
#define IWDG_TIMEOUT_500MS      { IWDG_PRESCALER_32, 500 }    /* ~500ms */
#define IWDG_TIMEOUT_1S         { IWDG_PRESCALER_32, 1000 }   /* ~1 second */
#define IWDG_TIMEOUT_2S         { IWDG_PRESCALER_64, 1000 }   /* ~2 seconds */
#define IWDG_TIMEOUT_5S         { IWDG_PRESCALER_128, 1250 }  /* ~5 seconds */
#define IWDG_TIMEOUT_10S        { IWDG_PRESCALER_256, 1250 }  /* ~10 seconds */
#define IWDG_TIMEOUT_30S        { IWDG_PRESCALER_256, 3750 }  /* ~30 seconds */

/****************************************************************************/
/* Function Prototypes                                                      */
/****************************************************************************/

/**
 * @brief Initialize and start the IWDG with specified configuration
 *
 * @param config Pointer to IWDG configuration structure
 * @return IWDG_Status_t Status of initialization
 *
 * @note Once started, IWDG cannot be stopped except by system reset
 * @note LSI clock (typically 32kHz) is automatically enabled
 */
IWDG_Status_t IWDG_Initialize(const IWDG_Config_t *config);

/**
 * @brief Refresh (kick) the watchdog timer
 *
 * @return IWDG_Status_t Status of refresh operation
 *
 * @note This must be called periodically before timeout expires
 * @note Calling this resets the watchdog counter to reload value
 */
IWDG_Status_t IWDG_Refresh(void);

/****************************************************************************/
/* Inline Helper Functions                                                  */
/****************************************************************************/

/**
 * @brief Quick watchdog refresh (inline for minimal overhead)
 */
static inline void IWDG_Kick(void) {
    IWDG_Refresh();
}

#endif /* IWDG_H */
