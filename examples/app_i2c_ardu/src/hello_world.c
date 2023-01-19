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

#include "i2c_drv.h"
#include "i2cdev.h"

#define DEBUG_MODULE "HELLOWORLD"
#include "debug.h"


void appMain() {
  DEBUG_PRINT("Waiting for activation ...\n");

  // Init i2c device to exposed bus
  i2cdrvInit(&deckBus);

  uint8_t slave_addr = 0x6;
  uint8_t msg_len = 5;

  uint8_t tx_buf[] = {0x0, 0x1, 0x2, 0x3, 0x4};
  uint8_t rx_buf[] = {0x0, 0x0, 0x0 ,0x0, 0x0};

  // Indicate that communication between cfclient and crazyflie works
  DEBUG_PRINT("Hello World!\n");

  bool success = false;
  while(1) {
    // I2C write
    success = i2cdevWrite(&deckBus, slave_addr, msg_len, tx_buf);
    if(success) {
      DEBUG_PRINT("Message transfered successfully\n");
    }
    else {
      DEBUG_PRINT("ERROR transmitting I2C message\n");
    }
    vTaskDelay(M2T(2000));

    // I2C read
    success = i2cdevRead(&deckBus, slave_addr, msg_len, rx_buf);
    if(success) {
      DEBUG_PRINT("Received data from slave: \n");
      for(uint8_t i = 0; i < msg_len; i++) {
        DEBUG_PRINT("%d\n", rx_buf[i]);
      }
    }
    else {
      DEBUG_PRINT("ERROR receving data. \n");
    }
    vTaskDelay(M2T(2000));
  }
}
