/**
@file 		i2c.h
@brief 		Librairie I2C qui prmet l'utilisation complète de la lecture et de l'écriture à un module I²C en interruption.
@author 	Thomas Desrosiers
@version 	1.0
@date 		2021/03/07
*/

#ifndef I2C_H_
#define I2C_H_

/**
*@brief  Fonction qui réinitialise la position du conteur de position du buffer.
*/
void i2cClearDataAvailable();

/**
*@brief  Fonction qui retourne le nombre d'octet dans le buffer.
*@return uint8_t nombre d'octet présent dans le buffer.
*/
uint8_t i2cDataAvailable();

/**
*@brief  Fonction qui permet de récupérer les octet dans le buffer.
*@return uint8_t octet contenu dans le buffer.
*/
uint8_t i2cGetData();

/**
*@brief  Fonction qui retourne un code d'ereur en cas de divers erreurs.
*@return uint8_t 0 si tout se passe bien, 1 si on ne reçoit pas d'acknowledge après avoir transmis une adresse de lecture, 2 si on ne reçoit pas d'acknowledge après avoir transmis une adresse de slave et une donnée et 3 si rien ne se passe .
*/
uint8_t i2cGetErrorCode();

/**
*@brief  Fonction d'initialisation du I²C à une fréquence voulu.
*@param  frequence fréquence fréquence voulu.
*@param  fcpu fréquence du CPU.
*/
void i2cInit(uint32_t frequence, uint32_t fcpu);

/**
*@brief  Fonction qui retourne l'état de la variable _i2cBusy qui indique si le I²C est occupé.
*@return uint8_t 0 si le I²C est libre et 1 si il est occupé.
*/
uint8_t i2cIsBusy();

/**
*@brief  Fonction qui lis l'état des I/O du slave à l'adresse spécifié.
*@param  add adresse du slave duquel on souhaite lire l'état des I/O.
*/
void i2cReadByte(uint8_t add);

/**
*@brief  Fonction qui lis l'état des I/O du slave à l'adresse spécifié sur plusieurs octets.
*@param  add adresse du module I²C.
*@param  size nombre d'octet à lire.
*/
void i2cReadBytes(uint8_t add, uint8_t size);

/**
*@brief  Fonction qui envoie un octet seulement.
*@param  add adresse du module I²C.
*@param  data octet à envoyer.
*/
void i2cWriteByte(uint8_t add, uint8_t data);

/**
*@brief  Fonction qui envoie plusieurs octet depuis un tableau d'octet.
*@param  add adresse du module I²C.
*@param  source pointeur vers le tableau de donnés à envoyer.
*@param  size nombre d'éléments du tableau qui seront transmis.
*@return uint8_t nombre d'octet envoyé.
*/
uint8_t i2cWriteBytes(uint8_t add, uint8_t *source, uint8_t size);

#endif /* I2C_H_ */