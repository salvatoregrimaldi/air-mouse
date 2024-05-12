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

#ifndef INC_DATETIME_H_
#define INC_DATETIME_H_
#define DATE_DIM 20
#include <stdio.h>


typedef struct {
	int16_t seconds;
	int16_t minutes;
	int16_t hour;
	int16_t dayofweek;
	int16_t dayofmonth;
	int16_t month;
	int16_t year;
	char _str[DATE_DIM];
} DateTime;

int8_t DateTime_Init(DateTime* dateTime, int16_t seconds, int16_t minutes, int16_t hour, int16_t dayofweek, int16_t dayofmonth, int16_t month, int16_t year);

char* DateTime_toString(DateTime* dateTime);

#endif /* INC_DATETIME_H_ */
