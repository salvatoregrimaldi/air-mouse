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

#include "HIDKeyboard.h"
#include "usbd_customhid.h"

/**
* @brief Funzione che converte un carattere compreso tra 'a' e 'z' nel relativo codice esadecimale della tabella HID.
* @param argument: char c
* @retval uint8_t
*/
uint8_t HIDKeyboard_toKeyHex(char c){
	if(c>='a' && c<='z')
		return c - 'a' + 0x4;
	if(c>='1' && c<='9')
		return c - '1'+ 0x1e;
	if(c=='0')
		return 0x27;
	if(c=='\n')
		return 0x28;
	if(c==' ')
		return 0x2c;
	if(c=='\t')
		return 0x2b;
	return 0x00;
}

/**
* @brief Funzione che inizializza la struttura dati della tastiera mettendo tutti i tasti a 0 o a HIDKeyboard_ButtonReleased.
* @param argument: HIDKeyboard* HIDkeyboard, USBD_HandleTypeDef* handler
* @retval None
*/
void HIDKeyboard_Init(HIDKeyboard* HIDkeyboard, USBD_HandleTypeDef* handler){
		HIDkeyboard->handler=handler;
		HIDkeyboard->L_CTRL = HIDKeyboard_ButtonReleased;
		HIDkeyboard->L_ALT = HIDKeyboard_ButtonReleased;
		HIDkeyboard->L_SHIFT = HIDKeyboard_ButtonReleased;
		HIDkeyboard->L_GUI = HIDKeyboard_ButtonReleased;
		HIDkeyboard->R_CTRL = HIDKeyboard_ButtonReleased;
		HIDkeyboard->R_ALT = HIDKeyboard_ButtonReleased;
		HIDkeyboard->R_SHIFT = HIDKeyboard_ButtonReleased;
		HIDkeyboard->R_GUI = HIDKeyboard_ButtonReleased;
		HIDkeyboard->key1 = 0;
		HIDkeyboard->key2 = 0;
		HIDkeyboard->key3 = 0;
		HIDkeyboard->key4 = 0;
		HIDkeyboard->key5 = 0;
		HIDkeyboard->key6 = 0;
	}

/**
* @brief Funzione che mette tutti i tasti a 0 o a HIDKeyboard_ButtonReleased e invia un report HID.
* @param argument: HIDKeyboard* HIDkeyboard
* @retval None
*/
void _HIDKeyboard_releaseKey(HIDKeyboard* HIDkeyboard){
	HIDkeyboard->L_CTRL = HIDKeyboard_ButtonReleased;
	HIDkeyboard->L_ALT = HIDKeyboard_ButtonReleased;
	HIDkeyboard->L_SHIFT = HIDKeyboard_ButtonReleased;
	HIDkeyboard->L_GUI = HIDKeyboard_ButtonReleased;
	HIDkeyboard->R_CTRL = HIDKeyboard_ButtonReleased;
	HIDkeyboard->R_ALT = HIDKeyboard_ButtonReleased;
	HIDkeyboard->R_SHIFT = HIDKeyboard_ButtonReleased;
	HIDkeyboard->R_GUI = HIDKeyboard_ButtonReleased;
	HIDkeyboard->key1 = 0;
	HIDkeyboard->key2 = 0;
	HIDkeyboard->key3 = 0;
	HIDkeyboard->key4 = 0;
	HIDkeyboard->key5 = 0;
	HIDkeyboard->key6 = 0;

	_HIDKeyboard_pressKey(HIDkeyboard);
}

/**
* @brief Funzione che costruisce il buffer di invio usando i valori della struttura dati della tastiera e poi manda il report HID.
* @param argument: HIDKeyboard* HIDkeyboard
* @retval None
*/
void _HIDKeyboard_pressKey(HIDKeyboard* HIDkeyboard){
	uint8_t buff[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; /* lunghezza del report */

	buff[0] = 0x01; /* Id della tastiera */

	buff[1] = 0;
	buff[1] |= HIDkeyboard->L_CTRL 	<< 0;	/* Bit 0 */
	buff[1] |= HIDkeyboard->L_SHIFT << 1;	/* Bit 1 */
	buff[1] |= HIDkeyboard->L_ALT 	<< 2;	/* Bit 2 */
	buff[1] |= HIDkeyboard->L_GUI 	<< 3;	/* Bit 3 */
	buff[1] |= HIDkeyboard->R_CTRL 	<< 4;	/* Bit 4 */
	buff[1] |= HIDkeyboard->R_SHIFT << 5;	/* Bit 5 */
	buff[1] |= HIDkeyboard->R_ALT 	<< 6;	/* Bit 6 */
	buff[1] |= HIDkeyboard->R_GUI 	<< 7;	/* Bit 7 */

	/* Bit di padding */
	buff[2] = 0x00;

	buff[3] = HIDkeyboard->key1;
	buff[4] = HIDkeyboard->key2;
	buff[5] = HIDkeyboard->key3;
	buff[6] = HIDkeyboard->key4;
	buff[7] = HIDkeyboard->key5;
	buff[8] = HIDkeyboard->key6;

	USBD_CUSTOM_HID_SendReport(HIDkeyboard->handler, buff, 9);
}

/**
* @brief Funzione che manda in rapida successione l'input di pressione e poi quello di rilascio dei tasti.
* @param argument: HIDKeyboard* HIDkeyboard
* @retval None
*/
void _HIDKeyboard_pressAndReleaseKey(HIDKeyboard* HIDkeyboard){
	_HIDKeyboard_pressKey(HIDkeyboard);
	HAL_Delay(15);
	_HIDKeyboard_releaseKey(HIDkeyboard);
}

/**
* @brief Funzione che modifica i valori della struttura dati della tastiera a seconda dei parametri ricevuti che identificano i tasti.
* @param argument: HIDKeyboard* HIDkeyboard, HIDKeyboard_Key firstButton, HIDKeyboard_Key secondButton, uint8_t thirdButton, int8_t releaseButtons
* @retval int8_t
*/
int8_t HIDKeyboard_sendInput(HIDKeyboard* HIDkeyboard, HIDKeyboard_Key firstButton, HIDKeyboard_Key secondButton, uint8_t thirdButton, int8_t releaseButtons){
	if(firstButton==secondButton && firstButton!=HIDKeyboard_NULL)
		return -1;

	if(thirdButton<0x00 || thirdButton>0x8c)
		return -2;

	HIDkeyboard->L_CTRL = HIDKeyboard_ButtonReleased;
	HIDkeyboard->L_ALT = HIDKeyboard_ButtonReleased;
	HIDkeyboard->L_SHIFT = HIDKeyboard_ButtonReleased;
	HIDkeyboard->L_GUI = HIDKeyboard_ButtonReleased;
	HIDkeyboard->R_CTRL = HIDKeyboard_ButtonReleased;
	HIDkeyboard->R_ALT = HIDKeyboard_ButtonReleased;
	HIDkeyboard->R_SHIFT = HIDKeyboard_ButtonReleased;
	HIDkeyboard->R_GUI = HIDKeyboard_ButtonReleased;

	switch(firstButton){
		case HIDKeyboard_NULL:
			break;
		case HIDKeyboard_AltL:
			HIDkeyboard->L_ALT=HIDKeyboard_ButtonPressed;
			break;
		case HIDKeyboard_CtrlL:
			HIDkeyboard->L_CTRL=HIDKeyboard_ButtonPressed;
			break;
		case HIDKeyboard_ShiftL:
			HIDkeyboard->L_SHIFT=HIDKeyboard_ButtonPressed;
			break;
		case HIDKeyboard_WinL:
			HIDkeyboard->L_GUI=HIDKeyboard_ButtonPressed;
			break;
		case HIDKeyboard_AltR:
			HIDkeyboard->R_ALT=HIDKeyboard_ButtonPressed;
			break;
		case HIDKeyboard_CtrlR:
			HIDkeyboard->R_CTRL=HIDKeyboard_ButtonPressed;
			break;
		case HIDKeyboard_ShiftR:
			HIDkeyboard->R_SHIFT=HIDKeyboard_ButtonPressed;
			break;
		case HIDKeyboard_WinR:
			HIDkeyboard->R_GUI=HIDKeyboard_ButtonPressed;
			break;
	}

	switch(secondButton){
		case HIDKeyboard_NULL:
			break;
		case HIDKeyboard_AltL:
			HIDkeyboard->L_ALT=HIDKeyboard_ButtonPressed;
			break;
		case HIDKeyboard_CtrlL:
			HIDkeyboard->L_CTRL=HIDKeyboard_ButtonPressed;
			break;
		case HIDKeyboard_ShiftL:
			HIDkeyboard->L_SHIFT=HIDKeyboard_ButtonPressed;
			break;
		case HIDKeyboard_WinL:
			HIDkeyboard->L_GUI=HIDKeyboard_ButtonPressed;
			break;
		case HIDKeyboard_AltR:
			HIDkeyboard->R_ALT=HIDKeyboard_ButtonPressed;
			break;
		case HIDKeyboard_CtrlR:
			HIDkeyboard->R_CTRL=HIDKeyboard_ButtonPressed;
			break;
		case HIDKeyboard_ShiftR:
			HIDkeyboard->R_SHIFT=HIDKeyboard_ButtonPressed;
			break;
		case HIDKeyboard_WinR:
			HIDkeyboard->R_GUI=HIDKeyboard_ButtonPressed;
			break;
	}

	HIDkeyboard->key1 = thirdButton;
	if(releaseButtons==1)
		_HIDKeyboard_pressAndReleaseKey(HIDkeyboard);
	else
		_HIDKeyboard_pressKey(HIDkeyboard);

	return 0;
}





