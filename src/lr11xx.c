/*
 * lr11xx.c
 *
 *  Created on: 07 mar. 2026
 *      Author: Ludo
 */

#include "lr11xx.h"

#ifndef LR11XX_DRIVER_DISABLE_FLAGS_FILE
#include "lr11xx_driver_flags.h"
#endif
#include "lr11xx_hw.h"
#include "types.h"

#ifndef LR11XX_DRIVER_DISABLE

/*** LR11XX local macros ***/

#define LR11XX_TCXO_TIMEOUT_DELAY_STEP_NS       30520
#define LR11XX_TCXO_TIMEOUT_MIN_MS              1
#define LR11XX_TCXO_TIMEOUT_MAX_MS              512000

#define LR11XX_BIT_RATE_BPS_MIN                 100
#define LR11XX_BIT_RATE_BPS_MAX                 300000

#define LR11XX_IMAGE_CALIBRATION_STEP_MHZ       4

#define LR11XX_WIFI_BASIC_RESULT_SIZE_BYTES     9
#define LR11XX_WIFI_RESPONSE_SIZE_BYTES(n)      ((uint8_t) (LR11XX_RESPONSE_SIZE_WIFI_READ_RESULTS + (n * LR11XX_WIFI_BASIC_RESULT_SIZE_BYTES)))
#define LR11XX_WIFI_RESPONSE_RESULT_OFFSET      ((uint8_t) (LR11XX_RESPONSE_SIZE_WIFI_READ_RESULTS + (LR11XX_WIFI_BASIC_RESULT_SIZE_BYTES * result_idx)))

#define LR11XX_LP_PA_POWER_TABLE_SIZE           (LR11XX_RF_OUTPUT_POWER_LP_PA_DBM_MAX - LR11XX_RF_OUTPUT_POWER_LP_PA_DBM_MIN + 1)
#define LR11XX_HP_PA_POWER_TABLE_SIZE           (LR11XX_RF_OUTPUT_POWER_HP_PA_DBM_MAX - LR11XX_RF_OUTPUT_POWER_HP_PA_DBM_MIN + 1)

/*** LR11XX local structures ***/

/*******************************************************************/
typedef enum {
    // NOP.
    LR11XX_OP_CODE_NOP = 0x0000,
    // Register and memory access.
    LR11XX_OP_CODE_WRITE_REG_MEM_32 = 0x0105,
    LR11XX_OP_CODE_READ_REG_MEM_32 = 0x0106,
    LR11XX_OP_CODE_WRITE_BUFFER_8 = 0x0109,
    LR11XX_OP_CODE_READ_BUFFER_8 = 0x010A,
    LR11XX_OP_CODE_CLEAR_RX_BUFFER = 0x010B,
    LR11XX_OP_CODE_WRITE_REG_MEM_MASK_32 = 0x010C,
    // System.
    LR11XX_OP_CODE_GET_STATUS = 0x0100,
    LR11XX_OP_CODE_GET_VERSION = 0x0101,
    LR11XX_OP_CODE_GET_ERRORS = 0x010D,
    LR11XX_OP_CODE_CLEAR_ERRORS = 0x010E,
    LR11XX_OP_CODE_CALIBRATE = 0x010F,
    LR11XX_OP_CODE_SET_REG_MODE = 0x0110,
    LR11XX_OP_CODE_CALIB_IMAGE = 0x0111,
    LR11XX_OP_CODE_SET_DIO_AS_RF_SWITCH = 0x0112,
    LR11XX_OP_CODE_SET_DIO_IRQ_PARAMS = 0x0113,
    LR11XX_OP_CODE_CLEAR_IRQ = 0x0114,
    LR11XX_OP_CODE_CONFIG_LF_CLOCK = 0x0116,
    LR11XX_OP_CODE_SET_TCXO_MODE = 0x0117,
    LR11XX_OP_CODE_REBOOT = 0x0118,
    LR11XX_OP_CODE_GET_VBAT = 0x0119,
    LR11XX_OP_CODE_GET_TEMP = 0x011A,
    LR11XX_OP_CODE_SET_SLEEP = 0x011B,
    LR11XX_OP_CODE_SET_STANDBY = 0x011C,
    LR11XX_OP_CODE_SET_FS = 0x011D,
    LR11XX_OP_CODE_GET_RANDOM_NUMBER = 0x0120,
    LR11XX_OP_CODE_ERASE_INFO_PAGE = 0x0121,
    LR11XX_OP_CODE_WRITE_INFO_PAGE = 0x0122,
    LR11XX_OP_CODE_READ_INFO_PAGE = 0x0123,
    LR11XX_OP_CODE_GET_CHIP_EUI = 0x0125,
    LR11XX_OP_CODE_GET_SEMTECH_JOIN_EUI = 0x0126,
    LR11XX_OP_CODE_ENABLE_SPI_CRC = 0x0128,
    LR11XX_OP_CODE_DRIVE_DIOS_IN_SLEEP_MODE = 0x12A,
    // Sub-Ghz radio.
    LR11XX_OP_CODE_RESET_STATS = 0x0200,
    LR11XX_OP_CODE_GET_STATS = 0x0201,
    LR11XX_OP_CODE_GET_PACKET_TYPE = 0x0202,
    LR11XX_OP_CODE_GET_RX_BUFFER_STATUS = 0x0203,
    LR11XX_OP_CODE_GET_PACKET_STATUS = 0x0204,
    LR11XX_OP_CODE_GET_RSSI_INST = 0x0205,
    LR11XX_OP_CODE_SET_GFSK_SYNC_WORD = 0x0206,
    LR11XX_OP_CODE_SET_LORA_PUBLIC_NETWORK = 0x0208,
    LR11XX_OP_CODE_SET_RX = 0x0209,
    LR11XX_OP_CODE_SET_TX = 0x020A,
    LR11XX_OP_CODE_SET_RF_FREQUENCY = 0x020B,
    LR11XX_OP_CODE_AUTO_TX_RX = 0x020C,
    LR11XX_OP_CODE_SET_CAD_PARAMETERS = 0x020D,
    LR11XX_OP_CODE_SET_PACKET_TYPE = 0x020E,
    LR11XX_OP_CODE_SET_MODULATION_PARAMS = 0x020F,
    LR11XX_OP_CODE_SET_PACKET_PARAMS = 0x0210,
    LR11XX_OP_CODE_SET_TX_PARAMS = 0x0211,
    LR11XX_OP_CODE_SET_PACKET_ADRS = 0x0212,
    LR11XX_OP_CODE_SET_TX_FALLBACK_MODE = 0x0213,
    LR11XX_OP_CODE_SET_RX_DUTY_CYCLE = 0x0214,
    LR11XX_OP_CODE_SET_PA_CONFIG = 0x0215,
    LR11XX_OP_CODE_STOP_TIMEOUT_ON_PREAMBLE = 0x0217,
    LR11XX_OP_CODE_SET_CAD = 0x0218,
    LR11XX_OP_CODE_SET_TX_CW = 0x0219,
    LR11XX_OP_CODE_SET_TX_INFINITE_PREAMBLE = 0x021A,
    LR11XX_OP_CODE_SET_LORA_SYNC_TIMEOUT = 0x021B,
    LR11XX_OP_CODE_SET_GFSK_CRC_PARAMS = 0x0224,
    LR11XX_OP_CODE_SET_GFSK_WHIT_PARAMS = 0x0225,
    LR11XX_OP_CODE_SET_RX_BOOSTED = 0x0227,
    LR11XX_OP_CODE_SET_RSSI_CALIBRATION = 0x0229,
    LR11XX_OP_CODE_SET_LORA_SYNC_WORD = 0x022B,
    LR11XX_OP_CODE_LR_FHSS_BUILD_FRAME = 0x022C,
    LR11XX_OP_CODE_LR_FHSS_SET_SYNC_WORD = 0x022D,
    LR11XX_OP_CODE_CONFIG_BLE_BEACON = 0x022E,
    LR11XX_OP_CODE_GET_LORA_RX_HEADER_INFOS = 0x0230,
    LR11XX_OP_CODE_BLE_BEACON_SEND = 0x0231,
    // WiFi.
    LR11XX_OP_CODE_WIFI_SCAN = 0x0300,
    LR11XX_OP_CODE_WIFI_SCAN_TIME_LIMIT = 0x0301,
    LR11XX_OP_CODE_WIFI_COUNTRY_CODE = 0x0302,
    LR11XX_OP_CODE_WIFI_COUNTRY_CODE_TIME_LIMIT = 0x0303,
    LR11XX_OP_CODE_WIFI_GET_NB_RESULTS = 0x0305,
    LR11XX_OP_CODE_WIFI_READ_RESULTS = 0x0306,
    LR11XX_OP_CODE_WIFI_RESET_CUMUL_TIMINGS = 0x0307,
    LR11XX_OP_CODE_WIFI_READ_CUMUL_TIMINGS = 0x0308,
    LR11XX_OP_CODE_WIFI_GET_NB_COUNTRY_CODE_RESULTS = 0x0309,
    LR11XX_OP_CODE_WIFI_READ_COUNTRY_CODE_RESULTS = 0x030A,
    LR11XX_OP_CODE_WIFI_CFG_TIMESTAMP_AP_PHONE = 0x030B,
    LR11XX_OP_CODE_WIFI_READ_VERSION = 0x0320
} LR11XX_op_code_t;

/*******************************************************************/
typedef enum {
    // Register and memory access.
    LR11XX_COMMAND_SIZE_WRITE_REG_MEM_32 = 10,
    LR11XX_COMMAND_SIZE_READ_REG_MEM_32 = 7,
    LR11XX_COMMAND_SIZE_WRITE_BUFFER_8 = 2,
    LR11XX_COMMAND_SIZE_READ_BUFFER_8 = 4,
    LR11XX_COMMAND_SIZE_CLEAR_RX_BUFFER = 2,
    LR11XX_COMMAND_SIZE_WRITE_REG_MEM_MASK_32 = 14,
    // System.
    LR11XX_COMMAND_SIZE_GET_STATUS = 6,
    LR11XX_COMMAND_SIZE_GET_VERSION = 2,
    LR11XX_COMMAND_SIZE_GET_ERRORS = 2,
    LR11XX_COMMAND_SIZE_CLEAR_ERRORS = 2,
    LR11XX_COMMAND_SIZE_CALIBRATE = 3,
    LR11XX_COMMAND_SIZE_SET_REG_MODE = 3,
    LR11XX_COMMAND_SIZE_CALIB_IMAGE = 4,
    LR11XX_COMMAND_SIZE_SET_DIO_AS_RF_SWITCH = 10,
    LR11XX_COMMAND_SIZE_SET_DIO_IRQ_PARAMS = 10,
    LR11XX_COMMAND_SIZE_CLEAR_IRQ = 6,
    LR11XX_COMMAND_SIZE_CONFIG_LF_CLOCK = 3,
    LR11XX_COMMAND_SIZE_SET_TCXO_MODE = 6,
    LR11XX_COMMAND_SIZE_REBOOT = 3,
    LR11XX_COMMAND_SIZE_GET_VBAT = 2,
    LR11XX_COMMAND_SIZE_GET_TEMP = 2,
    LR11XX_COMMAND_SIZE_SET_SLEEP = 7,
    LR11XX_COMMAND_SIZE_SET_STANDBY = 3,
    LR11XX_COMMAND_SIZE_SET_FS = 2,
    LR11XX_COMMAND_SIZE_GET_RANDOM_NUMBER = 2,
    LR11XX_COMMAND_SIZE_ERASE_INFO_PAGE = 3,
    LR11XX_COMMAND_SIZE_WRITE_INFO_PAGE = 5,
    LR11XX_COMMAND_SIZE_READ_INFO_PAGE = 6,
    LR11XX_COMMAND_SIZE_GET_CHIP_EUI = 2,
    LR11XX_COMMAND_SIZE_GET_SEMTECH_JOIN_EUI = 2,
    LR11XX_COMMAND_SIZE_ENABLE_SPI_CRC = 3,
    LR11XX_COMMAND_SIZE_DRIVE_DIOS_IN_SLEEP_MODE = 3,
    // Sub-Ghz radio.
    LR11XX_COMMAND_SIZE_RESET_STATS = 2,
    LR11XX_COMMAND_SIZE_GET_STATS = 2,
    LR11XX_COMMAND_SIZE_GET_PACKET_TYPE = 2,
    LR11XX_COMMAND_SIZE_GET_RX_BUFFER_STATUS = 2,
    LR11XX_COMMAND_SIZE_GET_PACKET_STATUS = 2,
    LR11XX_COMMAND_SIZE_GET_RSSI_INST = 2,
    LR11XX_COMMAND_SIZE_SET_GFSK_SYNC_WORD = (2 + LR11XX_SYNC_WORD_SIZE_BYTES_MAX),
    LR11XX_COMMAND_SIZE_SET_LORA_PUBLIC_NETWORK = 10,
    LR11XX_COMMAND_SIZE_SET_RX = 5,
    LR11XX_COMMAND_SIZE_SET_TX = 5,
    LR11XX_COMMAND_SIZE_SET_RF_FREQUENCY = 6,
    LR11XX_COMMAND_SIZE_AUTO_TX_RX = 9,
    LR11XX_COMMAND_SIZE_SET_CAD_PARAMETERS = 9,
    LR11XX_COMMAND_SIZE_SET_PACKET_TYPE = 3,
    LR11XX_COMMAND_SIZE_SET_MODULATION_PARAMS_GFSK = 12,
    LR11XX_COMMAND_SIZE_SET_MODULATION_PARAMS_BPSK = 7,
    LR11XX_COMMAND_SIZE_SET_MODULATION_PARAMS_LORA = 6,
    LR11XX_COMMAND_SIZE_SET_MODULATION_PARAMS_LR_FHSS = 7,
    LR11XX_COMMAND_SIZE_SET_PACKET_PARAMS_GFSK = 11,
    LR11XX_COMMAND_SIZE_SET_PACKET_PARAMS_BPSK = 9,
    LR11XX_COMMAND_SIZE_SET_PACKET_PARAMS_LORA = 8,
    LR11XX_COMMAND_SIZE_SET_TX_PARAMS = 4,
    LR11XX_COMMAND_SIZE_SET_PACKET_ADRS = 4,
    LR11XX_COMMAND_SIZE_SET_TX_FALLBACK_MODE = 3,
    LR11XX_COMMAND_SIZE_SET_RX_DUTY_CYCLE = 9,
    LR11XX_COMMAND_SIZE_SET_PA_CONFIG = 6,
    LR11XX_COMMAND_SIZE_STOP_TIMEOUT_ON_PREAMBLE = 3,
    LR11XX_COMMAND_SIZE_SET_CAD = 2,
    LR11XX_COMMAND_SIZE_SET_TX_CW = 2,
    LR11XX_COMMAND_SIZE_SET_TX_INFINITE_PREAMBLE = 2,
    LR11XX_COMMAND_SIZE_SET_LORA_SYNC_TIMEOUT = 4,
    LR11XX_COMMAND_SIZE_SET_GFSK_CRC_PARAMS = 10,
    LR11XX_COMMAND_SIZE_SET_GFSK_WHIT_PARAMS = 4,
    LR11XX_COMMAND_SIZE_SET_RX_BOOSTED = 3,
    LR11XX_COMMAND_SIZE_SET_RSSI_CALIBRATION = 13,
    LR11XX_COMMAND_SIZE_SET_LORA_SYNC_WORD = 3,
    LR11XX_COMMAND_SIZE_LR_FHSS_BUILD_FRAME = 11,
    LR11XX_COMMAND_SIZE_LR_FHSS_SET_SYNC_WORD = 3,
    LR11XX_COMMAND_SIZE_CONFIG_BLE_BEACON = 3,
    LR11XX_COMMAND_SIZE_GET_LORA_RX_HEADER_INFOS = 2,
    LR11XX_COMMAND_SIZE_BLE_BEACON_SEND = 3,
    // WiFi.
    LR11XX_COMMAND_SIZE_WIFI_SCAN = 11,
    LR11XX_COMMAND_SIZE_WIFI_SCAN_TIME_LIMIT = 11,
    LR11XX_COMMAND_SIZE_WIFI_COUNTRY_CODE = 9,
    LR11XX_COMMAND_SIZE_WIFI_COUNTRY_CODE_TIME_LIMIT = 9,
    LR11XX_COMMAND_SIZE_WIFI_GET_NB_RESULTS = 2,
    LR11XX_COMMAND_SIZE_WIFI_READ_RESULTS = 5,
    LR11XX_COMMAND_SIZE_WIFI_RESET_CUMUL_TIMINGS = 2,
    LR11XX_COMMAND_SIZE_WIFI_READ_CUMUL_TIMINGS = 2,
    LR11XX_COMMAND_SIZE_WIFI_GET_NB_COUNTRY_CODE_RESULTS = 2,
    LR11XX_COMMAND_SIZE_WIFI_READ_COUNTRY_CODE_RESULTS = 4,
    LR11XX_COMMAND_SIZE_WIFI_CFG_TIMESTAMP_AP_PHONE = 6,
    LR11XX_COMMAND_SIZE_WIFI_READ_VERSION = 2
} LR11XX_command_size_t;

/*******************************************************************/
typedef enum {
    // Register and memory access.
    LR11XX_RESPONSE_SIZE_READ_REG_MEM_32 = 5,
    LR11XX_RESPONSE_SIZE_READ_BUFFER_8 = 1,
    // System.
    LR11XX_RESPONSE_SIZE_GET_ERRORS = 3,
    // Sub-Ghz radio.
    LR11XX_RESPONSE_SIZE_GET_PACKET_STATUS = 4,
    LR11XX_RESPONSE_SIZE_GET_RSSI = 2,
    LR11XX_RESPONSE_SIZE_GET_RX_BUFFER_STATUS = 3,
    // WiFi.
    LR11XX_RESPONSE_SIZE_WIFI_GET_NB_RESULTS = 2,
    LR11XX_RESPONSE_SIZE_WIFI_READ_RESULTS = 1
} LR11XX_response_size_t;

/*******************************************************************/
typedef struct {
    int8_t power;
    uint8_t pa_duty_cycle;
} LR11XX_lp_pa_power_settings_t;

/*******************************************************************/
typedef struct {
    int8_t power;
    uint8_t pa_duty_cycle;
    uint8_t pa_hp_sel;
} LR11XX_hp_pa_power_settings_t;

/*** LR11XX local global variables ***/

#ifdef LR11XX_DRIVER_TX_ENABLE
static const int8_t LR11XX_RF_OUTPUT_POWER_DBM_MIN[LR11XX_PA_LAST] = { LR11XX_RF_OUTPUT_POWER_LP_PA_DBM_MIN, LR11XX_RF_OUTPUT_POWER_HP_PA_DBM_MIN };
static const int8_t LR11XX_RF_OUTPUT_POWER_DBM_MAX[LR11XX_PA_LAST] = { LR11XX_RF_OUTPUT_POWER_LP_PA_DBM_MAX, LR11XX_RF_OUTPUT_POWER_HP_PA_DBM_MAX };

static const LR11XX_lp_pa_power_settings_t LR11XX_LP_PA_POWER_TABLE[LR11XX_LP_PA_POWER_TABLE_SIZE] = {
    { .power = -15, .pa_duty_cycle = 0x00 }, // -17dBm.
    { .power = -14, .pa_duty_cycle = 0x00 }, // -16dBm.
    { .power = -13, .pa_duty_cycle = 0x00 }, // -15dBm.
    { .power = -12, .pa_duty_cycle = 0x00 }, // -14dBm.
    { .power = -11, .pa_duty_cycle = 0x00 }, // -13dBm.
    { .power = -9,  .pa_duty_cycle = 0x00 }, // -12dBm.
    { .power = -8,  .pa_duty_cycle = 0x00 }, // -11dBm.
    { .power = -7,  .pa_duty_cycle = 0x00 }, // -10dBm.
    { .power = -6,  .pa_duty_cycle = 0x00 }, // -9dBm.
    { .power = -5,  .pa_duty_cycle = 0x00 }, // -8dBm.
    { .power = -4,  .pa_duty_cycle = 0x00 }, // -7dBm.
    { .power = -3,  .pa_duty_cycle = 0x00 }, // -6dBm.
    { .power = -2,  .pa_duty_cycle = 0x00 }, // -5dBm.
    { .power = -1,  .pa_duty_cycle = 0x00 }, // -4dBm.
    { .power =  0,  .pa_duty_cycle = 0x00 }, // -3dBm.
    { .power =  1,  .pa_duty_cycle = 0x00 }, // -2dBm.
    { .power =  2,  .pa_duty_cycle = 0x00 }, // -1dBm.
    { .power =  3,  .pa_duty_cycle = 0x00 }, //  0dBm.
    { .power =  3,  .pa_duty_cycle = 0x01 }, //  1dBm.
    { .power =  4,  .pa_duty_cycle = 0x01 }, //  2dBm.
    { .power =  7,  .pa_duty_cycle = 0x00 }, //  3dBm.
    { .power =  8,  .pa_duty_cycle = 0x00 }, //  4dBm.
    { .power =  9,  .pa_duty_cycle = 0x00 }, //  5dBm.
    { .power =  10, .pa_duty_cycle = 0x00 }, //  6dBm.
    { .power =  12, .pa_duty_cycle = 0x00 }, //  7dBm.
    { .power =  13, .pa_duty_cycle = 0x00 }, //  8dBm.
    { .power =  14, .pa_duty_cycle = 0x00 }, //  9dBm.
    { .power =  13, .pa_duty_cycle = 0x01 }, //  10dBm.
    { .power =  13, .pa_duty_cycle = 0x02 }, //  11dBm.
    { .power =  14, .pa_duty_cycle = 0x02 }, //  12dBm.
    { .power =  14, .pa_duty_cycle = 0x03 }, //  13dBm.
    { .power =  14, .pa_duty_cycle = 0x04 }, //  14dBm.
    { .power =  14, .pa_duty_cycle = 0x07 }  //  15dBm.
};

static const LR11XX_hp_pa_power_settings_t LR11XX_HP_PA_POWER_TABLE[LR11XX_HP_PA_POWER_TABLE_SIZE] = {
    { .power = 9,   .pa_duty_cycle = 0x00, .pa_hp_sel = 0x00 }, // -9dBm.
    { .power = 10,  .pa_duty_cycle = 0x00, .pa_hp_sel = 0x00 }, // -8dBm.
    { .power = 11,  .pa_duty_cycle = 0x00, .pa_hp_sel = 0x00 }, // -7dBm.
    { .power = 12,  .pa_duty_cycle = 0x00, .pa_hp_sel = 0x00 }, // -6dBm.
    { .power = 13,  .pa_duty_cycle = 0x00, .pa_hp_sel = 0x00 }, // -5dBm.
    { .power = 13,  .pa_duty_cycle = 0x01, .pa_hp_sel = 0x00 }, // -4dBm.
    { .power = 13,  .pa_duty_cycle = 0x02, .pa_hp_sel = 0x00 }, // -3dBm.
    { .power = 17,  .pa_duty_cycle = 0x02, .pa_hp_sel = 0x00 }, // -2dBm.
    { .power = 14,  .pa_duty_cycle = 0x04, .pa_hp_sel = 0x00 }, // -1dBm.
    { .power = 12,  .pa_duty_cycle = 0x00, .pa_hp_sel = 0x01 }, //  0dBm.
    { .power = 13,  .pa_duty_cycle = 0x00, .pa_hp_sel = 0x01 }, //  1dBm.
    { .power = 13,  .pa_duty_cycle = 0x01, .pa_hp_sel = 0x01 }, //  2dBm.
    { .power = 13,  .pa_duty_cycle = 0x02, .pa_hp_sel = 0x01 }, //  3dBm.
    { .power = 15,  .pa_duty_cycle = 0x00, .pa_hp_sel = 0x02 }, //  4dBm.
    { .power = 15,  .pa_duty_cycle = 0x04, .pa_hp_sel = 0x01 }, //  5dBm.
    { .power = 14,  .pa_duty_cycle = 0x02, .pa_hp_sel = 0x02 }, //  6dBm.
    { .power = 14,  .pa_duty_cycle = 0x01, .pa_hp_sel = 0x03 }, //  7dBm.
    { .power = 17,  .pa_duty_cycle = 0x04, .pa_hp_sel = 0x02 }, //  8dBm.
    { .power = 22,  .pa_duty_cycle = 0x00, .pa_hp_sel = 0x01 }, //  9dBm.
    { .power = 22,  .pa_duty_cycle = 0x01, .pa_hp_sel = 0x01 }, //  10dBm.
    { .power = 22,  .pa_duty_cycle = 0x02, .pa_hp_sel = 0x01 }, //  11dBm.
    { .power = 22,  .pa_duty_cycle = 0x03, .pa_hp_sel = 0x01 }, //  12dBm.
    { .power = 22,  .pa_duty_cycle = 0x00, .pa_hp_sel = 0x03 }, //  13dBm.
    { .power = 22,  .pa_duty_cycle = 0x01, .pa_hp_sel = 0x03 }, //  14dBm.
    { .power = 22,  .pa_duty_cycle = 0x04, .pa_hp_sel = 0x02 }, //  15dBm.
    { .power = 22,  .pa_duty_cycle = 0x01, .pa_hp_sel = 0x04 }, //  16dBm.
    { .power = 22,  .pa_duty_cycle = 0x02, .pa_hp_sel = 0x04 }, //  17dBm.
    { .power = 22,  .pa_duty_cycle = 0x01, .pa_hp_sel = 0x06 }, //  18dBm.
    { .power = 22,  .pa_duty_cycle = 0x03, .pa_hp_sel = 0x05 }, //  19dBm.
    { .power = 22,  .pa_duty_cycle = 0x03, .pa_hp_sel = 0x07 }, //  20dBm.
    { .power = 22,  .pa_duty_cycle = 0x04, .pa_hp_sel = 0x06 }, //  21dBm.
    { .power = 22,  .pa_duty_cycle = 0x04, .pa_hp_sel = 0x07 }  //  22dBm.
};
#endif
#ifdef LR11XX_DRIVER_RX_ENABLE
static const uint8_t LR11XX_RXBW[LR11XX_RXBW_LAST] = { 0x1F, 0x17, 0x0F, 0x1E, 0x16, 0x0E, 0x1D, 0x15, 0x0D, 0x1C, 0x14, 0x0C, 0x1B, 0x13, 0x0B, 0x1A, 0x12, 0x0A, 0x19, 0x11, 0x09 };
#endif

/*** LR11XX local functions ***/

/*******************************************************************/
static LR11XX_status_t _LR11XX_spi_write_read_8(uint8_t* tx_data, uint8_t* rx_data, uint8_t transfer_size) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    // Wait for BUSY line to be low.
    status = LR11XX_HW_wait_busy_low();
    if (status != LR11XX_SUCCESS) {
        status = LR11XX_ERROR_BUSY_TIMEOUT;
        goto errors;
    }
    // Perform SPI transfer.
    status = LR11XX_HW_spi_write_read_8(tx_data, rx_data, transfer_size);
    if (status != LR11XX_SUCCESS) goto errors;
    // Check if command has been successfully executed (STAT1 bits).
    if (((rx_data[0] >> 1) & 0x07) < 2) {
        status = LR11XX_ERROR_COMMAND_EXECUTION;
        goto errors;
    }
errors:
    return status;
}

/*******************************************************************/
static LR11XX_status_t _LR11XX_write_command(uint8_t* command, uint8_t command_size) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    uint8_t unused_response[255] = { 0x00 };
    // Command SPI transfer.
    status = _LR11XX_spi_write_read_8(command, unused_response, command_size);
    if (status != LR11XX_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
static LR11XX_status_t _LR11XX_read_command(uint8_t* command, uint8_t command_size, uint8_t* response, uint8_t response_size) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    uint8_t unused_command[255] = { 0x00 };
    uint8_t unused_response[255] = { 0x00 };
    // Command SPI transfer.
    status = _LR11XX_spi_write_read_8(command, unused_response, command_size);
    if (status != LR11XX_SUCCESS) goto errors;
    // Response SPI transfer.
    status = _LR11XX_spi_write_read_8(unused_command, response, response_size);
    if (status != LR11XX_SUCCESS) goto errors;
errors:
    return status;
}

#if 0
/*******************************************************************/
static LR11XX_status_t _LR11XX_write_register(uint32_t register_address, uint32_t value) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    uint8_t command[LR11XX_COMMAND_SIZE_WRITE_REG_MEM_32] = {
        (uint8_t) (LR11XX_OP_CODE_WRITE_REG_MEM_32 >> 8),
        (uint8_t) (LR11XX_OP_CODE_WRITE_REG_MEM_32 >> 0),
        (uint8_t) (register_address >> 24),
        (uint8_t) (register_address >> 16),
        (uint8_t) (register_address >> 8),
        (uint8_t) (register_address >> 0),
        (uint8_t) (value >> 24),
        (uint8_t) (value >> 16),
        (uint8_t) (value >> 8),
        (uint8_t) (value >> 0)
    };
    // Write access sequence.
    status = _LR11XX_write_command(command, LR11XX_COMMAND_SIZE_WRITE_REG_MEM_32);
    if (status != LR11XX_SUCCESS) goto errors;
errors:
    return status;
}
#endif

#if 0
/*******************************************************************/
static LR11XX_status_t _LR11XX_read_register(uint32_t register_address, uint32_t* value) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    uint8_t command[LR11XX_COMMAND_SIZE_READ_REG_MEM_32] = {
        (uint8_t) (LR11XX_OP_CODE_READ_REG_MEM_32 >> 8),
        (uint8_t) (LR11XX_OP_CODE_READ_REG_MEM_32 >> 0),
        (uint8_t) (register_address >> 24),
        (uint8_t) (register_address >> 16),
        (uint8_t) (register_address >> 8),
        (uint8_t) (register_address >> 0),
        (uint8_t) 1
    };
    uint8_t response[LR11XX_RESPONSE_SIZE_READ_REG_MEM_32];
    // Read access sequence.
    status = _LR11XX_read_command(command, LR11XX_COMMAND_SIZE_READ_REG_MEM_32, response, LR11XX_RESPONSE_SIZE_READ_REG_MEM_32);
    if (status != LR11XX_SUCCESS) goto errors;
    // Update value.
    (*value) = 0;
    (*value) |= (uint32_t) (response[4] << 0);
    (*value) |= (uint32_t) (response[3] << 8);
    (*value) |= (uint32_t) (response[2] << 16);
    (*value) |= (uint32_t) (response[1] << 24);
errors:
    return status;
}
#endif

/*** LR11XX functions ***/

/*******************************************************************/
LR11XX_status_t LR11XX_init(void) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    // Init hardware interface.
    status = LR11XX_HW_init();
    if (status != LR11XX_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
LR11XX_status_t LR11XX_de_init(void) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    // Release hardware interface.
    status = LR11XX_HW_de_init();
    if (status != LR11XX_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
LR11XX_status_t LR11XX_reset(uint8_t reset_enable) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    // Check enable.
    if (reset_enable == 0) {
        // Put NRESET high.
        status = LR11XX_HW_set_nreset_gpio(1);
        if (status != LR11XX_SUCCESS) goto errors;
        // Wait for reset time.
        status = LR11XX_HW_delay_milliseconds(LR11XX_EXIT_RESET_DELAY_MS);
        if (status != LR11XX_SUCCESS) goto errors;
    }
    else {
        // Put NRESET low.
        status = LR11XX_HW_set_nreset_gpio(0);
        if (status != LR11XX_SUCCESS) goto errors;
    }
errors:
    return status;
}

/*******************************************************************/
LR11XX_status_t LR11XX_get_errors(uint16_t* error_stat) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    uint8_t command[LR11XX_COMMAND_SIZE_GET_ERRORS] = {  (uint8_t) (LR11XX_OP_CODE_GET_ERRORS >> 8), (uint8_t) (LR11XX_OP_CODE_GET_ERRORS >> 0) };
    uint8_t response[LR11XX_RESPONSE_SIZE_GET_ERRORS];
    // Check parameter.
    if (error_stat == NULL) {
        status = LR11XX_ERROR_NULL_PARAMETER;
        goto errors;
    }
    // Send command.
    status = _LR11XX_read_command(command, LR11XX_COMMAND_SIZE_GET_ERRORS, response, LR11XX_RESPONSE_SIZE_GET_ERRORS);
    if (status != LR11XX_SUCCESS) goto errors;
    // Update data.
    (*error_stat) = (uint16_t) ((response[1] << 8) + response[2]);
errors:
    return status;
}

/*******************************************************************/
LR11XX_status_t LR11XX_clear_errors(void) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    uint8_t command[LR11XX_COMMAND_SIZE_CLEAR_ERRORS] = { (uint8_t) (LR11XX_OP_CODE_CLEAR_ERRORS >> 8), (uint8_t) (LR11XX_OP_CODE_CLEAR_ERRORS >> 0) };
    // Send command.
    status = _LR11XX_write_command(command, LR11XX_COMMAND_SIZE_CLEAR_ERRORS);
    if (status != LR11XX_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
LR11XX_status_t LR11XX_set_regulation_mode(LR11XX_regulation_mode_t regulation_mode) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    uint8_t command[LR11XX_COMMAND_SIZE_SET_REG_MODE] = { (uint8_t) (LR11XX_OP_CODE_SET_REG_MODE >> 8), (uint8_t) (LR11XX_OP_CODE_SET_REG_MODE >> 0), 0x00 };
    // Check regulation mode.
    switch (regulation_mode) {
    case LR11XX_REGULATION_MODE_LDO:
        break;
    case LR11XX_REGULATION_MODE_DCDC:
        command[2] = 0x01;
        break;
    default:
        status = LR11XX_ERROR_REGULATION_MODE;
        goto errors;
    }
    // Send command.
    status = _LR11XX_write_command(command, LR11XX_COMMAND_SIZE_SET_REG_MODE);
    if (status != LR11XX_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
LR11XX_status_t LR11XX_set_oscillator(LR11XX_oscillator_t oscillator, LR11XX_tcxo_voltage_t tcxo_voltage, uint32_t tcxo_timeout_ms, LR11XX_lf_clock_t lf_clock) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    uint8_t command[LR11XX_COMMAND_SIZE_SET_TCXO_MODE] = { (uint8_t) (LR11XX_OP_CODE_SET_TCXO_MODE >> 8), (uint8_t) (LR11XX_OP_CODE_SET_TCXO_MODE >> 0), 0x00, 0x00, 0x00, 0x00 };
    uint64_t tmp_u64 = 0;
    // Check oscillator.
    switch (oscillator) {
    case LR11XX_OSCILLATOR_QUARTZ:
        // Nothing to do.
        break;
    case LR11XX_OSCILLATOR_TCXO:
        // Check parameters.
        if (tcxo_voltage >= LR11XX_TCXO_VOLTAGE_LAST) {
            status = LR11XX_ERROR_TCXO_VOLTAGE;
            goto errors;
        }
        if ((tcxo_timeout_ms < LR11XX_TCXO_TIMEOUT_MIN_MS) || (tcxo_timeout_ms > LR11XX_TCXO_TIMEOUT_MAX_MS)) {
            status = LR11XX_ERROR_TCXO_TIMEOUT;
            goto errors;
        }
        // Compute timeout.
        tmp_u64 = (((uint64_t) tcxo_timeout_ms) * 1000000);
        tmp_u64 /= ((uint64_t) LR11XX_TCXO_TIMEOUT_DELAY_STEP_NS);
        // Compute parameters.
        command[2] = (uint8_t) (tcxo_voltage);
        command[3] = (uint8_t) (tmp_u64 >> 16);
        command[4] = (uint8_t) (tmp_u64 >> 8);
        command[5] = (uint8_t) (tmp_u64 >> 0);
        // Send command.
        status = _LR11XX_write_command(command, LR11XX_COMMAND_SIZE_SET_TCXO_MODE);
        if (status != LR11XX_SUCCESS) goto errors;
        break;
    default:
        status = LR11XX_ERROR_OSCILLATOR;
        goto errors;
    }
    // Check low frequency clock.
    if (lf_clock >= LR11XX_LF_CLOCK_LAST) {
        status = LR11XX_ERROR_LF_CLOCK;
        goto errors;
    }
    command[0] = (uint8_t) (LR11XX_OP_CODE_CONFIG_LF_CLOCK >> 8);
    command[1] = (uint8_t) (LR11XX_OP_CODE_CONFIG_LF_CLOCK >> 0);
    command[2] = ((0b1 << 2) | (lf_clock & 0x03));
    // Send command.
    status = _LR11XX_write_command(command, LR11XX_COMMAND_SIZE_CONFIG_LF_CLOCK);
    if (status != LR11XX_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
LR11XX_status_t LR11XX_calibrate(uint16_t frequency_range_low_mhz, uint16_t frequency_range_high_mhz) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    uint8_t command[LR11XX_COMMAND_SIZE_CALIB_IMAGE] = { (uint8_t) (LR11XX_OP_CODE_CALIBRATE >> 8), (uint8_t) (LR11XX_OP_CODE_CALIBRATE >> 0) , 0x3F, 0x00 };
    uint8_t image_cal_freq1 = 0;
    uint8_t image_cal_freq2 = 0;
    // Send command.
    status = _LR11XX_write_command(command, LR11XX_COMMAND_SIZE_CALIBRATE);
    if (status != LR11XX_SUCCESS) goto errors;
    // Wait for calibration to complete.
    status = LR11XX_HW_wait_busy_low();
    if (status != LR11XX_SUCCESS) {
        status = LR11XX_ERROR_CALIBRATION_TIMEOUT;
        goto errors;
    }
    // Calibrate image according to requested frequency.
    image_cal_freq1 = (uint8_t) (frequency_range_low_mhz / LR11XX_IMAGE_CALIBRATION_STEP_MHZ);
    image_cal_freq2 = (uint8_t) ((frequency_range_high_mhz + LR11XX_IMAGE_CALIBRATION_STEP_MHZ - 1) / (LR11XX_IMAGE_CALIBRATION_STEP_MHZ));
    command[0] = (uint8_t) (LR11XX_OP_CODE_CALIB_IMAGE >> 8);
    command[1] = (uint8_t) (LR11XX_OP_CODE_CALIB_IMAGE >> 0);
    command[2] = image_cal_freq1;
    command[3] = image_cal_freq2;
    // Send command.
    status = _LR11XX_write_command(command, LR11XX_COMMAND_SIZE_CALIB_IMAGE);
    if (status != LR11XX_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
LR11XX_status_t LR11XX_set_mode(LR11XX_mode_t mode) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    uint8_t command[LR11XX_COMMAND_SIZE_SET_SLEEP] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    uint8_t command_size = 0;
    // Compute register.
    switch (mode) {
    case LR11XX_MODE_SLEEP:
        command[0] = (uint8_t) (LR11XX_OP_CODE_SET_SLEEP >> 8);
        command[1] = (uint8_t) (LR11XX_OP_CODE_SET_SLEEP >> 0);
        command_size = LR11XX_COMMAND_SIZE_SET_SLEEP;
        break;
    case LR11XX_MODE_STANDBY_RC:
        command[0] = (uint8_t) (LR11XX_OP_CODE_SET_STANDBY >> 8);
        command[1] = (uint8_t) (LR11XX_OP_CODE_SET_STANDBY >> 0);
        command_size = LR11XX_COMMAND_SIZE_SET_STANDBY;
        break;
    case LR11XX_MODE_STANDBY_XOSC:
        command[0] = (uint8_t) (LR11XX_OP_CODE_SET_STANDBY >> 8);
        command[1] = (uint8_t) (LR11XX_OP_CODE_SET_STANDBY >> 0);
        command[2] = 0x01;
        command_size = LR11XX_COMMAND_SIZE_SET_STANDBY;
        break;
    case LR11XX_MODE_FS:
        command[0] = (uint8_t) (LR11XX_OP_CODE_SET_FS >> 8);
        command[1] = (uint8_t) (LR11XX_OP_CODE_SET_FS >> 0);
        command_size = LR11XX_COMMAND_SIZE_SET_FS;
        break;
    case LR11XX_MODE_TX:
        command[0] = (uint8_t) (LR11XX_OP_CODE_SET_TX >> 8);
        command[1] = (uint8_t) (LR11XX_OP_CODE_SET_TX >> 0);
        command_size = LR11XX_COMMAND_SIZE_SET_TX;
        break;
    case LR11XX_MODE_TX_CW:
        command[0] = (uint8_t) (LR11XX_OP_CODE_SET_TX_CW >> 8);
        command[1] = (uint8_t) (LR11XX_OP_CODE_SET_TX_CW >> 0);
        command_size = LR11XX_COMMAND_SIZE_SET_TX_CW;
        break;
    case LR11XX_MODE_RX:
        command[0] = (uint8_t) (LR11XX_OP_CODE_SET_RX >> 8);
        command[1] = (uint8_t) (LR11XX_OP_CODE_SET_RX >> 0);
        command_size = LR11XX_COMMAND_SIZE_SET_RX;
        break;
    default:
        status = LR11XX_ERROR_MODE;
        goto errors;
    }
    // Send command.
    status = _LR11XX_write_command(command, command_size);
    if (status != LR11XX_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
LR11XX_status_t LR11XX_set_rf_frequency(uint32_t rf_frequency_hz) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    uint8_t command[LR11XX_COMMAND_SIZE_SET_RF_FREQUENCY] = {
        (uint8_t) (LR11XX_OP_CODE_SET_RF_FREQUENCY >> 8),
        (uint8_t) (LR11XX_OP_CODE_SET_RF_FREQUENCY >> 0),
        (uint8_t) (rf_frequency_hz >> 24),
        (uint8_t) (rf_frequency_hz >> 16),
        (uint8_t) (rf_frequency_hz >> 8),
        (uint8_t) (rf_frequency_hz >> 0)
    };
    // Check frequency range.
    if (rf_frequency_hz > LR11XX_RF_FREQUENCY_HZ_MAX) {
        status = LR11XX_ERROR_RF_FREQUENCY_OVERFLOW;
        goto errors;
    }
    if (rf_frequency_hz < LR11XX_RF_FREQUENCY_HZ_MIN) {
        status = LR11XX_ERROR_RF_FREQUENCY_UNDERFLOW;
        goto errors;
    }
    // Send command.
    status = _LR11XX_write_command(command, LR11XX_COMMAND_SIZE_SET_RF_FREQUENCY);
    if (status != LR11XX_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
LR11XX_status_t LR11XX_set_modulation(LR11XX_modulation_parameters_t* modulation_parameters) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    uint8_t command[LR11XX_COMMAND_SIZE_SET_MODULATION_PARAMS_GFSK] = {
        (uint8_t) (LR11XX_OP_CODE_SET_PACKET_TYPE >> 8),
        (uint8_t) (LR11XX_OP_CODE_SET_PACKET_TYPE >> 0),
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x1F,
        0x00,
        0x00,
        0x00,
        0x00
    };
    // Check parameter.
    if (modulation_parameters == NULL) {
        status = LR11XX_ERROR_NULL_PARAMETER;
        goto errors;
    }
    // Packet type.
    switch (modulation_parameters->modulation) {
    case LR11XX_MODULATION_GFSK:
        command[2] = 0x01;
        break;
    case LR11XX_MODULATION_BPSK:
        command[2] = 0x03;
        break;
    default:
        status = LR11XX_ERROR_MODULATION;
        goto errors;
    }
    // Send command.
    status = _LR11XX_write_command(command, LR11XX_COMMAND_SIZE_SET_PACKET_TYPE);
    if (status != LR11XX_SUCCESS) goto errors;
    // Modulation parameters.
    command[0] = (uint8_t) (LR11XX_OP_CODE_SET_MODULATION_PARAMS >> 8);
    command[1] = (uint8_t) (LR11XX_OP_CODE_SET_MODULATION_PARAMS >> 0);
    // Bit rate.
    if ((modulation_parameters->bit_rate_bps) > LR11XX_BIT_RATE_BPS_MAX) {
        status = LR11XX_ERROR_BIT_RATE_OVERFLOW;
        goto errors;
    }
    if ((modulation_parameters->bit_rate_bps) < LR11XX_BIT_RATE_BPS_MIN) {
        status = LR11XX_ERROR_BIT_RATE_UNDERFLOW;
        goto errors;
    }
    command[2] = (uint8_t) (((modulation_parameters->bit_rate_bps) >> 24) & 0x7F);
    command[3] = (uint8_t)  ((modulation_parameters->bit_rate_bps) >> 16);
    command[4] = (uint8_t)  ((modulation_parameters->bit_rate_bps) >> 8);
    command[5] = (uint8_t)  ((modulation_parameters->bit_rate_bps) >> 0);
    // Modulation shaping.
    switch (modulation_parameters->modulation_shaping) {
    case LR11XX_MODULATION_SHAPING_NONE:
        command[6] = 0x00;
        break;
    case LR11XX_MODULATION_SHAPING_GAUSSIAN_BT_03:
        command[6] = 0x08;
        break;
    case LR11XX_MODULATION_SHAPING_GAUSSIAN_BT_05:
        command[6] = 0x09;
        break;
    case LR11XX_MODULATION_SHAPING_GAUSSIAN_BT_07:
        command[6] = 0x0A;
        break;
    case LR11XX_MODULATION_SHAPING_GAUSSIAN_BT_1:
        command[6] = 0x0B;
        break;
    case LR11XX_MODULATION_SHAPING_DBPSK:
        command[6] = 0x16;
        break;
    default:
        status = LR11XX_ERROR_MODULATION_SHAPING;
        goto errors;
    }
    if (modulation_parameters->rx_bandwidth >= LR11XX_RXBW_LAST) {
        status = LR11XX_ERROR_RX_BANDWIDTH;
        goto errors;
    }
#ifdef LR11XX_DRIVER_RX_ENABLE
    command[7] = LR11XX_RXBW[modulation_parameters->rx_bandwidth];
#endif
    // Deviation.
    command[8] =  (uint8_t) ((modulation_parameters->fsk_deviation_hz) >> 24);
    command[9] =  (uint8_t) ((modulation_parameters->fsk_deviation_hz) >> 16);
    command[10] = (uint8_t) ((modulation_parameters->fsk_deviation_hz) >> 8);
    command[11] = (uint8_t) ((modulation_parameters->fsk_deviation_hz) >> 0);
    // Send command.
    status = _LR11XX_write_command(command, LR11XX_COMMAND_SIZE_SET_MODULATION_PARAMS_GFSK);
    if (status != LR11XX_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
LR11XX_status_t LR11XX_set_gfsk_packet(LR11XX_gfsk_packet_parameters_t* packet_parameters) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    uint8_t command[LR11XX_COMMAND_SIZE_SET_PACKET_PARAMS_GFSK] = {
        (uint8_t) (LR11XX_OP_CODE_SET_PACKET_PARAMS >> 8),
        (uint8_t) (LR11XX_OP_CODE_SET_PACKET_PARAMS >> 0),
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x01,
        0x00
    };
    uint8_t idx = 0;
    // Check parameter.
    if (packet_parameters == NULL) {
        status = LR11XX_ERROR_NULL_PARAMETER;
        goto errors;
    }
    // Preamble length.
    command[2] = (uint8_t) ((packet_parameters->preamble_length_bits) >> 8);
    command[3] = (uint8_t) ((packet_parameters->preamble_length_bits) >> 0);
    // Preamble detector length.
    switch (packet_parameters->preamble_detector_length) {
    case LR11XX_PREAMBLE_DETECTOR_LENGTH_OFF:
        command[4] = 0x00;
        break;
    case LR11XX_PREAMBLE_DETECTOR_LENGTH_8BITS:
        command[4] = 0x04;
        break;
    case LR11XX_PREAMBLE_DETECTOR_LENGTH_16BITS:
        command[4] = 0x05;
        break;
    case LR11XX_PREAMBLE_DETECTOR_LENGTH_24BITS:
        command[4] = 0x06;
        break;
    case LR11XX_PREAMBLE_DETECTOR_LENGTH_32BITS:
        command[4] = 0x07;
        break;
    default:
        status = LR11XX_ERROR_PREAMBLE_DETECTOR_LENGTH;
        goto errors;
    }
    // Sync word length.
    if ((packet_parameters->sync_word_length_bits) > (LR11XX_SYNC_WORD_SIZE_BYTES_MAX << 3)) {
        status = LR11XX_ERROR_SYNC_WORD_LENGTH;
        goto errors;
    }
    command[5] = (packet_parameters->sync_word_length_bits);
    // Payload length.
    command[8] = (packet_parameters->payload_length_bytes);
    // Send command.
    status = _LR11XX_write_command(command, LR11XX_COMMAND_SIZE_SET_PACKET_PARAMS_GFSK);
    if (status != LR11XX_SUCCESS) goto errors;
    // Sync word.
    command[0] = (uint8_t) (LR11XX_OP_CODE_SET_GFSK_SYNC_WORD >> 8);
    command[1] = (uint8_t) (LR11XX_OP_CODE_SET_GFSK_SYNC_WORD >> 0);
    for (idx = 0; idx < LR11XX_SYNC_WORD_SIZE_BYTES_MAX; idx++) {
        command[2 + idx] = (packet_parameters->sync_word[idx]);
    }
    // Send command.
    status = _LR11XX_write_command(command, LR11XX_COMMAND_SIZE_SET_GFSK_SYNC_WORD);
    if (status != LR11XX_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
LR11XX_status_t LR11XX_set_bpsk_packet(LR11XX_bpsk_packet_parameters_t* packet_parameters) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    uint8_t command[LR11XX_COMMAND_SIZE_SET_PACKET_PARAMS_BPSK] = {
        (uint8_t) (LR11XX_OP_CODE_SET_PACKET_PARAMS >> 8),
        (uint8_t) (LR11XX_OP_CODE_SET_PACKET_PARAMS >> 0),
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00
    };
    // Check parameter.
    if (packet_parameters == NULL) {
        status = LR11XX_ERROR_NULL_PARAMETER;
        goto errors;
    }
    // Payload length.
    command[2] = (packet_parameters->payload_length_bytes);
    command[3] = (uint8_t) ((packet_parameters->ramp_up_delay) >> 8);
    command[4] = (uint8_t) ((packet_parameters->ramp_up_delay) >> 0);
    command[5] = (uint8_t) ((packet_parameters->ramp_down_delay) >> 8);
    command[6] = (uint8_t) ((packet_parameters->ramp_down_delay) >> 0);
    command[7] = (uint8_t) ((packet_parameters->payload_length_bits) >> 8);
    command[8] = (uint8_t) ((packet_parameters->payload_length_bits) >> 0);
    // Send command.
    status = _LR11XX_write_command(command, LR11XX_COMMAND_SIZE_SET_PACKET_PARAMS_BPSK);
    if (status != LR11XX_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
LR11XX_status_t LR11XX_set_dio_irq_mask(uint32_t irq_mask_dio9, uint32_t irq_mask_dio11) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    uint8_t command[LR11XX_COMMAND_SIZE_SET_DIO_IRQ_PARAMS] = {
        (uint8_t) (LR11XX_OP_CODE_SET_DIO_IRQ_PARAMS >> 8),
        (uint8_t) (LR11XX_OP_CODE_SET_DIO_IRQ_PARAMS >> 0),
        (uint8_t) (irq_mask_dio9  >> 24),
        (uint8_t) (irq_mask_dio9  >> 16),
        (uint8_t) (irq_mask_dio9  >> 8),
        (uint8_t) (irq_mask_dio9  >> 0),
        (uint8_t) (irq_mask_dio11 >> 24),
        (uint8_t) (irq_mask_dio11 >> 16),
        (uint8_t) (irq_mask_dio11 >> 8),
        (uint8_t) (irq_mask_dio11 >> 0),
    };
    // Send command.
    status = _LR11XX_write_command(command, LR11XX_COMMAND_SIZE_SET_DIO_IRQ_PARAMS);
    if (status != LR11XX_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
LR11XX_status_t LR11XX_get_irq_flag(LR11XX_irq_index_t irq_index, uint8_t* irq_flag) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    uint8_t command[LR11XX_COMMAND_SIZE_GET_STATUS] = {
        (uint8_t) (LR11XX_OP_CODE_GET_STATUS >> 8),
        (uint8_t) (LR11XX_OP_CODE_GET_STATUS >> 0),
        (uint8_t) LR11XX_OP_CODE_NOP,
        (uint8_t) LR11XX_OP_CODE_NOP,
        (uint8_t) LR11XX_OP_CODE_NOP,
        (uint8_t) LR11XX_OP_CODE_NOP
    };
    uint8_t data[LR11XX_COMMAND_SIZE_GET_STATUS];
    // Check parameters.
    if (irq_index >= LR11XX_IRQ_INDEX_LAST) {
        status = LR11XX_ERROR_IRQ_INDEX;
        goto errors;
    }
    if (irq_flag == NULL) {
        status = LR11XX_ERROR_NULL_PARAMETER;
        goto errors;
    }
    // Send command.
    status = _LR11XX_spi_write_read_8(command, data, LR11XX_COMMAND_SIZE_GET_STATUS);
    if (status != LR11XX_SUCCESS) goto errors;
    // Read bit.
    (*irq_flag) = ((data[LR11XX_COMMAND_SIZE_GET_STATUS - 1 - (irq_index >> 3)] >> (irq_index % 8)) & 0x01);
errors:
    return status;
}

#ifdef LR11XX_DRIVER_TX_ENABLE
/*******************************************************************/
LR11XX_status_t LR11XX_set_rf_output_power(LR11XX_pa_t pa, int8_t rf_output_power_dbm, LR11XX_pa_ramp_time_t pa_ramp_time) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    uint8_t command_pa_config[LR11XX_COMMAND_SIZE_SET_PA_CONFIG] = {
        (uint8_t) (LR11XX_OP_CODE_SET_PA_CONFIG >> 8),
        (uint8_t) (LR11XX_OP_CODE_SET_PA_CONFIG >> 0),
        pa,
        pa,
        0x00,
        0x00
    };
    uint8_t command_tx_params[LR11XX_COMMAND_SIZE_SET_TX_PARAMS] = {
        (uint8_t) (LR11XX_OP_CODE_SET_TX_PARAMS >> 8),
        (uint8_t) (LR11XX_OP_CODE_SET_TX_PARAMS >> 0),
        (uint8_t) 0x00,
        (uint8_t) (pa_ramp_time)
    };
    uint8_t pa_power_table_idx = 0;
    // Check parameters.
    if (pa >= LR11XX_PA_LAST) {
        status = LR11XX_ERROR_PA;
        goto errors;
    }
    if (rf_output_power_dbm > LR11XX_RF_OUTPUT_POWER_DBM_MAX[pa]) {
        status = LR11XX_ERROR_RF_OUTPUT_POWER_OVERFLOW;
        goto errors;
    }
    if (rf_output_power_dbm < LR11XX_RF_OUTPUT_POWER_DBM_MIN[pa]) {
        status = LR11XX_ERROR_RF_OUTPUT_POWER_UNDERFLOW;
        goto errors;
    }
    if (pa_ramp_time >= LR11XX_PA_RAMP_TIME_LAST) {
        status = LR11XX_ERROR_PA_RAMP_TIME;
        goto errors;
    }
    pa_power_table_idx =  (uint8_t) (rf_output_power_dbm - LR11XX_RF_OUTPUT_POWER_DBM_MIN[pa]);
    // Set PA power settings.
    switch (pa) {
    case LR11XX_PA_LOW_POWER:
        command_pa_config[4] = LR11XX_LP_PA_POWER_TABLE[pa_power_table_idx].pa_duty_cycle;
        command_tx_params[2] = (uint8_t) LR11XX_LP_PA_POWER_TABLE[pa_power_table_idx].power;
        break;
    case LR11XX_PA_HIGH_POWER:
        command_pa_config[4] = LR11XX_HP_PA_POWER_TABLE[pa_power_table_idx].pa_duty_cycle;
        command_pa_config[5] = LR11XX_HP_PA_POWER_TABLE[pa_power_table_idx].pa_hp_sel;
        command_tx_params[2] = (uint8_t) LR11XX_HP_PA_POWER_TABLE[pa_power_table_idx].power;
        break;
    default:
        status = LR11XX_ERROR_PA;
        goto errors;
    }
    // Set PA configuration.
    status = _LR11XX_write_command(command_pa_config, LR11XX_COMMAND_SIZE_SET_PA_CONFIG);
    if (status != LR11XX_SUCCESS) goto errors;
    // Set TX parameters.
    status = _LR11XX_write_command(command_tx_params, LR11XX_COMMAND_SIZE_SET_TX_PARAMS);
    if (status != LR11XX_SUCCESS) goto errors;
errors:
    return status;
}
#endif

#ifdef LR11XX_DRIVER_TX_ENABLE
/*******************************************************************/
LR11XX_status_t LR11XX_differential_encoding(uint8_t* data_in, uint8_t data_in_size_bytes, uint8_t* data_out, uint8_t* data_out_size_bytes, uint16_t* data_out_size_bits) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    uint8_t in_byte = 0;
    uint8_t out_byte = 0;
    uint16_t data_in_size_bits = (uint16_t) ((data_in_size_bytes << 3) + 2);
    int16_t data_in_bytes_count = data_in_size_bytes;
    uint8_t current = 0;
    uint8_t idx = 0;
    // Check parameters.
    if ((data_in == NULL) || (data_out == NULL) || (data_out_size_bytes == NULL) || (data_out_size_bits == NULL)) {
        status = LR11XX_ERROR_NULL_PARAMETER;
        goto errors;
    }
    in_byte = (*data_in++);
    // Process full bytes
    while (--data_in_bytes_count >= 0) {
        for (idx = 0; idx < 8; ++idx) {
            out_byte = (out_byte << 1) | current;
            if ((in_byte & 0x80) == 0) {
                current = current ^ 0x01;
            }
            in_byte <<= 1;
        }
        in_byte = (*data_in++);
        *data_out++ = out_byte;
    }
    // Process remaining bits
    for (idx = 0; idx < (data_in_size_bits & 0x07); ++idx) {
        out_byte = (out_byte << 1) | current;
        if ((in_byte & 0x80) == 0) {
            current = current ^ 0x01;
        }
        in_byte <<= 1;
    }
    // Process last data bit
    out_byte = (out_byte << 1) | current;
    if ((data_in_size_bits & 0x07) == 0x07) {
        *data_out++ = out_byte;
    }
    // Add duplicate bit and store
    out_byte = (out_byte << 1) | current;
    (*data_out) = out_byte << (7 - ((data_in_size_bits + 1) & 0x07));
    // Update output sizes.
    (*data_out_size_bytes) = (uint8_t) ((data_in_size_bits + 7) >> 3);
    (*data_out_size_bits) = data_in_size_bits;
errors:
    return status;
}
#endif

#ifdef LR11XX_DRIVER_TX_ENABLE
/*******************************************************************/
LR11XX_status_t LR11XX_write_fifo(uint8_t* tx_data, uint8_t tx_data_size) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    uint8_t command[255];
    uint8_t idx = 0;
    // Check parameters.
    if (tx_data == NULL) {
        status = LR11XX_ERROR_NULL_PARAMETER;
        goto errors;
    }
    if (tx_data_size > (255 - LR11XX_COMMAND_SIZE_WRITE_BUFFER_8)) {
        status = LR11XX_ERROR_FIFO_TX_DATA_SIZE;
        goto errors;
    }
    // Operation code.
    command[0] = (uint8_t) (LR11XX_OP_CODE_WRITE_BUFFER_8 >> 8);
    command[1] = (uint8_t) (LR11XX_OP_CODE_WRITE_BUFFER_8 >> 0);
    // Data.
    for (idx = 0; idx < tx_data_size; idx ++) {
        command[LR11XX_COMMAND_SIZE_WRITE_BUFFER_8 + idx] = tx_data[idx];
    }
    // Send command.
    status = _LR11XX_write_command(command, (LR11XX_COMMAND_SIZE_WRITE_BUFFER_8 + tx_data_size));
    if (status != LR11XX_SUCCESS) goto errors;
errors:
    return status;
}
#endif

#ifdef LR11XX_DRIVER_RX_ENABLE
/*******************************************************************/
LR11XX_status_t LR11XX_set_lna_mode(LR11XX_lna_mode_t lna_mode) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    uint8_t command[LR11XX_COMMAND_SIZE_SET_RX_BOOSTED] = { (uint8_t) (LR11XX_OP_CODE_SET_RX_BOOSTED >> 8), (uint8_t) (LR11XX_OP_CODE_SET_RX_BOOSTED >> 0), lna_mode };
    // Check parameter.
    if (lna_mode >= LR11XX_LNA_MODE_LAST) {
        status = LR11XX_ERROR_LNA_MODE;
        goto errors;
    }
    // Send command.
    status = _LR11XX_write_command(command, LR11XX_COMMAND_SIZE_SET_RX_BOOSTED);
    if (status != LR11XX_SUCCESS) goto errors;
errors:
    return status;
}
#endif

#ifdef LR11XX_DRIVER_RX_ENABLE
/*******************************************************************/
LR11XX_status_t LR11XX_get_rssi(LR11XX_rssi_t rssi_type, int16_t* rssi_dbm) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    uint8_t command[LR11XX_COMMAND_SIZE_GET_PACKET_STATUS] = { 0x00 };
    uint8_t response[LR11XX_RESPONSE_SIZE_GET_PACKET_STATUS];
    // Check parameters.
    if (rssi_type >= LR11XX_RSSI_TYPE_LAST) {
        status = LR11XX_ERROR_RSSI_TYPE;
        goto errors;
    }
    if (rssi_dbm == NULL) {
        status = LR11XX_ERROR_NULL_PARAMETER;
        goto errors;
    }
    if (rssi_type == LR11XX_RSSI_TYPE_INSTANTANEOUS) {
        // Update operation code.
        command[0] = (uint8_t) (LR11XX_OP_CODE_GET_RSSI_INST >> 8);
        command[1] = (uint8_t) (LR11XX_OP_CODE_GET_RSSI_INST >> 0);
        // Send command.
        status = _LR11XX_read_command(command, LR11XX_COMMAND_SIZE_GET_RSSI_INST, response, LR11XX_RESPONSE_SIZE_GET_RSSI);
        if (status != LR11XX_SUCCESS) goto errors;
    }
    else {
        // Update operation code.
        command[0] = (uint8_t) (LR11XX_OP_CODE_GET_PACKET_STATUS >> 8);
        command[1] = (uint8_t) (LR11XX_OP_CODE_GET_PACKET_STATUS >> 0);
        // Send command.
        status = _LR11XX_read_command(command, LR11XX_COMMAND_SIZE_GET_PACKET_STATUS, response, LR11XX_RESPONSE_SIZE_GET_PACKET_STATUS);
        if (status != LR11XX_SUCCESS) goto errors;
    }
    // Compute RSSI.
    (*rssi_dbm) = (-1) * ((int16_t) (response[1] >> 1));
errors:
    return status;
}
#endif

#ifdef LR11XX_DRIVER_RX_ENABLE
/*******************************************************************/
LR11XX_status_t LR11XX_clear_fifo(void) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    uint8_t command[LR11XX_COMMAND_SIZE_CLEAR_RX_BUFFER] = { (uint8_t) (LR11XX_OP_CODE_CLEAR_RX_BUFFER >> 8), (uint8_t) (LR11XX_OP_CODE_CLEAR_RX_BUFFER >> 0) };
    // Send command.
    status = _LR11XX_write_command(command, LR11XX_COMMAND_SIZE_CLEAR_RX_BUFFER);
    if (status != LR11XX_SUCCESS) goto errors;
errors:
    return status;
}
#endif

#ifdef LR11XX_DRIVER_RX_ENABLE
/*******************************************************************/
LR11XX_status_t LR11XX_read_fifo(uint8_t* rx_data, uint8_t rx_data_size) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    uint8_t command[LR11XX_COMMAND_SIZE_READ_BUFFER_8] = { (uint8_t) (LR11XX_OP_CODE_GET_RX_BUFFER_STATUS >> 8), (uint8_t) (LR11XX_OP_CODE_GET_RX_BUFFER_STATUS >> 0), 0x00, 0x00 };
    uint8_t response[255];
    uint8_t idx = 0;
    // Check parameters.
    if (rx_data == NULL) {
        status = LR11XX_ERROR_NULL_PARAMETER;
        goto errors;
    }
    if (rx_data_size > (255 - LR11XX_RESPONSE_SIZE_READ_BUFFER_8)) {
        status = LR11XX_ERROR_FIFO_RX_DATA_SIZE;
        goto errors;
    }
    // Get RX buffer status.
    status = _LR11XX_read_command(command, LR11XX_COMMAND_SIZE_GET_RX_BUFFER_STATUS, response, LR11XX_RESPONSE_SIZE_GET_RX_BUFFER_STATUS);
    if (status != LR11XX_SUCCESS) goto errors;
    // Check received payload length.
    if (rx_data_size > response[1]) {
        status = LR11XX_ERROR_RX_PAYLOAD_SIZE;
        goto errors;
    }
    // Build FIFO command.
    command[0] = (uint8_t) (LR11XX_OP_CODE_READ_BUFFER_8 >> 8);
    command[1] = (uint8_t) (LR11XX_OP_CODE_READ_BUFFER_8 >> 0);
    command[2] = response[2];
    command[3] = rx_data_size;
    // Send command.
    status = _LR11XX_read_command(command, LR11XX_COMMAND_SIZE_READ_BUFFER_8, response, (LR11XX_RESPONSE_SIZE_READ_BUFFER_8 + rx_data_size));
    if (status != LR11XX_SUCCESS) goto errors;
    // Update data.
    for (idx = 0; idx < rx_data_size; idx ++) {
        rx_data[idx] = response[LR11XX_RESPONSE_SIZE_READ_BUFFER_8 + idx];
    }
errors:
    return status;
}
#endif

#ifdef LR11XX_DRIVER_WIFI_ENABLE
/*******************************************************************/
LR11XX_status_t LR11XX_wifi_scan(LR11XX_wifi_scan_parameters_t* wifi_scan_parameters) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    uint8_t command[LR11XX_COMMAND_SIZE_WIFI_SCAN] = { 0x00 };
    // Check parameters.
    if (wifi_scan_parameters == NULL) {
        status = LR11XX_ERROR_NULL_PARAMETER;
        goto errors;
    }
    if ((wifi_scan_parameters->signal_type) >= LR11XX_WIFI_SIGNAL_TYPE_LAST) {
        status = LR11XX_ERROR_WIFI_SIGNAL_TYPE;
        goto errors;
    }
    if ((wifi_scan_parameters->acquisition_mode) >= LR11XX_WIFI_ACQUISITION_MODE_LAST) {
        status = LR11XX_ERROR_WIFI_ACQUISITION_MODE;
        goto errors;
    }
    if ((wifi_scan_parameters->mac_address_list_size) > LR11XX_WIFI_MAC_ADDRESS_LIST_SIZE_MAX) {
        status = LR11XX_ERROR_WIFI_MAC_ADDRESS_LIST_SIZE;
        goto errors;
    }
    if ((wifi_scan_parameters->number_of_scans_per_channel) == 0) {
        status = LR11XX_ERROR_WIFI_NUMBER_OF_SCANS_PER_CHANNEL;
        goto errors;
    }
    if ((wifi_scan_parameters->single_scan_timeout_ms) == 0) {
        status = LR11XX_ERROR_WIFI_SINGLE_SCAN_TIMEOUT;
        goto errors;
    }
    // Build WiFi scan command.
    command[0] = (uint8_t) (LR11XX_OP_CODE_WIFI_SCAN >> 8);
    command[1] = (uint8_t) (LR11XX_OP_CODE_WIFI_SCAN >> 0);
    command[2] = (wifi_scan_parameters->signal_type);
    command[3] = (uint8_t) ((wifi_scan_parameters->channel_mask) >> 8);
    command[4] = (uint8_t) ((wifi_scan_parameters->channel_mask) >> 0);
    command[5] = (wifi_scan_parameters->acquisition_mode);
    command[6] = (wifi_scan_parameters->mac_address_list_size);
    command[7] = (wifi_scan_parameters->number_of_scans_per_channel);
    command[8] = (uint8_t) ((wifi_scan_parameters->single_scan_timeout_ms) >> 8);
    command[9] = (uint8_t) ((wifi_scan_parameters->single_scan_timeout_ms) >> 0);
    command[10] = 0x00;
    // Send command.
    status = _LR11XX_write_command(command, LR11XX_COMMAND_SIZE_WIFI_SCAN);
    if (status != LR11XX_SUCCESS) goto errors;
errors:
    return status;
}
#endif

#ifdef LR11XX_DRIVER_WIFI_ENABLE
/*******************************************************************/
LR11XX_status_t LR11XX_wifi_read(LR11XX_wifi_scan_results_t* wifi_scan_results) {
    // Local variables.
    LR11XX_status_t status = LR11XX_SUCCESS;
    uint8_t command[LR11XX_COMMAND_SIZE_WIFI_SCAN] = { 0x00 };
    uint8_t response[LR11XX_WIFI_RESPONSE_SIZE_BYTES(LR11XX_WIFI_MAC_ADDRESS_LIST_SIZE_MAX)];
    uint8_t number_of_results = 0;
    uint8_t result_idx = 0;
    uint8_t byte_idx = 0;
    // Check parameters.
    if (wifi_scan_results == NULL) {
        status = LR11XX_ERROR_NULL_PARAMETER;
        goto errors;
    }
    if ((wifi_scan_results->access_point_list_size) == 0) {
        status = LR11XX_ERROR_WIFI_ACCESS_POINT_LIST_SIZE;
        goto errors;
    }
    // Reset results.
    wifi_scan_results->number_of_access_points_written = 0;
    wifi_scan_results->number_of_access_points_detected = 0;
    // Read number of results.
    command[0] = (uint8_t) (LR11XX_OP_CODE_WIFI_GET_NB_RESULTS >> 8);
    command[1] = (uint8_t) (LR11XX_OP_CODE_WIFI_GET_NB_RESULTS >> 0);
    // Send command.
    status = _LR11XX_read_command(command, LR11XX_COMMAND_SIZE_WIFI_GET_NB_RESULTS, response, LR11XX_RESPONSE_SIZE_WIFI_GET_NB_RESULTS);
    if (status != LR11XX_SUCCESS) goto errors;
    number_of_results = response[1];
    // Directly exit if there are no result.
    if (number_of_results == 0) goto errors;
    // Build WiFi results command.
    command[0] = (uint8_t) (LR11XX_OP_CODE_WIFI_READ_RESULTS >> 8);
    command[1] = (uint8_t) (LR11XX_OP_CODE_WIFI_READ_RESULTS >> 0);
    command[2] = 0x00;
    command[3] = number_of_results;
    command[4] = 0x04;
    // Send command.
    status = _LR11XX_read_command(command, LR11XX_COMMAND_SIZE_WIFI_READ_RESULTS, response, LR11XX_WIFI_RESPONSE_SIZE_BYTES(number_of_results));
    if (status != LR11XX_SUCCESS) goto errors;
    // Parse results.
    for (result_idx = 0; result_idx < number_of_results; result_idx++) {
        // Check size.
        if (result_idx >= (wifi_scan_results->access_point_list_size)) break;
        // Metadata.
        wifi_scan_results->access_point_list[result_idx].wifi_type.all = response[LR11XX_WIFI_RESPONSE_RESULT_OFFSET + 0];
        wifi_scan_results->access_point_list[result_idx].channel_info.all = response[LR11XX_WIFI_RESPONSE_RESULT_OFFSET + 1];
        wifi_scan_results->access_point_list[result_idx].rssi_dbm = (int8_t) response[LR11XX_WIFI_RESPONSE_RESULT_OFFSET + 2];
        // MAC address.
        for (byte_idx = 0; byte_idx < LR11XX_WIFI_MAC_ADDRESS_SIZE_BYTES; byte_idx++) {
            wifi_scan_results->access_point_list[result_idx].mac_address[byte_idx] = response[LR11XX_WIFI_RESPONSE_RESULT_OFFSET + 3 + byte_idx];
        }
    }
    wifi_scan_results->number_of_access_points_written = result_idx;
    wifi_scan_results->number_of_access_points_detected = number_of_results;
errors:
    return status;
}
#endif

#endif /* LR11XX_DRIVER_DISABLE */
