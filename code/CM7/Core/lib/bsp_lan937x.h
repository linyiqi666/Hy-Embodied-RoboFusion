

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

#ifndef BSP_LAN937X_H_
#define BSP_LAN937X_H_

#include <stdint.h>

#define LAN937X_SPI_BUF_SIZE 32

typedef enum
{
    LAN937x_WRITE = 2,
    LAN937x_READ = 3,
} LAN937x_CMD;

/* Exported functions --------------------------------------------------------*/
extern void BspLan937x_SpiRead(uint16_t reg_addr, uint8_t *pdata, uint8_t count);
extern void BspLan937x_SpiWrite(uint16_t reg_addr, uint8_t *pdata, uint8_t count);
extern void BspLan937x_MdioRead(uint32_t phy_addr, uint32_t reg_addr, uint32_t *reg_val);
extern void BspLan937x_MdioWrite(uint32_t phy_addr, uint32_t reg_addr, uint32_t reg_val);
void BspLan937x_Init(void);

#endif /* BSP_LAN937X_H_ */
