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

#include "robo_fusion_pdo.h"

#include <string.h>
#include <stdio.h>

static ROFU_PDO_T pdo;
static ROFU_MOTOR_TXPDO_T txpdo;
static uint32_t pdo_timeout_cnt = 0;
static uint32_t pdo_timeout_flag = 0;
static void TxPdo(PAYLOAD *p);
static void RxPdo(PAYLOAD *p);
static void NoUsedPdo(PAYLOAD *p);

// udp命令列表
static const UDP_PDO_LIST pdo_list[] = {
    {ROFU_SYNC_FRAME_TYPE, TxPdo},
    {ROFU_MOTOR_RXPDO_TYPE, RxPdo},
    {ROFU_NOUSED_TYPE, NoUsedPdo},
};

void RofuPdo_Handle(PAYLOAD *p)
{
    uint8_t index = 0;
    if (*p->prx_len == 1)
    {
        pdo.type = p->prx_buf[0];
    }
    else
    {
        pdo.type = p->prx_buf[0];
        pdo.len = *(uint16_t *)&p->prx_buf[1];
        pdo.pdata = &p->prx_buf[3];
        if (pdo.len != *p->prx_len - 3)
        {
            *p->ptx_len = 0;
            return;
        }
    }

    while (pdo_list[index].type != 0)
    {
        if (pdo_list[index].type == pdo.type)
        {
            p->prx_buf = pdo.pdata;
            p->prx_len = &pdo.len;
            break;
        }
        else
        {
            index++;
        }
    }
    SCB_InvalidateDCache_by_Addr(ps_msg, sizeof(ROFU_MSG_T));
    if (pdo_list[index].Callback != NULL)
    {
        pdo_list[index].Callback(p);
    }
}

void RofuPdo_TimeoutCallback()
{
#if PDO_OUTTIME_ENABLE
    if (pdo_timeout_flag)
    {
        if (pdo_timeout_cnt == 0)
        {
            ps_msg->rxpdo.header.ctrl.bits.enable = 0;
            txpdo.header.seq = 0;
            pdo_timeout_flag = 0;
        }
        else
        {
            pdo_timeout_cnt--;
        }
    }
#endif
}

void RofuPdo_UpdataTxpdo(void)
{
    HAL_HSEM_Take(HSEM_ID_1, PROCESS_SHARE_MSG_PDO_TX);
    if (ps_msg->active.bits.txpdo)
    {
        memcpy(&txpdo, &ps_msg->txpdo, sizeof(ROFU_MOTOR_TXPDO_T));
        ps_msg->active.bits.txpdo = 0;
    }
    HAL_HSEM_Release(HSEM_ID_1, PROCESS_SHARE_MSG_PDO_TX);
}

static void TxPdo(PAYLOAD *p)
{
    uint8_t *ptx_buf = p->ptx_buf;

    *ptx_buf++ = ROFU_MOTOR_TXPDO_TYPE;
    *(uint16_t *)ptx_buf = sizeof(ROFU_MOTOR_TXHEAD_T) + sizeof(ROFU_MOTOR_TXPDO0_T);

    ptx_buf += 2;

    txpdo.header.id = ps_msg->ecu_id;
    memcpy(ptx_buf, &txpdo.header, sizeof(ROFU_MOTOR_TXHEAD_T));
    ptx_buf += sizeof(ROFU_MOTOR_TXHEAD_T);

    memcpy(ptx_buf, &txpdo.pdo0, sizeof(ROFU_MOTOR_TXPDO0_T));
    ptx_buf += sizeof(ROFU_MOTOR_TXPDO0_T);

    pdo_timeout_cnt = PDO_OUTTIME_MS;
    pdo_timeout_flag = 1;
    *p->ptx_len = ptx_buf - p->ptx_buf;
}

static void RxPdo(PAYLOAD *p)
{
    uint8_t *prx_buf = p->prx_buf;
    uint8_t num = 0;

    num = *p->prx_len / sizeof(ROFU_MOTOR_RXPDO_T);
    for (int i = 0; i < num; i++)
    {
        HAL_HSEM_Take(HSEM_ID_1, PROCESS_SHARE_MSG_PDO_RX);
        ROFU_MOTOR_RXHEAD_T header;
        memcpy(&header, prx_buf, sizeof(ROFU_MOTOR_RXHEAD_T));
        if (header.id == ps_msg->ecu_id)
        {
            memcpy(&ps_msg->rxpdo.header, &header, sizeof(ROFU_MOTOR_RXHEAD_T));
            prx_buf += sizeof(ROFU_MOTOR_RXHEAD_T);

            switch (header.ctrl.bits.mode)
            {
            case ROFU_MOTOR_MODE_MIT:
                memcpy(&ps_msg->rxpdo.mit, prx_buf, sizeof(ROFU_MOTOR_RXPDO_MIT_T));
                break;
            case ROFU_MOTOR_MODE_PP:
                memcpy(&ps_msg->rxpdo.pp, prx_buf, sizeof(ROFU_MOTOR_RXPDO_PP_T));
                break;
            case ROFU_MOTOR_MODE_PV:
                memcpy(&ps_msg->rxpdo.pv, prx_buf, sizeof(ROFU_MOTOR_RXPDO_PV_T));
                break;
            case ROFU_MOTOR_MODE_PT:
                memcpy(&ps_msg->rxpdo.pt, prx_buf, sizeof(ROFU_MOTOR_RXPDO_PT_T));
                break;
            case ROFU_MOTOR_MODE_CSP:
                memcpy(&ps_msg->rxpdo.csp, prx_buf, sizeof(ROFU_MOTOR_RXPDO_CSP_T));
                break;
            case ROFU_MOTOR_MODE_CSV:
                memcpy(&ps_msg->rxpdo.csv, prx_buf, sizeof(ROFU_MOTOR_RXPDO_CSV_T));
                break;
            case ROFU_MOTOR_MODE_CST:
                memcpy(&ps_msg->rxpdo.cst, prx_buf, sizeof(ROFU_MOTOR_RXPDO_CST_T));
                break;
            default:
                break;
            }
            if (ps_msg->rxpdo.header.ctrl.bits.enable)
            {
                ps_msg->txpdo.header.seq++;
            }
            ps_msg->active.bits.rxpdo = 1;
            prx_buf += sizeof(ROFU_MOTOR_RXPDO_T) - sizeof(ROFU_MOTOR_RXHEAD_T);
        }
        else
        {
            prx_buf += sizeof(ROFU_MOTOR_RXPDO_T);
        }
        HAL_HSEM_Release(HSEM_ID_1, PROCESS_SHARE_MSG_PDO_RX);
    }
    *p->ptx_len = 0;
}

static void NoUsedPdo(PAYLOAD *p)
{
    *p->ptx_len = 0;
}
