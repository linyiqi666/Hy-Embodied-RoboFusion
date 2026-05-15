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

#ifndef ROFU_PDO_H_
#define ROFU_PDO_H_

#include <stdint.h>

#include "stm32h7xx.h"
#include "robo_fusion_msg.h"

// pdo超时使能
#define PDO_OUTTIME_ENABLE 1

// pdo超时时间
#define PDO_OUTTIME_MS 100

typedef struct
{
    uint8_t type;
    void (*Callback)(PAYLOAD *p);
} UDP_PDO_LIST;

extern ROFU_MSG_T *ps_msg;

void RofuPdo_Handle(PAYLOAD *p);
void RofuPdo_TimeoutCallback();
void RofuPdo_UpdataTxpdo();

#endif /* ROFU_PDO_H_ */
