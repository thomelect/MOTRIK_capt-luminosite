/**
@file 		usart.c
@brief 		Librairie que permet d'assurer l'envoie et la réception de données à l'aide du protocole de communication USART. La taille du buffer de réception et de transmission sont réglables en fonction de RX_BUFFER_SIZE et de TX_BUFFER_SIZE présents dans le fichier usart.h..
@author 	Thomas Desrosiers
@version 	1.1
@date 		2021/05/02
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"

// Déclaration des variables nécessaires à la réception.
volatile uint8_t _rxBuffer[RX_BUFFER_SIZE];// Tableau d'integer qui contient les donnés reçus et qui à la taille de RX_BUFFER_SIZE (16).
volatile uint16_t _rxBufferIn = 0;// rxBufferIn est incrémenté à chaque fois qu'une nouvelle donnée est placée dans rxBuffer.
uint8_t _rxBufferOut = 0;// rxBufferOut est incrémenté à chaque fois qu'une donnée est est sortie de rxBuffer.
volatile uint8_t _rxCnt = 0;

// Déclaration des variables nécessaires à la transmission.
volatile uint8_t _txBuffer[TX_BUFFER_SIZE];// Tableau d'integer qui contient les donnés reçus et qui à la taille de RX_BUFFER_SIZE (16).
volatile uint8_t _txBufferOut = 0;// txBufferOut est incrémenté à chaque fois qu'une donnée est sortie de txBuffer.
uint16_t _txBufferIn = 0;// txBufferIn est incrémenté à chaque fois qu'une nouvelle donnée est placée dans txBuffer.
volatile uint8_t _txCnt = 0;// txCnt est un compteurs qui permet de savoir combien de données 

/**
*@brief Interruption nécessaires à la réception. Le buffer de réception est remplis avec la valeur de UDR1.
*/
ISR(USART1_RX_vect)
{
	_rxCnt++;
	_rxBuffer[_rxBufferIn++] = UDR1;
	if (_rxBufferIn >= RX_BUFFER_SIZE)// Si la limite du buffer est atteinte rxBufferIn retourne à 0 et les premières donnés (les plus ancienne) sont écrasés.
		_rxBufferIn = 0;
}

/**
*@brief Interruption nécessaires à la transmission. Le buffer de transmission est vidé en donnant à UDR1 la valeur de l'octet à envoyer.
*/
ISR(USART1_UDRE_vect)
{
	if (!_txCnt)
		UCSR1B &= ~(1<<UDRIE1);// Permet de sortir de l'interruption lorsqu'on sort du if de transmission c'est à dire lorsqu'il n'y a plus aucun caractère à transmettre.
	else
	{
		_txCnt--;
		UDR1 = _txBuffer[_txBufferOut++];
		if (_txBufferOut >= TX_BUFFER_SIZE)
			_txBufferOut = 0;
	}
}

void usartInit(uint32_t baudRate, uint32_t fcpu)
{
	if (baudRate >= 115200)
	{
		UCSR1A |= (1<<U2X1);
		UBRR1 = ((fcpu >> 3) / baudRate)-0.5; // Calcul de UDRR1 la vitesse en fonction de baudrate en paramètre de la fonction.
	}
	else
	{
		UCSR1A &= ~(1<<U2X1);
		UBRR1 = ((fcpu >> 4) / baudRate)-0.5; // Calcul de UDRR1 la vitesse en fonction de baudrate en paramètre de la fonction.
	}
	UCSR1B |= ((1<<RXEN1) | (1<<TXEN1)); // Active RX et TX.
	sei();
	UCSR1B |= (1<<RXCIE1);
	UCSR1B |= (1<<UDRIE1);
}

uint8_t usartRemRxData(void)
{
	uint8_t rxData = 0;
	if (_rxCnt)// Si rxCnt est suppérieur à 0 c'est qu'il reste des données dans le buffer de réception.
	{
		cli();// Arrêt des interruption le temps de modifier rxCnt afin d'éviter tout conflit.
		_rxCnt--;
		sei();
		rxData = _rxBuffer[_rxBufferOut++];// Les donnés précedement placés dans rxBuffer sont placés unes à unes dans rxData afin de vider rxBuffer. Lorsque rxBufferOut est = à rxBufferIn c'est que tout à été affiché.
		if (_rxBufferOut >= RX_BUFFER_SIZE)
			_rxBufferOut = 0;
	}
	return rxData;// rxData est la donnée la plus ancienne dans rxBuffer.
}

uint8_t usartRxAvailable(void)
{
	return _rxCnt;// rxCnt augmente quand une donnée est placée dans le buffer de réception et diminue lorsque des données en sont retirés.
}

uint8_t usartSendByte(uint8_t u8Data)
{
	if(_txCnt >= TX_BUFFER_SIZE)
		return 1;
	_txCnt++;
	_txBuffer[_txBufferIn++] =  u8Data;// La donnée contenue par byteSend est placé dans le buffer de transmission.
	if (_txBufferIn >= TX_BUFFER_SIZE)
		_txBufferIn = 0;// Si plus de données sont reçu qu'il n'y en à qui sont envoyer, txBufferIn retourne à 0.
	UCSR1B |= (1<<UDRIE1);// UDRIE1 est mis à 1 ce qui a pour effet de générer l'interruption USART1_UDRE_vect dans laquelle la transmission de caractère sera effectuée.
	return 0;
}

uint8_t usartSendBytes(const uint8_t * source, uint8_t size)
{
	for(uint8_t i = 0; i < size; i++)
	{
		if(usartSendByte(source[i]))
			return i;
	}
	return size;
}

uint8_t usartSendString(const char * str)
{
	uint8_t nbChar = 0;
	for (uint8_t i = 0; str[i]; i++)
	{
		if(usartSendByte(str[i]))// Appel de la fonction usartSendByte afin d'envoyer un à un les caractères qui composent la string reçu.
			return i;
		nbChar ++;
	}
	return nbChar;
}