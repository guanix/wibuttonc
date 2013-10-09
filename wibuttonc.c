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

#include "nrf24.h"

uint8_t my_address[5] = {0xfa, 0xfa, 0xfa, 0xfa, 0xfa};
uint8_t other_address[5] = {0xaf, 0xaf, 0xaf, 0xaf, 0xaf};
	
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
	
	blinkleft(1);
	blinkright(1);
	
	nrf24_init();
			
	nrf24_config(2, 1);
	nrf24_tx_address(other_address);
	nrf24_rx_address(my_address);
	
	uint8_t left_btn = 0, right_btn = 0;
	uint8_t left_led = 0, right_led = 0;
	
	nrf24_powerUpRx();
	
	for (;;) {
		// debouncing!
		if (!left_btn && (BTN_PORT.IN & _BV(LEFT_BTN)) == 0) {
			_delay_ms(100);
			if ((BTN_PORT.IN & _BV(LEFT_BTN)) == 0) {
				left_btn = 1;
//				left(1);
				blinkleft(1);
				transmit('L');
			}
		}
		
		if (!right_btn && (BTN_PORT.IN & _BV(RIGHT_BTN)) == 0) {
			_delay_ms(100);
			if ((BTN_PORT.IN & _BV(RIGHT_BTN)) == 0) {
				right_btn = 1;
//				right(1);
				blinkright(1);
				transmit('R');
			}
		}
		
		if (left_btn && (BTN_PORT.IN & _BV(LEFT_BTN))) {
			_delay_ms(100);
			if (BTN_PORT.IN & _BV(LEFT_BTN)) {
				left_btn = 0;
//				left(0);
				blinkleft(1);
				transmit('l');
			}
		}
		
		if (right_btn && (BTN_PORT.IN & _BV(RIGHT_BTN))) {
			_delay_ms(100);
			if (BTN_PORT.IN & _BV(RIGHT_BTN)) {
				right_btn = 0;
//				right(0);
				blinkright(1);
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
					left(1);
					break;
				case 'R':
					right_led = 1;
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
	
		_delay_ms(10);
	}
}
