/*
 * lr11xx_hw.h
 *
 *  Created on: 07 mar. 2026
 *      Author: Ludo
 */

#ifndef __LR11XX_HW_H__
#define __LR11XX_HW_H__

#ifndef LR11XX_DRIVER_DISABLE_FLAGS_FILE
#include "lr11xx_driver_flags.h"
#endif
#include "lr11xx.h"
#include "types.h"

#ifndef LR11XX_DRIVER_DISABLE

/*** LR11XX HW functions ***/

/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_HW_init(void)
 * \brief Init LR11XX hardware interface.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_HW_init(void);

/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_HW_de_init(void)
 * \brief Release LR11XX hardware interface.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_HW_de_init(void);

/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_HW_set_nreset_gpio(uint8_t state)
 * \brief Set LR11XX NRESET pin state.
 * \param[in]   state: NRESET pin state to apply.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_HW_set_nreset_gpio(uint8_t state);

/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_HW_wait_busy_low(void)
 * \brief Wait for LR11XX BUSY pin to be low.
 * \param[in]   none.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_HW_wait_busy_low(void);

/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_HW_spi_write_read_8(uint8_t* tx_data, uint8_t* rx_data, uint8_t transfer_size)
 * \brief Transfer data to transceiver over SPI interface.
 * \param[in]   tx_data: Byte array to send.
 * \param[in]   transfer_size: Number of shorts to send and receive.
 * \param[out]  rx_data: Pointer to the received bytes.
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_HW_spi_write_read_8(uint8_t* tx_data, uint8_t* rx_data, uint8_t transfer_size);

/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_HW_delay_milliseconds(uint32_t delay_ms)
 * \brief Delay function.
 * \param[in]   delay_ms: Delay to wait in ms.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_HW_delay_milliseconds(uint32_t delay_ms);

#endif /* LR11XX_DRIVER_DISABLE */

#endif /* __LR11XX_HW_H__ */
