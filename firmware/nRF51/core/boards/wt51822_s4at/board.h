/***************************************************************
 *
 * OpenBeacon.org - WT51822-S4AT board definition
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

/**
 * Board has a 32 kHz crystal for the low-frequency clock.
 */
#define BOARD_HAVE_LF_XTAL		0

/** Pin definitions */
#define CONFIG_UART_TXD_PIN  2
#define CONFIG_UART_RXD_PIN  1

/* These are not on the module, so they depend on external wiring. */
#ifndef CONFIG_LED_PIN
#define CONFIG_LED_PIN       13		/* pin9, NC */
#endif
#ifndef CONFIG_SWITCH_PIN
#define CONFIG_SWITCH_PIN    4		/* pin6, TEST */
#endif

#ifndef CONFIG_UART_FORCE_POWERED
#define CONFIG_UART_FORCE_POWERED	1
#endif

#endif /* __BOARD_H__ */
