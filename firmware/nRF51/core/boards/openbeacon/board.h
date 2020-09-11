/***************************************************************
 *
 * OpenBeacon.org - OpenBeacon board definition
 *
 * Copyright 2020 Alex Badea <vamposdecampos@gmail.com>
 *
 ***************************************************************

 This file is part of the OpenBeacon.org active RFID firmware

 OpenBeacon is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 OpenBeacon is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

*/
#ifndef __BOARD_H__
#define __BOARD_H__

/** Board has a 32 kHz crystal for the low-frequency clock. */
#define BOARD_HAVE_LF_XTAL		1

/** Board has an SPI flash chip. */
#define BOARD_HAVE_SPI_FLASH		1

/** Board has an SPI accelerometer. */
#define BOARD_HAVE_SPI_ACCEL		1


/** Pin definitions */
/* TODO: rename these CONFIG_* to BOARD_* */
#define CONFIG_UART_TXD_PIN  9
#ifdef  CONFIG_UART_RX
#define CONFIG_UART_RXD_PIN  8
#else
#define CONFIG_GPIO3_PIN     8
#endif /* CONFIG_UART_RX */

#define CONFIG_FLASH_MISO    16
#define CONFIG_FLASH_MOSI    15
#define CONFIG_FLASH_SCK     14
#define CONFIG_FLASH_nRESET  13
#define CONFIG_FLASH_nCS     12

#define CONFIG_ADC0          1
#define CONFIG_ADC1          2

#define CONFIG_ACC_INT1_CH   0
#define CONFIG_ACC_INT1      3
#define CONFIG_ACC_nCS       4
#define CONFIG_ACC_MISO      5
#define CONFIG_ACC_MOSI      6
#define CONFIG_ACC_SCK       7

#define CONFIG_LED_PIN       17
#define CONFIG_SWITCH_PIN    29

#endif /* __BOARD_H__ */
