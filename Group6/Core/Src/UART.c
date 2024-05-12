/*
 * Course: Embedded System 2021/2022 vcarletti@unisa.it
 *
 * Lecturer: Carletti Vincenzo
 *
 * Group:
 * Enrico Maria Di Mauro             0612701706             e.dimauro5@studenti.unisa.it
 * Allegra Cuzzocrea                 0612701707           a.cuzzocrea2@studenti.unisa.it
 * Andrea De Gruttola                0612701880           a.degruttola@studenti.unisa.it
 * Salvatore Grimaldi                0612701742           s.grimaldi29@studenti.unisa.it
 *
 *
 * Copyright (C) 2022 - All Rights Reserved
 *
 * This file is part of Embedded System
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "UART.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

/**
* @brief Funzione che inizializza la struttura dati della libreria passando l'handler UART.
* @param argument: UARTCom* uartC, UART_HandleTypeDef* uartPort
* @retval None
*/
void UARTCom_Init(UARTCom* uartC, UART_HandleTypeDef* uartPort){
	uartC->uartPort=uartPort;
}

/**
* @brief Funzione che implementa la stampa formattata tramite UART.
* @param argument: UARTCom* uartC, const char *fmt, ...
* @retval None
*/
void UARTCom_printf(UARTCom* uartC, const char *fmt, ...){
	static char buffer[256];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	int len = strlen(buffer);
	HAL_UART_Transmit(uartC->uartPort, (uint8_t*) buffer, len, HAL_MAX_DELAY);
}

/**
* @brief Funzione che implementa lo scan formattato tramite UART.
* @param argument: UARTCom* uartC, const char *fmt, ...
* @retval None
*/
void UARTCom_scanf(UARTCom* uartC, const char *fmt, ...){
	char buffer[256];
	va_list args;
	va_start(args, fmt);
	int i=0;
	while(i<256){
		HAL_UART_Receive(uartC->uartPort,(uint8_t*) &buffer[i], sizeof(char), HAL_MAX_DELAY);
		if(buffer[i]=='\n')
			break;
		i++;
	}
	vsiscanf(buffer,fmt,args);
	va_end(args);
}

/**
* @brief Funzione che implementa la gets tramite UART.
* @param argument: UARTCom* uartC, char *buffer, int8_t maxDim
* @retval None
*/
void UARTCom_gets(UARTCom* uartC, char *buffer, int8_t maxDim){
	int i=0;
	while(i<maxDim-1){
		HAL_UART_Receive(uartC->uartPort,(uint8_t*) &buffer[i], sizeof(char), HAL_MAX_DELAY);
		if(buffer[i]=='\n')
			break;
		i++;
	}
	buffer[i]='\0';
}
