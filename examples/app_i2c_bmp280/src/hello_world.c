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


// Import the necessary libraries
#include "bstdr_comm_support.h"
#include "bmp280.h"

// Initialize the BMP280
static struct bmp280_t bmp280;

void appMain(void) {
  DEBUG_PRINT("Waiting for activation ...\n");

  bstdr_ret_t rslt;

  // Using api from crazyflie as an example: src/hal/src/sensors_bosch.c
  // Initialize the BMP280, set func ptrs to bosch sensortec implementations
  bmp280.bus_read = bstdr_burst_read;
  bmp280.bus_write = bstdr_burst_write;
  bmp280.delay_ms = bstdr_ms_delay;
  bmp280.dev_addr = BMP280_I2C_ADDRESS2; //0x77 SDO HIGH
  rslt = bmp280_init(&bmp280);

  if(rslt == BSTDR_OK) {
    while(1) {
      // Read the uncompensated temperature from the BMP280
      uint32_t a = 0;
      rslt = bmp280_read_uncomp_temperature(&a);
      if(rslt == BSTDR_OK) {
        DEBUG_PRINT("Temperature: %lu\n", a);
      }
      else {
        DEBUG_PRINT("ERROR: Could not read temperature %d\n", rslt);
      }
    }
  }
  else {
    DEBUG_PRINT("ERROR: Could not initialize I2C interface %d\n", rslt);
  }
}
