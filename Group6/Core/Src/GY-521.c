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

#include <math.h>
#include <GY-521.h>

#define PWR_MGMT_1_REG 0x6B
#define WHO_AM_I_REG 0x75
#define GYRO_XOUT_H_REG 0x43
#define ACCEL_CONFIG_REG 0x1C
#define SMPLRT_DIV_REG 0x19
#define ACCEL_XOUT_H_REG 0x3B
#define GYRO_CONFIG_REG 0x1B
#define RAD_TO_DEG 57.295779513082320876798154814105

#define GYRO_XYZ_CORRECTOR 131.0
#define ACCEL_XY_CORRECTOR 16384.0
#define ACCEL_Z_CORRECTOR 14418.0

/**
* @brief Funzione che inizializza il GY-521 scrivendo dei byte nei suoi registri per disattivare la sleep mode e configurare giroscopio e accelerometro.
* @param argument: GY521* gy521, I2C_HandleTypeDef* i2cPort, uint16_t address
* @retval uint8_t
*/
uint8_t GY521_Init(GY521* gy521, I2C_HandleTypeDef* i2cPort, uint16_t address){
	uint8_t check;
	uint8_t Data;

	gy521->i2cPort=i2cPort;
	gy521->address=address;

	HAL_I2C_Mem_Read(gy521->i2cPort, gy521->address<<1, WHO_AM_I_REG, 1, &check, 1, 1000);

	if (check !=0) // il sensore restituirà i valori 0x68 o 0x98 se non ci sono problemi
	{
		// disabilita la sleep mode del sensore
		Data = 0;
		HAL_I2C_Mem_Write(gy521->i2cPort, gy521->address<<1, PWR_MGMT_1_REG, 1, &Data, 1, 1000);

		// imposta il data rate a 1kHz
		Data = 0x07;
		HAL_I2C_Mem_Write(gy521->i2cPort, gy521->address<<1, SMPLRT_DIV_REG, 1, &Data, 1, 1000);

		// Configura l'accelerometro
		Data = 0x00;
		HAL_I2C_Mem_Write(gy521->i2cPort, gy521->address<<1, ACCEL_CONFIG_REG, 1, &Data, 1, 1000);

		// Configura il giroscopio
		Data = 0x00;
		HAL_I2C_Mem_Write(gy521->i2cPort, gy521->address<<1, GYRO_CONFIG_REG, 1, &Data, 1, 1000);
		return 0;
	}
	return 1;

}

/**
* @brief Funzione che converte i valori grezzi del giroscopio dividendoli per una costante che dipende dalla scala impostata in fase di inizializzazione.
* @param argument: GY521* gy521
* @retval None
*/
void GY521_convertGyr(GY521* gy521){
	gy521->Gyro_X_RAW = (int16_t)(gy521->_recDataAG[8] << 8 | gy521->_recDataAG[9]);
	gy521->Gyro_Y_RAW = (int16_t)(gy521->_recDataAG[10] << 8 | gy521->_recDataAG[11]);
	gy521->Gyro_Z_RAW = (int16_t)(gy521->_recDataAG[12] << 8 | gy521->_recDataAG[13]);

	gy521->Gx = gy521->Gyro_X_RAW / GYRO_XYZ_CORRECTOR;
	gy521->Gy = gy521->Gyro_Y_RAW / GYRO_XYZ_CORRECTOR;
	gy521->Gz = gy521->Gyro_Z_RAW / GYRO_XYZ_CORRECTOR;
}

/**
* @brief Funzione che converte i valori grezzi dell'accelerometro dividendoli per una costante che dipende dalla scala impostata in fase di inizializzazione.
* @param argument: GY521* gy521
* @retval None
*/
void GY521_convertAcc(GY521* gy521){
	gy521->Accel_X_RAW = (int16_t)(gy521->_recDataAG[0] << 8 | gy521->_recDataAG[1]);
	gy521->Accel_Y_RAW = (int16_t)(gy521->_recDataAG[2] << 8 | gy521->_recDataAG[3]);
	gy521->Accel_Z_RAW = (int16_t)(gy521->_recDataAG[4] << 8 | gy521->_recDataAG[5]);

	gy521->Ax = gy521->Accel_X_RAW / ACCEL_XY_CORRECTOR;
	gy521->Ay = gy521->Accel_Y_RAW / ACCEL_XY_CORRECTOR;
	gy521->Az = gy521->Accel_Z_RAW / ACCEL_Z_CORRECTOR;
}

/**
* @brief Funzione che legge dal sensore i valori dell'accelerometro e li memorizza nel buffer di lettura della struttura dati.
* @param argument: GY521* gy521
* @retval None
*/
void GY521_readAccel(GY521* gy521){
    HAL_I2C_Mem_Read(gy521->i2cPort, gy521->address<<1, ACCEL_XOUT_H_REG, 1, &(gy521->_recDataAG[0]), 6, 1000);

    GY521_convertAcc(gy521);
}

/**
* @brief Funzione che legge dal sensore i valori del giroscopio e li memorizza nel buffer di lettura della struttura dati.
* @param argument: GY521* gy521
* @retval None
*/
void GY521_readGyro(GY521* gy521){
    HAL_I2C_Mem_Read(gy521->i2cPort, gy521->address<<1, GYRO_XOUT_H_REG, 1, &(gy521->_recDataAG[8]), 6, 1000);

    GY521_convertGyr(gy521);

}

/**
* @brief Funzione che legge dal sensore i valori di accelerometro e giroscopio e li memorizza nel buffer di lettura della struttura dati.
* @param argument: GY521* gy521
* @retval None
*/
void GY521_readAll(GY521* gy521){
    HAL_I2C_Mem_Read(gy521->i2cPort, gy521->address<<1, ACCEL_XOUT_H_REG, 1, gy521->_recDataAG, 14, 1000);

    GY521_convertAcc(gy521);
    GY521_convertGyr(gy521);

}

/**
* @brief Funzione che calcola l'angolo di pitch partendo dai valori convertiti della struttura dati.
* @param argument: GY521* gy521
* @retval double
*/
double GY521_getPitch(GY521* gy521){
	 double app, value;
	  app = (gy521->Ax * gy521->Ax) + (gy521->Az * gy521->Az);
	  app = sqrt(app);

	  value = atan2(gy521->Ay, app);
	  value = value * 180 / 3.14;

	 return value;


}

/**
* @brief Funzione che calcola l'angolo di roll partendo dai valori convertiti della struttura dati.
* @param argument: GY521* gy521
* @retval double
*/
double GY521_getRoll(GY521* gy521){
	double app, value;
	app = (gy521->Ay * gy521->Ay) + (gy521->Az * gy521->Az);
	app = sqrt(app);

	value = atan2(gy521->Ax, app);
	value = value * 180 / 3.14;

	return value;
}

/**
* @brief Funzione che legge (con interrupt) dal sensore i valori dell'accelerometro e li memorizza nel buffer di lettura della struttura dati.
* @param argument: GY521* gy521
* @retval None
*/
void GY521_readAccelIT(GY521* gy521){
    HAL_I2C_Mem_Read_IT(gy521->i2cPort, gy521->address<<1, ACCEL_XOUT_H_REG, 1, &(gy521->_recDataAG[0]), 6);
}

/**
* @brief Funzione che legge (con interrupt) dal sensore i valori del giroscopio e li memorizza nel buffer di lettura della struttura dati.
* @param argument: GY521* gy521
* @retval None
*/
void GY521_readGyroIT(GY521* gy521){
    HAL_I2C_Mem_Read_IT(gy521->i2cPort, gy521->address<<1, GYRO_XOUT_H_REG, 1, &(gy521->_recDataAG[8]), 6);
}

/**
* @brief Funzione che legge (con interrupt) dal sensore i valori di accelerometro e giroscopio e li memorizza nel buffer di lettura della struttura dati.
* @param argument: GY521* gy521
* @retval None
*/
void GY521_readAllIT(GY521* gy521){
	HAL_I2C_Mem_Read_IT(gy521->i2cPort, gy521->address<<1, ACCEL_XOUT_H_REG, 1, gy521->_recDataAG, 14);
}
