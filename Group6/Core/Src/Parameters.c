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

#include "Parameters.h"

#include <stdlib.h>

/**
* @brief Funzione che inizializza la struttura dati dei parametri con dei valori predefiniti.
* @param argument: Parameters* param
* @retval None
*/
void Param_Init(Parameters* param){
	param->deadZone=10;
	param->maxSpeedValue=6;
	param->maxSpeedAngleX=90;
	param->maxSpeedAngleY=90;
	param->accThresh=4;
}

/**
* @brief Funzione che effettua la procedura di calibrazione per la velocità massima.
* @param argument: Parameters* param, UARTCom* port
* @retval None
*/
void setMaxSpeedValue(Parameters* param, UARTCom* port){
	UARTCom_printf(port, "Inserire un valore da 3 a 10 per la velocità massima del cursore\n");
	int value=-1;
	while(value<3 || value>10){
		UARTCom_scanf(port, "%d", &value);
		if(value<3 || value>10)
			UARTCom_printf(port, "Inserire un valore accettabile\n");
	}

	param->maxSpeedValue=value;

}

/**
* @brief Funzione che effettua la procedura di calibrazione per l'angolo della velocità massima.
* @param argument: Parameters* param, GY521* gy521, UARTCom* port, int pitchRoll
* @retval None
*/
void setMaxSpeedAngle(Parameters* param, GY521* gy521, UARTCom* port, int pitchRoll){ //Y=0
	char choose;
	if(pitchRoll==0)
		choose='Y';
	else
		choose='X';

	UARTCom_printf(port, "Avvio calibrazione dell'angolo %c corrispondente alla velocità massima\n",choose);

	int count=0;
	float value;
	for(;;){
		GY521_readAll(gy521);
		if(pitchRoll==0)
			value=GY521_getRoll(gy521);
		else
			value=GY521_getPitch(gy521);
		UARTCom_printf(port, "%d\n",(int)value);

		if(value>75)
			count++;
		else
			count=0;

		if(count==10)
			break;
		HAL_Delay(100);
	}

	count=0;

	UARTCom_printf(port, "Inclinare la mano per impostare l'angolo desiderato\n");
	float maxValue=0;

	for(;;){
		GY521_readAll(gy521);
		if(pitchRoll==0)
			value = (float) GY521_getRoll(gy521);
		else
			value = (float) GY521_getPitch(gy521);

		if(value>param->deadZone)
			maxValue=value;

		count++;

		if(count==50)
			break;

		HAL_Delay(100);
	}
	if(pitchRoll==0)
		param->maxSpeedAngleY=maxValue;
	else
		param->maxSpeedAngleX=maxValue;

	UARTCom_printf(port, "Valore dell'angolo %c impostato correttamente\n",choose);

}

/**
* @brief Funzione che effettua la procedura di calibrazione per le deadzones.
* @param argument: Parameters* param, GY521* gy521, UARTCom* port
* @retval None
*/
void setDeadZones(Parameters* param, GY521* gy521, UARTCom* port){
	UARTCom_printf(port, "Avvio calibrazione delle deadzones\n");
	int count=0;
	float value;
	for(;;){
		GY521_readAll(gy521);
		value=GY521_getPitch(gy521);
		if(value>75)
			count++;
		else
			count=0;

		UARTCom_printf(port, "%d\n",(int)value);

		if(count==10)
			break;
		HAL_Delay(100);
	}

	count=0;

	UARTCom_printf(port, "Inclinare la mano per impostare l'angolo desiderato per le deadzones\n");
	float minValue=0;

	for(;;){
		GY521_readAll(gy521);
		value = (float) GY521_getPitch(gy521);
		if(value>0)
			minValue=value;

		count++;

		if(count==50)
			break;

		HAL_Delay(100);
	}
	param->deadZone=minValue;

	UARTCom_printf(port, "Valore delle deadzones impostato correttamente\n");

}

/**
* @brief Funzione che effettua la procedura di calibrazione per la soglia di accelerazione.
* @param argument: Parameters* param, UARTCom* port
* @retval None
*/
void setAccThresh(Parameters* param, UARTCom* port){
	UARTCom_printf(port, "Inserire un valore da 3 a 5 la soglia di accelerazione delle gesture\n");
	int value=-1;
	while(value<3 || value>5){
		UARTCom_scanf(port, "%d", &value);
		if(value<3 || value>5)
			UARTCom_printf(port, "Inserire un valore accettabile\n");
	}

	param->accThresh=value;

}

/**
* @brief Funzione che imposta la data e l'ora del Real Time Clock.
* @param argument: Parameters* param, DS3231* ds3231, UARTCom* port
* @retval None
*/
void setDateTime(Parameters* param, DS3231* ds3231, UARTCom* port){
	int day,month,year,min,sec,hour,dow,res;
	DateTime dateToSet;

	do{
		UARTCom_printf(port, "Inserire data e ora: gg-mm-aaaa_hh:mm:ss\n");
		UARTCom_scanf(port, "%d-%d-%d_%d:%d:%d",&day,&month,&year,&hour,&min,&sec);
		UARTCom_printf(port, "Inserire il numero del giorno della settimana\n");
		UARTCom_scanf(port, "%d",&dow);
		res = DateTime_Init(&dateToSet, sec, min, hour, dow, day, month, year);
		if(res<0)
			UARTCom_printf(port, "Inserire dei valori accettabili\n");
	}while(res<0);
	UARTCom_printf(port, "La data impostata è: %s numero del giorno della settimana: %d \n", DateTime_toString(&dateToSet),dateToSet.dayofweek);
	DS3231_setDateTime(ds3231, dateToSet);
}



