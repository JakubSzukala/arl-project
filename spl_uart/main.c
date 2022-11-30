/**
  ******************************************************************************
  * @file    USART/USART_Printf/main.c 
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 0 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup USART_Printf
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;
static uint8_t buffer[] = {
  'H', 'e', 'l', 'l', 'o', ' ', 'S', 'T', 'M', '3', '2', '!', '\n'
};

/* Private function prototypes -----------------------------------------------*/
static void USART_Config(void);
static void UsartSendData(uint32_t size, uint8_t* data, USART_TypeDef * usart);

static void Delay(__IO uint32_t nTime);

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
  
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       files (startup_stm32f40_41xxx.s/startup_stm32f427_437xx.s/startup_stm32f429_439xx.s)
       before to branch to application main. 
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file.
     */

  // Init Systick to count in ms according to example (SystemCoreClock values match)
  if(SysTick_Config(SystemCoreClock / 1000))
  {
    // Error
    while(1);
  }
  USART_Config();

  while (1)
  {
    Delay(1000);
    UsartSendData(sizeof(buffer), buffer,USART3);
  }
}

/**
  * @brief  Configures the USART Peripheral.
  * @param  None
  * @retval None
  */
static void USART_Config(void)
{
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* USARTx configured as follows:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  // Enable GPIO clock
  RCC_AHB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

  // Enable USART clock
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

  // Remap uart pins TODO: will it be an issue in Renode?
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3); // Tx
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3); // Rx

  /* Configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF;

  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Configure USART Rx as alternate function  */
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_11;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  // Initialize and enable
  USART_Init(USART3, &USART_InitStructure);
  USART_Cmd(USART3, ENABLE);
}

static void UsartSendData(uint32_t size, uint8_t* data, USART_TypeDef * usart)
{
  uint32_t i;

  for(i = 0; i < size; i++)
  {
    while (!(usart->SR & USART_FLAG_TXE));
    usart->DR = (data[i] & 0x00FF);
  }
}

static void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;
  while(TimingDelay != 0);
}

void DecraseTimingDelay()
{
  if(TimingDelay != 0) TimingDelay--;
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

