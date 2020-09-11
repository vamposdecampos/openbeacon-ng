/***************************************************************
 *
 * OpenBeacon.org - nRF51 board config files
 *
 * Copyright 2013-2015 Milosch Meriac <milosch@meriac.com>
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
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define SOUND NRF_TIMER2
#define SOUND_IRQn TIMER2_IRQn
#define SOUND_IRQ_Handler TIMER2_IRQ_Handler

#define CONFIG_LED_PIN       17
#define CONFIG_SWITCH_PIN    29

#define CONFIG_PWM_PIN_A     1
#define CONFIG_PWM_PIN_B     2

#define CONFIG_LF_CLOCK_SRC  Xtal

/* only two priority bits available ! */

#define IRQ_PRIORITY_HIGH        0
#define IRQ_PRIORITY_SOUND       (IRQ_PRIORITY_HIGH)

#define IRQ_PRIORITY_LOW         (IRQ_PRIORITY_HIGH+3)
#define IRQ_PRIORITY_RTC0        (IRQ_PRIORITY_LOW)
#define IRQ_PRIORITY_RTC1        (IRQ_PRIORITY_LOW)

#endif/*__CONFIG_H__*/
