/*
 * lr11xx_hw.c
 *
 *  Created on: 07 mar. 2026
 *      Author: Ludo
 */

#include "lr11xx_hw.h"

#ifndef LR11XX_DRIVER_DISABLE_FLAGS_FILE
#include "lr11xx_driver_flags.h"
#endif
#include "lr11xx.h"
#include "types.h"

#ifndef LR11XX_DRIVER_DISABLE

/*** LR11XX HW functions ***/

/*******************************************************************/
LR11XX_status_t __attribute__((weak)) LR11XX_HW_init(void) {
    // Local variables.
    LR11XX_status_t status = LR11XX_ERROR_HW_FUNCTION_NOT_IMPLEMENTED;
    /* To be implemented */
    return status;
}

/*******************************************************************/
LR11XX_status_t __attribute__((weak)) LR11XX_HW_de_init(void) {
    // Local variables.
    LR11XX_status_t status = LR11XX_ERROR_HW_FUNCTION_NOT_IMPLEMENTED;
    /* To be implemented */
    return status;
}

/*******************************************************************/
LR11XX_status_t __attribute__((weak)) LR11XX_HW_set_nreset_gpio(uint8_t state) {
    // Local variables.
    LR11XX_status_t status = LR11XX_ERROR_HW_FUNCTION_NOT_IMPLEMENTED;
    /* To be implemented */
    UNUSED(state);
    return status;
}

/*******************************************************************/
LR11XX_status_t __attribute__((weak)) LR11XX_HW_wait_busy_low(void) {
    // Local variables.
    LR11XX_status_t status = LR11XX_ERROR_HW_FUNCTION_NOT_IMPLEMENTED;
    /* To be implemented */
    return status;
}

/*******************************************************************/
LR11XX_status_t __attribute__((weak)) LR11XX_HW_spi_write_read_8(uint8_t* tx_data, uint8_t* rx_data, uint8_t transfer_size) {
    // Local variables.
    LR11XX_status_t status = LR11XX_ERROR_HW_FUNCTION_NOT_IMPLEMENTED;
    /* To be implemented */
    UNUSED(tx_data);
    UNUSED(rx_data);
    UNUSED(transfer_size);
    return status;
}

/*******************************************************************/
LR11XX_status_t __attribute__((weak)) LR11XX_HW_delay_milliseconds(uint32_t delay_ms) {
    // Local variables.
    LR11XX_status_t status = LR11XX_ERROR_HW_FUNCTION_NOT_IMPLEMENTED;
    /* To be implemented */
    UNUSED(delay_ms);
    return status;
}

#endif /* LR11XX_DRIVER_DISABLE */
