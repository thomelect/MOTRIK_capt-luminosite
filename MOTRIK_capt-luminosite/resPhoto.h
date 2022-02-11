/**
 * @file 	 photoRes.c
 * @brief 	 Librairie pour le capteur de luminosite qui comprend les fonctions
 * @author 	 Thomas Desrosiers et Matis Goulet
 * @version  1.0
 * @date 	 2022/02/11
 */

#ifndef RESPHOTO_H_
#define RESPHOTO_H_

#define PENTE -1.4
#define DIX_POW_B 11935573.53318

#define _DEL_V_VALUE(val)	(OCR4B = val) //Valeur PWM output V.
#define _DEL_B_VALUE(val)	(OCR4D = val) //Valeur PWM output B.

/**
 * @brief         Fonction qui permet au main.c de calculer les LUXs pour le backlight et le set.
 * @param valLux  Valeur mesurée en LUXs.
 */
void lux10bits(uint16_t valLux);

/**
 * @brief         Fonction qui est utilisée afin de convertir la valeur mesurée de l'adc en LUXs.
 * @param adcVal  Valeur mesurée par l'adc.
 * @return        Luminosité calculée en LUXs.
 */
uint16_t luxCalculator(uint16_t adcLux);

#endif /* RESPHOTO_H_ */