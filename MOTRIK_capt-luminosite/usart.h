/**
 * @file 	 usart.h
 * @brief 	 Librairie que permet d'assurer l'envoie et la réception de données à l'aide du protocole de communication USART. La taille du buffer de réception et de transmission sont réglables en fonction de RX_BUFFER_SIZE et de TX_BUFFER_SIZE présents dans le fichier usart.h..
 * @author 	 Thomas Desrosiers
 * @version  1.1
 * @date 	 2021/05/02
 */

#ifndef USART_H_
#define USART_H_

#define RX_BUFFER_SIZE 64
#define TX_BUFFER_SIZE 64

/**
 * @brief           Fonction qui permet d'initialiser la communication USART en activant TXEN1 (TRANSMISSION) et RXEN1 (RECEPTION) du registre UCSR1B.
 * @param baudRate  Vitesse de communication en baud désirée.
 * @param fcpu      Fréquence du CPU.
 */
void usartInit(uint32_t baudRate, uint32_t fcpu);

/**
 * @brief   Fonction qui "vide" le buffer de reception en retirant la plus ancienne donnée dans le buffer de rÉception et en retournant cette même donnée.
 * @return  L'octet le plus ancien dans rxBuffer.
 */
uint8_t usartRemRxData(void);

/**
 * @brief   Fonction qui indique si de nouvelles données sont en attente d'être traités.
 * @return  Nombre d'octet présents dans le buffer de réception. 
 */
uint8_t usartRxAvailable(void);

/**
 * @brief         Fonction qui envoie un octet en le plaçant dans txBuffer et en générant l'interruption de transmission.
 * @param u8Data  L'octet à transmettre.
 * @return        0 si tout c'est bien déroulé, sinon 1.
 */
uint8_t usartSendByte(uint8_t u8Data);

/**
 * @brief         Fonction qui envoie un tableau d'octects.
 * @param source  Pointeur vers le tableau de donnés à envoyer.
 * @param size    Nombre d'éléments du tableau qui seront transmis.
 * @return        Nombre d'octet transmis.
 */
uint8_t usartSendBytes(const uint8_t * source, uint8_t size);

/**
 * @brief      Fonction qui permet la transmission de string complète et ce, à l'aide de la fonction usartSendByte.
 * @param str  Pointeur vers la chaine de caractère à envoyer.
 * @return     Nombre de caractère transmis.
 */
uint8_t usartSendString(const char * str);

#endif /* USART_H_ */