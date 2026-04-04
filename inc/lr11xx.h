/*
 * lr11xx.h
 *
 *  Created on: 07 mar. 2026
 *      Author: Ludo
 */

#ifndef __LR11XX_H__
#define __LR11XX_H__

#ifndef LR11XX_DRIVER_DISABLE_FLAGS_FILE
#include "lr11xx_driver_flags.h"
#endif
#include "error.h"
#include "types.h"

/*** LR11XX macros ***/

#define LR11XX_RF_FREQUENCY_HZ_MIN              150000000
#define LR11XX_RF_FREQUENCY_HZ_MAX              960000000

#define LR11XX_RF_OUTPUT_POWER_LP_PA_DBM_MIN    (-17)
#define LR11XX_RF_OUTPUT_POWER_LP_PA_DBM_MAX    15

#define LR11XX_RF_OUTPUT_POWER_HP_PA_DBM_MIN    (-9)
#define LR11XX_RF_OUTPUT_POWER_HP_PA_DBM_MAX    22

#define LR11XX_EXIT_RESET_DELAY_MS              50
#define LR11XX_CALIBRATION_DELAY_MS             4

#define LR11XX_SYNC_WORD_SIZE_BYTES_MAX         8

#define LR11XX_RAMP_UP_DELAY_DBPSK_100BPS       0x370F
#define LR11XX_RAMP_UP_DELAY_DBPSK_600BPS       0x092F
#define LR11XX_RAMP_DOWN_DELAY_DBPSK_100BPS     0x1D70
#define LR11XX_RAMP_DOWN_DELAY_DBPSK_600BPS     0x04E1

#define LR11XX_WIFI_MAC_ADDRESS_SIZE_BYTES      6
#define LR11XX_WIFI_MAC_ADDRESS_LIST_SIZE_MAX   25

/*** LR11XX structures ***/

/*!******************************************************************
 * \enum LR11XX_status_t
 * \brief LR11XX driver error codes.
 *******************************************************************/
typedef enum {
    // Driver errors.
    LR11XX_SUCCESS = 0,
    LR11XX_ERROR_NULL_PARAMETER,
    LR11XX_ERROR_BUSY_TIMEOUT,
    LR11XX_ERROR_COMMAND_EXECUTION,
    LR11XX_ERROR_REGULATION_MODE,
    LR11XX_ERROR_OSCILLATOR,
    LR11XX_ERROR_TCXO_VOLTAGE,
    LR11XX_ERROR_TCXO_TIMEOUT,
    LR11XX_ERROR_LF_CLOCK,
    LR11XX_ERROR_CALIBRATION_TIMEOUT,
    LR11XX_ERROR_MODE,
    LR11XX_ERROR_RF_FREQUENCY_OVERFLOW,
    LR11XX_ERROR_RF_FREQUENCY_UNDERFLOW,
    LR11XX_ERROR_MODULATION,
    LR11XX_ERROR_MODULATION_SHAPING,
    LR11XX_ERROR_MODULATION_NOT_SELECTED,
    LR11XX_ERROR_BIT_RATE_OVERFLOW,
    LR11XX_ERROR_BIT_RATE_UNDERFLOW,
    LR11XX_ERROR_RX_BANDWIDTH,
    LR11XX_ERROR_PREAMBLE_DETECTOR_LENGTH,
    LR11XX_ERROR_SYNC_WORD_LENGTH,
    LR11XX_ERROR_IRQ_INDEX,
    LR11XX_ERROR_PA,
    LR11XX_ERROR_RF_OUTPUT_POWER_OVERFLOW,
    LR11XX_ERROR_RF_OUTPUT_POWER_UNDERFLOW,
    LR11XX_ERROR_PA_RAMP_TIME,
    LR11XX_ERROR_LNA_MODE,
    LR11XX_ERROR_RSSI_TYPE,
    LR11XX_ERROR_RX_PAYLOAD_SIZE,
    LR11XX_ERROR_FIFO_TX_DATA_SIZE,
    LR11XX_ERROR_FIFO_RX_DATA_SIZE,
    LR11XX_ERROR_WIFI_SIGNAL_TYPE,
    LR11XX_ERROR_WIFI_ACQUISITION_MODE,
    LR11XX_ERROR_WIFI_MAC_ADDRESS_LIST_SIZE,
    LR11XX_ERROR_WIFI_NUMBER_OF_SCANS_PER_CHANNEL,
    LR11XX_ERROR_WIFI_SINGLE_SCAN_TIMEOUT,
    LR11XX_ERROR_WIFI_ACCESS_POINT_LIST_SIZE,
    // Low level drivers errors.
    LR11XX_ERROR_HW_FUNCTION_NOT_IMPLEMENTED,
    LR11XX_ERROR_BASE_GPIO = ERROR_BASE_STEP,
    LR11XX_ERROR_BASE_SPI = (LR11XX_ERROR_BASE_GPIO + LR11XX_DRIVER_GPIO_ERROR_BASE_LAST),
    LR11XX_ERROR_BASE_DELAY = (LR11XX_ERROR_BASE_SPI + LR11XX_DRIVER_SPI_ERROR_BASE_LAST),
    // Last base value.
    LR11XX_ERROR_BASE_LAST = (LR11XX_ERROR_BASE_DELAY + LR11XX_DRIVER_DELAY_ERROR_BASE_LAST)
} LR11XX_status_t;

#ifndef LR11XX_DRIVER_DISABLE

/*!******************************************************************
 * \enum LR11XX_regulation_mode_t
 * \brief LR11XX power regulation modes.
 *******************************************************************/
typedef enum {
    LR11XX_REGULATION_MODE_LDO = 0,
    LR11XX_REGULATION_MODE_DCDC,
    LR11XX_REGULATION_MODE_LAST
} LR11XX_regulation_mode_t;

/*!******************************************************************
 * \enum LR11XX_oscillator_t
 * \brief LR11XX external oscillator type.
 *******************************************************************/
typedef enum {
    LR11XX_OSCILLATOR_QUARTZ = 0,
    LR11XX_OSCILLATOR_TCXO,
    LR11XX_OSCILLATOR_LAST
} LR11XX_oscillator_t;

/*!******************************************************************
 * \enum LR11XX_tcxo_voltage_t
 * \brief LR11XX TCXO control voltages list.
 *******************************************************************/
typedef enum {
    LR11XX_TCXO_VOLTAGE_1V6 = 0,
    LR11XX_TCXO_VOLTAGE_1V7,
    LR11XX_TCXO_VOLTAGE_1V8,
    LR11XX_TCXO_VOLTAGE_2V2,
    LR11XX_TCXO_VOLTAGE_2V4,
    LR11XX_TCXO_VOLTAGE_2V7,
    LR11XX_TCXO_VOLTAGE_3V0,
    LR11XX_TCXO_VOLTAGE_3V3,
    LR11XX_TCXO_VOLTAGE_LAST
} LR11XX_tcxo_voltage_t;

/*!******************************************************************
 * \enum LR11XX_lf_clock_t
 * \brief LR11XX low frequency clocks list.
 *******************************************************************/
typedef enum {
    LR11XX_LF_CLOCK_INTERNAL_RC = 0,
    LR11XX_LF_CLOCK_EXTERNAL_CRYSTAL,
    LR11XX_LF_CLOCK_EXTERNAL_DIO11,
    LR11XX_LF_CLOCK_LAST
} LR11XX_lf_clock_t;

/*!******************************************************************
 * \enum LR11XX_mode_t
 * \brief LR11XX transceiver modes.
 *******************************************************************/
typedef enum {
    LR11XX_MODE_SLEEP = 0,
    LR11XX_MODE_STANDBY_RC,
    LR11XX_MODE_STANDBY_XOSC,
    LR11XX_MODE_FS,
    LR11XX_MODE_TX,
    LR11XX_MODE_TX_CW,
    LR11XX_MODE_RX,
    LR11XX_MODE_LAST
} LR11XX_mode_t;

/*!******************************************************************
 * \enum LR11XX_modulation_t
 * \brief LR11XX modulations list.
 *******************************************************************/
typedef enum {
    LR11XX_MODULATION_GFSK = 0,
    LR11XX_MODULATION_BPSK,
    LR11XX_MODULATION_LAST
} LR11XX_modulation_t;

/*!******************************************************************
 * \enum LR11XX_modulation_shaping_t
 * \brief LR11XX modulations shaping list.
 *******************************************************************/
typedef enum {
    LR11XX_MODULATION_SHAPING_NONE = 0,
    LR11XX_MODULATION_SHAPING_GAUSSIAN_BT_03,
    LR11XX_MODULATION_SHAPING_GAUSSIAN_BT_05,
    LR11XX_MODULATION_SHAPING_GAUSSIAN_BT_07,
    LR11XX_MODULATION_SHAPING_GAUSSIAN_BT_1,
    LR11XX_MODULATION_SHAPING_DBPSK,
    LR11XX_MODULATION_SHAPING_LAST
} LR11XX_modulation_shaping_t;

/*!******************************************************************
 * \enum LR11XX_rxbw_mantissa_t
 * \brief LR11XX RX bandwidth values.
 *******************************************************************/
typedef enum {
    LR11XX_RXBW_4800HZ = 0,
    LR11XX_RXBW_5800HZ,
    LR11XX_RXBW_7300HZ,
    LR11XX_RXBW_9700HZ,
    LR11XX_RXBW_11700HZ,
    LR11XX_RXBW_14600HZ,
    LR11XX_RXBW_19500HZ,
    LR11XX_RXBW_23400HZ,
    LR11XX_RXBW_29300HZ,
    LR11XX_RXBW_39000HZ,
    LR11XX_RXBW_46900HZ,
    LR11XX_RXBW_58600HZ,
    LR11XX_RXBW_78200HZ,
    LR11XX_RXBW_93800HZ,
    LR11XX_RXBW_117300HZ,
    LR11XX_RXBW_156200HZ,
    LR11XX_RXBW_187200HZ,
    LR11XX_RXBW_234300HZ,
    LR11XX_RXBW_312000HZ,
    LR11XX_RXBW_373600HZ,
    LR11XX_RXBW_467000HZ,
    LR11XX_RXBW_LAST
} LR11XX_rxbw_t;

/*!******************************************************************
 * \enum LR11XX_irq_index_t
 * \brief LR11XX internal interrupts list.
 *******************************************************************/
typedef enum {
    LR11XX_IRQ_INDEX_TX_DONE = 2,
    LR11XX_IRQ_INDEX_RX_DONE = 3,
    LR11XX_IRQ_INDEX_PREAMBLE_DETECTED = 4,
    LR11XX_IRQ_INDEX_SYNC_WORD_HEADER_VALID = 5,
    LR11XX_IRQ_INDEX_HEADER_ERROR = 6,
    LR11XX_IRQ_INDEX_ERROR = 7,
    LR11XX_IRQ_INDEX_CAD_DONE = 8,
    LR11XX_IRQ_INDEX_CAD_DETECTED = 9,
    LR11XX_IRQ_INDEX_TIMEOUT = 10,
    LR11XX_IRQ_INDEX_LR_FHSS_HOP = 11,
    LR11XX_IRQ_INDEX_GNSS_DONE = 19,
    LR11XX_IRQ_INDEX_WIFI_DONE = 20,
    LR11XX_IRQ_INDEX_LDB = 21,
    LR11XX_IRQ_INDEX_CMD_ERROR = 22,
    LR11XX_IRQ_INDEX_OTHER_ERROR = 23,
    LR11XX_IRQ_INDEX_FSK_LEN_ERROR = 24,
    LR11XX_IRQ_INDEX_FSK_ADDR_ERROR = 25,
    LR11XX_IRQ_INDEX_LORA_RX_TIMESTAMP = 27,
    LR11XX_IRQ_INDEX_GNSS_ABORT = 28,
    LR11XX_IRQ_INDEX_LAST
} LR11XX_irq_index_t;

/*!******************************************************************
 * \enum LR11XX_preamble_detector_length_t
 * \brief LR11XX preamble detector length list.
 *******************************************************************/
typedef enum {
    LR11XX_PREAMBLE_DETECTOR_LENGTH_OFF = 0,
    LR11XX_PREAMBLE_DETECTOR_LENGTH_8BITS,
    LR11XX_PREAMBLE_DETECTOR_LENGTH_16BITS,
    LR11XX_PREAMBLE_DETECTOR_LENGTH_24BITS,
    LR11XX_PREAMBLE_DETECTOR_LENGTH_32BITS,
    LR11XX_PREAMBLE_DETECTOR_LENGTH_LAST
} LR11XX_preamble_detector_length_t;

/*!******************************************************************
 * \enum LR11XX_pa_t
 * \brief LR11XX power amplifiers list.
 *******************************************************************/
typedef enum {
    LR11XX_PA_LOW_POWER = 0,
    LR11XX_PA_HIGH_POWER,
    LR11XX_PA_LAST
} LR11XX_pa_t;

/*!******************************************************************
 * \enum LR11XX_pa_ramp_time_t
 * \brief LR11XX power amplifier ramp time list.
 *******************************************************************/
typedef enum {
    LR11XX_PA_RAMP_TIME_16U = 0,
    LR11XX_PA_RAMP_TIME_32U,
    LR11XX_PA_RAMP_TIME_48U,
    LR11XX_PA_RAMP_TIME_64U,
    LR11XX_PA_RAMP_TIME_80U,
    LR11XX_PA_RAMP_TIME_96U,
    LR11XX_PA_RAMP_TIME_112U,
    LR11XX_PA_RAMP_TIME_128U,
    LR11XX_PA_RAMP_TIME_144U,
    LR11XX_PA_RAMP_TIME_160U,
    LR11XX_PA_RAMP_TIME_176U,
    LR11XX_PA_RAMP_TIME_192U,
    LR11XX_PA_RAMP_TIME_208U,
    LR11XX_PA_RAMP_TIME_240U,
    LR11XX_PA_RAMP_TIME_272U,
    LR11XX_PA_RAMP_TIME_304U,
    LR11XX_PA_RAMP_TIME_LAST
} LR11XX_pa_ramp_time_t;

/*!******************************************************************
 * \enum LR11XX_lna_mode_t
 * \brief LR11XX internal LNA modes list.
 *******************************************************************/
typedef enum {
    LR11XX_LNA_MODE_NORMAL = 0,
    LR11XX_LNA_MODE_BOOST,
    LR11XX_LNA_MODE_LAST
} LR11XX_lna_mode_t;

/*!******************************************************************
 * \enum LR11XX_rssi_t
 * \brief LR11XX RSSI measurement methods.
 *******************************************************************/
typedef enum {
    LR11XX_RSSI_TYPE_INSTANTANEOUS = 0,
    LR11XX_RSSI_TYPE_AVERAGED,
    LR11XX_RSSI_TYPE_LAST
} LR11XX_rssi_t;

/*!******************************************************************
 * \enum LR11XX_wifi_signal_type_t
 * \brief LR11XX WiFi signal types list.
 *******************************************************************/
typedef enum {
    LR11XX_WIFI_SIGNAL_TYPE_B = 0x01,
    LR11XX_WIFI_SIGNAL_TYPE_G = 0x02,
    LR11XX_WIFI_SIGNAL_TYPE_N = 0x03,
    LR11XX_WIFI_SIGNAL_TYPE_ALL = 0x04,
    LR11XX_WIFI_SIGNAL_TYPE_LAST
} LR11XX_wifi_signal_type_t;

/*!******************************************************************
 * \struct LR11XX_wifi_type_t
 * \brief LR11XX WiFi types structure.
 *******************************************************************/
typedef union {
    uint8_t all;
    struct {
        unsigned signal_type :2;
        unsigned datarate_id :6;
    } __attribute__((packed));
} LR11XX_wifi_type_t;

/*!******************************************************************
 * \enum LR11XX_wifi_signal_type_t
 * \brief LR11XX WiFi signal types list.
 *******************************************************************/
typedef enum {
    LR11XX_WIFI_ACQUISITION_MODE_BEACON = 0x01,
    LR11XX_WIFI_ACQUISITION_MODE_BEACON_PACKET = 0x02,
    LR11XX_WIFI_ACQUISITION_MODE_FULL_TRAFFIC = 0x03,
    LR11XX_WIFI_ACQUISITION_MODE_FULL_BEACON = 0x04,
    LR11XX_WIFI_ACQUISITION_MODE_SSID_BEACON = 0x05,
    LR11XX_WIFI_ACQUISITION_MODE_LAST
} LR11XX_wifi_acquisition_mode_t;

/*!******************************************************************
 * \enum LR11XX_wifi_mac_address_origin_t
 * \brief LR11XX WiFi MAC address origins list.
 *******************************************************************/
typedef enum {
    LR11XX_WIFI_MAC_ADDRESS_ORIGIN_NONE = 0,
    LR11XX_WIFI_MAC_ADDRESS_ORIGIN_GATEWAY,
    LR11XX_WIFI_MAC_ADDRESS_ORIGIN_PHONE,
    LR11XX_WIFI_MAC_ADDRESS_ORIGIN_UNDETERMINED,
    LR11XX_WIFI_MAC_ADDRESS_ORIGIN_LAST
} LR11XX_wifi_mac_address_origin_t;

/*!******************************************************************
 * \struct LR11XX_modulation_parameters_t
 * \brief LR11XX modulation parameters structure.
 *******************************************************************/
typedef struct {
    LR11XX_modulation_t modulation;
    LR11XX_modulation_shaping_t modulation_shaping;
    uint32_t fsk_deviation_hz;
    uint32_t bit_rate_bps;
    LR11XX_rxbw_t rx_bandwidth;
} LR11XX_modulation_parameters_t;

/*!******************************************************************
 * \struct LR11XX_gfsk_packet_parameters_t
 * \brief LR11XX GFSK packet parameters structure.
 *******************************************************************/
typedef struct {
    uint16_t preamble_length_bits;
    LR11XX_preamble_detector_length_t preamble_detector_length;
    uint8_t sync_word[LR11XX_SYNC_WORD_SIZE_BYTES_MAX];
    uint8_t sync_word_length_bits;
    uint8_t payload_length_bytes;
} LR11XX_gfsk_packet_parameters_t;

/*!******************************************************************
 * \struct LR11XX_bpsk_packet_parameters_t
 * \brief LR11XX BPSK packet parameters structure.
 *******************************************************************/
typedef struct {
    uint8_t payload_length_bytes;
    uint16_t payload_length_bits;
    uint16_t ramp_up_delay;
    uint16_t ramp_down_delay;
} LR11XX_bpsk_packet_parameters_t;

/*!******************************************************************
 * \struct LR11XX_wifi_channel_info_t
 * \brief LR11XX WiFi channel informations structure.
 *******************************************************************/
typedef union {
    uint8_t all;
    struct {
        unsigned channel_id :4;
        unsigned mac_origin :4;
    } __attribute__((packed));
} LR11XX_wifi_channel_info_t;

/*!******************************************************************
 * \struct LR11XX_wifi_scan_parameters_t
 * \brief LR11XX passive WiFi scan parameters structure.
 *******************************************************************/
typedef struct {
    LR11XX_wifi_signal_type_t signal_type;
    uint16_t channel_mask;
    LR11XX_wifi_acquisition_mode_t acquisition_mode;
    uint8_t mac_address_list_size;
    uint8_t number_of_scans_per_channel;
    uint16_t single_scan_timeout_ms;
} LR11XX_wifi_scan_parameters_t;

/*!******************************************************************
 * \struct LR11XX_wifi_scan_parameters_t
 * \brief LR11XX passive WiFi scan parameters structure.
 *******************************************************************/
typedef struct {
    LR11XX_wifi_type_t wifi_type;
    LR11XX_wifi_channel_info_t channel_info;
    int8_t rssi_dbm;
    uint8_t mac_address[LR11XX_WIFI_MAC_ADDRESS_SIZE_BYTES];
} LR11XX_wifi_access_point_t;

/*!******************************************************************
 * \struct LR11XX_wifi_scan_result_t
 * \brief LR11XX passive WiFi scan results structure.
 *******************************************************************/
typedef struct {
    LR11XX_wifi_access_point_t* access_point_list;
    uint8_t access_point_list_size;
    uint8_t number_of_access_points_written;
    uint8_t number_of_access_points_detected;
} LR11XX_wifi_scan_results_t;

/*** LR11XX functions ***/

/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_init(void)
 * \brief Init LR11XX driver.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_init(void);

/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_de_init(void)
 * \brief Release LR11XX driver.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_de_init(void);

/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_reset(uint8_t reset_enable)
 * \brief Control LR11XX NRESET pin.
 * \param[in]   reset_enable: 0 to release chip, any other value to reset.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_reset(uint8_t reset_enable);

/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_get_errors(uint16_t* error_stat)
 * \brief Read LR11XX internal errors.
 * \param[in]   none
 * \param[out]  error_stat: Pointer to the errors bitfield.
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_get_errors(uint16_t* error_stat);

/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_clear_errors(void)
 * \brief Clear LR11XX internal errors.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_clear_errors(void);

/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_set_regulation_mode(LR11XX_regulation_mode_t regulation_mode)
 * \brief Set LR11XX regulation mode.
 * \param[in]   regulation_mode: Power regulation mode to select.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_set_regulation_mode(LR11XX_regulation_mode_t regulation_mode);

/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_set_oscillator(LR11XX_oscillator_t oscillator, LR11XX_tcxo_voltage_t tcxo_voltage, uint32_t tcxo_timeout_ms)
 * \brief Set LR11XX oscillator type.
 * \param[in]   oscillator: Oscillator type.
 * \param[in]   tcxo_voltage: TCXO control voltage on DIO3.
 * \param[in]   tcxo_timeout_ms: TCXO startup timeout in ms.
 * \param[in]   lf_clock: Low frequency clock selection.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_set_oscillator(LR11XX_oscillator_t oscillator, LR11XX_tcxo_voltage_t tcxo_voltage, uint32_t tcxo_timeout_ms, LR11XX_lf_clock_t lf_clock);

/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_calibrate(uint16_t frequency_range_low_mhz, uint16_t frequency_range_high_mhz)
 * \brief Calibrate all LR11XX internal blocks.
 * \param[in]   frequency_range_low_mhz: Minimum frequency of the operating band in MHz.
 * \param[in]   frequency_range_high_mhz: Maximum frequency of the operating band in MHz
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_calibrate(uint16_t frequency_range_low_mhz, uint16_t frequency_range_high_mhz);

/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_set_mode(LR11XX_mode_t mode)
 * \brief Set LR11XX state.
 * \param[in]   mode: New mode to switch to.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_set_mode(LR11XX_mode_t mode);

/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_set_rf_frequency(uint32_t rf_frequency_hz)
 * \brief Set LR11XX RF center frequency.
 * \param[in]   rf_frequency_hz: Center frequency to set in Hz.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_set_rf_frequency(uint32_t rf_frequency_hz);

/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_set_modulation(LR11XX_modulation_parameters_t* modulation_parameters)
 * \brief Configure LR11XX modulation scheme.
 * \param[in]   modulation_parameters: Pointer to the modulation parameters to set.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_set_modulation(LR11XX_modulation_parameters_t* modulation_parameters);

/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_set_gfsk_packet(LR11XX_gfsk_packet_parameters_t* packet_parameters)
 * \brief Set LR11XX GFSK packet parameters.
 * \param[in]   packet_parameters: Pointer to the packet parameters structure.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_set_gfsk_packet(LR11XX_gfsk_packet_parameters_t* packet_parameters);

/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_set_bpsk_packet(LR11XX_bpsk_packet_parameters_t* packet_parameters)
 * \brief Set LR11XX BPSK packet parameters.
 * \param[in]   packet_parameters: Pointer to the packet parameters structure.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_set_bpsk_packet(LR11XX_bpsk_packet_parameters_t* packet_parameters);

/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_set_dio_irq_mask(uint32_t irq_mask_dio9, uint32_t irq_mask_dio11)
 * \brief Configure LR11XX interrupts and GPIO.
 * \param[in]   irq_mask_dio9: Interrupts mask attached to DIO9.
 * \param[in]   irq_mask_dio11: Interrupts mask attached to DIO11.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_set_dio_irq_mask(uint32_t irq_mask_dio9, uint32_t irq_mask_dio11);

/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_get_irq_flag(LR11XX_irq_index_t irq_index, uint8_t* irq_flag)
 * \brief Read LR11XX internal interrupt status.
 * \param[in]   irq_index: Interrupt index.
 * \param[out]  irq_flag: Pointer to bit that will contain interrupt status.
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_get_irq_flag(LR11XX_irq_index_t irq_index, uint8_t* irq_flag);

#ifdef LR11XX_DRIVER_TX_ENABLE
/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_set_rf_output_power(LR11XX_pa_t pa, int8_t rf_output_power_dbm, LR11XX_pa_ramp_time_t pa_ramp_time)
 * \brief Set LR11XX RF output power.
 * \param[in]   pa: Power amplifier to select.
 * \param[in]   rf_output_power_dbm: RF output power in dBm.
 * \param[in]   pa_ramp_time: PA ramp time.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_set_rf_output_power(LR11XX_pa_t pa, int8_t rf_output_power_dbm, LR11XX_pa_ramp_time_t pa_ramp_time);
#endif

#ifdef LR11XX_DRIVER_TX_ENABLE
/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_differential_encoding(uint8_t* data_in, uint8_t data_in_size_bytes, uint8_t* data_out, uint8_t* data_out_size_bytes, uint16_t* data_out_size_bits)
 * \brief Convert a payload buffer using differential encoding.
 * \param[in]   data_in: Input byte array.
 * \param[in]   data_in_size_bytes: Input data size in bytes.
 * \param[out]  data_out: Pointer to the output data.
 * \param[out]  data_out_size_bytes: Pointer to the output data size in bytes.
 * \param[out]  data_out_size_bits: Pointer to the output data size in bits.
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_differential_encoding(uint8_t* data_in, uint8_t data_in_size_bytes, uint8_t* data_out, uint8_t* data_out_size_bytes, uint16_t* data_out_size_bits);
#endif

#ifdef LR11XX_DRIVER_TX_ENABLE
/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_write_fifo(uint8_t* tx_data, uint8_t tx_data_size)
 * \brief Write LR11XX TX buffer.
 * \param[in]   tx_data: Byte array to write in FIFO.
 * \param[in]   tx_data_size: Number of bytes to write.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_write_fifo(uint8_t* tx_data, uint8_t tx_data_size);
#endif

#ifdef LR11XX_DRIVER_RX_ENABLE
/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_set_lna_mode(LR11XX_lna_mode_t lna_mode)
 * \brief Configure LR11XX internal LNA.
 * \param[in]   lna_mode: Internal LNA mode.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_set_lna_mode(LR11XX_lna_mode_t lna_mode);
#endif

#ifdef LR11XX_DRIVER_RX_ENABLE
/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_get_rssi(LR11XX_rssi_t rssi_type, int16_t* rssi_dbm)
 * \brief Get LR11XX RX RSSI.
 * \param[in]   rssi_type: RSSI type to read.
 * \param[out]  rssi_dbm: Pointer to signed 16-bits value that will contain the RSSI in dBm.
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_get_rssi(LR11XX_rssi_t rssi_type, int16_t* rssi_dbm);
#endif

#ifdef LR11XX_DRIVER_RX_ENABLE
/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_clear_fifo(void)
 * \brief Clear LR11XX FIFO.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_clear_fifo(void);
#endif

#ifdef LR11XX_DRIVER_RX_ENABLE
/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_read_fifo(uint8_t* rx_data, uint8_t rx_data_size)
 * \brief Read LR11XX FIFO.
 * \param[in]   rx_data_size: Number of bytes to read.
 * \param[out]  rx_data: Byte array that will contain the RX FIFO bytes.
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_read_fifo(uint8_t* rx_data, uint8_t rx_data_size);
#endif

#ifdef LR11XX_DRIVER_WIFI_ENABLE
/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_wifi_scan(LR11XX_wifi_scan_parameters_t* wifi_scan_parameters)
 * \brief Perform a passive WiFi scan.
 * \param[in]   wifi_scan_parameters: Pointer to the scan parameters.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_wifi_scan(LR11XX_wifi_scan_parameters_t* wifi_scan_parameters);
#endif

#ifdef LR11XX_DRIVER_WIFI_ENABLE
/*!******************************************************************
 * \fn LR11XX_status_t LR11XX_wifi_read(LR11XX_wifi_scan_results_t* wifi_scan_results)
 * \brief Perform a passive WiFi scan.
 * \param[in]   none
 * \param[out]  wifi_scan_results: Pointer to the scan results.
 * \retval      Function execution status.
 *******************************************************************/
LR11XX_status_t LR11XX_wifi_read(LR11XX_wifi_scan_results_t* wifi_scan_results);
#endif

/*******************************************************************/
#define LR11XX_exit_error(base) { ERROR_check_exit(lr11xx_status, LR11XX_SUCCESS, base) }

/*******************************************************************/
#define LR11XX_stack_error(base) { ERROR_check_stack(lr11xx_status, LR11XX_SUCCESS, base) }

/*******************************************************************/
#define LR11XX_stack_exit_error(base, code) { ERROR_check_stack_exit(lr11xx_status, LR11XX_SUCCESS, base, code) }

#endif /* LR11XX_DRIVER_DISABLE */

#endif /* __LR11XX_H__ */
