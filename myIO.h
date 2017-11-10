/*
 * myIO.h
 *
 *  Created on: Jun 20, 2015
 *      Author: NhatTan
 */

#ifndef QUADROTOR_CURRENT_WORK_MYIO_H_
#define QUADROTOR_CURRENT_WORK_MYIO_H_

void myIO_init();

#define LED_RED		GPIO_PIN_1
#define LED_GREEN	GPIO_PIN_3
#define LED_BLUE	GPIO_PIN_2
#define ALL_OFF		2

extern void led(uint8_t led_color,uint8_t state);

extern volatile uint8_t toggle_led[3];

#endif /* QUADROTOR_CURRENT_WORK_MYIO_H_ */
