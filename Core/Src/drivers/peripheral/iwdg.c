/****************************************************************************/
/* Author            : Ameer Alwadiya                                       */
/* Version           : V1.0.0                                               */
/* Date              : 09 Nov 2025                                          */
/* Description       : IWDG (Independent Watchdog) Implementation           */
/****************************************************************************/

/****************************************************************************/
/* Include Files                                                            */
/****************************************************************************/
#include "iwdg.h"
#include "rcc.h"
#include <stddef.h>

/****************************************************************************/
/* Private Functions                                                        */
/****************************************************************************/

/**
 * @brief Wait for IWDG status register to be ready
 * @return IWDG_Status_t Status of wait operation
 */
static IWDG_Status_t IWDG_WaitForUpdate(void) {
    uint32_t timeout = IWDG_TIMEOUT_US;

    while ((IWDG->SR != 0) && (timeout > 0)) {
        timeout--;
    }

    return (timeout > 0) ? IWDG_STATUS_OK : IWDG_STATUS_TIMEOUT;
}

/****************************************************************************/
/* Public Functions Implementation                                          */
/****************************************************************************/

IWDG_Status_t IWDG_Initialize(const IWDG_Config_t *config) {
    IWDG_Status_t status;

    /* Validate parameters */
    if (config == NULL || config->prescaler > IWDG_PRESCALER_256 || config->reload_value > IWDG_MAX_RELOAD) {
        return IWDG_STATUS_INVALID_PARAM;
    }

    /* Start IWDG (this cannot be undone!) */
    IWDG->KR = IWDG_KEY_START;

    /* Enable write access to IWDG registers */
    IWDG->KR = IWDG_KEY_ACCESS;

    /* Configure prescaler */
    IWDG->PR = (uint32_t)config->prescaler;

    /* Wait for prescaler update */
    status = IWDG_WaitForUpdate();
    if (status != IWDG_STATUS_OK) {
        return status;
    }

    /* Enable write access again (required after waiting) */
    IWDG->KR = IWDG_KEY_ACCESS;

    /* Configure reload value */
    IWDG->RLR = (uint32_t)config->reload_value;

    /* Wait for reload update */
    status = IWDG_WaitForUpdate();
    if (status != IWDG_STATUS_OK) {
        return status;
    }

    /* Reload counter to start with configured value */
    IWDG->KR = IWDG_KEY_REFRESH;

    return IWDG_STATUS_OK;
}

IWDG_Status_t IWDG_Refresh(void) {
    /* Reload watchdog counter */
    IWDG->KR = IWDG_KEY_REFRESH;

    return IWDG_STATUS_OK;
}
