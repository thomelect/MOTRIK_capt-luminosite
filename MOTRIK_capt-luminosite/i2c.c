/**
@file 		i2c.c
@brief 		Librairie I2C qui prmet l'utilisation complète de la lecture et de l'écriture à un module I²C en interruption.
@author 	Thomas Desrosiers
@version 	1.0
@date 		2021/03/07
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "i2c.h"

#define  I2C_DATA_SIZE_MAX 128 //Taille du buffer.

#define TW_START		0x08 //Transmission d'une condition de départ.
#define TW_MR_SLA_ACK	0x40 //Envoie de l'adresse du slave pour la lecture envoyé, acknowledge reçu.
#define TW_MR_SLA_NACK	0x38 //Envoie de l'adresse du slave pour la lecture envoyé, aucun acknowledge reçu.
#define TW_MR_DATA_ACK	0x50 //Données reçues, acknowledge retourné.
#define TW_MR_DATA_NACK	0x58 //Données reçues, aucun acknowledge retourné.
#define TW_MT_SLA_ACK	0x18 //Envoie de l'adresse du slave pour l'écriture envoyé, acknowledge reçu.
#define TW_MT_SLA_NACK	0x20 //Envoie de l'adresse du slave pour l'écriture envoyé, aucun acknowledge reçu.
#define TW_MT_DATA_ACK	0x28 //Données envoyées, acknowledge reçu.
#define TW_MT_DATA_NACK	0x30 //Données envoyées, aucun acknowledge reçu.

uint8_t _i2cAdd = 0; //Adresse du module I²C.
uint8_t _i2cModeRead = 1; //Indique si le I²C effectu actuellement une lecture ou une écriture.
uint8_t _i2cData[I2C_DATA_SIZE_MAX]; //Buffer du I²C.
uint8_t _i2cRxData[I2C_DATA_SIZE_MAX]; //Buffer du I²C.
uint8_t _i2cDataSize = 0; //Nombre d'octet envoyé.
volatile uint8_t _i2cErrorCode = 0; //Variable contenant le code d'erreur.
volatile uint8_t _i2cDataCnt = 0; //Donnée disponique.
volatile uint8_t _i2cRxDataCnt = 0; //Donnée disponique.
volatile uint8_t _i2cBusy = 0; //Variable indiquand l'état actuel du I²C.

void i2cClearDataAvailable()
{
	_i2cRxDataCnt = 0; //Remise du compteur à 0.
}

uint8_t i2cDataAvailable()
{
	return _i2cRxDataCnt; //Retourne si une donnée est disponible ou non.
}

uint8_t i2cGetData()
{
	if (_i2cRxDataCnt)
		return _i2cRxData[(_i2cRxDataCnt--)-1]; //Retourne la valeur précédant la position actuelle de _i2cDataCnt. (vide le buffer)
	return 0;
}

uint8_t i2cGetErrorCode()
{
	return _i2cErrorCode; //Retourne Le code d'erreur.
}

void i2cInit(uint32_t frequence, uint32_t fcpu)
{
	//TWBR = TWBR7 TWBR6 TWBR5 TWBR4 TWBR3 TWBR2 TWBR1 TWBR0
	//TWSR = TWS7 TWS6 TWS5 TWS4 TWS3 – TWPS1 TWPS0
	TWSR = 0;
	TWBR = ((fcpu/frequence)-16)/2; //Calcul de la valeur de TWBR en fonction de la fréquence désirée.
	sei(); //Activation des interruptions.
}

uint8_t i2cIsBusy()
{
	return _i2cBusy; //Retourne l'état du I²C [Occupé / Disponible].
}

void i2cReadByte(uint8_t add)
{
	while (_i2cBusy || (TWCR & (1<<TWSTO))); //On s'assure que l'oppération précédente est terminé.
	
	_i2cErrorCode = 0; //Efface l'erreur sur nouvelle opération.
	_i2cRxDataCnt = 0; //Aucune données de disponible.
	_i2cDataSize = 1;
	_i2cAdd = add | 1; //On conserve l'adresse en mémoire + 1 pour l'adresse d'écriture.
	_i2cModeRead = 1; //Lecture.
	_i2cBusy = 1; //On est occupé.
	
	//TWCR = TWINT TWEA TWSTA TWSTO TWWC TWEN – TWIE
	TWCR |= (1<<TWINT) | (1<<TWSTA) | (1<<TWEN) | (1<<TWIE); //Le bit TWINT est effacé | Bit start | TWI activé | TWI interrupt enable.
}

void i2cReadBytes(uint8_t add, uint8_t size)
{
	while (_i2cBusy || (TWCR & (1<<TWSTO))); //On s'assure que l'oppération précédente est terminé.
	
	_i2cErrorCode = 0; //Efface l'erreur sur nouvelle opération.
	_i2cRxDataCnt = 0; //Aucune données de disponible.
	if (size > I2C_DATA_SIZE_MAX) //Contrôle du débordement de size.
		size = I2C_DATA_SIZE_MAX;
	_i2cDataSize = size;
	_i2cAdd = add | 1; //On conserve l'adresse en mémoire + 1 pour l'adresse d'écriture.
	_i2cModeRead = 1; //Lecture.
	_i2cBusy = 1; //On est occupé.
	
	//TWCR = TWINT TWEA TWSTA TWSTO TWWC TWEN – TWIE
	TWCR |= (1<<TWINT) | (1<<TWSTA) | (1<<TWEN) | (1<<TWIE); //Le bit TWINT est effacé | Bit start | TWI activé | TWI interrupt enable.
}

void i2cWriteByte(uint8_t add, uint8_t data)
{
	while (_i2cBusy || (TWCR & (1<<TWSTO))); //On s'assure que l'oppération précédente est terminé.
	
	_i2cErrorCode = 0; //Efface l'erreur sur nouvelle opération.
	_i2cDataCnt = 0; //Aucune données de disponible.
	_i2cAdd = add; //On conserve l'adresse en mémoire.
	_i2cModeRead = 0; //Écriture.
	_i2cData[0] = data; //on Conserve la donnée envoyée en mémoire.
	_i2cDataSize = 1;
	_i2cBusy = 1; //On est occupé.
	
	//TWCR = TWINT TWEA TWSTA TWSTO TWWC TWEN – TWIE
	TWCR |= (1<<TWINT) | (1<<TWSTA) | (1<<TWEN) | (1<<TWIE); //Le bit TWINT est effacé | Bit start | TWI activé | TWI interrupt enable.
}

uint8_t i2cWriteBytes(uint8_t add, uint8_t *source, uint8_t size)
{
	while (_i2cBusy || (TWCR & (1<<TWSTO))); //On s'assure que l'oppération précédente est terminé.
	
	_i2cErrorCode = 0; //Efface l'erreur sur nouvelle opération.
	_i2cDataCnt = 0; //Aucune données de disponible.
	_i2cAdd = add; //On conserve l'adresse en mémoire.
	_i2cModeRead = 0; //Écriture.
	if (size > I2C_DATA_SIZE_MAX) //Contrôle du débordement de size.
		size = I2C_DATA_SIZE_MAX;
	for (uint8_t i = 0; i < size; i++)
		_i2cData[i] = source[i]; //on Conserve la donnée envoyée en mémoire.
	_i2cDataSize = size;
	_i2cBusy = 1; //On est occupé.
	
	//TWCR = TWINT TWEA TWSTA TWSTO TWWC TWEN – TWIE
	TWCR |= (1<<TWINT) | (1<<TWSTA) | (1<<TWEN) | (1<<TWIE); //Le bit TWINT est effacé | Bit start | TWI activé | TWI interrupt enable.
	return _i2cDataCnt;
}

/**
*@brief  Interruption qui gère l'envoie et la réception de la communication I²C.
*/
ISR(TWI_vect)
{
	switch (TWSR & 0xF8)
	{
		case TW_START: //Transmission d'une condition de départ.
			TWDR = _i2cAdd;
			TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWIE);
			break;
		//////////////////////////////////////////////////////////////////////////
		case TW_MT_SLA_ACK: //Envoie de l'adresse du slave pour l'écriture envoyé, acknowledge reçu.
			TWDR = _i2cData[_i2cDataCnt++];
			TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWIE);
			break;
		//////////////////////////////////////////////////////////////////////////
		case TW_MT_DATA_ACK: //Données envoyées, acknowledge reçu.
		if (_i2cDataCnt >= _i2cDataSize)
			{
				TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
				_i2cBusy = 0; //On n'est pas occupé.
			}
			else
			{
				TWDR = _i2cData[_i2cDataCnt++];
				TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWIE);
			}
			break;
		//////////////////////////////////////////////////////////////////////////
		case TW_MR_SLA_ACK: //Envoie de l'adresse du slave pour la lecture envoyé, acknowledge reçu.
			//Pas de NACK pour dernier octet.
			if (_i2cDataSize > 1)
				TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWIE) | (1<<TWEA);
			else
				TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWIE);
			break;
		//////////////////////////////////////////////////////////////////////////
		case TW_MR_DATA_ACK: //Données reçues, acknowledge retourné.
			_i2cRxData[_i2cRxDataCnt++] = TWDR;
			if (_i2cRxDataCnt >= _i2cDataSize-1)
				TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWIE);
			else
				TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWIE) | (1<<TWEA);
			_i2cBusy = 0; //On n'est pas occupé.
			break;
		//////////////////////////////////////////////////////////////////////////
		case TW_MR_DATA_NACK: //Données reçues, aucun acknowledge retourné.
			_i2cRxData[_i2cRxDataCnt++] = TWDR;
			TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
			_i2cBusy = 0; //On n'est pas occupé.
			break;
		//////////////////////////////////////////////////////////////////////////
		case TW_MT_SLA_NACK: //Envoie de l'adresse du slave pour l'écriture envoyé, aucun acknowledge reçu.
		//////////////////////////////////////////////////////////////////////////
		case TW_MR_SLA_NACK: //Envoie de l'adresse du slave pour la lecture envoyé, aucun acknowledge reçu.
			_i2cErrorCode = 1;
			TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
			_i2cBusy = 0; //On n'est pas occupé.
			break;
		//////////////////////////////////////////////////////////////////////////
		case TW_MT_DATA_NACK: //Données envoyées, aucun acknowledge reçu.
			_i2cErrorCode = 2;
			TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
			_i2cBusy = 0; //On n'est pas occupé.
			break;
		//////////////////////////////////////////////////////////////////////////
		default: //rien ne se passe.
			_i2cErrorCode = 3;
			TWCR = (1<<TWINT) | (1<<TWSTO); //On enlève le enable afin de désactiver le module.
			_i2cBusy = 0; //On n'est pas occupé.
			break;
	}
}