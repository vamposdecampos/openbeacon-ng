/***************************************************************
 *
 * OpenBeacon.org - nRF51 Main Entry
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
#include <openbeacon.h>

void blink(uint8_t times)
{
	while(times--)
	{
		nrf_gpio_pin_set(CONFIG_LED_PIN);
		timer_wait_ms(10);
		nrf_gpio_pin_clear(CONFIG_LED_PIN);
		timer_wait_ms(490);
	}
}

void halt(uint8_t times)
{
	while(TRUE)
	{
		blink(times);
		timer_wait(SECONDS(3));
	}
}

void main_entry(void)
{
	uint32_t tag_id;

	/* enabled LED output */
	nrf_gpio_cfg_output(CONFIG_LED_PIN);
	nrf_gpio_pin_set(CONFIG_LED_PIN);

	/* enabled input pin */
	nrf_gpio_cfg_input(CONFIG_SWITCH_PIN, NRF_GPIO_PIN_NOPULL);

	/* initialize UART */
	uart_init();

	/* start timer */
	timer_init();

	/* initialize flash */
	if(flash_init())
		halt(2);

	/* initialize accelerometer */
	if(acc_init())
		halt(3);

	/* calculate tag ID from NRF_FICR->DEVICEID */
	tag_id = crc32((void*)&NRF_FICR->DEVICEID, sizeof(NRF_FICR->DEVICEID));

	/* start radio */
	debug_printf("\n\rInitializing Tag[%08X] v" PROGRAM_VERSION "\n\r",
		tag_id);
	radio_init(tag_id);

	/* enter main loop */
	nrf_gpio_pin_clear(CONFIG_LED_PIN);
	entry();
}
