/*
 * Copyright (c) 2026 Tencent. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "robo_fusion.h"
#include "robo_fusion_handle.h"
#include "robo_fusion_pdo.h"

#include <string.h>

#include "lan8742.h"
#include "main.h"
#include "lwip.h"

extern lan8742_Object_t LAN8742;
ROFU_MSG_T *ps_msg = NULL;

void BspLan937x_SpiRead(uint16_t reg_addr, uint8_t *pdata, uint8_t count)
{
    uint8_t spi_tx_buf[LAN937X_SPI_BUF_SIZE];
    uint32_t tx_data = 0, tx_data_ = 0;

    tx_data = (LAN937x_READ) << 29 | (reg_addr << 5);
    tx_data_ = __builtin_bswap32(tx_data);
    memcpy(spi_tx_buf, (uint8_t *)&tx_data_, 4);

    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi2, spi_tx_buf, pdata, count + 4, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);

    memcpy(&pdata[0], &pdata[4], count);
    HAL_Delay(1);
}

void BspLan937x_SpiWrite(uint16_t reg_addr, uint8_t *pdata, uint8_t count)
{
    uint8_t spi_tx_buf[LAN937X_SPI_BUF_SIZE];
    uint32_t tx_data = 0, tx_data_ = 0;

    tx_data = (LAN937x_WRITE) << 29 | (reg_addr << 5);
    tx_data_ = __builtin_bswap32(tx_data);
    memcpy(&spi_tx_buf[0], &tx_data_, 4);
    memcpy(&spi_tx_buf[4], pdata, count);

    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi2, spi_tx_buf, count + 4, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
}

void BspLan937x_MdioRead(uint32_t phy_addr, uint32_t reg_addr, uint32_t *reg_val)
{
    LAN8742.IO.ReadReg(phy_addr, reg_addr, reg_val);
}

void BspLan937x_MdioWrite(uint32_t phy_addr, uint32_t reg_addr, uint32_t reg_val)
{
    LAN8742.IO.WriteReg(phy_addr, reg_addr, reg_val);
}

void Rofu_Init(void)
{
    ps_msg = (ROFU_MSG_T *)CORE_SHARE_ADDR;
    memset(ps_msg,0,sizeof(ROFU_MSG_T));
    ps_msg->ecu_id = IP_ADDR3;
    MX_LWIP_Init();
    BspLan937x_Init();
    RofuHandle_Init();
}

void Rofu_Run(void)
{
    MX_LWIP_Process();
    RofuPdo_UpdataTxpdo();
}
