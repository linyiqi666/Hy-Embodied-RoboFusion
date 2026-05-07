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

#ifndef ROFU_MSG_H_
#define ROFU_MSG_H_

#include <stdint.h>

// ETH
#define IP_ADDR0 ((uint8_t)192U)
#define IP_ADDR1 ((uint8_t)168U)
#define IP_ADDR2 ((uint8_t)47U)
#define IP_ADDR3 ((uint8_t)200U)

#define NETMASK_ADDR0 ((uint8_t)255U)
#define NETMASK_ADDR1 ((uint8_t)255U)
#define NETMASK_ADDR2 ((uint8_t)255U)
#define NETMASK_ADDR3 ((uint8_t)0U)

#define GW_ADDR0 ((uint8_t)192U)
#define GW_ADDR1 ((uint8_t)168U)
#define GW_ADDR2 ((uint8_t)47U)
#define GW_ADDR3 ((uint8_t)1U)

#define MAC_ADDR0 ((uint8_t)0x02U)
#define MAC_ADDR1 ((uint8_t)0x00U)
#define MAC_ADDR2 ((uint8_t)0x54U)
#define MAC_ADDR3 ((uint8_t)0x76U)
#define MAC_ADDR4 ((uint8_t)0x89U)
#define MAC_ADDR5 (IP_ADDR3)

// 核间共享信息首地址
#define CORE_SHARE_ADDR (void *)(0x38000000)

// 硬件信号量
#define HSEM_ID_1 (1U)
#define PROCESS_SHARE_MSG_SDO (0)
#define PROCESS_SHARE_MSG_PDO_RX (1)
#define PROCESS_SHARE_MSG_PDO_TX (2)

// Message type definition
typedef enum
{
    ROFU_NOUSED_TYPE = 0x00,

    // Sync frame
    ROFU_SYNC_FRAME_TYPE = 0x80,

    // Motor class
    ROFU_MOTOR_SDO_TYPE = 0x10,
    ROFU_MOTOR_RXPDO_TYPE = 0x20,
    ROFU_MOTOR_TXPDO_TYPE = 0x30,

    // Sensor class
    ROFU_SENSOR_TXPDO_FT_TYPE = 0x60,
    ROFU_SENSOR_TXPDO_IMU_TYPE = 0x61,
    ROFU_SENSOR_TXPDO_ENCODE_TYPE = 0x62,
} ROFU_TYPE_T;

// Port number definition
typedef enum
{
    // Motor PDO communication port
    ROFU_MOTOR_PDO_PORT = 9110,

    // Motor SDO communication port
    ROFU_MOTOR_SDO_PORT = 8880,

    // CM7 debug port
    ROFU_DEBUG_M7_PORT = 8887,

    // CM4 debug port
    ROFU_DEBUG_M4_PORT = 8884,
} ROFU_PORT_T;

// SDO operation definition
typedef enum
{
    ROFU_NOUSED_READ = 0x00,

    // sdo read
    ROFU_SDO_OP_READ = 0x01,

    // sdo write
    ROFU_SDO_OP_WRITE = 0x02,

    // sdo response
    ROFU_SDO_OP_RESPONSE = 0x03,

    // sdo error
    ROFU_SDO_OP_ERROR = 0x04,
} ROFU_SDO_OP_T;

// Motor mode definitions
typedef enum
{
    ROFU_MOTOR_MODE_MIT = 0x00,
    ROFU_MOTOR_MODE_PP = 0x01,
    ROFU_MOTOR_MODE_PV = 0x02,
    ROFU_MOTOR_MODE_PT = 0x03,
    ROFU_MOTOR_MODE_CSP = 0x04,
    ROFU_MOTOR_MODE_CSV = 0x05,
    ROFU_MOTOR_MODE_CST = 0x06,
} ROFU_MOTOR_MODE_T;

// SDO address definition
typedef enum
{
    ROFU_SDO_ADDR_NOUSED = 0x0000,

    // Motor SDO address
    ROFU_SDO_ADDR_MOTOR_ALL = 0x1000,
    ROFU_SDO_ADDR_MOTOR_POLE = 0x1001,
    ROFU_SDO_ADDR_MOTOR_PHASE_SEQ = 0x1002,
    ROFU_SDO_ADDR_MOTOR_REDU_RATIO = 0x1003,
    ROFU_SDO_ADDR_MOTOR_RS = 0x1004,
    ROFU_SDO_ADDR_MOTOR_LQ = 0x1005,
    ROFU_SDO_ADDR_MOTOR_LD = 0x1006,
    ROFU_SDO_ADDR_MOTOR_KT = 0x1007,
    ROFU_SDO_ADDR_MOTOR_INERTIA = 0x1008,
    ROFU_SDO_ADDR_MOTOR_KE = 0x1009,

    // Three-loop PID address
    ROFU_SDO_ADDR_PID_ALL = 0x2000,
    ROFU_SDO_ADDR_PID_CUR_KP = 0x2001,
    ROFU_SDO_ADDR_PID_CUR_KI = 0x2002,
    ROFU_SDO_ADDR_PID_VEL_KP = 0x2003,
    ROFU_SDO_ADDR_PID_VEL_KI = 0x2004,
    ROFU_SDO_ADDR_PID_POS_KP = 0x2005,

    // Protection thresholds address
    ROFU_SDO_ADDR_THR_ALL = 0x2010,
    ROFU_SDO_ADDR_THR_VBUS_HIGH = 0x2011,
    ROFU_SDO_ADDR_THR_VBUS_LOW = 0x2012,
    ROFU_SDO_ADDR_THR_IPHASE_HIGH = 0x2013,
    ROFU_SDO_ADDR_THR_MOS_TEMP_MAX = 0x2014,
    ROFU_SDO_ADDR_THR_MOTOR_TEMP_MAX = 0x2015,
    ROFU_SDO_ADDR_THR_MOTOR_SPEED_MAX = 0x2016,
} ROFU_SDO_ADDR_T;

typedef enum
{
    ROFU_SDO_FRAME_OK = 0x00,

    // 未定义帧
    ROFU_SDO_FRAME_NO_DEFINED = 0x01,

    // 帧长度错误
    ROFU_SDO_FRAME_LEN_ERROR = 0x02,

    // pdo帧超时
    ROFU_SDO_FRAME_OUTTIME = 0x03,
} ROFU_SDO_STATE_T;

#pragma pack(1)

// PDO structure definition
typedef struct
{
    uint8_t type;
    uint16_t len;
    uint8_t *pdata;
} ROFU_PDO_T;

// SDO structure definition
typedef struct
{
    uint8_t type;
    uint16_t len;
    uint8_t id;
    uint8_t op;
    uint8_t op_len;
    uint16_t addr;
    uint8_t *pdata;
} ROFU_SDO_T;

typedef union
{
    struct
    {
        // 0: disable
        // 1: enable
        uint16_t enable : 1;

        // 0: MIT
        // 1: PP
        // 2: PV
        // 3: PT
        // 4: CSP
        // 5: CSV
        // 6: CST
        uint16_t mode : 7;

        uint16_t : 8;

    } bits;
    uint16_t data;
} ROFU_MOTOR_CTRL_T;

typedef struct
{
    int16_t kp;
    int32_t pos_target; // Target position
    int16_t kd;
    int32_t vel_target; // Target velocity
    int16_t taff;       // Torque compensation
} ROFU_MOTOR_RXPDO_MIT_T;

typedef struct
{
    int16_t tor_target;  // Target torque
    int16_t tor_profile; // Torque slope
} ROFU_MOTOR_RXPDO_PT_T;

typedef struct
{
    int32_t vel_target;  // Target velocity
    int32_t vel_max;     // Maximum velocity
    int32_t acc_profile; // Acceleration slope
    int32_t dec_profile; // Deceleration slope
} ROFU_MOTOR_RXPDO_PV_T;

typedef struct
{
    int32_t pos_target;  // Target position
    int32_t vel_target;  // Target velocity
    int32_t acc_profile; // Acceleration slope
    int32_t dec_profile; // Deceleration slope
} ROFU_MOTOR_RXPDO_PP_T;

typedef struct
{
    int16_t tor_target; // Target torque
} ROFU_MOTOR_RXPDO_CST_T;

typedef struct
{
    int32_t vel_target; // Target velocity
} ROFU_MOTOR_RXPDO_CSV_T;

typedef struct
{
    int32_t pos_target; // Target position
} ROFU_MOTOR_RXPDO_CSP_T;

typedef struct
{
    int32_t pos_real;     // Actual position
    int32_t vel_real;     // Actual velocity
    int16_t tor_real;     // Actual torque
    int16_t mos_temper;   // MOS temperature
    int16_t motor_temper; // Motor temperature
    int16_t error_code;   // Error code
} ROFU_MOTOR_TXPDO0_T;

typedef struct
{
    uint8_t id;             // Motor ID number
    uint16_t seq;           // Command sequence number
    ROFU_MOTOR_CTRL_T ctrl; // Control word
} ROFU_MOTOR_RXHEAD_T;

// Motor RXPDO
typedef struct
{
    ROFU_MOTOR_RXHEAD_T header;
    union
    {
        ROFU_MOTOR_RXPDO_MIT_T mit;
        ROFU_MOTOR_RXPDO_PT_T pt;
        ROFU_MOTOR_RXPDO_PV_T pv;
        ROFU_MOTOR_RXPDO_PP_T pp;
        ROFU_MOTOR_RXPDO_CST_T cst;
        ROFU_MOTOR_RXPDO_CSV_T csv;
        ROFU_MOTOR_RXPDO_CSP_T csp;
    };
} ROFU_MOTOR_RXPDO_T;

typedef struct
{
    uint8_t id;
    uint16_t seq;
    ROFU_MOTOR_CTRL_T state;
} ROFU_MOTOR_TXHEAD_T;

// Motor TXPDO
typedef struct
{
    ROFU_MOTOR_TXHEAD_T header;
    union
    {
        ROFU_MOTOR_TXPDO0_T pdo0;
    };
} ROFU_MOTOR_TXPDO_T;

// Motor class SDO definition
typedef struct
{
    uint8_t pole;          // Pole pairs
    uint8_t phase_seq;     // Phase sequence
    uint8_t redu_ratio;    // Reduction ratio
    int16_t Rs;            // Stator resistance
    int16_t Lq;            // Lq inductance
    int16_t Ld;            // Ld inductance
    int16_t Kt;            // Torque constant
    int16_t inertia;       // Moment of inertia
    int16_t Ke;            // Back EMF constant
} ROFU_MOTOR_SDO_CONFIG_T; // Motor parameters

typedef struct
{
    uint16_t vbus_high;    // Vbus high threshold
    uint16_t vbus_low;     // Vbus low threshold
    uint16_t iphase_high;  // Iphase high threshold
    uint16_t mos_temper;   // MOS temperature threshold
    uint16_t motor_temper; // Motor temperature threshold
    uint16_t speed_max;    // Speed maximum threshold
} ROFU_MOTOR_SDO_THR_T;    // Protection thresholds

typedef struct
{
    int16_t cur_kp;     // Current KP gain
    int16_t cur_ki;     // Current KI gain
    int16_t vel_kp;     // Velocity KP gain
    int16_t vel_ki;     // Velocity KI gain
    int16_t pos_kp;     // Position KP gain
} ROFU_MOTOR_SDO_PID_T; // Three-loop PID parameters

// IMU class TXPDO
typedef struct
{
    uint8_t id;
    uint16_t seq;
    uint32_t sample_time_fine;
    int16_t quaternion[4];
    int16_t acceleration[3];
    int16_t rate_of_turn[3];
    uint8_t state;
    uint8_t err_code;
} ROFU_IMU_TXPDO_T;

// Force sensor TXPDO
typedef struct
{
    uint8_t id;
    uint16_t seq;
    int16_t fx;
    int16_t fy;
    int16_t fz;
    int16_t mx;
    int16_t my;
    int16_t mz;
    uint8_t err_code;
} ROFU_FT_TXPDO_T;

// Encoder TXPDO
typedef struct
{
    uint8_t id;
    uint16_t seq;
    int32_t pos;
    uint8_t state;
} ROFU_ENCODE_TXPDO_T;

#pragma pack()

typedef struct
{
    uint8_t *prx_buf;
    uint16_t *prx_len;
    uint8_t *ptx_buf;
    uint16_t *ptx_len;
} PAYLOAD;

typedef union
{
    struct
    {
        uint16_t motor_config : 1;
        uint16_t motor_thr : 1;
        uint16_t motor_pid : 1;
        uint16_t txpdo : 1;
        uint16_t rxpdo : 1;
        uint16_t : 11;
    } bits;
    uint16_t data;
} ROFU_ACTIVE_T;

typedef struct
{
    uint8_t ecu_id;
    ROFU_ACTIVE_T active;
    ROFU_MOTOR_SDO_CONFIG_T motor;
    ROFU_MOTOR_SDO_THR_T thr;
    ROFU_MOTOR_SDO_PID_T pid;
    ROFU_MOTOR_RXPDO_T rxpdo;
    ROFU_MOTOR_TXPDO_T txpdo;
} ROFU_MSG_T;

#endif /* ROFU_MSG_H_ */
