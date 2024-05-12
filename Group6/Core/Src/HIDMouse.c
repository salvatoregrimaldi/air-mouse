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

#include "HIDMouse.h"
#include "usbd_customhid.h"

/**
* @brief Funzione che inizializza la struttura dati del mouse mettendo tutti i tasti a 0 o a HIDMouse_ButtonReleased.
* @param argument: HIDMouse* HIDmouse, USBD_HandleTypeDef* handler
* @retval None
*/
void HIDMouse_Init(HIDMouse* HIDmouse, USBD_HandleTypeDef* handler){
	HIDmouse->handler=handler;
	HIDmouse->leftButton=HIDMouse_ButtonReleased;
	HIDmouse->rightButton=HIDMouse_ButtonReleased;
	HIDmouse->middleButton=HIDMouse_ButtonReleased;

	HIDmouse->wheel=0;
	HIDmouse->XAxis=0;
	HIDmouse->YAxis=0;
}

/**
* @brief Funzione che costruisce il buffer di invio usando i valori della struttura dati del mouse e poi manda il report HID.
* @param argument: HIDMouse* HIDmouse
* @retval None
*/
void _HIDMouse_sendReport(HIDMouse* HIDmouse){
	uint8_t buff[5]; /* 5 bytes long report */

	/* Report ID */
	buff[0] = 0x02; /* Mouse */

	/* Set buttons */
	buff[1] = 0;
	buff[1] |= HIDmouse->leftButton << 0; /* Bit 0 */
	buff[1] |= HIDmouse->rightButton << 1; /* Bit 1 */
	buff[1] |= HIDmouse->middleButton << 2; /* Bit 2 */

	/* Set X and Y offset */
	buff[2] = HIDmouse->XAxis;
	buff[3] = HIDmouse->YAxis;

	/* Set wheel */
	buff[4] = HIDmouse->wheel;

	/* Send to USB */
	USBD_CUSTOM_HID_SendReport(HIDmouse->handler, buff, 5);

}

/**
* @brief Funzione che modifica i valori della struttura dati del mouse a seconda dei parametri ricevuti che identificano i tasti, chiamando infine la funzione _HIDMouse_sendReport().
* @param argument: HIDMouse* HIDmouse, int8_t X, int8_t Y, int8_t wheel, HIDMouse_Input button, int8_t releaseButtons
* @retval int8_t
*/
int8_t HIDMouse_sendInput(HIDMouse* HIDmouse, int8_t X, int8_t Y, int8_t wheel, HIDMouse_Input button, int8_t releaseButtons){
	if(X<-127 || X>127 || Y<-127 || Y>127 || wheel<-127 || wheel>127)
		return -1;

	HIDmouse->leftButton=HIDMouse_ButtonReleased;
	HIDmouse->rightButton=HIDMouse_ButtonReleased;
	HIDmouse->middleButton=HIDMouse_ButtonReleased;

	switch(button){
		case HIDMouse_NULL:
			break;
		case HIDMouse_Right:
			HIDmouse->rightButton=HIDMouse_ButtonPressed;
			break;
		case HIDMouse_Left:
			HIDmouse->leftButton=HIDMouse_ButtonPressed;
			break;
		case HIDMouse_Middle:
			HIDmouse->middleButton=HIDMouse_ButtonPressed;
			break;
	}

	HIDmouse->XAxis=X;
	HIDmouse->YAxis=Y;
	HIDmouse->wheel=wheel;

	_HIDMouse_sendReport(HIDmouse);

	if(button!=HIDMouse_NULL && releaseButtons==1){
		HAL_Delay(15);

		HIDmouse->leftButton=HIDMouse_ButtonReleased;
		HIDmouse->rightButton=HIDMouse_ButtonReleased;
		HIDmouse->middleButton=HIDMouse_ButtonReleased;

		HIDmouse->XAxis=0;
		HIDmouse->YAxis=0;
		HIDmouse->wheel=0;

		_HIDMouse_sendReport(HIDmouse);
	}

	return 0;
}
