/**
 * ,---------,       ____  _ __
 * |  ,-^-,  |      / __ )(_) /_______________ _____  ___
 * | (  O  ) |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * | / ,--´  |    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *    +------`   /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * Crazyflie control firmware
 *
 * Copyright (C) 2019 Bitcraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, in version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * hello_world.c - App layer application of a simple hello world debug print every
 *   2 seconds.
 */


#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "app.h"

#include "FreeRTOS.h"
#include "task.h"

#include "stm32f4xx_gpio.h"

#define DEBUG_MODULE "HELLOWORLD"
#include "debug.h"


void appMain() {
  DEBUG_PRINT("Waiting for activation ...\n");

  GPIO_InitTypeDef gpio_init_struct = {
    .GPIO_Pin = GPIO_Pin_4,
    .GPIO_Mode = GPIO_Mode_OUT,
    .GPIO_Speed = GPIO_Low_Speed,
    .GPIO_OType = GPIO_OType_PP,
    .GPIO_PuPd = GPIO_PuPd_NOPULL
  };

  GPIO_Init(GPIOB, &gpio_init_struct);

  uint8_t counter = 0;
  while(1) {
    vTaskDelay(M2T(2000));

    // Toggle GPIO
    counter++;
    GPIO_WriteBit(GPIOB, GPIO_Pin_4, counter % 2);
  }
}
