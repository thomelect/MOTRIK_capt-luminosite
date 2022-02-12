/**
@file 		photoRes.c
@brief 		Librairie pour le capteur de luminosite qui comprend les fonctions
@author 	Thomas Desrosiers et Matis Goulet
@version 	1.0
@date 		2022/03/2
*/

#ifndef RESPHOTO_H_
#define RESPHOTO_H_

#define PENTE -1.4

#define DIX_POW_B 11935573.53318

/**
*@brief  Fonction qui permet au main.c de calculer les lux pour le backlight et le set.
*@param  lesLux valeur de lux mesurer.
*@return void
*/
void backLight(uint16_t lesLux);

/**
*@brief  Fonction qui permet au main.c de convertir la valeur mesure de l'adc en nombre de lux.
*@param  adcLux valeur mesurer par l'adc .
*@return uint16_t valeur de lux calculer.
*/
uint16_t luxCalculator(uint16_t adcLux);

#endif /* RESPHOTO_H_ */