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

#ifndef INC_HIDMOUSE_H_
#define INC_HIDMOUSE_H_
	#include "usbd_def.h"

	typedef enum {
		HIDMouse_NULL = 0,
		HIDMouse_Left = 1,
		HIDMouse_Right = 2,
		HIDMouse_Middle = 3
	} HIDMouse_Input;

	typedef enum {
		HIDMouse_ButtonReleased = 0,
		HIDMouse_ButtonPressed = 1
	} HIDMouse_InputState;

	typedef struct {
		USBD_HandleTypeDef* handler;
		HIDMouse_InputState leftButton;
		HIDMouse_InputState rightButton;
		HIDMouse_InputState middleButton;
		int8_t XAxis;
		int8_t YAxis;
		int8_t wheel;
	} HIDMouse;

	void HIDMouse_Init(HIDMouse* HIDmouse, USBD_HandleTypeDef* handler);
	void _HIDMouse_sendReport(HIDMouse* HIDmouse);
	int8_t HIDMouse_sendInput(HIDMouse* HIDmouse, int8_t X, int8_t Y, int8_t wheel, HIDMouse_Input button, int8_t releaseButtons);


#endif /* INC_HIDMOUSE_H_ */
