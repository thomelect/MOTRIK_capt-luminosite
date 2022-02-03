/**
@file 		main.c
@brief 		Laboratoire #4 en Int�gration de syst�mes dans lequel on mesure la temp�rature avec le LM65, la luminosit� en LUX avec une photor�sistance PDV-7002. Ces valeurs sont affich�s sur l'afficheur LCD.
@author 	Thomas Desrosiers
@version 	1.0
@date 		2021/03/24

@mainpage 	lab_4_integration
@author 	Thomas Desrosiers
@section 	MainSection1 Description
			Laboratoire #4 en Int�gration de syst�mes dans lequel on mesure la temp�rature avec le LM65, la luminosit� en LUX avec une photor�sistance PDV-7002. Ces valeurs sont affich�s sur l'afficheur LCD.
*/

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <math.h>
#include "adc.h"
#include "resPhoto.h"
#include "lcdI2c.h"

volatile uint8_t cntCinqCentMs = 0;
volatile uint8_t cinqCentMSFlag = 0;
volatile uint8_t refreshMesure = 0;
char msgLux[17];
uint16_t adcValLux = 0;
uint16_t lux = 0;

/**
 *@brief  Fonction d'initialisation du timer 0 avec une p�riode de 4ms.
 */
void timer1Init();

/**
 *@brief  Fonction d'initialision du timer 4 en mode PWM.
 */
void timer4Init();

/**
 *@brief  Fonction d'initialision.
 */
void initAll();

int main(void)
{
	initAll();
	while (1)
	{
		if (refreshMesure) //Flag qui est vrai � chaque quatre ms.
		{
			refreshMesure = 0;
			adcValLux = adcGetValue(0); //Lecture du canal 0 du ADC.
		}
		if (cinqCentMSFlag) //Flag qui est vrai � chaque cinq cent ms.
		{
			cinqCentMSFlag = 0;
			
			backLight(lux);
			
			sprintf(msgLux, "Luminosite:%0d ", lux); //Conversion de la mesure de LUXs en string.
			lcdI2cEcrireChaineCursor(msgLux, 0, 0); //LUXs est affich� sur la 1�re ligne du LCD.
		}
		lux = luxCalculator(adcValLux);
	}
}

/**
*@brief  Le timer 1 est initialis� � 4ms. � chaques 4ms, refresh mesure est HAUT et apr�s 500ms(125 x 4ms) cinqCentMSFlag est HAUT.
*/
ISR(TIMER1_COMPA_vect)
{
	refreshMesure = 1; //� chaque 4ms. Ce flag sera utilis� pour faire une nouvelle mesure d'ADC.
	cntCinqCentMs++;
	if (cntCinqCentMs >= 125)
	{
		cntCinqCentMs -= 125;
		cinqCentMSFlag = 1; //� chaque 500ms. Ce flag sera utilis� pour changer le backlight et l'affichage des valeurs des capteurs.
	}
}

void timer4Init()
{
	//TCCR4A: COM4A1 COM4A0 COM4B1 COM4B0 FOC4A FOC4B PWM4A PWM4B
	//TCCR4B: PWM4X PSR4 DTPS41 DTPS40 CS43 CS42 CS41 CS40
	//TCCR4C: COM4A1S COM4A0S COM4B1S COMAB0S COM4D1 COM4D0 FOC4D PWM4D
	//TCCR4D: FPIE4 FPEN4 FPNC4 FPES4 FPAC4 FPF4 WGM41 WGM40
	//TCCR4E: TLOCK4 ENHC4 OC4OE5 OC4OE4 OC4OE3 OC4OE2 OC4OE1 OC4OE0
	
	DDRD |= (1<<7); //Initialisation de la pin PD7 en sortie. Cette pin est celle de OC4D.
	uint16_t top = 640-1; //Valeur TOP de comparaison pour le timer 4.
	uint16_t topD = 640-1; //Valeur TOP de comparaison pour le port D du timer 4.
	TCCR4B = 0b00000001; //Prescaler de /1
	TCCR4C = 0b00001001; //PWM OC4D
	TC4H = (top >> 8); //Utilisation des 2bits de TC4H affin de fonctionner en mode 10bits.
	OCR4C = top;
	TC4H = (topD >> 8); //Utilisation des 2bits de TC4H affin de fonctionner en mode 10bits.
	OCR4D = topD;
}

void timer1Init()
{
	//TCCR1A : COM1A1 COM1A0 COM1B1 COM1B0 COM1C1 COM1C0 WGM11 WGM10
	//TCCR1B: ICNC1 ICES1 � WGM13 WGM12 CS12 CS11 CS10
	//TIMSK1: � � ICIE1 � OCIE1C OCIE1B OCIE1A TOIE1
	TCCR1B = (1<<WGM12); //mode CTC.
	TCCR1B |= (1<<CS12); //Prescaler de 256.
	TIMSK1 |= (1<<OCIE1A); //Output Compare A Match Interrupt Enable
	OCR1A = 250-1; //62.5ns * 256 * 250 * 125 = 500ms
	sei();
}

/**
 *@brief  Fonction d'initialision.
 */
void initAll()
{
	timer1Init(); //Initialisation des timers
	timer4Init();
	adcInit(); //Initialisation du adc.
	lcdI2cInit(); //Initialisation du lcd en I2C.
}