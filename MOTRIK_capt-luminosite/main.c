/**
 * @file 	  main.c
 * @brief 	  Partie du projet Motrik qui sera utilisé afin de mesure la luminosité ambiante en LUX avec une photorésistance PDV-7002.
 *
 * @author 	  Thomas Desrosiers
 * @version   1.0
 * @date 	  2021/03/24

 * @mainpage  lab_4_integration
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

volatile uint8_t cntCinqCentMs = 0;
volatile uint8_t cinqCentMSFlag = 0;
volatile uint8_t refreshMesure = 0;
char msgLux[17];
uint16_t adcValLux = 0;
float resistance = 0;
uint16_t lux = 0;
uint16_t luxBackLight = 0;

// Prototypes des fonctions locales
/**
 * @brief  Fonction d'initialisation du timer 0 avec une période de 4ms.
 */
void timer1Init();

/**
 * @brief  Fonction d'initialision du timer 4 en mode PWM.
 */
void timer4Init();

/**
 *@brief  Fonction qui regroupe l'initialisation des différents I/O et des librairies.
 */
void miscInit(void);

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
			
			luxBackLight = (int)(lux / 5); // Valeur en LUXs calculée est divisé par 5 = (3000lux / 640) afin d'obtenir une valeur de 0-640. 3000LUXs à été choisi comme valeur puisque c'est la valeur maximale mesurée en intérieur.
			if (luxBackLight < 1)
				luxBackLight = 1;		// La valeur minimale de luxBackLight est de 1.
			TC4H = (luxBackLight >> 8); // Encore une fois, on utilise le mode 10bits puisque la valeur peut monter en haut de 255 jusqu'à 640.
			OCR4D = luxBackLight;

			sprintf(msgLux, "Luminosite:%0d ", lux); // Conversion de la mesure de LUXs en string.
		}
		resistance = ((1024 - adcValLux) * 1000.0) / adcValLux; // Calcul de notre valeur de résistance.
		lux = (int)(pow(resistance, -1.4) * 11935573.53318);	// Calcul de la valeur des LUXs avec la fonction pow de math.h. Les valeurs -1.4 et 11935573.53318 ont étés obtenus en faisant plusieurs mesures et du fichier excel.
	}
}

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

miscInit(void)
{
	timer1Init(); // Initialisation des timers
	timer4Init();
	adcInit(); // Initialisation du adc.
}

void timer4Init()
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

void timer1Init()
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