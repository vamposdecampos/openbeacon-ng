/***************************************************************
 *
 * OpenBeacon.org - OpenBeacon.org main include file
 *
 * Copyright 2013 Milosch Meriac <meriac@openbeacon.de>
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
#ifndef __OPENBEACON_H__
#define __OPENBEACON_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <nrf5.h>

#define PACKED __attribute__((packed))
#define WEAK __attribute__ ((weak))
#define ALIAS(f) __attribute__ ((weak, alias (#f)))
#define LINKTO(f) __attribute__ ((alias (#f)))
#define ALIGN4 __attribute__ ((aligned (4)))

typedef uint8_t BOOL;
#define TRUE 1
#define FALSE 0

#include <config.h>
#include <board.h>

/* this definition is linked weakly against uart_tx */
extern BOOL default_putchar (uint8_t data);

#ifdef  __nrf5__
#include <nrf.h>
#include <system_nrf51.h>
#else /*__nrf5__*/
#error Please specify architecture
#endif/*__nrf5__*/

#include <debug_printf.h>
#include <string.h>
#include <crc8.h>
#include <crc16.h>
#include <crc32.h>
#include <xxtea.h>
#include <uart.h>
#include <helper.h>

static inline uint16_t
htons (uint16_t x)
{
  __asm__ ("rev16 %0, %1": "=r" (x):"r" (x));
	return x;
}

static inline uint32_t
htonl (uint32_t x)
{
  __asm__ ("rev %0, %1": "=r" (x):"r" (x));
	return x;
}

#define ntohl(l) htonl(l)
#define ntohs(s) htons(s)

#define BIT_REVERSE(x) ((unsigned char)(__RBIT(x)>>24))

#ifndef CONFIG_LF_CLOCK_SRC
#error Please set CONFIG_LF_CLOCK_SRC
#endif

#if !BOARD_HAVE_LF_XTAL
#if CONFIG_LF_CLOCK_SRC == Xtal
#error Board does not have a 32 kHz crystal
#endif
#endif

#define ____CONFIG_LFCLKSRC_VAL(bitname) CLOCK_LFCLKSRC_SRC_ ## bitname
#define __CONFIG_LFCLKSRC_VAL(bitname) ____CONFIG_LFCLKSRC_VAL(bitname)
#define CONFIG_LFCLKSRC \
	(__CONFIG_LFCLKSRC_VAL(CONFIG_LF_CLOCK_SRC) << CLOCK_LFCLKSRC_SRC_Pos)

extern void main_entry(void);

#endif/*__OPENBEACON_H__*/
