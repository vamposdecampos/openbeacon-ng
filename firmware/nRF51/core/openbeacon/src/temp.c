/***************************************************************
 *
 * OpenBeacon.org - nRF51 Temperature routines
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
#include <openbeacon.h>
#include <main.h>
#include <radio.h>
#include <adc.h>
#include <timer.h>

static int32_t g_temp;

void TEMP_IRQ_Handler(void)
{
	if (NRF_TEMP->EVENTS_DATARDY) {
		NRF_TEMP->EVENTS_DATARDY = 0;

		/* hundeths of degrees */
		g_temp = ((int32_t)NRF_TEMP->TEMP) * 25;
	}
}

extern void temp_get(int32_t *temp_p)
{
	*temp_p = g_temp;
}

void temp_start(void)
{
	NRF_TEMP->TASKS_START = 1;
}

void temp_init(void)
{
	NRF_TEMP->INTENSET =
		(TEMP_INTENSET_DATARDY_Enabled << TEMP_INTENSET_DATARDY_Pos);
	NVIC_SetPriority(TEMP_IRQn, IRQ_PRIORITY_TEMP);
	NVIC_EnableIRQ(TEMP_IRQn);
}
