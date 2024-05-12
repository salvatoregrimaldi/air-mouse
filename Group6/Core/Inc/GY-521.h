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

#ifndef INC_GY521_H_
#define INC_GY521_H_

#include <stdint.h>
#include "i2c.h"

#define GY521_DEF_ADDR1 0x68
#define GY521_DEF_ADDR2 0x69

typedef struct
{
	I2C_HandleTypeDef* i2cPort;
	uint16_t address;

	uint8_t _recDataAG[14];

    int16_t Accel_X_RAW;
    int16_t Accel_Y_RAW;
    int16_t Accel_Z_RAW;
    double Ax;
    double Ay;
    double Az;

    int16_t Gyro_X_RAW;
    int16_t Gyro_Y_RAW;
    int16_t Gyro_Z_RAW;
    double Gx;
    double Gy;
    double Gz;

} GY521;

uint8_t GY521_Init(GY521* gy521, I2C_HandleTypeDef* i2cPort, uint16_t address);

void GY521_convertGyr(GY521* gy521);
void GY521_convertAcc(GY521* gy521);
void GY521_readAll(GY521* gy521);
void GY521_readAccel(GY521* gy521);
void GY521_readGyro(GY521* gy521);
double GY521_getPitch(GY521* gy521);
double GY521_getRoll(GY521* gy521);
void GY521_readAccelIT(GY521* gy521);
void GY521_readGyroIT(GY521* gy521);
void GY521_readAllIT(GY521* gy521);


#endif /* INC_GY521_H_ */
