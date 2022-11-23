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

#include "uart1.h"

#include "FreeRTOS.h"
#include "task.h"

#define DEBUG_MODULE "HELLOWORLD"
#include "debug.h"


void appMain() {
  DEBUG_PRINT("Waiting for activation ...\n");

  while(1) {
    vTaskDelay(M2T(2000));

    // UART
    uart1Init(9600);
    uint8_t buffer = 0x0;
    bool gotData = uart1GetDataWithTimeout(&buffer, 1000);
    if(gotData) {
      DEBUG_PRINT("Got letter over UART: %c\n", (char)buffer);
      uint8_t buffer_to_capital = buffer - 32;
      DEBUG_PRINT("As capital letter: %c \n", (char)buffer_to_capital);
      uart1SendData(1, &buffer_to_capital);
    }
    else {
      DEBUG_PRINT("Data not received\n");
    }
  }
}
