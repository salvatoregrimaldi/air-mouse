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

#ifndef INC_RTC_DS3231_H_
#define INC_RTC_DS3231_H_

#include "i2c.h"
#include "Datetime.h"


#define DS3231_DEF_ADDR 0xD0

typedef struct {
	I2C_HandleTypeDef* i2cPort;
	uint16_t address;
	uint8_t _rawDateTime[7];
	uint8_t _rawTemp[2];

} DS3231;

void DS3231_Init(DS3231* ds3231, I2C_HandleTypeDef* i2cPort, uint16_t address);
uint8_t _DS3231_decimalToBinaryCoded(int16_t num);
int16_t _DS3231_binaryCodedToDecimal(uint8_t num);
DateTime DS3231_rawtoDateTime(uint8_t* rawDateTime);
float DS3231_rawToTemp(uint8_t* temp);
uint8_t* DS3231_getRawDateTime(DS3231 *ds3231);
uint8_t* DS3231_getRawTemp(DS3231 *ds3231);

void DS3231_setDateTime (DS3231* ds3231, DateTime dateTime);
DateTime DS3231_getDateTime (DS3231* ds3231);
float DS3231_getTemp (DS3231* ds3231);
void DS3231_getTempIT(DS3231 *ds3231);
void DS3231_setDateTimeIT (DS3231* ds3231, DateTime dateTime);
void DS3231_getDateTimeIT (DS3231* ds3231);


#endif /* INC_RTC_DS3231_H_ */
