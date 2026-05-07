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

#include "robo_fusion_sdo.h"

#include <string.h>
#include <stdio.h>

// read motor params
static void GetAllMotorPram(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void GetPole(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void GetPhaseSeq(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void GetReductionRatio(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void GetRs(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void GetLq(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void GetLd(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void GetKt(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void GetInertia(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void GetKe(PAYLOAD *p, ROFU_MSG_T *pmsg);

// read/write pid params
static void GetAllPidPram(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void GetCurKp(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void GetCurKi(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void GetVelKp(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void GetVelKi(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void GetPosKp(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void SetAllPidPram(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void SetCurKp(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void SetCurKi(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void SetVelKp(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void SetVelKi(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void SetPosKp(PAYLOAD *p, ROFU_MSG_T *pmsg);

// read/write threshold params
static void GetAllthrPram(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void GetVbusHigh(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void GetVbusLow(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void GetIphaseHigh(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void GetMostemperHigh(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void GetMotorTemperHigh(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void GetSpeedMax(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void SetAllthrPram(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void SetVbusHigh(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void SetVbusLow(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void SetIphaseHigh(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void SetMostemperHigh(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void SetMotorTemperHigh(PAYLOAD *p, ROFU_MSG_T *pmsg);
static void SetSpeedMax(PAYLOAD *p, ROFU_MSG_T *pmsg);

// 未定义的sdo命令
static void SdoNoUsed(PAYLOAD *p, ROFU_MSG_T *pmsg);

// sdo命令列表
static const UDP_SDO_LIST sdo_list[] = {
    // motor params
    {ROFU_SDO_OP_READ, ROFU_SDO_ADDR_MOTOR_ALL, GetAllMotorPram},
    {ROFU_SDO_OP_READ, ROFU_SDO_ADDR_MOTOR_POLE, GetPole},
    {ROFU_SDO_OP_READ, ROFU_SDO_ADDR_MOTOR_PHASE_SEQ, GetPhaseSeq},
    {ROFU_SDO_OP_READ, ROFU_SDO_ADDR_MOTOR_REDU_RATIO, GetReductionRatio},
    {ROFU_SDO_OP_READ, ROFU_SDO_ADDR_MOTOR_RS, GetRs},
    {ROFU_SDO_OP_READ, ROFU_SDO_ADDR_MOTOR_LQ, GetLq},
    {ROFU_SDO_OP_READ, ROFU_SDO_ADDR_MOTOR_LD, GetLd},
    {ROFU_SDO_OP_READ, ROFU_SDO_ADDR_MOTOR_KT, GetKt},
    {ROFU_SDO_OP_READ, ROFU_SDO_ADDR_MOTOR_INERTIA, GetInertia},
    {ROFU_SDO_OP_READ, ROFU_SDO_ADDR_MOTOR_KE, GetKe},

    // pid params
    {ROFU_SDO_OP_READ, ROFU_SDO_ADDR_PID_ALL, GetAllPidPram},
    {ROFU_SDO_OP_READ, ROFU_SDO_ADDR_PID_CUR_KP, GetCurKp},
    {ROFU_SDO_OP_READ, ROFU_SDO_ADDR_PID_CUR_KI, GetCurKi},
    {ROFU_SDO_OP_READ, ROFU_SDO_ADDR_PID_VEL_KP, GetVelKp},
    {ROFU_SDO_OP_READ, ROFU_SDO_ADDR_PID_VEL_KI, GetVelKi},
    {ROFU_SDO_OP_READ, ROFU_SDO_ADDR_PID_POS_KP, GetPosKp},
    {ROFU_SDO_OP_WRITE, ROFU_SDO_ADDR_PID_ALL, SetAllPidPram},
    {ROFU_SDO_OP_WRITE, ROFU_SDO_ADDR_PID_CUR_KP, SetCurKp},
    {ROFU_SDO_OP_WRITE, ROFU_SDO_ADDR_PID_CUR_KI, SetCurKi},
    {ROFU_SDO_OP_WRITE, ROFU_SDO_ADDR_PID_VEL_KP, SetVelKp},
    {ROFU_SDO_OP_WRITE, ROFU_SDO_ADDR_PID_VEL_KI, SetVelKi},
    {ROFU_SDO_OP_WRITE, ROFU_SDO_ADDR_PID_POS_KP, SetPosKp},

    // threshold params
    {ROFU_SDO_OP_READ, ROFU_SDO_ADDR_THR_ALL, GetAllthrPram},
    {ROFU_SDO_OP_READ, ROFU_SDO_ADDR_THR_VBUS_HIGH, GetVbusHigh},
    {ROFU_SDO_OP_READ, ROFU_SDO_ADDR_THR_VBUS_LOW, GetVbusLow},
    {ROFU_SDO_OP_READ, ROFU_SDO_ADDR_THR_IPHASE_HIGH, GetIphaseHigh},
    {ROFU_SDO_OP_READ, ROFU_SDO_ADDR_THR_MOS_TEMP_MAX, GetMostemperHigh},
    {ROFU_SDO_OP_READ, ROFU_SDO_ADDR_THR_MOTOR_TEMP_MAX, GetMotorTemperHigh},
    {ROFU_SDO_OP_READ, ROFU_SDO_ADDR_THR_MOTOR_SPEED_MAX, GetSpeedMax},
    {ROFU_SDO_OP_WRITE, ROFU_SDO_ADDR_THR_ALL, SetAllthrPram},
    {ROFU_SDO_OP_WRITE, ROFU_SDO_ADDR_THR_VBUS_HIGH, SetVbusHigh},
    {ROFU_SDO_OP_WRITE, ROFU_SDO_ADDR_THR_VBUS_LOW, SetVbusLow},
    {ROFU_SDO_OP_WRITE, ROFU_SDO_ADDR_THR_IPHASE_HIGH, SetIphaseHigh},
    {ROFU_SDO_OP_WRITE, ROFU_SDO_ADDR_THR_MOS_TEMP_MAX, SetMostemperHigh},
    {ROFU_SDO_OP_WRITE, ROFU_SDO_ADDR_THR_MOTOR_TEMP_MAX, SetMotorTemperHigh},
    {ROFU_SDO_OP_WRITE, ROFU_SDO_ADDR_THR_MOTOR_SPEED_MAX, SetSpeedMax},

    // no used
    {ROFU_SDO_ADDR_NOUSED, 0x0000, SdoNoUsed},
};

static void SdoReadResponse(PAYLOAD *p, uint16_t addr, void *pdata, uint16_t len)
{
    uint8_t *ptx_buf = p->ptx_buf;
    *ptx_buf++ = ROFU_MOTOR_SDO_TYPE;
    *(uint16_t *)ptx_buf = SDO_HEADER_LEN + len;
    ptx_buf += 2;
    *ptx_buf++ = ps_msg->ecu_id;
    *ptx_buf++ = ROFU_SDO_OP_RESPONSE;
    *ptx_buf++ = len;
    *(uint16_t *)ptx_buf = addr;
    ptx_buf += 2;
    memcpy(ptx_buf, pdata, len);
    ptx_buf += len;
    *p->ptx_len = ptx_buf - p->ptx_buf;
}

static void SdoWriteResponse(PAYLOAD *p, uint16_t addr, uint8_t op, uint16_t state)
{
    uint8_t *ptx_buf = p->ptx_buf;
    *ptx_buf++ = ROFU_MOTOR_SDO_TYPE;
    *(uint16_t *)ptx_buf = SDO_HEADER_LEN + 2;
    ptx_buf += 2;
    *ptx_buf++ = ps_msg->ecu_id;
    *ptx_buf++ = op;
    *ptx_buf++ = 4;
    *(uint16_t *)ptx_buf = addr;
    ptx_buf += 2;
    *(uint16_t *)ptx_buf = state;
    ptx_buf += 2;
    *p->ptx_len = ptx_buf - p->ptx_buf;
}

void RofuSdo_Handler(PAYLOAD *p)
{
    uint8_t index = 0;
    ROFU_SDO_T sdo;
    sdo.type = p->prx_buf[0];
    sdo.len = *(uint16_t *)&p->prx_buf[1];
    sdo.id = p->prx_buf[3];
    sdo.op = p->prx_buf[4];
    sdo.op_len = p->prx_buf[5];
    sdo.addr = *(uint16_t *)&p->prx_buf[6];

    if ((sdo.type == ROFU_MOTOR_SDO_TYPE) &&
        (sdo.len == *p->prx_len - 3) &&
        (sdo.id == ps_msg->ecu_id || sdo.id == 0xff))
    {
        while (sdo_list[index].op != 0x00)
        {
            if (sdo_list[index].op == sdo.op && sdo_list[index].addr == sdo.addr)
            {
                p->prx_buf = &p->prx_buf[8];
                *p->prx_len = sdo.len;
                break;
            }
            else
            {
                index++;
            }
        }
        SCB_InvalidateDCache_by_Addr(ps_msg, sizeof(ROFU_MSG_T));
        if (sdo_list[index].Callback != NULL)
        {
            HAL_HSEM_Take(HSEM_ID_1, PROCESS_SHARE_MSG_SDO);
            sdo_list[index].Callback(p, ps_msg);
            HAL_HSEM_Release(HSEM_ID_1, PROCESS_SHARE_MSG_SDO);
        }
    }
}

static void GetAllMotorPram(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    SdoReadResponse(p, ROFU_SDO_ADDR_MOTOR_ALL, &pmsg->motor, sizeof(pmsg->motor));
}
static void GetPole(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    SdoReadResponse(p, ROFU_SDO_ADDR_MOTOR_POLE, &pmsg->motor.pole, sizeof(pmsg->motor.pole));
}
static void GetPhaseSeq(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    SdoReadResponse(p, ROFU_SDO_ADDR_MOTOR_PHASE_SEQ, &pmsg->motor.phase_seq, sizeof(pmsg->motor.phase_seq));
}
static void GetReductionRatio(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    SdoReadResponse(p, ROFU_SDO_ADDR_MOTOR_REDU_RATIO, &pmsg->motor.redu_ratio, sizeof(pmsg->motor.redu_ratio));
}
static void GetRs(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    SdoReadResponse(p, ROFU_SDO_ADDR_MOTOR_RS, &pmsg->motor.Rs, sizeof(pmsg->motor.Rs));
}
static void GetLq(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    SdoReadResponse(p, ROFU_SDO_ADDR_MOTOR_LQ, &pmsg->motor.Lq, sizeof(pmsg->motor.Lq));
}
static void GetLd(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    SdoReadResponse(p, ROFU_SDO_ADDR_MOTOR_LD, &pmsg->motor.Ld, sizeof(pmsg->motor.Ld));
}
static void GetKt(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    SdoReadResponse(p, ROFU_SDO_ADDR_MOTOR_KT, &pmsg->motor.Kt, sizeof(pmsg->motor.Kt));
}
static void GetInertia(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    SdoReadResponse(p, ROFU_SDO_ADDR_MOTOR_INERTIA, &pmsg->motor.inertia, sizeof(pmsg->motor.inertia));
}
static void GetKe(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    SdoReadResponse(p, ROFU_SDO_ADDR_MOTOR_KE, &pmsg->motor.Ke, sizeof(pmsg->motor.Ke));
}
static void GetAllPidPram(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    SdoReadResponse(p, ROFU_SDO_ADDR_PID_ALL, &pmsg->pid, sizeof(pmsg->pid));
}
static void GetCurKp(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    SdoReadResponse(p, ROFU_SDO_ADDR_PID_CUR_KP, &pmsg->pid.cur_kp, sizeof(pmsg->pid.cur_kp));
}
static void GetCurKi(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    SdoReadResponse(p, ROFU_SDO_ADDR_PID_CUR_KI, &pmsg->pid.cur_ki, sizeof(pmsg->pid.cur_ki));
}
static void GetVelKp(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    SdoReadResponse(p, ROFU_SDO_ADDR_PID_VEL_KP, &pmsg->pid.vel_kp, sizeof(pmsg->pid.vel_kp));
}
static void GetVelKi(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    SdoReadResponse(p, ROFU_SDO_ADDR_PID_VEL_KI, &pmsg->pid.vel_ki, sizeof(pmsg->pid.vel_ki));
}
static void GetPosKp(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    SdoReadResponse(p, ROFU_SDO_ADDR_PID_POS_KP, &pmsg->pid.pos_kp, sizeof(pmsg->pid.pos_kp));
}
static void SetAllPidPram(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    memcpy(&pmsg->pid, p->prx_buf, sizeof(pmsg->pid));
    pmsg->active.bits.motor_pid = 1;
    SdoWriteResponse(p, ROFU_SDO_ADDR_PID_ALL, ROFU_SDO_OP_RESPONSE, ROFU_SDO_FRAME_OK);
}
static void SetCurKp(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    memcpy(&pmsg->pid.cur_kp, p->prx_buf, sizeof(pmsg->pid.cur_kp));
    pmsg->active.bits.motor_pid = 1;
    SdoWriteResponse(p, ROFU_SDO_ADDR_PID_CUR_KP, ROFU_SDO_OP_RESPONSE, ROFU_SDO_FRAME_OK);
}
static void SetCurKi(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    memcpy(&pmsg->pid.cur_ki, p->prx_buf, sizeof(pmsg->pid.cur_ki));
    pmsg->active.bits.motor_pid = 1;
    SdoWriteResponse(p, ROFU_SDO_ADDR_PID_CUR_KI, ROFU_SDO_OP_RESPONSE, ROFU_SDO_FRAME_OK);
}
static void SetVelKp(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    memcpy(&pmsg->pid.vel_kp, p->prx_buf, sizeof(pmsg->pid.vel_kp));
    pmsg->active.bits.motor_pid = 1;
    SdoWriteResponse(p, ROFU_SDO_ADDR_PID_VEL_KP, ROFU_SDO_OP_RESPONSE, ROFU_SDO_FRAME_OK);
}
static void SetVelKi(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    memcpy(&pmsg->pid.vel_ki, p->prx_buf, sizeof(pmsg->pid.vel_ki));
    pmsg->active.bits.motor_pid = 1;
    SdoWriteResponse(p, ROFU_SDO_ADDR_PID_VEL_KI, ROFU_SDO_OP_RESPONSE, ROFU_SDO_FRAME_OK);
}
static void SetPosKp(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    memcpy(&pmsg->pid.pos_kp, p->prx_buf, sizeof(pmsg->pid.pos_kp));
    pmsg->active.bits.motor_pid = 1;
    SdoWriteResponse(p, ROFU_SDO_ADDR_PID_POS_KP, ROFU_SDO_OP_RESPONSE, ROFU_SDO_FRAME_OK);
}
static void GetAllthrPram(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    SdoReadResponse(p, ROFU_SDO_ADDR_THR_ALL, &pmsg->thr, sizeof(pmsg->thr));
}
static void GetVbusHigh(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    SdoReadResponse(p, ROFU_SDO_ADDR_THR_VBUS_HIGH, &pmsg->thr.vbus_high, sizeof(pmsg->thr.vbus_high));
}
static void GetVbusLow(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    SdoReadResponse(p, ROFU_SDO_ADDR_THR_VBUS_LOW, &pmsg->thr.vbus_low, sizeof(pmsg->thr.vbus_low));
}
static void GetIphaseHigh(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    SdoReadResponse(p, ROFU_SDO_ADDR_THR_IPHASE_HIGH, &pmsg->thr.iphase_high, sizeof(pmsg->thr.iphase_high));
}
static void GetMostemperHigh(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    SdoReadResponse(p, ROFU_SDO_ADDR_THR_MOS_TEMP_MAX, &pmsg->thr.mos_temper, sizeof(pmsg->thr.mos_temper));
}
static void GetMotorTemperHigh(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    SdoReadResponse(p, ROFU_SDO_ADDR_THR_MOTOR_TEMP_MAX, &pmsg->thr.motor_temper, sizeof(pmsg->thr.motor_temper));
}
static void GetSpeedMax(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    SdoReadResponse(p, ROFU_SDO_ADDR_THR_MOTOR_SPEED_MAX, &pmsg->thr.speed_max, sizeof(pmsg->thr.speed_max));
}
static void SetAllthrPram(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    memcpy(&pmsg->thr, p->prx_buf, sizeof(pmsg->thr));
    pmsg->active.bits.motor_thr = 1;
    SdoWriteResponse(p, ROFU_SDO_ADDR_THR_ALL, ROFU_SDO_OP_RESPONSE, ROFU_SDO_FRAME_OK);
}
static void SetVbusHigh(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    memcpy(&pmsg->thr.vbus_high, p->prx_buf, sizeof(pmsg->thr.vbus_high));
    pmsg->active.bits.motor_thr = 1;
    SdoWriteResponse(p, ROFU_SDO_ADDR_THR_VBUS_HIGH, ROFU_SDO_OP_RESPONSE, ROFU_SDO_FRAME_OK);
}
static void SetVbusLow(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    memcpy(&pmsg->thr.vbus_low, p->prx_buf, sizeof(pmsg->thr.vbus_low));
    pmsg->active.bits.motor_thr = 1;
    SdoWriteResponse(p, ROFU_SDO_ADDR_THR_VBUS_LOW, ROFU_SDO_OP_RESPONSE, ROFU_SDO_FRAME_OK);
}
static void SetIphaseHigh(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    memcpy(&pmsg->thr.iphase_high, p->prx_buf, sizeof(pmsg->thr.iphase_high));
    pmsg->active.bits.motor_thr = 1;
    SdoWriteResponse(p, ROFU_SDO_ADDR_THR_IPHASE_HIGH, ROFU_SDO_OP_RESPONSE, ROFU_SDO_FRAME_OK);
}
static void SetMostemperHigh(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    memcpy(&pmsg->thr.mos_temper, p->prx_buf, sizeof(pmsg->thr.mos_temper));
    pmsg->active.bits.motor_thr = 1;
    SdoWriteResponse(p, ROFU_SDO_ADDR_THR_MOS_TEMP_MAX, ROFU_SDO_OP_RESPONSE, ROFU_SDO_FRAME_OK);
}
static void SetMotorTemperHigh(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    memcpy(&pmsg->thr.motor_temper, p->prx_buf, sizeof(pmsg->thr.motor_temper));
    pmsg->active.bits.motor_thr = 1;
    SdoWriteResponse(p, ROFU_SDO_ADDR_THR_MOTOR_TEMP_MAX, ROFU_SDO_OP_RESPONSE, ROFU_SDO_FRAME_OK);
}
static void SetSpeedMax(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    memcpy(&pmsg->thr.speed_max, p->prx_buf, sizeof(pmsg->thr.speed_max));
    pmsg->active.bits.motor_thr = 1;
    SdoWriteResponse(p, ROFU_SDO_ADDR_THR_MOTOR_SPEED_MAX, ROFU_SDO_OP_RESPONSE, ROFU_SDO_FRAME_OK);
}

static void SdoNoUsed(PAYLOAD *p, ROFU_MSG_T *pmsg)
{
    *p->ptx_len = 0;
}
