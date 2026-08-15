/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */
#include "can_protocol.h"
#include <string.h>
/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;

/* CAN1 init function */
void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */
  CAN_FilterTypeDef CAN_FilterConfig = {0};
  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 3;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_9TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_4TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = ENABLE;
  hcan1.Init.AutoWakeUp = ENABLE;
  hcan1.Init.AutoRetransmission = ENABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */
  
  /* 使用16位ID列表模式 */
  CAN_FilterConfig.FilterBank = 0;
  CAN_FilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;
  CAN_FilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;

  /* 左移5位：把标准ID放到过滤器bit15～bit5 */

  /* 位置1：0x001呼吸灯控制 */
  CAN_FilterConfig.FilterIdHigh =(uint16_t)(CAN_ID_BREATH_CONTROL << 5U);

  /* 位置2：0x123蜂鸣器指令 */
  CAN_FilterConfig.FilterIdLow =(uint16_t)(CAN_ID_BUZZER_COMMAND << 5U);

  /* 位置3、4重复合法ID，避免意外放行0x000 */
  CAN_FilterConfig.FilterMaskIdHigh =(uint16_t)(CAN_ID_BUZZER_COMMAND << 5U);

  CAN_FilterConfig.FilterMaskIdLow =(uint16_t)(CAN_ID_BUZZER_COMMAND << 5U);

  /* 所有接收报文进入FIFO0 */
  CAN_FilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  CAN_FilterConfig.FilterActivation = ENABLE;

  /* CAN1使用Bank 0～13 */
  CAN_FilterConfig.SlaveStartFilterBank = 14U;

    /* 将配置写入CAN硬件 */
  if (HAL_CAN_ConfigFilter(&hcan1, &CAN_FilterConfig) != HAL_OK)
  {
      Error_Handler();
  }

  /* 启动CAN1 */
  if (HAL_CAN_Start(&hcan1) != HAL_OK)
  {
      Error_Handler();
  }
  //以后FIFO0中出现新报文时，产生中断。
  if (HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
    Error_Handler();
  }
    /* USER CODE END CAN1_Init 2 */
}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/**
 * @brief 发送主板0x02010101特定帧
 * @return HAL状态
 */
HAL_StatusTypeDef CAN_SendSlaveNoise(uint32_t sequence)
{
    CAN_TxHeaderTypeDef TxHeader = {0}; // CAN发送帧头
    uint32_t TxMailbox = 0U; // CAN发送邮箱
    uint8_t TxData[CAN_DLC_SLAVE_NOISE] = {0}; // 8字节发送数据

    /* uint32_t递增序号，小端序:低位字节放在低地址、先发送 */
    //sequence表示第几帧//
    //CAN_NOISE_PATTERN_0_INDEX：写到哪个字节 例如 Data[4]
    //CAN_NOISE_PATTERN_0：写入什么值 例如 0xA5
    //拆分32位递增序号//
    TxData[CAN_NOISE_SEQUENCE_BYTE0_INDEX] =(uint8_t)(sequence >> 0U);

    TxData[CAN_NOISE_SEQUENCE_BYTE1_INDEX] =(uint8_t)(sequence >> 8U);

    TxData[CAN_NOISE_SEQUENCE_BYTE2_INDEX] =(uint8_t)(sequence >> 16U);

    TxData[CAN_NOISE_SEQUENCE_BYTE3_INDEX] =(uint8_t)(sequence >> 24U);

    /* 固定数据A5 5A A5 5A */
    TxData[CAN_NOISE_PATTERN_0_INDEX] =CAN_NOISE_PATTERN_0;

    TxData[CAN_NOISE_PATTERN_1_INDEX] =CAN_NOISE_PATTERN_1;

    TxData[CAN_NOISE_PATTERN_2_INDEX] =CAN_NOISE_PATTERN_2;

    TxData[CAN_NOISE_PATTERN_3_INDEX] =CAN_NOISE_PATTERN_3;

    /* 0x02010101是扩展帧，所以写入ExtId */
    TxHeader.StdId = 0U;
    TxHeader.ExtId = CAN_ID_SLAVE_NOISE;
    TxHeader.IDE = CAN_IDE_SLAVE_NOISE;
    TxHeader.RTR = CAN_RTR_SLAVE_NOISE;
    TxHeader.DLC = CAN_DLC_SLAVE_NOISE;
    TxHeader.TransmitGlobalTime = DISABLE;

    return HAL_CAN_AddTxMessage(&hcan1,&TxHeader,TxData,&TxMailbox);
}

HAL_StatusTypeDef CAN_SendFloatFeedback(float value)
{
    CAN_TxHeaderTypeDef TxHeader = {0}; // CAN发送帧头
    uint32_t TxMailbox = 0U; // CAN发送邮箱
    uint8_t TxData[CAN_DLC_FLOAT_FEEDBACK] = {0}; // float的4字节数据

    /*
     * 将float的4字节原始二进制复制到Data[0..3]。
     */
    memcpy(&TxData[CAN_FLOAT_DATA_INDEX],&value,CAN_FLOAT_DATA_SIZE);

    /* 0x002是标准数据帧 */
    TxHeader.StdId = CAN_ID_FLOAT_FEEDBACK;
    TxHeader.ExtId = 0U;
    TxHeader.IDE = CAN_IDE_FLOAT_FEEDBACK;
    TxHeader.RTR = CAN_RTR_FLOAT_FEEDBACK;
    TxHeader.DLC = CAN_DLC_FLOAT_FEEDBACK;
    TxHeader.TransmitGlobalTime = DISABLE;

    return HAL_CAN_AddTxMessage(&hcan1,&TxHeader,TxData,&TxMailbox
    );
}

/* USER CODE END 1 */

