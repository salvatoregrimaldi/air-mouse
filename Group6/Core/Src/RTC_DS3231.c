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

#include <RTC_DS3231.h>
#include <stdio.h>
#include <stdlib.h>

/**
* @brief Funzione che inizializza la struttura dati del Real Time Clock.
* @param argument: DS3231 *ds3231, I2C_HandleTypeDef *i2cPort, uint16_t address
* @retval None
*/
void DS3231_Init(DS3231 *ds3231, I2C_HandleTypeDef *i2cPort, uint16_t address) {
	ds3231->address = address;
	ds3231->i2cPort = i2cPort;

}

/**
* @brief Funzione che converte un numero decimale in un binary coded decimal.
* @param argument: int16_t num
* @retval uint8_t
*/
uint8_t _DS3231_decimalToBinaryCoded(int16_t num) {
	return (uint8_t) ((num / 10 * 16) + (num % 10));
}

/**
* @brief Funzione che converte un binary coded decimal in un numero decimale.
* @param argument: uint8_t num
* @retval int16_t
*/
int16_t _DS3231_binaryCodedToDecimal(uint8_t num) {
	return (int16_t) ((num / 16 * 10) + (num % 16));
}

/**
* @brief Funzione che converte i valori grezzi letti da un RTC in un DateTime.
* @param argument: uint8_t* rawDateTime
* @retval DateTime
*/
DateTime DS3231_rawtoDateTime(uint8_t* rawDateTime) {
	DateTime ret;

	DateTime_Init(&ret,
			_DS3231_binaryCodedToDecimal(rawDateTime[0]),
			_DS3231_binaryCodedToDecimal(rawDateTime[1]),
			_DS3231_binaryCodedToDecimal(rawDateTime[2]),
			_DS3231_binaryCodedToDecimal(rawDateTime[3]),
			_DS3231_binaryCodedToDecimal(rawDateTime[4]),
			_DS3231_binaryCodedToDecimal(rawDateTime[5]),
			_DS3231_binaryCodedToDecimal(rawDateTime[6]) + 2000);

	return ret;
}

/**
* @brief Funzione che converte i valori grezzi letti da un RTC in un float che indica la temperatura letta dal sensore.
* @param argument: uint8_t* temp
* @retval float
*/
float DS3231_rawToTemp(uint8_t* temp){
	return ((temp[0]) + (temp[1] >> 6) / 4.0);
}

/**
* @brief Funzione che restituisce i valori grezzi di data e ora memorizzati nella struttura dati del Real Time Clock.
* @param argument: DS3231 *ds3231
* @retval uint8_t
*/
uint8_t* DS3231_getRawDateTime(DS3231 *ds3231){
	return ds3231->_rawDateTime;
}

/**
* @brief Funzione che restituisce i valori grezzi della temperatura memorizzati nella struttura dati del Real Time Clock.
* @param argument: DS3231 *ds3231
* @retval uint8_t
*/
uint8_t* DS3231_getRawTemp(DS3231 *ds3231){
	return ds3231->_rawTemp;
}

/**
* @brief Funzione che imposta la data e l'ora del RTC scrivendo nei suoi registri.
* @param argument: DS3231 *ds3231, DateTime dateTime
* @retval None
*/
void DS3231_setDateTime(DS3231 *ds3231, DateTime dateTime) {
	uint8_t sDateTime[7];
	sDateTime[0] = _DS3231_decimalToBinaryCoded(dateTime.seconds);
	sDateTime[1] = _DS3231_decimalToBinaryCoded(dateTime.minutes);
	sDateTime[2] = _DS3231_decimalToBinaryCoded(dateTime.hour);
	sDateTime[3] = _DS3231_decimalToBinaryCoded(dateTime.dayofweek);
	sDateTime[4] = _DS3231_decimalToBinaryCoded(dateTime.dayofmonth);
	sDateTime[5] = _DS3231_decimalToBinaryCoded(dateTime.month);
	sDateTime[6] = _DS3231_decimalToBinaryCoded(dateTime.year % 2000);

	HAL_I2C_Mem_Write(ds3231->i2cPort, ds3231->address, 0x00, 1, sDateTime, 7, 1000);

}

/**
* @brief Funzione che legge data e ora dai registri del RTC, converte i valori in un DateTime e poi lo restituisce.
* @param argument: DS3231 *ds3231
* @retval DateTime
*/
DateTime DS3231_getDateTime(DS3231 *ds3231) {
	DateTime ret;
	uint8_t gDateTime[7];

	HAL_I2C_Mem_Read(ds3231->i2cPort, ds3231->address, 0x00, 1, gDateTime, 7, 1000);

	ret=DS3231_rawtoDateTime(gDateTime);

	return ret;
}

/**
* @brief Funzione che legge la temperatura dai registri del RTC, converte i valori in un float e poi lo restituisce.
* @param argument: DS3231 *ds3231
* @retval float
*/
float DS3231_getTemp(DS3231 *ds3231) {
	uint8_t temp[2];

	HAL_I2C_Mem_Read(ds3231->i2cPort, ds3231->address, 0x11, 1, temp, 2, 1000);

	return DS3231_rawToTemp(temp);
}

/**
* @brief Funzione che legge (con interrupt) la temperatura dai registri del RTC, converte i valori in un float e poi lo restituisce.
* @param argument: DS3231 *ds3231
* @retval float
*/
void DS3231_getTempIT(DS3231 *ds3231){
	HAL_I2C_Mem_Read_IT(ds3231->i2cPort, ds3231->address, 0x11, 1, ds3231->_rawTemp, 2);
}

/**
* @brief Funzione che imposta (con interrupt) la data e l'ora del RTC scrivendo nei suoi registri.
* @param argument: DS3231 *ds3231, DateTime dateTime
* @retval None
*/
void DS3231_setDateTimeIT(DS3231 *ds3231, DateTime dateTime) {

	ds3231->_rawDateTime[0] = _DS3231_decimalToBinaryCoded(dateTime.seconds);
	ds3231->_rawDateTime[1] = _DS3231_decimalToBinaryCoded(dateTime.minutes);
	ds3231->_rawDateTime[2] = _DS3231_decimalToBinaryCoded(dateTime.hour);
	ds3231->_rawDateTime[3] = _DS3231_decimalToBinaryCoded(dateTime.dayofweek);
	ds3231->_rawDateTime[4] = _DS3231_decimalToBinaryCoded(dateTime.dayofmonth);
	ds3231->_rawDateTime[5] = _DS3231_decimalToBinaryCoded(dateTime.month);
	ds3231->_rawDateTime[6] = _DS3231_decimalToBinaryCoded(dateTime.year % 2000);

	HAL_I2C_Mem_Write_IT(ds3231->i2cPort, ds3231->address, 0x00, 1, ds3231->_rawDateTime, 7);

}

/**
* @brief Funzione che legge (con interrupt) data e ora dai registri del RTC, converte i valori in un DateTime e poi lo restituisce.
* @param argument: DS3231 *ds3231
* @retval DateTime
*/
void DS3231_getDateTimeIT(DS3231 *ds3231) {

	HAL_I2C_Mem_Read_IT(ds3231->i2cPort, ds3231->address, 0x00, 1, ds3231->_rawDateTime, 7);

}
