/**
@file 		lcdI2c.c
@brief 		Libraire qui adapte la librairie LCD fait lors de la session précédente avec la librairie I²C permettant ainsi d'utiliser le LCD avec un IO expander I²C
@author 	Thomas Desrosiers
@version 	1.0
@date 		2021/03/11
*/

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h> //Appel de la fonction delay utilisé dans l'init.
#include "i2c.h"
#include "lcdI2c.h"

uint8_t _lcdI2cData = 0xC0; //P6 & P7 à 1 pour faire la lecture des boutons plus tard.
uint8_t _etatBouton = 0;

#define  _LCDI2C_RS_H() _lcdI2cData |= (1<<4) //P4 du PCF8574AN.
#define  _LCDI2C_RS_L() _lcdI2cData &= ~(1<<4) //P4 du PCF8574AN.
#define  _LCDI2C_E_H() _lcdI2cData |= (1<<5) //P5 du PCF8574AN.
#define  _LCDI2C_E_L() _lcdI2cData &= ~(1<<5) //P5 du PCF8574AN.
#define  _LCDI2C_DATA(data) _lcdI2cData = ((_lcdI2cData & 0xF0) | data) //On met les 4bits les moins significatifs à 0 pour ensuite ajouter les 4bits de data.

#define _LCDI2C_BUFFERSIZE 128

uint8_t _lcdI2cBuffer[_LCDI2C_BUFFERSIZE];
uint8_t _lcdI2cBufferIndexIn = 0;

void _lcdI2cDataSend(uint8_t data)
{
	_LCDI2C_E_H(); //E est haut.
	_LCDI2C_DATA((data)>>4); //Bits décallés et placés dans le buffer 1/2.
	_lcdI2cBuffer[_lcdI2cBufferIndexIn++] = _lcdI2cData; //_lcdI2cData est placé dans le buffer.
	_LCDI2C_E_L(); //E est bas.
	_lcdI2cBuffer[_lcdI2cBufferIndexIn++] = _lcdI2cData; //Le changement d'état du enable est placé dans le buffer.
	_LCDI2C_E_H(); //E est haut.
	_LCDI2C_DATA(data); //Bits placés dans le buffer 2/2.
	_lcdI2cBuffer[_lcdI2cBufferIndexIn++] = _lcdI2cData; //_lcdI2cData est placé dans le buffer.
	_LCDI2C_E_L(); //E est bas.
	_lcdI2cBuffer[_lcdI2cBufferIndexIn++] = _lcdI2cData; //Le changement d'état du enable est placé dans le buffer.
}

void _lcdI2cWrite4bits(uint8_t data, uint8_t rs)
{
	if (rs)
	_LCDI2C_RS_H(); //Envoie de données alors RS HIGH.
	else
	_LCDI2C_RS_L(); //Envoie de commande alors RS LOW.
	_LCDI2C_E_H();
	_LCDI2C_DATA(data);
	i2cWriteByte(_I2C_LCD_ADDR, _lcdI2cData); //Puisque nous sommes
	_LCDI2C_E_L();
	i2cWriteByte(_I2C_LCD_ADDR, _lcdI2cData);
}

void _lcdI2cWrite8bits(uint8_t data, uint8_t rs)
{
	_lcdI2cWrite4bits(data>>4, rs);
	_lcdI2cWrite4bits(data, rs);
}

void lcdI2cClear()
{
	_lcdI2cWrite4bits(0,0);
	_lcdI2cWrite4bits(1,0);
	_delay_ms(2);
}

void lcdI2cEcrireCaractere(char c)
{
	_lcdI2cBufferIndexIn = 0;
	_LCDI2C_RS_H(); //Envoie de données alors RS HIGH.
	_lcdI2cDataSend(c);
	i2cWriteBytes(_I2C_LCD_ADDR, _lcdI2cBuffer, _lcdI2cBufferIndexIn);
}

void lcdI2cEcrireChaine(char *str)
{
	_lcdI2cBufferIndexIn = 0;
	_LCDI2C_RS_H(); //Envoie de données alors RS HIGH.
	while (*str && _lcdI2cBufferIndexIn < (_LCDI2C_BUFFERSIZE-4))
	{
		_lcdI2cDataSend(*str); //La string est envoyé au LCD.
		str++;
	}
	i2cWriteBytes(_I2C_LCD_ADDR, _lcdI2cBuffer, _lcdI2cBufferIndexIn); //Appel de la fonction i2cWriteBytes afin d'envoyer la chaine de caractère au LCD.
}

void lcdI2cEcrireChaineCursor(char *str, uint8_t cursorPosiX, uint8_t cursorPosiY)
{
	uint8_t commande = 0x80 | (cursorPosiX & 0xF); //Le paramètre de position x est inclu dans commande.
	if (cursorPosiY)
	commande |= 0x40; //Le paramètre de position y est inclu dans commande.
	_lcdI2cBufferIndexIn = 0;
	_LCDI2C_RS_L(); //Envoie de commande alors RS LOW.
	_lcdI2cDataSend(commande); //La commande est envoyé au LCD.
	_LCDI2C_RS_H(); //Envoie de données alors RS HIGH.
	while (*str && (_lcdI2cBufferIndexIn < (_LCDI2C_BUFFERSIZE-4))) //On s'assure de vider le buffer tout en s'assurant de ne pas faire déborder le compteur.
	{
		_lcdI2cDataSend(*str); //La string est envoyé au LCD.
		str++;
	}
	i2cWriteBytes(_I2C_LCD_ADDR, _lcdI2cBuffer, _lcdI2cBufferIndexIn); //Appel de la fonction i2cWriteBytes afin d'envoyer la chaine de caractère au LCD.
}

void lcdI2cInit()
{
	i2cInit(100000, F_CPU); //La fréquence est réglée À 100Kbps.
	_delay_ms(20);
	_lcdI2cWrite4bits(0x03, 0); //Function set - 8bits
	_delay_ms(5);
	_lcdI2cWrite4bits(0x03, 0); //Function set - 8bits
	_delay_ms(1);
	_lcdI2cWrite4bits(0x03, 0); //Function set - 8bits
	_delay_ms(1);
	_lcdI2cWrite4bits(0x02, 0); //Function set - 4bits
	_delay_ms(1);
	_lcdI2cWrite8bits(0x28, 0); //Function set - 4bits, 2ligne, 5x8
	_lcdI2cWrite8bits(0x08, 0); //Display on/off control - LCD à off.
	_lcdI2cWrite8bits(0x01, 0); //Clear display
	_delay_ms(2);
	_lcdI2cWrite8bits(0x06, 0); //Entry mode set - texte de gauche à droite.
	_lcdI2cWrite8bits(0x0C, 0); //Display on/off control - LCD on
	_delay_ms(5);
}

uint8_t lcdI2cReadBouton()
{
	i2cWriteByte(_I2C_LCD_ADDR, 0xC0); //Les pins  P7 et P6 du PCF8574AN sont mises à 1 de cette façon, l'orsqu'un bouton est appuyé le IO correspondant vaut 0.
	i2cReadByte(_I2C_LCD_ADDR);
	if(i2cDataAvailable()) //Si une donnée est disponible, elle est lu.
	{
		_etatBouton = ((~i2cGetData() & 0xC0) >> 6); //0b0011111 => 0b11000000 => 0b00000011 inversion bit à bit, Masque sur les 6 premiers LSB déplacement des bits 6 & 7 vers les bits 0 & 1.
		return _etatBouton;
	}
	return _etatBouton;
}

void lcdI2cSetCursor( uint8_t cursorPosiX, uint8_t cursorPosiY)
{
	uint8_t commande = 0x80 | (cursorPosiX & 0xF); //Le paramètre de position x est inclu dans commande.
	if (cursorPosiY)
	commande |= 0x40; //Le paramètre de position y est inclu dans commande.
	_lcdI2cBufferIndexIn = 0;
	_LCDI2C_RS_L(); //Envoie de commande alors RS LOW.
	_lcdI2cDataSend(commande); //La commande est envoyé au LCD.
}