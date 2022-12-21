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


// CTRL_MEAS
#define CTRL_MEAS   0x00F4
// Oversampling of temperature data control bits
#define OSRS_T_0    ( 1 << 5 )
#define OSRS_T_1    ( 1 << 6 )
#define OSRS_T_2    ( 1 << 7 )
// Oversampling of pressure data control bits
#define OSRS_P_0    ( 1 << 2 )
#define OSRS_P_1    ( 1 << 3 )
#define OSRS_P_2    ( 1 << 4 )
// Operating mode
#define OP_MODE_0   ( 1 << 0 )
#define OP_MODE_1   ( 1 << 1 )

// Temperature registers
#define TEMP_LSB    0xFB
#define TEMP_MSB    0xFA

// Device ID register
#define ID          0xD0


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
  GPIO_WriteBit(GPIOB, GPIO_Pin_4, 0); // Disable on init
  sleepus(1000);
  GPIO_WriteBit(GPIOB, GPIO_Pin_4, 1); // Disable on init

  spiBegin();

  uint8_t val = 0;
  // First read the id register
  register_read(*GPIOB, GPIO_Pin_4, ID, &val);
  DEBUG_PRINT("Read sensor id: %X and correct is 0x58\n", val);

  // Check mode
  register_read(*GPIOB, GPIO_Pin_4, CTRL_MEAS, &val);
  DEBUG_PRINT("Operation mode: %X\n", val & 0x03);

  // Normal mode with temperature measurement enabled
  uint8_t new_val = OP_MODE_0 | OP_MODE_1 | OSRS_T_0;

  // Wakeup from sleep (sensor after powerup is in sleep mode)
  register_write(*GPIOB, GPIO_Pin_4, CTRL_MEAS, new_val);

  // Confirm that writing was correct:
  register_read(*GPIOB, GPIO_Pin_4, CTRL_MEAS, &val);
  DEBUG_PRINT("After writing to the register, the read - back is %X\n", val);

  static uint8_t temperature_lsb, temperature_msb;
  static uint16_t temperature;
  while(1) {
    // Read the temperature in loop
    register_read(*GPIOB, GPIO_Pin_4, TEMP_MSB, &temperature_msb);
    register_read(*GPIOB, GPIO_Pin_4, TEMP_LSB, &temperature_lsb);
    temperature = ((uint16_t) temperature_msb << 8 ) | temperature_lsb;
    DEBUG_PRINT("Temperature read raw: %X\n", temperature);
    vTaskDelay(M2T(2000));
  }
}

// TODO: Get rid of timeouts: lookup https://github.com/adafruit/Adafruit_BusIO/blob/master/Adafruit_SPIDevice.cpp
// TODO: Add some prints to checkout the values that are modified in below functions
// TODO: Add handling of return bool value from spiExchange
// First send the full register addr without 7th bit (used as operation
// indicator RW=0 for write) that is control byte and then data byte

// Done:
// Mode is correct 00
// Mode 4 wire is correct
// Changed /CS order with spiBegin (mode select is after falling edge on cs and
// is determined on the idle state of the clock)
// Static buffers introduced
// Maybe we should write 2 bytes in one transmission instead of 1 by 1
static void register_write(
    const GPIO_TypeDef cs_gpio,
    const uint16_t cs_pin,
    uint8_t reg,
    uint8_t val) {
  // Not sure how necessary it is but in src/drivers/src/lh_flasher.c it was
  // done this way
  static uint8_t tx_buff[2], rx_buff[2];

  reg |= RW_BIT; // Set operation indicator bit as write

  tx_buff[0] = reg;
  tx_buff[1] = val;

  rx_buff[0] = 0;
  rx_buff[0] = 0;

  GPIO_WriteBit(GPIOB, GPIO_Pin_4, 0);
  spiBeginTransaction(SPI_BAUDRATE_2MHZ);

  spiExchange(2, tx_buff, rx_buff); // Send value

  GPIO_WriteBit(GPIOB, GPIO_Pin_4, 1);
  spiEndTransaction();
}

static void register_read(
    const GPIO_TypeDef cs_gpio,
    const uint16_t cs_pin,
    uint8_t reg,
    uint8_t *val){
  static uint8_t tx_buff[2], rx_buff[2];
  // clear operation indicator bit - read
  reg &= ~(RW_BIT);

  tx_buff[0] = reg;
  tx_buff[1] = 0;

  rx_buff[0] = 0;
  rx_buff[1] = 0;

  GPIO_WriteBit(GPIOB, GPIO_Pin_4, 0);
  spiBeginTransaction(SPI_BAUDRATE_2MHZ);

  spiExchange(2, tx_buff, rx_buff);

  GPIO_WriteBit(GPIOB, GPIO_Pin_4, 1);
  spiEndTransaction();

  *val = rx_buff[1];
}



