/*
 * lamppwm.c
 *
 * Created: 1/21/2013 3:47:13 PM
 * Author: rmd, guan
 */ 

#define F_CPU 2000000

#include "wibuttonc.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/wdt.h> 
#include <avr/sfr_defs.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include "nrf24.h"

// address prefix is 161 36 127

uint8_t address[5] = { 0, 0x9b, 0x20, 0x83, 0x92 };

void left(uint8_t state)
{
	if (state) {
		PORTA.OUTCLR = _BV(5);
	} else {
		PORTA.OUTSET = _BV(5);
	}
}

void right(uint8_t state)
{
	if (state) {
		PORTB.OUTCLR = _BV(2);
	} else {
		PORTB.OUTSET = _BV(2);
	}
}

void blinkleft(uint8_t times)
{
	for (uint8_t i = 0; i < times; i++) {
		PORTA.OUTCLR = _BV(5);
		_delay_ms(150);
		PORTA.OUTSET = _BV(5);
		_delay_ms(150);
	}
}

void blinkright(uint8_t times)
{
	for (uint8_t i = 0; i < times; i++) {
		PORTB.OUTCLR = _BV(2);
		_delay_ms(150);
		PORTB.OUTSET = _BV(2);
		_delay_ms(150);
	}
}

uint8_t transmit(uint8_t data)
{
	nrf24_send(&data);
	while (nrf24_isSending());
	uint8_t status = nrf24_lastMessageStatus();
	nrf24_powerUpRx();
	_delay_ms(10);
	return status;
}
	
int main()
{
//    CLK.CTRL = 1;
	PORTA.OUTSET = _BV(5);
	PORTB.OUTSET = _BV(2);
    PORTA.DIRSET = _BV(5);
    PORTB.DIRSET = _BV(2);
	
	PORTA.PIN6CTRL = 0b011000;
	PORTA.PIN7CTRL = 0b011000;
	
	// Read two addresses from EEPROM
	eeprom_busy_wait();
	uint8_t status = eeprom_read_byte((const uint8_t *)0);
	if (status != 42) {
		blinkleft(4);
		blinkright(4);

		for (;;) ;
	}

	eeprom_busy_wait();
	uint8_t channel = eeprom_read_byte((const uint8_t *)1);

	eeprom_busy_wait();
	uint8_t group = eeprom_read_byte((const uint8_t *)2);

	eeprom_busy_wait();
	uint8_t end = eeprom_read_byte((const uint8_t *)3);

	address[0] = group<<4;

	if (end == 1) {
		address[0] |= 1;
	} else {
		address[0] |= 2;
	}

	nrf24_tx_address(address);

	if (end == 1) {
		address[0] |= 2;
	} else {
		address[0] |= 1;
	}

	nrf24_rx_address(address);

	nrf24_init();
			
	nrf24_config(channel, 1);

	uint8_t left_btn = 0, right_btn = 0;
	uint8_t left_led = 0, right_led = 0;
	
	nrf24_powerUpRx();

	int32_t counter = 0;
	int32_t last_left_led = 0;
	int32_t last_right_led = 0;
	int32_t last_left_btn = 0;
	int32_t last_right_btn = 0;

	blinkleft(channel);
	blinkright(group);
	blinkleft(end);

	volatile uint8_t status = nrf24_getStatus();
		
	for (;;) {
		// debouncing!
		if (!left_btn && (BTN_PORT.IN & _BV(LEFT_BTN)) == 0) {
			_delay_ms(100);
			if ((BTN_PORT.IN & _BV(LEFT_BTN)) == 0) {
				left_btn = 1;
				last_left_btn = counter;
				left(1);
				transmit('L');
			}
		}
		
		if (!right_btn && (BTN_PORT.IN & _BV(RIGHT_BTN)) == 0) {
			_delay_ms(100);
			if ((BTN_PORT.IN & _BV(RIGHT_BTN)) == 0) {
				right_btn = 1;
				right(1);
				last_right_btn = counter;
				transmit('R');
			}
		}
		
		if (left_btn && (BTN_PORT.IN & _BV(LEFT_BTN))) {
			_delay_ms(100);
			if (BTN_PORT.IN & _BV(LEFT_BTN)) {
				left_btn = 0;
//				left(0);
				transmit('l');
			}
		}
		
		if (right_btn && (BTN_PORT.IN & _BV(RIGHT_BTN))) {
			_delay_ms(100);
			if (BTN_PORT.IN & _BV(RIGHT_BTN)) {
				right_btn = 0;
//				right(0);
				transmit('r');
			}
		}
		
		if (left_led) {
			left(1);
		} else {
			left(0);
		}
		
		if (right_led) {
			right(1);
		} else {
			right(0);
		}

		if (nrf24_dataReady()) {
			uint8_t recv;
			nrf24_getData(&recv);
			switch (recv) {
				case 'L':
					left_led = 1;
					last_left_led = counter;
					left(1);
					break;
				case 'R':
					right_led = 1;
					last_right_led = counter;
					right(1);
					break;
				case 'l':
					left_led = 0;
					left(0);
					break;
				case 'r':
					right_led = 0;
					right(0);
					break;
			}
		}

		// if no message for 8 cycles, turn LED off
		if (left_led && counter - last_left_led >= 8) {
			left_led = 0;
			left(0);
		}

		if (right_led && counter - last_right_led >= 8) {
			right_led = 0;
			right(0);
		}

		// Every 4 cycles, retransmit message
		if (left_btn && counter - last_left_btn >= 4) {
			transmit('L');
		}

		if (right_btn && counter - last_right_btn >= 4) {
			transmit('R');
		}
	
		_delay_ms(25);

		counter++;
	}
}
