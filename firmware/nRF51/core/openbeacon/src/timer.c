/***************************************************************
 *
 * OpenBeacon.org - nRF51 Timer Routines
 *
 * Copyright 2013-2015 Milosch Meriac <meriac@openbeacon.org>
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
#include <openbeacon.h>
#include <timer.h>

static volatile uint8_t g_timer_wait;

void timer_wait(uint32_t ticks)
{
	g_timer_wait = TRUE;

	/* configure timeout */
	NRF_RTC1->CC[0] = ticks;
	/* start timer */
	NRF_RTC1->TASKS_START = 1;

	while(g_timer_wait)
		__WFE();
}

void RTC1_IRQ_Handler(void)
{
	/* allow wait loop to exit */
	g_timer_wait = FALSE;

	/* stop timer */
	NRF_RTC1->TASKS_STOP = 1;
	NRF_RTC1->TASKS_CLEAR = 1;
	/* acknowledge interrupt */
	NRF_RTC1->EVENTS_COMPARE[0] = 0;
}

void timer_init(void)
{
	/* start 32kHz clock source */
	debug_printf("start LFCLK...\r\n");
	NRF_CLOCK->LFCLKSRC = CONFIG_LFCLKSRC;
	NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
	NRF_CLOCK->TASKS_LFCLKSTART = 1;
	while(!NRF_CLOCK->EVENTS_LFCLKSTARTED)
		;

	if ((NRF_CLOCK->LFCLKSRC & CLOCK_LFCLKSRC_SRC_Msk) >> CLOCK_LFCLKSRC_SRC_Pos == CLOCK_LFCLKSRC_SRC_RC) {
		debug_printf("start HFCLK...\r\n");
		NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
		NRF_CLOCK->TASKS_HFCLKSTART = 1;
		while(!NRF_CLOCK->EVENTS_HFCLKSTARTED)
			;
		NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;

		debug_printf("calibrate LFCLK...\r\n");
		NRF_CLOCK->CTIV = 16; /* TODO: make configurable */
		NRF_CLOCK->EVENTS_CTTO = 0;
		NRF_CLOCK->EVENTS_DONE = 0;
		NRF_CLOCK->TASKS_CTSTART = 1;
		NRF_CLOCK->TASKS_CAL = 1;
		while (!NRF_CLOCK->EVENTS_DONE && !NRF_CLOCK->EVENTS_CTTO)
			;
		debug_printf("DONE=%u CTTO=%u\r\n",
			NRF_CLOCK->EVENTS_DONE,
			NRF_CLOCK->EVENTS_CTTO);
		NRF_CLOCK->TASKS_CTSTOP = 1;

		debug_printf("stop HFCLK...\r\n");
		NRF_CLOCK->TASKS_HFCLKSTOP = 1;
	}

	debug_printf("clock setup done.\r\n");

	/* setup delay routine */
	NRF_RTC1->COUNTER = 0;
	NRF_RTC1->PRESCALER = 0;
	NRF_RTC1->TASKS_STOP = 1;
	NRF_RTC1->INTENSET =
		(RTC_INTENSET_COMPARE0_Enabled << RTC_INTENSET_COMPARE0_Pos);

	NVIC_SetPriority(RTC1_IRQn, IRQ_PRIORITY_RTC1);
	NVIC_EnableIRQ(RTC1_IRQn);
}
