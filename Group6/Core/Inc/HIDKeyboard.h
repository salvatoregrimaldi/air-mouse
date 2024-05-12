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

#ifndef INC_HIDKEYBOARD_H_
#define INC_HIDKEYBOARD_H_

	#include "usbd_def.h"

	typedef enum {
		HIDKeyboard_NULL = 0,
		HIDKeyboard_CtrlL = 1,
		HIDKeyboard_ShiftL = 2,
		HIDKeyboard_AltL = 3,
		HIDKeyboard_WinL = 4,
		HIDKeyboard_CtrlR = 5,
		HIDKeyboard_ShiftR = 6,
		HIDKeyboard_AltR = 7,
		HIDKeyboard_WinR = 8
	} HIDKeyboard_Key;

	typedef enum {
		HIDKeyboard_ButtonReleased = 0,
		HIDKeyboard_ButtonPressed = 1
	} HIDKeyboard_KeyState;

	typedef struct {
		USBD_HandleTypeDef* handler;
		HIDKeyboard_KeyState L_CTRL;
		HIDKeyboard_KeyState L_ALT;
		HIDKeyboard_KeyState L_SHIFT;
		HIDKeyboard_KeyState L_GUI;
		HIDKeyboard_KeyState R_CTRL;
		HIDKeyboard_KeyState R_ALT;
		HIDKeyboard_KeyState R_SHIFT;
		HIDKeyboard_KeyState R_GUI;
		uint8_t key1;
		uint8_t key2;
		uint8_t key3;
		uint8_t key4;
		uint8_t key5;
		uint8_t key6;
	} HIDKeyboard;

	uint8_t HIDKeyboard_toKeyHex(char c);
	void HIDKeyboard_Init(HIDKeyboard* HIDkeyboard, USBD_HandleTypeDef* handler);
	void _HIDKeyboard_releaseKey(HIDKeyboard* HIDkeyboard);
	void _HIDKeyboard_pressKey(HIDKeyboard* HIDkeyboard);
	void _HIDKeyboard_pressAndReleaseKey(HIDKeyboard* HIDkeyboard);
	int8_t HIDKeyboard_sendInput(HIDKeyboard* HIDkeyboard, HIDKeyboard_Key firstButton, HIDKeyboard_Key secondButton, uint8_t thirdButton, int8_t releaseButtons);




#endif /* INC_HIDKEYBOARD_H_ */
