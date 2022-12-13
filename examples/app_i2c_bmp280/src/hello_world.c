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
#include "i2cdev.h"
#include "bmp280.h"

// Set the I2C address of the BMP280
#define BMP280_I2C_ADDRESS 0x77

// Initialize the BMP280
bmp280_t bmp280;

int appMain(void) {
  DEBUG_PRINT("Waiting for activation ...\n");

  // Initialize the I2C bus
  i2cdevInit(I2C1_DEV);

  // Initialize the BMP280
  bmp280_init(&bmp280, BMP280_I2C_ADDRESS);


  while(1) {
  // Read the temperature from the BMP280
  float temperature = bmp280_readTemperature(&bmp280);

  // Print the temperature
  printf("Temperature: %.2f C\n", temperature);
  }

  return 0;
}
