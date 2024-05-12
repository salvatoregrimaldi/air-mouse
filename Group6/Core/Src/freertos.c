/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include <RTC_DS3231.h>
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "HIDKeyboard.h"
#include "HIDMouse.h"
#include "GY-521.h"
#include "usb_device.h"
#include "GestureRecognition.h"
#include "usart.h"
#include "UART.h"

#include "fatfs.h"
#include "spi.h"
#include "gpio.h"
#include "SDReader.h"

#include "Parameters.h"

#define FLAGS_MSK1 0x00000001U

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for cursorTask */
osThreadId_t cursorTaskHandle;
const osThreadAttr_t cursorTask_attributes = {
  .name = "cursorTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for gestureTask */
osThreadId_t gestureTaskHandle;
const osThreadAttr_t gestureTask_attributes = {
  .name = "gestureTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for calibrationTask */
osThreadId_t calibrationTaskHandle;
const osThreadAttr_t calibrationTask_attributes = {
  .name = "calibrationTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for SDTask */
osThreadId_t SDTaskHandle;
const osThreadAttr_t SDTask_attributes = {
  .name = "SDTask",
  .stack_size = 2000 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for mutexHID */
osMutexId_t mutexHIDHandle;
const osMutexAttr_t mutexHID_attributes = {
  .name = "mutexHID"
};

osMutexId_t mutexI2CHandle;
const osMutexAttr_t mutexI2C_attributes = {
  .name = "mutexI2C"
};

osMutexId_t mutexUARTHandle;
const osMutexAttr_t mutexUART_attributes = {
  .name = "mutexUART"
};

osMutexId_t mutexGestureHandle;
const osMutexAttr_t mutexGesture_attributes = {
  .name = "mutexGesture"
};
/* Definitions for SDQueue */
osMessageQueueId_t SDQueueHandle;
const osMessageQueueAttr_t SDQueue_attributes = {
  .name = "SDQueue"
};

osEventFlagsId_t evt_id;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void cursorHandler(void *argument);
void gestureHandler(void *argument);
void SDHandler(void *argument);
void calibrationHandler(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

uint32_t MAP(uint32_t au32_IN, uint32_t au32_INmin, uint32_t au32_INmax,
		uint32_t au32_OUTmin, uint32_t au32_OUTmax) {
	return ((((au32_IN - au32_INmin) * (au32_OUTmax - au32_OUTmin))
			/ (au32_INmax - au32_INmin)) + au32_OUTmin);
}


DS3231 ds3231;
GY521 gy521Move;
GY521 gy521Gest;
HIDKeyboard virtualKeyBoard;
HIDMouse virtualMouse;
char out[50];
FATFS FatFs;
FRESULT fres;
SdReader sdCard;
UARTCom uartPort;
Parameters param;
int valx = 0, valy = 0, count = 0, x = 0;
double pitch, roll, oldPitch, oldRoll;

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
    GY521_Init(&gy521Gest, &hi2c1, GY521_DEF_ADDR2);
    DS3231_Init(&ds3231, &hi2c3, DS3231_DEF_ADDR);
    GY521_Init(&gy521Move, &hi2c1, GY521_DEF_ADDR1);
    UARTCom_Init(&uartPort, &huart2);
    Param_Init(&param);
	HIDKeyboard_Init(&virtualKeyBoard, &hUsbDeviceFS);
	HIDMouse_Init(&virtualMouse, &hUsbDeviceFS);

	UARTCom_printf(&uartPort, "Initializing\n");

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of mutexHID */
  mutexHIDHandle = osMutexNew(&mutexHID_attributes);
  mutexI2CHandle = osMutexNew(&mutexI2C_attributes);
  mutexUARTHandle = osMutexNew(&mutexUART_attributes);
  mutexGestureHandle = osMutexNew(&mutexGesture_attributes);

  evt_id = osEventFlagsNew(NULL);

  /* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of SDQueue */
  SDQueueHandle = osMessageQueueNew (10, 100, &SDQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of cursorTask */
  cursorTaskHandle = osThreadNew(cursorHandler, NULL, &cursorTask_attributes);

  /* creation of gestureTask */
  gestureTaskHandle = osThreadNew(gestureHandler, NULL, &gestureTask_attributes);

  /* creation of SDTask */
  SDTaskHandle = osThreadNew(SDHandler, NULL, &SDTask_attributes);

  /* creation of calibrationTask */
  calibrationTaskHandle = osThreadNew(calibrationHandler, NULL, &calibrationTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
	/* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */
	/* Infinite loop */
	for (;;) {
		osDelay(1);
	}
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_cursorHandler */
/**
 * @brief Funzione che implementa il thread di movimento del cursore, leggendo i valori del sensore GY-521 dedicato al movimento e calcolando la velocità in base all'angolo corrente, per poi
 * mandare un report HID con la libreria HIDMouse.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_movementHandler */
void cursorHandler(void *argument)
{
  /* USER CODE BEGIN cursorHandler */
	/* Infinite loop */
	for (;;) {

		osMutexAcquire(mutexGestureHandle, osWaitForever);
		GY521_readAll(&gy521Move);
		osMutexRelease(mutexGestureHandle);
		pitch = GY521_getPitch(&gy521Move);
		roll = GY521_getRoll(&gy521Move);

		if (oldPitch == pitch && oldRoll == roll)
			count++;
		else
			count = 0;

		oldPitch = pitch;
		oldRoll = roll;

		if (count == 20) {
			osMutexAcquire(mutexGestureHandle, osWaitForever);
			GY521_Init(&gy521Move, &hi2c1, GY521_DEF_ADDR1);
			osMutexRelease(mutexGestureHandle);


			count = 0;
		}

		if(pitch > param.maxSpeedAngleX)
			valx = -param.maxSpeedValue;
		else if(pitch < -param.maxSpeedAngleX)
			valx = param.maxSpeedValue;
		else if (pitch > param.deadZone) {
			valx = -MAP((int) pitch, param.deadZone, param.maxSpeedAngleX, 1, param.maxSpeedValue);
		} else if (pitch < -param.deadZone) {
			valx = MAP((int) -pitch, param.deadZone, param.maxSpeedAngleX, 1, param.maxSpeedValue);
		} else
			valx = 0;

		if(roll > param.maxSpeedAngleY)
			valy = -param.maxSpeedValue;
		else if(roll < -param.maxSpeedAngleY)
			valy = param.maxSpeedValue;
		if (roll > param.deadZone) {
			valy = -MAP((int) roll, param.deadZone, param.maxSpeedAngleY, 1, param.maxSpeedValue);
		} else if (roll < -param.deadZone) {
			valy = MAP((int) -roll, param.deadZone, param.maxSpeedAngleY, 1, param.maxSpeedValue);
		} else
			valy = 0;

		if (valx != 0 || valy != 0) {
			osMutexAcquire(mutexHIDHandle, osWaitForever);
			HIDMouse_sendInput(&virtualMouse, valx, valy, 0, HIDMouse_NULL, 0);
			osDelay(15);
			osMutexRelease(mutexHIDHandle);
		} else
			osDelay(10);

		if(osMutexGetOwner(mutexI2CHandle)==calibrationTaskHandle){
			osMutexAcquire(mutexI2CHandle, osWaitForever);
			osMutexRelease(mutexI2CHandle);
		}

	}
  /* USER CODE END cursorHandler */
}

/* USER CODE BEGIN Header_gestureHandler */
/**
 * @brief Funzione che implementa il thread di gestione delle gesture: legge dai sensori GY-521 e quando i valori di accelerazione di almeno uno dei sensori superano una certa soglia,
 * richiama la funzione di riconoscimento delle gesture. Successivamente, effettua una specifica azione di invio di HID report a seconda della gesture identificata.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_gestureHandler */
void gestureHandler(void *argument)
{
  /* USER CODE BEGIN gestureHandler */
	/* Infinite loop */

	for (;;) {

		osMutexAcquire(mutexGestureHandle, osWaitForever);
		GY521_readAll(&gy521Gest);
		GY521_readAll(&gy521Move);
		osMutexRelease(mutexGestureHandle);

		float aSumGest = fabs(gy521Gest.Ax) + fabs(gy521Gest.Ay)
				+ fabs(gy521Gest.Az);
		float aSumMove = fabs(gy521Move.Ax) + fabs(gy521Move.Ay)
						+ fabs(gy521Move.Az);
		int8_t res = -1;

		if ((aSumGest >= param.accThresh)||(aSumMove >= param.accThresh)){
			osMutexAcquire(mutexGestureHandle, osWaitForever);
			res = startRecognition(&gy521Move, &gy521Gest, 25);
			UARTCom_printf(&uartPort, "%d\n", res);
			osMutexRelease(mutexGestureHandle);
		}

		if (res != -1) {
			osMutexAcquire(mutexHIDHandle, osWaitForever);
			if (res == 0) {
				HIDMouse_sendInput(&virtualMouse, 0, 0, 0, HIDMouse_Left, 1);
			} else if (res == 1) {
				HIDMouse_sendInput(&virtualMouse, 0, 0, 0, HIDMouse_Left, 1);
				osDelay(15);
				HIDMouse_sendInput(&virtualMouse, 0, 0, 0, HIDMouse_Left, 1);
			} else if (res == 2) {
				HIDMouse_sendInput(&virtualMouse, 0, 0, 0, HIDMouse_Right, 1);
			} else if (res == 3) {
				HIDKeyboard_sendInput(&virtualKeyBoard, HIDKeyboard_CtrlR, HIDKeyboard_NULL, HIDKeyboard_NULL, 0);
				osDelay(15);
				HIDMouse_sendInput(&virtualMouse, 0, 0, 10, HIDMouse_NULL, 0);
				osDelay(15);
				HIDKeyboard_sendInput(&virtualKeyBoard, HIDKeyboard_NULL, HIDKeyboard_NULL, HIDKeyboard_NULL, 0);
			} else if (res == 4) {
				HIDKeyboard_sendInput(&virtualKeyBoard, HIDKeyboard_CtrlR, HIDKeyboard_NULL, HIDKeyboard_NULL, 0);
				osDelay(15);
				HIDMouse_sendInput(&virtualMouse, 0, 0, -10, HIDMouse_NULL, 0);
				osDelay(15);
				HIDKeyboard_sendInput(&virtualKeyBoard, HIDKeyboard_NULL, HIDKeyboard_NULL, HIDKeyboard_NULL, 0);
			} else if (res == 5) {
				HIDMouse_sendInput(&virtualMouse, 0, 0, 10, HIDMouse_NULL, 0);
			} else if (res == 6) {
				HIDMouse_sendInput(&virtualMouse, 0, 0, -10, HIDMouse_NULL, 0);
			} else if (res == 7) {
				HIDKeyboard_sendInput(&virtualKeyBoard, HIDKeyboard_AltL, HIDKeyboard_NULL, 0x2B, 0);
				osDelay(20);
				HIDKeyboard_sendInput(&virtualKeyBoard, HIDKeyboard_NULL, HIDKeyboard_NULL, HIDKeyboard_NULL, 0);
			}
			osDelay(15);

			osMutexRelease(mutexHIDHandle);
			char msg[100];
			sprintf(msg, "Gesture %d", res);
			osMessageQueuePut(SDQueueHandle, msg, 0, osWaitForever);
		}

		osDelay(10);


		if(osMutexGetOwner(mutexI2CHandle)==calibrationTaskHandle){
			osMutexAcquire(mutexI2CHandle, osWaitForever);
			osMutexRelease(mutexI2CHandle);
		}
	}

  /* USER CODE END gestureHandler */
}

/* USER CODE BEGIN Header_SDHandler */
/**
 * @brief Funzione che implementa il thread di scrittura dei log. Legge dalla queue su cui scrive il thread delle gesture ogni volta che viene effettuata una gesture, e scrive sul file di log
 * informazioni relative a tale gesture e alla data e ora in cui è stata eseguita.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_SDHandler */
void SDHandler(void *argument)
{
  /* USER CODE BEGIN SDHandler */
	/* Infinite loop */

	fres = SdReader_Init(&sdCard, &FatFs, "LOG.txt");
	if (fres != FR_OK) {
		osMutexAcquire(mutexUARTHandle, osWaitForever);
		UARTCom_printf(&uartPort, "f_mount or f_open error (%d)\n",fres);
		osMutexRelease(mutexUARTHandle);
		while (1)
			;
	}else{
		osMutexAcquire(mutexUARTHandle, osWaitForever);
		UARTCom_printf(&uartPort, "SDCard OK\n");
		osMutexRelease(mutexUARTHandle);
	}

	char message[100];
	DateTime date;
	for (;;) {
		osMessageQueueGet(SDQueueHandle, message, 0, osWaitForever);
		date = DS3231_getDateTime(&ds3231);
		fres = SdReader_writeOnFile(&sdCard, date, message);

		if (fres == FR_OK) {
			osMutexAcquire(mutexUARTHandle, osWaitForever);
			UARTCom_printf(&uartPort, "Scrittura avvenuta con successo %s\n",message);
			osMutexRelease(mutexUARTHandle);
		} else {
			osMutexAcquire(mutexUARTHandle, osWaitForever);
			UARTCom_printf(&uartPort, "f_sync error (%i)\n",fres);
			osMutexRelease(mutexUARTHandle);
		}

		osDelay(1);

		if(osMutexGetOwner(mutexI2CHandle)==calibrationTaskHandle){
			osMutexAcquire(mutexI2CHandle, osWaitForever);
			osMutexRelease(mutexI2CHandle);
		}
	}

  /* USER CODE END SDHandler */
}

/* USER CODE BEGIN Header_calibrationHandler */
/**
* @brief Funzione che implementa il task di calibrazione. Stampa a schermo il menu dei comandi e attende una callback UART per la ricezione di una stringa. A seconda della stringa ricevuta,
* controlla che ci sia un comando corrispondente e in tal caso avvia la procedura di calibrazione per quel comando.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_calibrationHandler */
void calibrationHandler(void *argument)
{
  /* USER CODE BEGIN calibrationHandler */
  /* Infinite loop */

  for(;;)
  {
	  osMutexAcquire(mutexUARTHandle, osWaitForever);
	  UARTCom_printf(&uartPort, "Inserire uno dei seguenti comandi per la calibrazione:\n");
	  UARTCom_printf(&uartPort, "\t deadzones -> imposta le deadzones orizzontali e verticali\n");
	  UARTCom_printf(&uartPort, "\t maxSpeed -> imposta la velocità massima del cursore\n");
	  UARTCom_printf(&uartPort, "\t maxSpeedAngleX -> imposta l'angolo X corrispondente alla velocità massima del cursore\n");
	  UARTCom_printf(&uartPort, "\t maxSpeedAngleY -> imposta l'angolo Y corrispondente alla velocità massima del cursore\n");
	  UARTCom_printf(&uartPort, "\t accThresh -> imposta la soglia di accelerazione che è necessario superare per avviare la rilevazione di una gesture\n");
	  UARTCom_printf(&uartPort, "\t clock -> imposta data e ora del Real Time Clock\n");
	  osMutexRelease(mutexUARTHandle);
	  HAL_UART_Receive_IT(&huart2,(uint8_t*) &out[0], sizeof(char));
	  osEventFlagsWait(evt_id, FLAGS_MSK1, osFlagsWaitAny, osWaitForever);
	  if(strcmp(out,"deadzones")==0){
	  		  osMutexAcquire(mutexUARTHandle, osWaitForever);
	  		  osMutexAcquire(mutexI2CHandle, osWaitForever);
	  		  osDelay(1000);
	  		  setDeadZones(&param, &gy521Move, &uartPort);
	  		  UARTCom_printf(&uartPort, "Il parametro delle deadzones correttamente impostato è: %.2f\n", param.deadZone);
	  		  osMutexRelease(mutexI2CHandle);
	  		  osMutexRelease(mutexUARTHandle);
	  }else if(strcmp(out,"maxSpeed")==0){
		  osMutexAcquire(mutexUARTHandle, osWaitForever);
		  osMutexAcquire(mutexI2CHandle, osWaitForever);
		  osDelay(1000);
		  setMaxSpeedValue(&param, &uartPort);
		  UARTCom_printf(&uartPort, "Il parametro della velocità massima correttamente impostato è: %d\n", param.maxSpeedValue);
		  osMutexRelease(mutexI2CHandle);
		  osMutexRelease(mutexUARTHandle);
	  }else if(strcmp(out,"maxSpeedAngleX")==0){
		  osMutexAcquire(mutexUARTHandle, osWaitForever);
		  osMutexAcquire(mutexI2CHandle, osWaitForever);
		  osDelay(1000);
		  setMaxSpeedAngle(&param, &gy521Move, &uartPort, 1);
		  UARTCom_printf(&uartPort, "Il parametro maxSpeedAngleX correttamente impostato è: %.2f\n", param.maxSpeedAngleX);
		  osMutexRelease(mutexI2CHandle);
		  osMutexRelease(mutexUARTHandle);
	  }else if(strcmp(out,"maxSpeedAngleY")==0){
		  osMutexAcquire(mutexUARTHandle, osWaitForever);
		  osMutexAcquire(mutexI2CHandle, osWaitForever);
		  osDelay(1000);
		  setMaxSpeedAngle(&param, &gy521Move, &uartPort, 0);
		  UARTCom_printf(&uartPort, "Il parametro maxSpeedAngleY correttamente impostato è: %.2f\n", param.maxSpeedAngleY);
		  osMutexRelease(mutexI2CHandle);
		  osMutexRelease(mutexUARTHandle);
	  }else if(strcmp(out,"accThresh")==0){
		  osMutexAcquire(mutexUARTHandle, osWaitForever);
		  osMutexAcquire(mutexI2CHandle, osWaitForever);
		  osDelay(1000);
		  setAccThresh(&param, &uartPort);
		  osMutexRelease(mutexI2CHandle);
		  osMutexRelease(mutexUARTHandle);
	  }else if(strcmp(out,"clock")==0){
		  osMutexAcquire(mutexUARTHandle, osWaitForever);
		  osMutexAcquire(mutexI2CHandle, osWaitForever);
		  osDelay(1000);
		  setDateTime(&param, &ds3231, &uartPort);
		  osMutexRelease(mutexI2CHandle);
		  osMutexRelease(mutexUARTHandle);
	  }else{
		  osMutexAcquire(mutexUARTHandle, osWaitForever);
		  UARTCom_printf(&uartPort, "Comando non riconosciuto\n");
		  osMutexRelease(mutexUARTHandle);
	  }
	  osDelay(1);


  }
  /* USER CODE END calibrationHandler */
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	if(out[x]=='\n' || x>=99){
		out[x]='\0';
		x=0;
		osEventFlagsSet(evt_id, FLAGS_MSK1);
	}
	else{
		x++;
		HAL_UART_Receive_IT(&huart2,(uint8_t*) &out[x], sizeof(char));
	}

}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

