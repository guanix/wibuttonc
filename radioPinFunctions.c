#include <avr/io.h>

#include "wibuttonc.h"

/* ------------------------------------------------------------------------- */
void nrf24_setupPins()
{
	MOSI_PORT.DIRSET = _BV(MOSI_PIN);
	SCK_PORT.DIRSET = _BV(SCK_PIN);
	CE_PORT.DIRSET = _BV(CE_PIN);
	MISO_PORT.DIRCLR = _BV(MISO_PIN);
	
	CSN_PORT.OUTSET = _BV(CSN_PIN);
	CSN_PORT.DIRSET = _BV(CSN_PIN);
}
/* ------------------------------------------------------------------------- */
void nrf24_ce_digitalWrite(uint8_t state)
{
	// CE is PE0
	if(state)
	{
		CE_PORT.OUTSET = _BV(CE_PIN);
	}
	else
	{
		CE_PORT.OUTCLR = _BV(CE_PIN);
	}
}
/* ------------------------------------------------------------------------- */
void nrf24_csn_digitalWrite(uint8_t state)
{
	// CSN is PD4
	if(state)
	{
		CSN_PORT.OUTSET = _BV(CSN_PIN);
	}
	else
	{
		CSN_PORT.OUTCLR = _BV(CSN_PIN);
	}
}
/* ------------------------------------------------------------------------- */
void nrf24_sck_digitalWrite(uint8_t state)
{
	// SCK is PD7
	if(state)
	{
		SCK_PORT.OUTSET = _BV(SCK_PIN);
	}
	else
	{
		SCK_PORT.OUTCLR = _BV(SCK_PIN);
	}
}
/* ------------------------------------------------------------------------- */
void nrf24_mosi_digitalWrite(uint8_t state)
{
	// MOSI is PD5
	if(state)
	{
		MOSI_PORT.OUTSET = _BV(MOSI_PIN);
	}
	else
	{
		MOSI_PORT.OUTCLR = _BV(MOSI_PIN);
	}
}
/* ------------------------------------------------------------------------- */
uint8_t nrf24_miso_digitalRead()
{
	// MISO is PD6
	return (MISO_PORT.IN & _BV(MISO_PIN));
}
/* ------------------------------------------------------------------------- */
