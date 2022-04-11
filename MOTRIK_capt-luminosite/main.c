/**
 * @file 	  main.c
 * @brief 	  Partie du projet Motrik qui sera utilisé afin de mesure la luminosité ambiante en LUX avec une photorésistance PDV-7002.
 *
 * @author 	  Thomas Desrosiers
 * @version   2.0
 * @date 	  2022/02/11

 * @mainpage  MOTRIK_capt-luminosite
 * @author 	  Thomas Desrosiers
 * @section   MainSection1 Description
			  Partie du projet Motrik qui sera utilisé afin de mesure la luminosité ambiante en LUX avec une photorésistance PDV-7002.
 */

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <math.h>
#include "adc.h"
#include "resPhoto.h"
#include "usart.h"


/**********
 * DEFINE *
 **********/


/************
 * VARIABLE *
 ************/
volatile uint8_t cntCinqCentMs = 0;
volatile uint8_t cinqCentMSFlag = 0;
volatile uint8_t refreshMesure = 0;
char msgLux[17];
uint16_t adcValLux = 0;
uint16_t lux = 0;
uint16_t luxBackLight = 0;


/**************************
 * PROTOTYPES DE FONCTION *
 **************************/
/**
 * @brief  Fonction d'initialisation du timer 0 avec une période de 4ms.
 */
void timer1Init(void);

/**
 * @brief  Fonction d'initialision du timer 4 en mode PWM.
 */
void timer4Init(void);

/**
 *@brief  Fonction qui regroupe l'initialisation des différents I/O et des librairies.
 */
void miscInit(void);


/********
 * MAIN *
 ********/
int main(void)
{
	miscInit();
	while (1)
	{
		if (refreshMesure) // Flag qui est vrai à chaque quatre ms.
		{
			refreshMesure = 0;
			adcValLux = adcGetValue(0); // Lecture du canal 0 du ADC.
		}
		if (cinqCentMSFlag) // Flag qui est vrai à chaque cinq cent ms.
		{
			cinqCentMSFlag = 0;
			lux10bits(lux);
			sprintf(msgLux, "Luminosite:%0d \n\r", lux); // Conversion de la mesure de LUXs en string.
			usartSendString(msgLux);
		}
		lux = luxCalculator(adcValLux);
	}
}


/****************
 * INTERRUPTION *
 ****************/
/**
 * @brief  Le timer 1 est initialisé à 4ms. à chaques 4ms, refresh mesure est HAUT et après 500ms(125 x 4ms) cinqCentMSFlag est HAUT.
 */
ISR(TIMER1_COMPA_vect)
{
	refreshMesure = 1; //À chaque 4ms. Ce flag sera utilisé pour faire une nouvelle mesure d'ADC.
	cntCinqCentMs++;
	if (cntCinqCentMs >= 125)
	{
		cntCinqCentMs -= 125;
		cinqCentMSFlag = 1; //À chaque 500ms. Ce flag sera utilisé pour changer le backlight et l'affichage des valeurs des capteurs.
	}
}


/************************
 * DÉFINITION FONCTIONS *
 ************************/
void miscInit(void)
{
	timer1Init(); // Initialisation des timers
	timer4Init();
	adcInit(); // Initialisation du adc.
	usartInit(1000000, F_CPU);
}

void timer1Init(void)
{
	// TCCR1A : COM1A1 COM1A0 COM1B1 COM1B0 COM1C1 COM1C0 WGM11 WGM10
	// TCCR1B: ICNC1 ICES1 – WGM13 WGM12 CS12 CS11 CS10
	// TIMSK1: – – ICIE1 – OCIE1C OCIE1B OCIE1A TOIE1
	TCCR1B = (1 << WGM12);	 // mode CTC.
	TCCR1B |= (1 << CS12);	 // Prescaler de 256.
	TIMSK1 |= (1 << OCIE1A); // Output Compare A Match Interrupt Enable
	OCR1A = 250 - 1;		 // 62.5ns * 256 * 250 * 125 = 500ms
	sei();
}

void timer4Init(void)
{
	// TCCR4A: COM4A1 COM4A0 COM4B1 COM4B0 FOC4A FOC4B PWM4A PWM4B
	// TCCR4B: PWM4X PSR4 DTPS41 DTPS40 CS43 CS42 CS41 CS40
	// TCCR4C: COM4A1S COM4A0S COM4B1S COMAB0S COM4D1 COM4D0 FOC4D PWM4D
	// TCCR4D: FPIE4 FPEN4 FPNC4 FPES4 FPAC4 FPF4 WGM41 WGM40
	// TCCR4E: TLOCK4 ENHC4 OC4OE5 OC4OE4 OC4OE3 OC4OE2 OC4OE1 OC4OE0

	DDRD |= (1 << 7);		 // Initialisation de la pin PD7 en sortie. Cette pin est celle de OC4D.
	uint16_t top = 640 - 1;	 // Valeur TOP de comparaison pour le timer 4.
	uint16_t topD = 640 - 1; // Valeur TOP de comparaison pour le port D du timer 4.
	TCCR4B = 0b00000001;	 // Prescaler de /1
	TCCR4C = 0b00001001;	 // PWM OC4D
	TC4H = (top >> 8);		 // Utilisation des 2bits de TC4H affin de fonctionner en mode 10bits.
	OCR4C = top;
	TC4H = (topD >> 8); // Utilisation des 2bits de TC4H affin de fonctionner en mode 10bits.
	OCR4D = topD;
}