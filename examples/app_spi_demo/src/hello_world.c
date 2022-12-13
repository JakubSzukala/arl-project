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

#define DEBUG_MODULE "HELLOWORLD"
#include "debug.h"

#include "deck_spi.h"
#include "sleepus.h"

/**
 * Remarks
 * - Double check the SPI mode of operation
 * - SPI1 (PA(5-7)) is exposed on the header
 * - This means, we can use the driver from src/deck/api/deck_spi.c it is hard coded for SPI1
 * - It will be the same api that we will use for decks (duh...)
 * - It is a little bit cringe, that it is called as 'deck' but is actually pretty generic
 * - If CSB is pulled down, the SPI interface is activated (BMP280)
 * - Write a function that will write a byte and then verify this write with a read
 * - But also add some temperature read for good measure
 * - CTRL_MEAS OSRS_T registers for temperature oversampling should be good
 *     Temperature meas skip - Pressure mesure skip - Normal mode
 *   - 000 000 11
 *   - Then output when reading the values should be fixed to 0x80000
 * - Maybe sequence could be:
 *   - After power on reset read the chip id should be 0x58
 *   - Set the normal mode (default after power on reset is sleep), with validation
 *   - Read temperature in loop to se if it changes
 * - src/drivers/src/pmw3901.c
 **/

#define RW_BIT    0x0080u

static void register_write(
    const GPIO_TypeDef cs_gpio,
    const uint16_t cs_pin,
    uint8_t reg,
    uint8_t val);
static void register_read(
    const GPIO_TypeDef cs_gpio,
    const uint16_t cs_pin,
    uint8_t reg,
    uint8_t *val);

void appMain() {
  DEBUG_PRINT("Waiting for activation ...\n");

  // Initialize GPIO pin as SPI /CS for BMP280
  GPIO_InitTypeDef gpio_init_struct = {
    .GPIO_Pin = GPIO_Pin_4,
    .GPIO_Mode = GPIO_Mode_OUT,
    .GPIO_Speed = GPIO_Low_Speed,
    .GPIO_OType = GPIO_OType_PP,
    .GPIO_PuPd = GPIO_PuPd_NOPULL
  };
  GPIO_Init(GPIOB, &gpio_init_struct);
  GPIO_WriteBit(GPIOB, GPIO_Pin_4, 1); // Disable on init

  
  while(1) {
    vTaskDelay(M2T(2000));
    DEBUG_PRINT("Hello World!\n");
  }
}

// First send the full register addr without 7th bit (used as operation
// indicator RW=0 for write) that is control byte and then data byte
static void register_write(
    const GPIO_TypeDef cs_gpio,
    const uint16_t cs_pin,
    uint8_t reg,
    uint8_t val) {
  reg |= RW_BIT; // Set operation indicator bit as write

  spiBeginTransaction(SPI_BAUDRATE_2MHZ);
  GPIO_WriteBit(GPIOB, GPIO_Pin_4, 0);

  sleepus(50);

  // this &reg, &reg is little bit weird, not sure how safe it is
  spiExchange(1, &reg, &reg); // Send control byte
  sleepus(50);
  spiExchange(1, &val, &val);
  sleepus(50);

  GPIO_WriteBit(GPIOB, GPIO_Pin_4, 1);
  spiEndTransaction();
  sleepus(200);
}

// Delays are the same as in pmw3901 example, but not sure if they are necessary
static void register_read(
    const GPIO_TypeDef cs_gpio,
    const uint16_t cs_pin,
    uint8_t reg,
    uint8_t *val){
  uint8_t dummy = 0;
  reg &= ~(RW_BIT); // clear operation indicator bit - read
  spiBeginTransaction(SPI_BAUDRATE_2MHZ);
  GPIO_WriteBit(GPIOB, GPIO_Pin_4, 0);

  sleepus(50);

  spiExchange(1, &reg, &reg);
  sleepus(500);
  spiExchange(1, &dummy, val);
  sleepus(50);

  GPIO_WriteBit(GPIOB, GPIO_Pin_4, 1);
  spiEndTransaction();
  sleepus(200);
}



