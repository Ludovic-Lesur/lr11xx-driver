# Description

This repository contains the **LR11xx** RF transceiver driver.

# Dependencies

The driver relies on:

* An external `types.h` header file defining the **standard C types** of the targeted MCU.
* The **embedded utility functions** defined in the [embedded-utils](https://github.com/Ludovic-Lesur/embedded-utils) repository.

Here is the versions compatibility table:

| **lr11xx-driver** | **embedded-utils** |
|:---:|:---:|
| [sw2.1](https://github.com/Ludovic-Lesur/lr11xx-driver/releases/tag/sw2.1) | >= [sw5.0](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw5.0) |
| [sw2.0](https://github.com/Ludovic-Lesur/lr11xx-driver/releases/tag/sw2.0) | >= [sw5.0](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw5.0) |
| [sw1.0](https://github.com/Ludovic-Lesur/lr11xx-driver/releases/tag/sw1.0) | >= [sw5.0](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw5.0) |

# Compilation flags

| **Flag name** | **Value** | **Description** |
|:---:|:---:|:---:|
| `LR11XX_DRIVER_DISABLE_FLAGS_FILE` | `defined` / `undefined` | Disable the `lr11xx_driver_flags.h` header file inclusion when compilation flags are given in the project settings or by command line. |
| `LR11XX_DRIVER_DISABLE` | `defined` / `undefined` | Disable the LR11XX driver. |
| `LR11XX_DRIVER_GPIO_ERROR_BASE_LAST` | `<value>` | Last error base of the low level GPIO driver. |
| `LR11XX_DRIVER_SPI_ERROR_BASE_LAST` | `<value>` | Last error base of the low level SPI driver. |
| `LR11XX_DRIVER_DELAY_ERROR_BASE_LAST` | `<value>` | Last error base of the low level delay driver. |
| `LR11XX_DRIVER_TX_ENABLE` | `defined` / `undefined` | Enable radio transmission functions. |
| `LR11XX_DRIVER_RX_ENABLE` | `defined` / `undefined` | Enable radio reception functions. |
| `LR11XX_DRIVER_WIFI_ENABLE` | `defined` / `undefined` | Enable passive WiFi scanning. |

# Build

A static library can be compiled by command line with `cmake`.

```bash
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE="<toolchain_file_path>" \
      -DTOOLCHAIN_PATH="<arm-none-eabi-gcc_path>" \
      -DTYPES_PATH="<types_file_path>" \
      -DEMBEDDED_UTILS_PATH="<embedded-utils_path>" \
      -DLR11XX_DRIVER_GPIO_ERROR_BASE_LAST=0 \
      -DLR11XX_DRIVER_SPI_ERROR_BASE_LAST=0 \
      -DLR11XX_DRIVER_DELAY_ERROR_BASE_LAST=0 \
      -DLR11XX_DRIVER_TX_ENABLE=ON \
      -DLR11XX_DRIVER_RX_ENABLE=ON \
      -DLR11XX_DRIVER_WIFI_ENABLE=ON \
      -G "Unix Makefiles" ..
make all
```
