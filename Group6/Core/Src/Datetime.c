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

#include "Datetime.h"
#include <stdlib.h>

/**
* @brief Funzione che inizializza un DateTime con i parametri passati.
* @param argument: DateTime* dateTime, int16_t seconds, int16_t minutes, int16_t hour, int16_t dayofweek, int16_t dayofmonth, int16_t month, int16_t year
* @retval int8_t
*/
int8_t DateTime_Init(DateTime* dateTime, int16_t seconds, int16_t minutes, int16_t hour, int16_t dayofweek, int16_t dayofmonth, int16_t month, int16_t year){
	if((seconds<0 || seconds>59) || (minutes<0 || minutes>59) || (hour<0 || hour>23) || (dayofweek<1 || dayofweek>7) || (month<1 || month>12) || (year<2000 || year>2099))
		return -1;
	if(dayofmonth<1 || (month==1 && dayofmonth>31) || (month==3 && dayofmonth>31) || (month==4 && dayofmonth>30) || (month==5 && dayofmonth>31) || (month==6 && dayofmonth>30) || (month==7 && dayofmonth>31) || (month==8 && dayofmonth>31) || (month==9 && dayofmonth>30) || (month==10 && dayofmonth>31) || (month==11 && dayofmonth>30) || (month==12 && dayofmonth>31))
		return -1;
	if(month==2 && ((year%4 == 0 && year%100 != 0) || year%400 == 0) && dayofmonth>29)
		return -1;
	else if(month==2 && !((year%4 == 0 && year%100 != 0) || year%400 == 0) && dayofmonth>28)
		return -1;

	dateTime->seconds=seconds;
	dateTime->minutes=minutes;
	dateTime->hour=hour;
	dateTime->dayofweek=dayofweek;
	dateTime->dayofmonth=dayofmonth;
	dateTime->month=month;
	dateTime->year=year;

	return 0;
}

/**
* @brief Funzione che converte un DateTime in una stringa
* @param argument: DateTime* dateTime
* @retval char*
*/
char* DateTime_toString(DateTime* dateTime){
	sprintf(dateTime->_str,"%02d-%02d-%04d_%02d:%02d:%02d",dateTime->dayofmonth,dateTime->month,dateTime->year,dateTime->hour,dateTime->minutes,dateTime->seconds);
	return dateTime->_str;
}
