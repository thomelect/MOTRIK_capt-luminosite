/**
@file 		resPhoto.c
@brief 		Librairie pour le capteur de luminosite qui comprend les fonctions
@author 	Thomas Desrosiers et Matis Goulet
@version 	1.0
@date 		2022/03/2
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "resPhoto.h"
#include <math.h>

uint16_t luxBackLight = 0;
float resistance = 0;
uint16_t mesLux = 0;

/**
*@brief  Fonction qui permet au main.c de calculer les lux pour le backlight et le set.
*@param  lesLux valeur de lux mesurer.
*@return void
*/
void backLight(uint16_t lesLux)
{
	luxBackLight = (int)(lesLux/5); //Valeur en LUXs calculée est divisé par 5 = (3000lux / 640) afin d'obtenir une valeur de 0-640. 3000LUXs à été choisi comme valeur puisque c'est la valeur maximale mesurée en intérieur.
	if (luxBackLight < 1)
	luxBackLight = 1; //La valeur minimale de luxBackLight est de 1.
	TC4H = (luxBackLight >> 8); //Encore une fois, on utilise le mode 10bits puisque la valeur peut monter en haut de 255 jusqu'à 640.
	OCR4D = luxBackLight;
}

/**
*@brief  Fonction qui permet au main.c de convertir la valeur mesure de l'adc en nombre de lux.
*@param  adcLux valeur mesurer par l'adc .
*@return uint16_t valeur de lux calculer.
*/
uint16_t luxCalculator(uint16_t adcLux)
{
	resistance = ((1024 - adcLux)*1000.0)/adcLux; //Calcul de notre valeur de résistance.
	mesLux = (int)(pow(resistance, PENTE) * DIX_POW_B); //Calcul de la valeur des LUXs avec la fonction pow de math.h. Les valeurs -1.4 et 11935573.53318 ont étés obtenus en faisant plusieurs mesures et du fichier excel.
	return mesLux;
}