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

#include "SDReader.h"

/**
* @brief Funzione che inizializza la struttura dati SdReader effettuando il mount del file passato e la sua apertura in append.
* @param argument: SdReader* sdReader, FATFS* fatFs, char* filename
* @retval FRESULT
*/
FRESULT SdReader_Init(SdReader* sdReader, FATFS* fatFs, char* filename){
	FRESULT fres;
	sdReader->fatFs=fatFs;
	sdReader->filename=filename;


	fres = f_mount(fatFs, "", 1);
	if (fres != FR_OK)
		return fres;

	fres = f_open(&sdReader->fil, sdReader->filename, FA_OPEN_APPEND | FA_WRITE);

	return fres;

}

/**
* @brief Funzione che imposta i metadati di data e ora del file a cui fa riferimento il relativo campo della struttura dati SdReader.
* @param argument: SdReader* sdReader, DateTime dateTime
* @retval FRESULT
*/
FRESULT _SdReader_setDateTime(SdReader* sdReader, DateTime dateTime) {
	FILINFO fno;

	fno.fdate = (WORD) ((((int)dateTime.year - 1980) * 512U) | (int)dateTime.month * 32U | (int)dateTime.dayofmonth);
	fno.ftime = (WORD) ((int)dateTime.hour * 2048U | (int)dateTime.minutes * 32U | (int)dateTime.seconds / 2U);

	return f_utime(sdReader->filename, &fno);
}

/**
* @brief Funzione che chiude il file a cui fa riferimento il relativo campo della struttura dati SdReader.
* @param argument: SdReader* sdReader
* @retval FRESULT
*/
FRESULT SdReader_closeFile(SdReader* sdReader){
	return f_close(&sdReader->fil);
}

/**
* @brief Funzione che scrive la stringa passata come parametro sul file a cui fa riferimento il relativo campo della struttura dati SdReader.
* @param argument: SdReader* sdReader, DateTime dateTime, char * event
* @retval FRESULT
*/
FRESULT SdReader_writeOnFile(SdReader* sdReader, DateTime dateTime, char * event){
	FRESULT fres;
	char* strDate = DateTime_toString(&dateTime);

	f_printf(&sdReader->fil, "%s -> %s\n", strDate, event);
	fres=f_sync(&sdReader->fil);
	if (fres != FR_OK)
		return fres;

	return _SdReader_setDateTime(sdReader, dateTime);
}
