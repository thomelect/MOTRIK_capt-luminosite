/**
@file 		lcdI2c.h
@brief 		Libraire qui adapte la librairie LCD fait lors de la session précédente avec la librairie I²C permettant ainsi d'utiliser le LCD avec un IO expander I²C
@author 	Thomas Desrosiers
@version 	1.0
@date 		2021/03/11
*/

#ifndef LCDI2C_H_
#define LCDI2C_H_

#define _I2C_LCD_ADDR 0x70 //Adresse du IO expander.

/**
*@brief  Fonction qui regroupe plusieurs lignes de code communément utilisés par plusieurs fonctions. Elle est utilisé par lcdI2cEcrireCaractere, lcdI2cEcrireChaine, lcdI2cEcrireChaineCursor et lcdI2cSetCursor. Contrairement à _lcdI2cWrite8bits, cette fontion permet de tout envoyer d'un coup en plaçant les donnés à envoyer dans le buffer.
*@param  data donnée ou commande à envoyer.
*/
void _lcdI2cDataSend(uint8_t data);

/**
*@brief  Fonction qui envoie 4bits au LCD. Elle n'est seulement utilisée par la fonction _lcdI2cWrite8bits(uint8_t data, uint8_t rs), soit pendant l'initialisation.
*@param  data données transmises vers le LCD.
*@param  rs selection de l'état de rs 0 -> LOW | 1 -> HIGH.
*/
void _lcdI2cWrite4bits(uint8_t data, uint8_t rs);

/**
*@brief  Fonction qui utilise deux fois la fonction _lcdI2cWrite4bits afin d'envoyer 8bits au LCD. Elle est seulement utilisée par la fonction lcdI2cInit(), pour l'initialisation.
*@param  data données transmises vers le LCD.données transmises vers le LCD.
*@param  rs selection de l'état de rs 0 -> LOW | 1 -> HIGH.
*/
void _lcdI2cWrite8bits(uint8_t data, uint8_t rs);

/**
*@brief Fonction qui efface l'affichage du LCD.
*/
void lcdI2cClear();

/**
*@brief  Fonction qui sert a envoyer un caractère au LCD. et active le port RS pour indiquer au LCD que c'est un caractère.
*@param  c caractère de type char ä envoyer.
*/
void lcdI2cEcrireCaractere(char c);

/**
*@brief  Fonction qui écrit une chaine de caractère.
*@param  str chaine de caractère à écrire.
*/
void lcdI2cEcrireChaine(char *str);

/**
*@brief  Fonction qui combine lcdI2cSetCursor et lcdI2cEcrireChaine afin d'écrire le chaine str à la position spécifié en paramètre.
*@param  str chaine de caractère à écrire.
*@param  cursorPosiX position gauche/droite.
*@param  cursorPosiY position haut/bas.
*/
void lcdI2cEcrireChaineCursor(char *str, uint8_t cursorPosiX, uint8_t cursorPosiY);

/**
*@brief  Fonction qui initialise le LCD en I²C.
*/
void lcdI2cInit();


/**
*@brief  Fonction qui met les sortie 6 - 7 du PCF8574AN à HAUT pour ensuite lire leur état.
*@return uint8_t retourne un octet contenant le sur bit 0 et 1 l'état des boutons.
*/
uint8_t lcdI2cReadBouton();

/**
*@brief  Fonction qui indique la position du curseur en x et en y.
*@param  cursorPosiX position gauche/droite.
*@param  cursorPosiY position haut/bas.
*/
void lcdI2cSetCursor( uint8_t cursorPosiX, uint8_t cursorPosiY);

#endif /* LCDI2C_H_ */