#include <openbeacon.h>
#include <adc.h>
#include <temp.h>

static BOOL g_debug = false;

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

void radio_start_hook(void)
{
}

void radio_hfclk_hook(void)
{
}

void adc_done_hook(void)
{
}

void radio_advertise_hook(void)
{
	/* 8:8 fixed point format */
	int32_t temp = ((int32_t) NRF_TEMP->TEMP) << 6;

	g_beacon_pkt[sizeof(g_beacon_pkt)-4] = temp & 0xff;
	g_beacon_pkt[sizeof(g_beacon_pkt)-3] = (temp >> 8) & 0xff;
	g_beacon_pkt[sizeof(g_beacon_pkt)-2] = adc_bat();
	g_beacon_pkt[sizeof(g_beacon_pkt)-1] = adc_ain();
	if (g_debug)
		debug_printf("ain %d\r\n", adc_ain());
}

static inline void led_on(void)
{
	pin_clear(CONFIG_LED_PIN);
}

static inline void led_off(void)
{
	pin_set(CONFIG_LED_PIN);
}

void entry(void)
{
	nrf_gpio_cfg_input(CONFIG_ADC_PIN, GPIO_PIN_CNF_PULL_Disabled);
	nrf_gpio_cfg_input(CONFIG_TEST_PIN, GPIO_PIN_CNF_PULL_Pulldown);

	/* drive strength: standard 0, high 1 */
	led_off();
	nrf_gpio_cfg_output(CONFIG_LED_PIN);
	NRF_GPIO->PIN_CNF[CONFIG_LED_PIN] =
		(GPIO_PIN_CNF_DIR_Output       << GPIO_PIN_CNF_DIR_Pos) |
		(GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos) |
		(GPIO_PIN_CNF_DRIVE_S0S1       << GPIO_PIN_CNF_DRIVE_Pos);
	led_off();

	adc_init();
	adc_start();
	temp_init();
	temp_start(); // HFCLK not enabled, but mmkay.

	/* set advertisment packet */
	radio_advertise(&g_beacon_pkt, sizeof(g_beacon_pkt));
	/* run advertisement in background every 995ms */

	if (nrf_gpio_pin_read(CONFIG_TEST_PIN)) {
		led_on();
		timer_wait_ms(500);
		led_off();
		radio_interval_ms(995);
		g_debug = true;
	} else {
		led_on();
		timer_wait_ms(50);
		led_off();
		radio_interval_ms(9995);
		g_debug = false;
	}

	/* infinite foreground loop */
	while(TRUE) {
		led_on();
		timer_wait_ms(200);
		adc_start();
		temp_start();
		led_off();
		timer_wait_ms(g_debug ? 1500 : 10000);
	}
}
