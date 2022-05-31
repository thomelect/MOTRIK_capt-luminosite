/**
 * @file 	 adc.c
 * @brief 	 Librairie ADC qui comprend les fonctions d'initialisation et de transmission de la valeur lu vers le main (getter). La conversion se déclanche manuellement lorsque l'on appel la fonction adcGetValue().
 * @author 	 Thomas Desrosiers
 * @version  1.0
 * @date 	 2021/02/28
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "adc.h"

uint16_t _adcVal[2];

void adcInit()
{
	// ADMUX = REFS1 REFS0 ADLAR MUX4 MUX3 MUX2 MUX1 MUX0
	ADMUX = (1 << REFS0);								   // Vcc Voltage Reference | ADC Right Adjust Result | Input ADC0.
	// ADCSRA = ADEN ADSC ADATE ADIF ADIE ADPS2 ADPS1 ADPS0
	ADCSRA = (1 << ADEN) | (1 << ADPS2) /* | (1<<ADPS1)*/; // ADC Enable | ADC Auto Trigger Enable | ADC Prescaler \16 | ADC Interrupt Enable.
	// ADCSRB = ADHSM ACME MUX5 – ADTS3 ADTS2 ADTS1 ADTS0
	ADCSRB = (1 << ADHSM);								   // ADC High Speed Mode.
	// DIDR0 = ADC7D ADC6D ADC5D ADC4D - - ADC1D ADC0D
	// DIDR2 = - - ADC13D ADC12D ADC11D ADC10D ADC9D ADC8D
	DIDR0 = 0b00000011; // entrée numérique désactivée pour le ADC0 / ADC1.
}

uint16_t adcGetValue(uint8_t canal)
{
	ADMUX &= ~1; // ADC0
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));	   // Ligne qui permet d'attendre qui la mesure soit terminée.
	_adcVal[0] = (ADCL | (ADCH << 8)); // Mesure de ADC0
	ADMUX |= 1;						   // ADC1
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));	   // Ligne qui permet d'attendre qui la mesure soit terminée.
	_adcVal[1] = (ADCL | (ADCH << 8)); // Mesure de ADC1
	return _adcVal[canal];			   // Retourne la valeur de ADC 0 ou 1 en fopnction de la valeur de canal.
}