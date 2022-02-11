/**
 * @file 	 resPhoto.c
 * @brief 	 Librairie pour le capteur de luminosite qui comprend les fonctions
 * @author 	 Thomas Desrosiers et Matis Goulet
 * @version  1.0
 * @date 	 2022/02/11
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "resPhoto.h"
#include <math.h>

uint16_t _lux10bits = 0;
float _resistance = 0;
uint16_t _valLux = 0;

/**
 * @brief         Fonction qui permet au main.c de calculer les lux pour le backlight et le set.
 * @param valLux  Valeur mesurée en lux.
 */
void lux10bits(uint16_t valLux)
{
	_lux10bits = (int)(valLux / 5); // Valeur en LUXs calculée est divisé par 5 = (3000lux / 640) afin d'obtenir une valeur de 0-640. 3000LUXs à été choisi comme valeur puisque c'est la valeur maximale mesurée en intérieur.
	if (_lux10bits < 1)
		_lux10bits = 1;		// La valeur minimale de luxBackLight est de 1.
	TC4H = (_lux10bits >> 8); // Encore une fois, on utilise le mode 10bits puisque la valeur peut monter en haut de 255 jusqu'à 640.
	OCR4D = _lux10bits;
}

/**
 * @brief         Fonction qui est utilisée afin de convertir la valeur mesurée de l'adc en lux.
 * @param adcVal  Valeur mesurée par l'adc.
 * @return        Luminosité calculée en lux.
 */
uint16_t luxCalculator(uint16_t adcVal)
{
	_resistance = ((1024 - adcVal) * 1000.0) / adcVal;	// Calcul de notre valeur de résistance.
	_valLux = (int)(pow(_resistance, PENTE) * DIX_POW_B); // Calcul de la valeur des LUXs avec la fonction pow de math.h. Les valeurs -1.4 et 11935573.53318 ont étés obtenus en faisant plusieurs mesures et du fichier excel.
	return _valLux;
}