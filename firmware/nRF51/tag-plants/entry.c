#include <openbeacon.h>
#include <adc.h>
#include <temp.h>

/* physical web beacon packet */
static uint8_t g_beacon_pkt[] = {
#if 0
	/* 0x03: Service List */
	 3,0x03, 0xD8, 0xFE,
	/* 0x16: Service Data - 'http://get.OpenBeacon.org' */
	21,0x16, 0xD8, 0xFE, 0x00, 0x20,
	   PROTO_HTTP,'g','e','t','.','O','p','e','n','B','e','a','c','o','n',DOT_ORG
#else
	/* name */
	   6, 0x08, 'P','l','a','n','t',
	/* telemetry */
	   8, 0xFF, 0x42, 0x42,
	      0x01,
	      0x00, 0x00, 0x00,
	      0xff

#endif
};

void radio_hfclk_hook(void)
{
	temp_start();
	adc_start();
}

void radio_advertise_hook(void)
{
	/* 8:8 fixed point format */
	int32_t temp = ((int32_t) NRF_TEMP->TEMP) << 6;

	g_beacon_pkt[sizeof(g_beacon_pkt)-4] = temp & 0xff;
	g_beacon_pkt[sizeof(g_beacon_pkt)-3] = (temp >> 8) & 0xff;
	g_beacon_pkt[sizeof(g_beacon_pkt)-2] = adc_bat();
	g_beacon_pkt[sizeof(g_beacon_pkt)-1] = adc_ain();
}

void entry(void)
{
	nrf_gpio_cfg_input(CONFIG_ADC_PIN, GPIO_PIN_CNF_PULL_Disabled);
	nrf_gpio_cfg_input(13, GPIO_PIN_CNF_PULL_Pulldown);

	adc_init();
	adc_start();
	temp_init();
	temp_start(); // HFCLK not enabled, but mmkay.

	/* set advertisment packet */
	radio_advertise(&g_beacon_pkt, sizeof(g_beacon_pkt));
	/* run advertisement in background every 995ms */

	if (nrf_gpio_pin_read(13)) {
		pin_set(CONFIG_LED_PIN);
		timer_wait_ms(500);
		radio_interval_ms(995);
	} else {
		radio_interval_ms(9995);
	}

	/* infinite foreground loop */
	while(TRUE)
	{
		/* blink once every five seconds */
		timer_wait_ms(5000);
		pin_set(CONFIG_LED_PIN);
		timer_wait_ms(1);
		pin_clear(CONFIG_LED_PIN);
	}
}
