/**
  ******************************************************************************
  * @file    stm32h7xx_hal_comp.c
  * @author  MCD Application Team
  * @version V1.2.0
  * @date   29-December-2017
  * @brief   COMP HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the COMP peripheral:
  *           + Initialization and de-initialization functions
  *           + Start/Stop operation functions in polling mode
  *           + Start/Stop operation functions in interrupt mode
  *           + Peripheral control functions
  *           + Peripheral state functions
  @verbatim
  ================================================================================
                   ##### COMP Peripheral features #####
  ================================================================================

  [..]
      The STM32H7xx device family integrates two analog comparators instances
      COMP1 and COMP2:
      (#) The COMP input minus (inverting input) and input plus (non inverting input)
          can be set to internal references or to GPIO pins
          (refer to GPIO list in reference manual).

      (#) The COMP output level is available using HAL_COMP_GetOutputLevel()
          and can be redirected to other peripherals: GPIO pins (in mode
          alternate functions for comparator), timers.
          (refer to GPIO list in reference manual).

      (#) Pairs of comparators instances can be combined in window mode
          (2 consecutive instances odd and even COMP<x> and COMP<x+1>).

      (#) The comparators have interrupt capability through the EXTI controller
          with wake-up from sleep and stop modes:
          (++) COMP1 is internally connected to EXTI Line 20
          (++) COMP2 is internally connected to EXTI Line 21

  [..]
          From the corresponding IRQ handler, the right interrupt source can be retrieved
          using macro __HAL_COMP_COMP1_EXTI_GET_FLAG() and __HAL_COMP_COMP2_EXTI_GET_FLAG().



            ##### How to use this driver #####
  ================================================================================
  [..]
      This driver provides functions to configure and program the comparator instances of
      STM32H7xx devices.

      To use the comparator, perform the following steps:

      (#)  Initialize the COMP low level resources by implementing the HAL_COMP_MspInit():
      (++) Configure the GPIO connected to comparator inputs plus and minus in analog mode
           using HAL_GPIO_Init().
      (++) If needed, configure the GPIO connected to comparator output in alternate function mode
           using HAL_GPIO_Init().
      (++) If required enable the COMP interrupt by configuring and enabling EXTI line in Interrupt mode and
           selecting the desired sensitivity level using HAL_GPIO_Init() function. After that enable the comparator
           interrupt vector using HAL_NVIC_EnableIRQ() function.

      (#) Configure the comparator using HAL_COMP_Init() function:
      (++) Select the input minus (inverting input)
      (++) Select the input plus (non-inverting input)
      (++) Select the hysteresis
      (++) Select the blanking source
      (++) Select the output polarity
      (++) Select the power mode
      (++) Select the window mode
      -@@- HAL_COMP_Init() calls internally __HAL_RCC_SYSCFG_CLK_ENABLE()
          to enable internal control clock of the comparators.
          However, this is a legacy strategy.
          Therefore, for compatibility anticipation, it is recommended to
          implement __HAL_RCC_SYSCFG_CLK_ENABLE() in "HAL_COMP_MspInit()".
          In STM32H7,COMP clock enable  __HAL_RCC_COMP12_CLK_ENABLE() must
          be implemented by user in "HAL_COMP_MspInit()".
      (#) Reconfiguration on-the-fly of comparator can be done by calling again
          function HAL_COMP_Init() with new input structure parameters values.

      (#) Enable the comparator using HAL_COMP_Start() or HAL_COMP_Start_IT()to be enabled
          with the interrupt through NVIC of the CPU.
      Note: HAL_COMP_Start_IT() must be called after each interrupt otherwise the interrupt
      mode will stay disabled.

      (#) Use HAL_COMP_GetOutputLevel() or HAL_COMP_TriggerCallback()
          functions to manage comparator outputs(output level or events)

      (#) Disable the comparator using HAL_COMP_Stop() or HAL_COMP_Stop_IT()
          to disable the interrupt too.

      (#) De-initialize the comparator using HAL_COMP_DeInit() function.

      (#) For safety purpose, comparator configuration can be locked using HAL_COMP_Lock() function.
          The only way to unlock the comparator is a device hardware reset.
  @endverbatim
  ******************************************************************************

  Table 1. COMP inputs and output for STM32H7xx devices
  +---------------------------------------------------------+
  |                |                |   COMP1   |   COMP2   |
  |----------------|----------------|-----------|-----------|
  |                | IO1            |    PB0    |    PE9    |
  | Input plus     | IO2            |    PB2    |    PE11   |
  |                |                |           |           |
  |----------------|----------------|-----------------------|
  |                | 1/4 VrefInt    | Available | Available |
  |                | 1/2 VrefInt    | Available | Available |
  |                | 3/4 VrefInt    | Available | Available |
  | Input minus    | VrefInt        | Available | Available |
  |                | DAC1 channel 1 | Available | Available |
  |                | DAC1 channel 2 | Available | Available |
  |                | IO1            |    PB1    |    PE10   |
  |                | IO2            |    PC4    |    PE7    |
  |                |                |           |           |
  |                |                |           |           |
  |                |                |           |           |
  +---------------------------------------------------------+
  | Output         |                |  PC5  (1) |  PE8  (1) |
  |                |                |  PE12 (1) |  PE13 (1) |
  |                |                |  TIM  (2) |  TIM  (2) |
  +---------------------------------------------------------+
  (1) GPIO must be set to alternate function for comparator
  (2) Comparators output to timers is set in timers instances.

  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @defgroup COMP COMP
  * @brief COMP HAL module driver
  * @{
  */

#ifdef HAL_COMP_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @addtogroup COMP_Private_Constants
  * @{
  */

/* Delay for COMP startup time.                                               */
/* Note: Delay required to reach propagation delay specification.             */
/* Literal set to maximum value (refer to device datasheet,                   */
/* parameter "tSTART").                                                       */
/* Unit: us                                                                   */
#define COMP_DELAY_STARTUP_US          ((uint32_t) 80U)  /*!< Delay for COMP startup time */

/* Delay for COMP voltage scaler stabilization time.                          */
/* Literal set to maximum value (refer to device datasheet,                   */
/* parameter "tSTART_SCALER").                                                */
/* Unit: us                                                                   */
#define COMP_DELAY_VOLTAGE_SCALER_STAB_US ((uint32_t) 200U)  /*!< Delay for COMP voltage scaler stabilization time */


/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @defgroup COMP_Exported_Functions COMP Exported Functions
  * @{
  */

/** @defgroup COMP_Exported_Functions_Group1 Initialization/de-initialization functions
 *  @brief    Initialization and de-initialization functions.
 *
@verbatim
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This section provides functions to initialize and de-initialize comparators

@endverbatim
  * @{
  */

/**
  * @brief  Initialize the COMP according to the specified
  *         parameters in the COMP_InitTypeDef and initialize the associated handle.
  * @note   If the selected comparator is locked, initialization can't be performed.
  *         To unlock the configuration, perform a system reset.
  * @param  hcomp: COMP handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_COMP_Init(COMP_HandleTypeDef *hcomp)
{
  uint32_t tmp_csr = 0;
  uint32_t exti_line = 0;
  uint32_t comp_voltage_scaler_not_initialized = 0;
  __IO uint32_t wait_loop_index = 0;

  HAL_StatusTypeDef status = HAL_OK;

  /* Check the COMP handle allocation and lock status */
  if((hcomp == NULL) || (__HAL_COMP_IS_LOCKED(hcomp)))
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the parameters */
    assert_param(IS_COMP_ALL_INSTANCE(hcomp->Instance));
    assert_param(IS_COMP_INPUT_PLUS(hcomp->Instance, hcomp->Init.NonInvertingInput));
    assert_param(IS_COMP_INPUT_MINUS(hcomp->Instance, hcomp->Init.InvertingInput));
    assert_param(IS_COMP_OUTPUTPOL(hcomp->Init.OutputPol));
    assert_param(IS_COMP_POWERMODE(hcomp->Init.Mode));
    assert_param(IS_COMP_HYSTERESIS(hcomp->Init.Hysteresis));
    assert_param(IS_COMP_BLANKINGSRCE(hcomp->Init.BlankingSrce));
    assert_param(IS_COMP_TRIGGERMODE(hcomp->Init.TriggerMode));
    assert_param(IS_COMP_WINDOWMODE(hcomp->Init.WindowMode));

    if(hcomp->State == HAL_COMP_STATE_RESET)
    {
   /* Allocate lock resource and initialize it */
      hcomp->Lock = HAL_UNLOCKED;

      /* Init the low level hardware */
      HAL_COMP_MspInit(hcomp);
    }
    /* Memorize voltage scaler state before initialization */
    comp_voltage_scaler_not_initialized = (READ_BIT(hcomp->Instance->CFGR, COMP_CFGRx_SCALEN) == 0);

    /* Set COMP parameters */
    /*     Set INMSEL bits according to hcomp->Init.InvertingInput value       */
    /*     Set INPSEL bits according to hcomp->Init.NonInvertingInput value    */
    /*     Set BLANKING bits according to hcomp->Init.BlankingSrce value       */
    /*     Set HYST bits according to hcomp->Init.Hysteresis value             */
    /*     Set POLARITY bit according to hcomp->Init.OutputPol value           */
    /*     Set POWERMODE bits according to hcomp->Init.Mode value              */

    tmp_csr = (hcomp->Init.InvertingInput    |  \
              hcomp->Init.NonInvertingInput  |  \
              hcomp->Init.BlankingSrce       |  \
              hcomp->Init.Hysteresis         |  \
              hcomp->Init.OutputPol          |  \
              hcomp->Init.Mode                );

    /* Set parameters in COMP register */
    /* Note: Update all bits except read-only, lock and enable bits */
    MODIFY_REG(hcomp->Instance->CFGR,
               COMP_CFGRx_PWRMODE  | COMP_CFGRx_INMSEL   | COMP_CFGRx_INPSEL  |
               COMP_CFGRx_WINMODE  | COMP_CFGRx_POLARITY | COMP_CFGRx_HYST    |
               COMP_CFGRx_BLANKING | COMP_CFGRx_BRGEN    | COMP_CFGRx_SCALEN,
               tmp_csr
              );

    /* Set window mode */
    /* Note: Window mode bit is located into 1 out of the 2 pairs of COMP     */
    /*       instances. Therefore, this function can update another COMP      */
    /*       instance that the one currently selected.                        */
    if(hcomp->Init.WindowMode == COMP_WINDOWMODE_COMP1_INPUT_PLUS_COMMON)
    {
      SET_BIT(hcomp->Instance->CFGR, COMP_CFGRx_WINMODE);
    }
    else
    {
      CLEAR_BIT(hcomp->Instance->CFGR, COMP_CFGRx_WINMODE);
    }
    /* Delay for COMP scaler bridge voltage stabilization */
    /* Apply the delay if voltage scaler bridge is enabled for the first time */
    if ((READ_BIT(hcomp->Instance->CFGR, COMP_CFGRx_SCALEN) != 0) &&
        (comp_voltage_scaler_not_initialized != 0)               )
    {
      /* Wait loop initialization and execution */
      /* Note: Variable divided by 2 to compensate partially                  */
      /*       CPU processing cycles.*/

     wait_loop_index = (COMP_DELAY_VOLTAGE_SCALER_STAB_US * (SystemCoreClock / (1000000 * 2)));

     while(wait_loop_index != 0)
     {
       wait_loop_index --;
     }
    }

    /* Get the EXTI line corresponding to the selected COMP instance */
    exti_line = COMP_GET_EXTI_LINE(hcomp->Instance);

    /* Manage EXTI settings */
    if((hcomp->Init.TriggerMode & (COMP_EXTI_IT | COMP_EXTI_EVENT)) != RESET)
    {
      /* Configure EXTI rising edge */
      if((hcomp->Init.TriggerMode & COMP_EXTI_RISING) != RESET)
      {
        SET_BIT(EXTI->RTSR1, exti_line);
      }
      else
      {
        CLEAR_BIT(EXTI->RTSR1, exti_line);
      }

      /* Configure EXTI falling edge */
      if((hcomp->Init.TriggerMode & COMP_EXTI_FALLING) != RESET)
      {
        SET_BIT(EXTI->FTSR1, exti_line);
      }
      else
      {
        CLEAR_BIT(EXTI->FTSR1, exti_line);
      }
      /* Clear COMP EXTI pending bit (if any) */
      WRITE_REG(EXTI_D1->PR1, exti_line);

      /* Configure EXTI event mode */
      if((hcomp->Init.TriggerMode & COMP_EXTI_EVENT) != RESET)
      {
        SET_BIT(EXTI_D1->EMR1, exti_line);
      }
      else
      {
        CLEAR_BIT(EXTI_D1->EMR1, exti_line);
      }

       /* Configure EXTI interrupt mode */
      if((hcomp->Init.TriggerMode & COMP_EXTI_IT) != RESET)
      {
      SET_BIT(EXTI_D1->IMR1, exti_line);
      }
      else
      {
        CLEAR_BIT(EXTI_D1->IMR1, exti_line);
      }
    }
    else
    {
      /* Disable EXTI event mode */
      CLEAR_BIT(EXTI_D1->EMR1, exti_line);

      /* Disable EXTI interrupt mode */
      CLEAR_BIT(EXTI_D1->IMR1, exti_line);
    }
    /* Set HAL COMP handle state */
    /* Note: Transition from state reset to state ready,                      */
    /*       otherwise (coming from state ready or busy) no state update.     */
    if (hcomp->State == HAL_COMP_STATE_RESET)
    {

      hcomp->State = HAL_COMP_STATE_READY;
    }

  }

  return status;
}

/**
  * @brief  DeInitialize the COMP peripheral.
  * @note   Deinitialization cannot be performed if the COMP configuration is locked.
  *         To unlock the configuration, perform a system reset.
  * @param  hcomp  COMP handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_COMP_DeInit(COMP_HandleTypeDef *hcomp)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the COMP handle allocation and lock status */
  if((hcomp == NULL) || (__HAL_COMP_IS_LOCKED(hcomp)))
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the parameter */
    assert_param(IS_COMP_ALL_INSTANCE(hcomp->Instance));

    /* Set COMP_CFGR register to reset value */
    WRITE_REG(hcomp->Instance->CFGR, 0x00000000);

    /* DeInit the low level hardware */
    HAL_COMP_MspDeInit(hcomp);

    /* Set HAL COMP handle state */
    hcomp->State = HAL_COMP_STATE_RESET;

    /* Release Lock */
    __HAL_UNLOCK(hcomp);
  }

  return status;
}

/**
  * @brief  Initialize the COMP MSP.
  * @param  hcomp COMP handle
  * @retval None
  */
__WEAK void HAL_COMP_MspInit(COMP_HandleTypeDef *hcomp)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcomp);
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_COMP_MspInit could be implemented in the user file
   */
}

/**
  * @brief  DeInitialize the COMP MSP.
  * @param  hcomp COMP handle
  * @retval None
  */
__WEAK void HAL_COMP_MspDeInit(COMP_HandleTypeDef *hcomp)
{
  /* Prevent unused argument(s) compilation warning */
   UNUSED(hcomp);
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_COMP_MspDeInit could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup COMP_Exported_Functions_Group2 Start-Stop operation functions
 *  @brief   Start-Stop operation functions.
 *
@verbatim
 ===============================================================================
                      ##### IO operation functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Start a Comparator instance without interrupt.
      (+) Stop a Comparator instance without interrupt.
      (+) Start a Comparator instance with interrupt generation.
      (+) Stop a Comparator instance with interrupt generation.

@endverbatim
  * @{
  */

/**
  * @brief  Start the comparator.
  * @param  hcomp COMP handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_COMP_Start(COMP_HandleTypeDef *hcomp)
{
  __IO uint32_t wait_loop_index = 0;

  HAL_StatusTypeDef status = HAL_OK;

  /* Check the COMP handle allocation and lock status */
  if((hcomp == NULL) || (__HAL_COMP_IS_LOCKED(hcomp)))
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the parameter */
    assert_param(IS_COMP_ALL_INSTANCE(hcomp->Instance));

    if(hcomp->State == HAL_COMP_STATE_READY)
    {
      /* Enable the selected comparator */
      SET_BIT(hcomp->Instance->CFGR, COMP_CFGRx_EN);

      /* Set HAL COMP handle state */
      hcomp->State = HAL_COMP_STATE_BUSY;

     /* Delay for COMP startup time */
     /* Wait loop initialization and execution */
     /* Note: Variable divided by 2 to compensate partially    */
     /*       CPU processing cycles.                           */

     wait_loop_index = (COMP_DELAY_STARTUP_US * (SystemCoreClock / (1000000 * 2)));
     while(wait_loop_index != 0)
     {
       wait_loop_index--;
     }
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}

/**
  * @brief  Stop the comparator.
  * @param  hcomp COMP handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_COMP_Stop(COMP_HandleTypeDef *hcomp)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the COMP handle allocation and lock status */
  if((hcomp == NULL) || (__HAL_COMP_IS_LOCKED(hcomp)))
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the parameter */
    assert_param(IS_COMP_ALL_INSTANCE(hcomp->Instance));

    if((hcomp->State == HAL_COMP_STATE_BUSY)  ||
       (hcomp->State == HAL_COMP_STATE_READY)   )
    {

      /* Disable the selected comparator */
      CLEAR_BIT(hcomp->Instance->CFGR, COMP_CFGRx_EN);

      /* Set HAL COMP handle state */
      hcomp->State = HAL_COMP_STATE_READY;
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}

/**
  * @brief  Enable the interrupt and start the comparator.
  * @param  hcomp COMP handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_COMP_Start_IT(COMP_HandleTypeDef *hcomp)
{

 __IO uint32_t wait_loop_index = 0;
 HAL_StatusTypeDef status = HAL_OK;

  /* Check the COMP handle allocation and lock status */
  if((hcomp == NULL) || (__HAL_COMP_IS_LOCKED(hcomp)))
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the parameter */
    assert_param(IS_COMP_ALL_INSTANCE(hcomp->Instance));
    /* Set HAL COMP handle state */
    if(hcomp->State == HAL_COMP_STATE_READY)
    {

    /* Enable the selected comparator */
    SET_BIT(hcomp->Instance->CFGR, COMP_CFGRx_EN);
    /* Enable the Interrupt comparator */
    SET_BIT(hcomp->Instance->CFGR, COMP_CFGRx_ITEN);

    hcomp->State = HAL_COMP_STATE_BUSY;
      /* Delay for COMP startup time */
      /* Wait loop initialization and execution */
      /* Note: Variable divided by 2 to compensate partially                  */
      /*       CPU processing cycles.                                         */

     wait_loop_index = (COMP_DELAY_STARTUP_US * (SystemCoreClock / (1000000 * 2)));
     while(wait_loop_index != 0)
     {
       wait_loop_index--;
     }

   }
   else
   {
      status = HAL_ERROR;
   }
  }

  return status;
}

/**
  * @brief  Disable the interrupt and Stop the comparator.
  * @param  hcomp COMP handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_COMP_Stop_IT(COMP_HandleTypeDef *hcomp)
{
  HAL_StatusTypeDef status = HAL_OK;
  /* Disable the EXTI Line interrupt mode */
   CLEAR_BIT(EXTI_D1->IMR1, COMP_GET_EXTI_LINE(hcomp->Instance));
  /* Disable the Interrupt comparator */
   CLEAR_BIT(hcomp->Instance->CFGR, COMP_CFGRx_ITEN);

  status = HAL_COMP_Stop(hcomp);

  return status;

}

/**
  * @brief  Comparator IRQ Handler.
  * @param  hcomp COMP handle
  * @retval HAL status
  */
void HAL_COMP_IRQHandler(COMP_HandleTypeDef *hcomp)
{
  /* Get the EXTI line corresponding to the selected COMP instance */
  uint32_t exti_line = COMP_GET_EXTI_LINE(hcomp->Instance);


    /* Check COMP EXTI flag */
    if(READ_BIT(EXTI_D1->PR1, exti_line) != RESET)
    {
       /* Check whether comparator is in independent or window mode */
        if(READ_BIT(COMP12_COMMON->CFGR, COMP_CFGRx_WINMODE) != RESET)
        {
          /* Clear COMP EXTI line pending bit of the pair of comparators          */
          /* in window mode.                                                      */
          /* Note: Pair of comparators in window mode can both trig IRQ when      */
          /*       input voltage is changing from "out of window" area            */
          /*       (low or high ) to the other "out of window" area (high or low).*/
          /*       Both flags must be cleared to call comparator trigger          */
          /*       callback is called once.                                       */
          WRITE_REG(EXTI_D1->PR1, (COMP_EXTI_LINE_COMP1 | COMP_EXTI_LINE_COMP2));
        }
        else
        {
          /* Clear COMP EXTI line pending bit */
          WRITE_REG(EXTI_D1->PR1, exti_line);
        }

      /* COMP trigger user callback */
        HAL_COMP_TriggerCallback(hcomp);
    }

   /* Get COMP interrupt source */
  if (__HAL_COMP_GET_IT_SOURCE(hcomp, COMP_IT_EN) != RESET)
  {

    if((__HAL_COMP_GET_FLAG( COMP_FLAG_C1I)) != RESET)
    {
      /* Clear the COMP channel 1 interrupt flag */
         __HAL_COMP_CLEAR_C1IFLAG();

      /* Disable COMP interrupt */
       __HAL_COMP_DISABLE_IT(hcomp,COMP_IT_EN);

    }
    if((__HAL_COMP_GET_FLAG( COMP_FLAG_C2I)) != RESET)
    {
     /* Clear the COMP channel 2 interrupt flag */
       __HAL_COMP_CLEAR_C2IFLAG();

     /* Disable COMP interrupt */
     __HAL_COMP_DISABLE_IT(hcomp,COMP_IT_EN);

    }

    /* Change COMP state */
    hcomp->State = HAL_COMP_STATE_READY;

  /* COMP trigger user callback */
    HAL_COMP_TriggerCallback(hcomp);
  }


}

/**
  * @}
  */

/** @defgroup COMP_Exported_Functions_Group3 Peripheral Control functions
 *  @brief   Management functions.
 *
@verbatim
 ===============================================================================
                      ##### Peripheral Control functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to control the comparators.

@endverbatim
  * @{
  */

/**
  * @brief  Lock the selected comparator configuration.
  * @note   A system reset is required to unlock the comparator configuration.
  * @param  hcomp COMP handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_COMP_Lock(COMP_HandleTypeDef *hcomp)
{
 HAL_StatusTypeDef status = HAL_OK;

  /* Check the COMP handle allocation and lock status */
  if((hcomp == NULL) || (__HAL_COMP_IS_LOCKED(hcomp)))
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Check the parameter */
    assert_param(IS_COMP_ALL_INSTANCE(hcomp->Instance));
    /* Set HAL COMP handle state */
    hcomp->State = ((HAL_COMP_StateTypeDef)(hcomp->State | COMP_STATE_BITFIELD_LOCK));
  }

  if(status == HAL_OK)
  {
    /* Set the lock bit corresponding to selected comparator */
    __HAL_COMP_LOCK(hcomp);
  }

  return status;
}

/**
  * @brief  Return the output level (high or low) of the selected comparator.
  * @note   The output level depends on the selected polarity.
  *         If the polarity is not inverted:
  *           - Comparator output is low when the input plus is at a lower
  *             voltage than the input minus
  *           - Comparator output is high when the input plus is at a higher
  *             voltage than the input minus
  *         If the polarity is inverted:
  *           - Comparator output is high when the input plus is at a lower
  *             voltage than the input minus
  *           - Comparator output is low when the input plus is at a higher
  *             voltage than the input minus
  * @param  hcomp  COMP handle
  * @retval Returns the selected comparator output level:
  *         @arg @ref COMP_OUTPUT_LEVEL_LOW
  *         @arg @ref COMP_OUTPUT_LEVEL_HIGH
  *
  */
uint32_t HAL_COMP_GetOutputLevel(COMP_HandleTypeDef *hcomp)
{
  /* Check the parameter */
  assert_param(IS_COMP_ALL_INSTANCE(hcomp->Instance));

  if (hcomp->Instance == COMP1)
  {
    return (uint32_t)(READ_BIT(COMP12->SR, COMP_SR_C1VAL));
  }
  else
  {
    return (uint32_t)((READ_BIT(COMP12->SR, COMP_SR_C2VAL))>> 1);
  }
}

/**
  * @brief  Comparator callback.
  * @param  hcomp COMP handle
  * @retval None
  */
__WEAK void HAL_COMP_TriggerCallback(COMP_HandleTypeDef *hcomp)
{
   /* Prevent unused argument(s) compilation warning */
   UNUSED(hcomp);
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_COMP_TriggerCallback should be implemented in the user file
   */
}


/**
  * @}
  */

/** @defgroup COMP_Exported_Functions_Group4 Peripheral State functions
 *  @brief   Peripheral State functions.
 *
@verbatim
 ===============================================================================
                      ##### Peripheral State functions #####
 ===============================================================================
    [..]
    This subsection permit to get in run-time the status of the peripheral.

@endverbatim
  * @{
  */

/**
  * @brief  Return the COMP handle state.
  * @param  hcomp  COMP handle
  * @retval HAL state
  */
HAL_COMP_StateTypeDef HAL_COMP_GetState(COMP_HandleTypeDef *hcomp)
{
  /* Check the COMP handle allocation */
  if(hcomp == NULL)
  {
    return HAL_COMP_STATE_RESET;
  }

  /* Check the parameter */
  assert_param(IS_COMP_ALL_INSTANCE(hcomp->Instance));

  /* Return HAL COMP handle state */
  return hcomp->State;
}
/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_COMP_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/