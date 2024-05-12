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


#include <string.h>
#include "GestureRecognition.h"
#include "eml_trees.h"
#include "usart.h"
#include "gesture.h"
#define length 300
float values[2*length];

/**
* @brief Funzione che effettua la lettura dei sample dai due sensori e richiama la funzione predict della rete neurale per identificare la gesture.
* @param argument: GY521* move, GY521* gest, uint8_t num_samples
* @retval uint8_t
*/
uint8_t startRecognition(GY521* move,GY521* gest, uint8_t num_samples){



	uint8_t samples_read = 0;
	uint8_t i = 0;
	while(samples_read < num_samples){
		GY521_readAll(move);
		GY521_readAll(gest);
		samples_read++;

		values[i+0]=move->Ax;
		values[i+1]=move->Ay;
	    values[i+2]=move->Az;
		values[i+3]=move->Gx;
		values[i+4]=move->Gy;
		values[i+5]=move->Gz;
		i+=6;

		values[i+0]=gest->Ax;
		values[i+1]=gest->Ay;
		values[i+2]=gest->Az;
		values[i+3]=gest->Gx;
		values[i+4]=gest->Gy;
		values[i+5]=gest->Gz;
		i+=6;


		HAL_Delay(20);
	}
	uint8_t predicted_gesture = gesture_predict(values, length);
	return predicted_gesture;
}

void recordGesture(GY521* move,GY521* gest, uint8_t num_samples, UARTCom *port){

	uint8_t samples_read = 0;
		while(samples_read < num_samples){
			GY521_readAll(move);
			GY521_readAll(gest);
			samples_read++;

			UARTCom_printf(port, "%f",move->Ax);
			UARTCom_printf(port,",");
			UARTCom_printf(port, "%f",move->Ay);
			UARTCom_printf(port,",");
			UARTCom_printf(port, "%f",move->Az);
			UARTCom_printf(port,",");
			UARTCom_printf(port, "%f",move->Gx);
			UARTCom_printf(port,",");
			UARTCom_printf(port, "%f",move->Gy);
			UARTCom_printf(port,",");
			UARTCom_printf(port, "%f",move->Gz);
			UARTCom_printf(port,",");

		    UARTCom_printf(port, "%f",gest->Ax);
		    UARTCom_printf(port,",");
			UARTCom_printf(port, "%f",gest->Ay);
			UARTCom_printf(port,",");
			UARTCom_printf(port, "%f",gest->Az);
			UARTCom_printf(port,",");
			UARTCom_printf(port, "%f",gest->Gx);
			UARTCom_printf(port,",");
			UARTCom_printf(port, "%f",gest->Gy);
			UARTCom_printf(port,",");
			UARTCom_printf(port, "%f",gest->Gz);
			UARTCom_printf(port,",");

			// print target at the end of samples acquisition
			if (samples_read == num_samples) {
				UARTCom_printf(port, "7\n");
				}


			HAL_Delay(10);
		}

}

