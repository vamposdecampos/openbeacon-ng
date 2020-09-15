/***************************************************************
 *
 * OpenBeacon.org - nRF51 2.4GHz Radio Routines
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
#include <main.h>
#include <radio.h>
#include <adc.h>
#include <timer.h>

/* don't start DC/DC converter for voltages below 2.3V */
#define NRF_DCDC_STARTUP_VOLTAGE 23

#define BLE_ADDRESS 0x8E89BED6UL
#define BLE_PREFIX_SIZE 9
#define BLE_POSTFIX (BLE_PREFIX_SIZE+2)
#define BLE_PAYLOAD_LEN 27

typedef struct {
	uint8_t channel;
	uint8_t frequency;
} TMapping;

static int g_advertisment_index;
static uint32_t g_uid;
static uint32_t g_sequence_counter;

static const uint8_t* g_beacon_pkt;
static uint8_t g_beacon_pkt_len;
static uint32_t g_beacon_pkt_interval;

static uint8_t g_pkt_buffer[64];

static const TMapping g_advertisment[] = {
	{37,  2},
	{38, 26},
	{39, 80},
};
#define ADVERTISMENT_CHANNELS ((int)(sizeof(g_advertisment)/sizeof(g_advertisment[0])))

void RTC0_IRQ_Handler(void)
{
#if BOARD_HAVE_RADIO_DCDC
	/* run every second */
	if(NRF_RTC0->EVENTS_COMPARE[0])
	{
		/* acknowledge event */
		NRF_RTC0->EVENTS_COMPARE[0] = 0;

		/* re-trigger timer */
		NRF_RTC0->CC[0]+= MILLISECONDS(1000);

		/* start ADC conversion */
		adc_start();
	}
#endif

	/* run every g_beacon_pkt_interval */
	if(NRF_RTC0->EVENTS_COMPARE[1])
	{
		/* increment sequence counter once per second */
		g_sequence_counter++;

		/* acknowledge event */
		NRF_RTC0->EVENTS_COMPARE[1] = 0;

		/* re-trigger timer */
		NRF_RTC0->CC[1]+= g_beacon_pkt_interval;

		g_advertisment_index = 0;

		/* start HF crystal oscillator */
		NRF_CLOCK->TASKS_HFCLKSTART = 1;

#if BOARD_HAVE_RADIO_DCDC
		/* only start DC/DC converter for
		 * RX & higher battery voltages */
		if(adc_bat()>=NRF_DCDC_STARTUP_VOLTAGE)
		{
			/* start DC-DC converter */
			NRF_POWER->DCDCEN = (
				(POWER_DCDCEN_DCDCEN_Enabled << POWER_DCDCEN_DCDCEN_Pos)
			);
		}
#endif
	}
}

static void radio_send_advertisment(void)
{
	const TMapping *map;

	/* transmit beacon on all advertisment channels */
	map = &g_advertisment[g_advertisment_index];

	/* switch frequency & whitening */
	NRF_RADIO->FREQUENCY = map->frequency;
	NRF_RADIO->DATAWHITEIV = map->channel;

	/* BLE header */
	g_pkt_buffer[0] = 0x42;

	/* add MAC address */
	g_pkt_buffer[2] = (uint8_t)(g_uid>>24);
	g_pkt_buffer[3] = (uint8_t)(g_uid>>16);
	g_pkt_buffer[4] = (uint8_t)(g_uid>> 8);
	g_pkt_buffer[5] = (uint8_t)(g_uid>> 0);
	g_pkt_buffer[6] = 0;
	g_pkt_buffer[7] = 0;

	/* No BT/EDR - Tx only */
	g_pkt_buffer[8] = 2;
	g_pkt_buffer[9] = 0x01;
	g_pkt_buffer[10]= 0x04;

	radio_advertise_hook();

	/* append beacon packet */
	g_pkt_buffer[1]= BLE_PREFIX_SIZE+g_beacon_pkt_len;
	if(g_beacon_pkt_len)
		memcpy(
			&g_pkt_buffer[BLE_POSTFIX],
			g_beacon_pkt,
			g_beacon_pkt_len
		);

	/* set packet pointer */
	NRF_RADIO->PACKETPTR = (uint32_t)&g_pkt_buffer;

	NRF_RADIO->EVENTS_END = 0;
	/* start tracker TX */
	NRF_RADIO->TASKS_TXEN = 1;
}

void POWER_CLOCK_IRQ_Handler(void)
{
	/* always transmit proximity packet */
	if(NRF_CLOCK->EVENTS_HFCLKSTARTED)
	{
		/* acknowledge event */
		NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;

		radio_hfclk_hook();

		/* start advertising */
		radio_send_advertisment();
	}
}

void RADIO_IRQ_Handler(void)
{
	/* transmitted packet */
	if(NRF_RADIO->EVENTS_DISABLED)
	{
		/* acknowledge event */
		NRF_RADIO->EVENTS_DISABLED = 0;

		/* switch to next channel */
		g_advertisment_index++;
		if(g_advertisment_index<ADVERTISMENT_CHANNELS)
			radio_send_advertisment();
		else
		{
			g_advertisment_index = 0;
			/* stop HF clock */
			NRF_CLOCK->TASKS_HFCLKSTOP = 1;
			/* disable DC-DC converter */
			NRF_POWER->DCDCEN = 0;
		}
	}

	/* received packet */
	if(NRF_RADIO->EVENTS_END)
	{
		/* acknowledge event */
		NRF_RADIO->EVENTS_END = 0;
	}
}

void radio_interval(uint32_t milliseconds)
{
	if(!milliseconds)
		g_beacon_pkt_interval = 0;
	else
		g_beacon_pkt_interval = (milliseconds<50) ? 50 : milliseconds;
}

int radio_advertise(const void* packet, uint32_t len)
{
	/* filter long packets arguments */
	if(len>BLE_PAYLOAD_LEN)
		len = BLE_PAYLOAD_LEN;

	/* ensure safe update by disabling IRQs */
	__disable_irq();
	{
		/* remember settings */
		g_beacon_pkt = len ? (uint8_t*) packet : NULL;
		g_beacon_pkt_len = len;
	}
	__enable_irq();

	return len;
}

void radio_enable(BOOL enable)
{
	if(enable)
		NRF_RTC0->TASKS_START = 1;
	else
		NRF_RTC0->TASKS_STOP = 1;
}

void radio_init(uint32_t uid)
{
	/* remember uid */
	g_uid = uid;

	/* reset default advertisment packet */
	g_beacon_pkt = NULL;
	g_beacon_pkt_len = 0;
	g_beacon_pkt_interval = MILLISECONDS(1000);

	/* reset sequence counter */
	g_sequence_counter = 0;

	/* initialize ADC battery voltage measurements */
	adc_init();

	/* setup default radio settings for proximity mode */
	NRF_RADIO->MODE = RADIO_MODE_MODE_Ble_1Mbit << RADIO_MODE_MODE_Pos;
	NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Pos4dBm;
	NRF_RADIO->TXADDRESS = 0;
	NRF_RADIO->PREFIX0 = ((BLE_ADDRESS>>24) & RADIO_PREFIX0_AP0_Msk);
	NRF_RADIO->BASE0 = (BLE_ADDRESS<<8);
	NRF_RADIO->RXADDRESSES = 0;
	NRF_RADIO->PCNF0 =
		(1 << RADIO_PCNF0_S0LEN_Pos)|
		(8 << RADIO_PCNF0_LFLEN_Pos);
	NRF_RADIO->PCNF1 =
		(RADIO_PCNF1_WHITEEN_Enabled << RADIO_PCNF1_WHITEEN_Pos)|
		(0xFF                        << RADIO_PCNF1_MAXLEN_Pos)|
		(3 << RADIO_PCNF1_BALEN_Pos);
	NRF_RADIO->CRCCNF =
		(RADIO_CRCCNF_LEN_Three << RADIO_CRCCNF_LEN_Pos) |
		(1 << RADIO_CRCCNF_SKIP_ADDR_Pos);
	NRF_RADIO->CRCINIT = 0x00555555UL;
	NRF_RADIO->CRCPOLY = 0x0100065BUL;
	NRF_RADIO->SHORTS = (
		(RADIO_SHORTS_READY_START_Enabled       << RADIO_SHORTS_READY_START_Pos) |
		(RADIO_SHORTS_END_DISABLE_Enabled       << RADIO_SHORTS_END_DISABLE_Pos)
	);
	NRF_RADIO->INTENSET = (
		(RADIO_INTENSET_DISABLED_Enabled        << RADIO_INTENSET_DISABLED_Pos) |
		(RADIO_INTENSET_END_Enabled             << RADIO_INTENSET_END_Pos)
	);
	NVIC_SetPriority(RADIO_IRQn, IRQ_PRIORITY_RADIO);
	NVIC_EnableIRQ(RADIO_IRQn);

	/* setup HF-clock IRQ */
	NRF_CLOCK->INTENSET = (
		(CLOCK_INTENSET_HFCLKSTARTED_Enabled << CLOCK_INTENSET_HFCLKSTARTED_Pos)
	);
	NVIC_SetPriority(POWER_CLOCK_IRQn, IRQ_PRIORITY_POWER_CLOCK);
	NVIC_EnableIRQ(POWER_CLOCK_IRQn);

	/* setup radio timer */
	NRF_RTC0->TASKS_STOP = 1;
	NRF_RTC0->COUNTER = 0;
	NRF_RTC0->PRESCALER = 0;
	NRF_RTC0->CC[0] = LF_FREQUENCY;
	NRF_RTC0->CC[1] = LF_FREQUENCY*2;
	NRF_RTC0->INTENSET = (
		(RTC_INTENSET_COMPARE0_Enabled   << RTC_INTENSET_COMPARE0_Pos) |
		(RTC_INTENSET_COMPARE1_Enabled   << RTC_INTENSET_COMPARE1_Pos)
	);
	NVIC_SetPriority(RTC0_IRQn, IRQ_PRIORITY_RTC0);
	NVIC_EnableIRQ(RTC0_IRQn);
	NRF_RTC0->TASKS_START = 1;
}
