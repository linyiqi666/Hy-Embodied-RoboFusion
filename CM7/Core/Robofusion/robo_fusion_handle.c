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

#include "robo_fusion_handle.h"

#include <string.h>

static uint8_t tx_buf[UDP_TX_BUF_SIZE] = {0};
static uint16_t tx_len = 0;
static struct udp_pcb *upcb_pdo = NULL;
static struct udp_pcb *upcb_sdo = NULL;

uint8_t Udp_SendData(struct udp_pcb *upcb, const ip_addr_t *addr, uint16_t port, uint8_t *tx_buf, uint32_t tx_len)
{
    struct pbuf *p_tx;
    p_tx = pbuf_alloc(PBUF_TRANSPORT, tx_len, PBUF_RAM);
    if (p_tx == NULL)
    {
        return 1;
    }
    pbuf_take(p_tx, tx_buf, tx_len);
    err_t err = udp_sendto(upcb, p_tx, addr, port);
    pbuf_free(p_tx);
    if (err != ERR_OK)
    {
        return 2;
    }
    return 0;
}

uint8_t Udp_SendStr(struct udp_pcb *upcb, const ip_addr_t *addr, uint16_t port, char *tx_str)
{
    struct pbuf *p_tx;
    p_tx = pbuf_alloc(PBUF_TRANSPORT, strlen(tx_str), PBUF_RAM);
    if (p_tx == NULL)
    {
        return 1;
    }
    pbuf_take(p_tx, tx_str, strlen(tx_str));
    err_t err = udp_sendto(upcb, p_tx, addr, port);
    pbuf_free(p_tx);
    if (err != ERR_OK)
    {
        return 2;
    }
    return 0;
}

// 处理pdo数据
static void RofuHandle_RevPdo(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    if (p != NULL)
    {
        PAYLOAD payload;
        payload.prx_buf = (uint8_t *)p->payload;
        payload.prx_len = &p->len;
        payload.ptx_buf = tx_buf;
        payload.ptx_len = &tx_len;
        RofuPdo_Handle(&payload);
        if (tx_len > 0 && tx_len < UDP_TX_BUF_SIZE)
        {
            Udp_SendData(upcb, addr, port, tx_buf, tx_len);
        }
    }
    pbuf_free(p);
}

// 处理sdo数据
static void RofuHandle_RevSdo(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    if (p != NULL)
    {
        PAYLOAD payload;
        payload.prx_buf = (uint8_t *)p->payload;
        payload.prx_len = &p->len;
        payload.ptx_buf = tx_buf;
        payload.ptx_len = &tx_len;
        RofuSdo_Handler(&payload);
        if (tx_len > 0 && tx_len < UDP_TX_BUF_SIZE)
        {
            Udp_SendData(upcb, addr, port, tx_buf, tx_len);
        }
    }
    pbuf_free(p);
}

uint8_t RofuHandle_Init()
{
    upcb_pdo = udp_new();
    if (upcb_pdo == NULL)
    {
        return 1;
    }
    udp_bind(upcb_pdo, IP_ADDR_ANY, ROFU_MOTOR_PDO_PORT);
    udp_recv(upcb_pdo, RofuHandle_RevPdo, NULL);

    upcb_sdo = udp_new();
    if (upcb_sdo == NULL)
    {
        return 2;
    }
    udp_bind(upcb_sdo, IP_ADDR_ANY, ROFU_MOTOR_SDO_PORT);
    udp_recv(upcb_sdo, RofuHandle_RevSdo, NULL);
    return 0;
}
