/**
 * @file 	 adc.h
 * @brief 	 Librairie ADC qui comprend les fonctions d'initialisation et de transmission de la valeur lu vers le main (getter). La converssion se déclanche manuellement larsque l'on appel la fonction adcGetValue().
 * @author 	 Thomas Desrosiers
 * @version  1.0
 * @date 	 2021/02/28
 */

#ifndef ADC_H_
#define ADC_H_

#define _NB_POINTS_MAX 400

/**
 * @brief  Fonction d'initialisation du ADC.
 */
void adcInit();

/**
 * @brief  Fonction qui permet au main.c d'acceder au tableau d'entiers correspondant au canal choisi en paramètre.
 * @param  canal prend la valeur de 0 ou de 1 tout dépendant du canal dont on souhaite connaitre la valeur.
 * @return uint8_t* retourne le tableau correspondant au canal choisi en paramètre.
 */
uint16_t adcGetValue(uint8_t canal);

#endif /* ADC_H_ */