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

/**
 * @brief         Fonction qui permet au main.c de calculer les lux pour le backlight et le set.
 * @param lesLux  Valeur mesurée en lux.
 */
void backLight(uint16_t lesLux);

/**
 * @brief         Fonction qui est utilisée afin de convertir la valeur mesurée de l'adc en lux.
 * @param adcLux  Valeur mesurée par l'adc.
 * @return        Luminosité calculée en lux.
 */
uint16_t luxCalculator(uint16_t adcLux);

#endif /* RESPHOTO_H_ */