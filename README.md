# Robo Fusion 通信协议规范

## 文档信息
- **协议版本**: 1.0
- **创建日期**: 2026-04-13
- **最后更新**: 2026-04-14

## 1. 协议概述

Robo Fusion 协议是一个基于UDP的应用层通信协议，专为机器人控制系统设计，支持电机控制、传感器数据采集和设备状态监控。协议采用结构化的二进制数据格式，支持多种设备类型和数据交换模式。

- **PDO通信**：周期性传输实时数据，如电机控制指令和状态信息、传感器数据等，采用事件驱动机制
- **SDO通信**：客户端/服务器通信模式，用于传输关键配置参数和设备状态信息，主要用于初始化配置，采用请求-响应机制
- **端口分配**：不同服务使用独立的端口号，便于上层应用开发和管理

### 1.1 协议特性
- 支持多种电机控制模式（MIT、PP、PV、PT、CSP、CSV、CST）
- 支持多类型传感器数据采集（IMU、力传感器、编码器）
- 采用紧凑的二进制格式，确保高效数据传输
- 支持实时状态监控和故障诊断功能

### 1.2 设备类型
| 类型值 | 设备类型 | 描述 |
|--------|----------|------|
| 0x00   | 未使用   | 保留类型 |
| 0x80   | 同步帧   | 用于时序协调的同步帧 |
| 0x10   | 电机SDO  | 电机服务数据对象 |
| 0x20   | 电机RXPDO | 电机接收过程数据对象 |
| 0x30   | 电机TXPDO | 电机发送过程数据对象 |
| 0x60   | 力传感器TXPDO | 力传感器发送数据 |
| 0x61   | IMU TXPDO | 惯性测量单元发送数据 |
| 0x62   | 编码器TXPDO | 编码器发送数据 |

### 1.3 协议格式规范

#### PDO（过程数据对象）格式
PDO用于周期性传输实时数据，支持多个PDO类型组合在单个UDP数据包中传输。

**PDO数据包结构：**
```
+--------------+---------------+---------------+---------------+---------------+---------------+
|  类型 (1字节) |  长度 (2字节) |   数据 (N字节) |  类型 (1字节) |  长度 (2字节) |   数据 (N字节) | ...
+--------------+---------------+---------------+---------------+---------------+---------------+
```

**字段说明：**

| 字段 | 长度  | 描述                                                                                 |
|------|-------|--------------------------------------------------------------------------------------|
| 类型 | 1字节 | PDO类型标识符 (0x20: 电机RXPDO, 0x30: 电机TXPDO, 0x60-0x62: 传感器TXPDO)             |
| 长度 | 2字节 | 后续数据字段的长度（字节数）                                                         |
| 数据 | N字节 | 具体的PDO数据内容，格式取决于PDO类型                                                 |

#### SDO（服务数据对象）格式
SDO用于配置参数传输和设备状态查询，采用请求-响应模式。

**SDO数据包结构：**
```
+--------------+---------------+------------+-----------------+----------------+-------------------+---------------+
|  类型 (1字节) |  长度 (2字节) |  ID (1字节) |  操作码 (1字节) | 操作长度 (1字节) | 寄存器地址 (2字节) |   数据 (N字节) |...
+--------------+---------------+------------+-----------------+----------------+-------------------+---------------+
```

**字段说明：**

| 字段       | 长度  | 描述                                                                                 |
|------------|-------|--------------------------------------------------------------------------------------|
| 类型       | 1字节 | SDO类型标识符 (0x10)                                                                 |
| 长度       | 2字节 | 后续数据字段的长度（字节数）                                                         |
| ID         | 1字节 | 设备标识符                                                                           |
| 操作码     | 1字节 | 操作类型 (0x01: 读, 0x02: 写, 0x03: 响应, 0x04: 错误)                                |
| 操作长度   | 1字节 | 后续数据字段的长度（字节数）                                                         |
| 寄存器地址 | 2字节 | 目标寄存器地址                                                                       |
| 数据       | N字节 | 读写的数据内容                                                                       |

### 1.4 通信端口
| 端口号 | 端口类型 | 描述 |
|--------|----------|------|
| 9110   | 电机PDO  | 电机过程数据对象通信端口 |
| 8880   | 电机SDO  | 电机服务数据对象通信端口 |
| 8887   | CM7调试  | Cortex-M7处理器调试端口 |
| 8884   | CM4调试  | Cortex-M4处理器调试端口 |

### 1.5 通信示例

**PDO通信示例（多个PDO组合）：**
```
UDP数据包: [0x30][0x0018][电机TXPDO数据][0x61][0x0020][IMU数据]
```

**SDO通信示例（参数配置）：**
```
写操作: [0x10][0x000C][0x01][0x01][0x04][0x1000][0x3F800000] // 写入电机参数
读操作: [0x10][0x0008][0x01][0x00][0x04][0x1000][00000000] // 读取电机参数
```

## 2. 数据结构定义

### 2.1 基本数据类型

#### ROFU_TYPE_T (枚举)
定义协议中使用的设备类型标识符。

```c
typedef enum {
    ROFU_NOUSED_TYPE = 0x00,

    // 同步帧
    ROFU_SYNC_FRAME_TYPE = 0x80,

    // 电机类
    ROFU_MOTOR_SDO_TYPE = 0x10,
    ROFU_MOTOR_RXPDO_TYPE = 0x20,
    ROFU_MOTOR_TXPDO_TYPE = 0x30,

    // 传感器类
    ROFU_SENSOR_TXPDO_FT_TYPE = 0x60,
    ROFU_SENSOR_TXPDO_IMU_TYPE = 0x61,
    ROFU_SENSOR_TXPDO_ENCODE_TYPE = 0x62,
} ROFU_TYPE_T;
```

#### ROFU_PORT_T (枚举)
通信端口定义。
```c
typedef enum {
    // 电机PDO通信端口
    ROFU_MOTOR_PDO_PORT = 9110,
    // 电机SDO通信端口
    ROFU_MOTOR_SDO_PORT = 8880,
    // CM7调试端口
    ROFU_DEBUG_M7_PORT = 8887,
    // CM4调试端口
    ROFU_DEBUG_M4_PORT = 8884,
} ROFU_PORT_T; // 端口号
```

#### ROFU_PDO_T (结构体)
通用过程数据对象结构。
```c
typedef struct {
    uint8_t type;      // 数据类型
    uint16_t len;      // 数据长度
    uint8_t *pdata;    // 数据指针
} ROFU_PDO_T;
```

#### ROFU_SDO_T (结构体)
服务数据对象结构。
```c
typedef struct {
    uint8_t type;      // 数据类型
    uint16_t len;      // 数据长度
    uint8_t id;        // 设备ID
    uint8_t op;        // 操作码
    uint8_t op_len;    // 操作数据长度
    uint16_t addr;     // 寄存器地址
    uint8_t *pdata;    // 数据指针
} ROFU_SDO_T;
```

#### ROFU_SDO_OP_T (枚举)
SDO操作码定义。
```c
typedef enum {
    ROFU_NOUSED_READ = 0x00,

    // SDO读操作
    ROFU_SDO_OP_READ = 0x01,

    // SDO写操作
    ROFU_SDO_OP_WRITE = 0x02,

    // SDO响应
    ROFU_SDO_OP_RESPONSE = 0x03,

    // SDO错误
    ROFU_SDO_OP_ERROR = 0x04,
} ROFU_SDO_OP_T;
```

#### ROFU_SDO_STATE_T (枚举)
SDO状态定义。
```c
typedef enum {
    ROFU_SDO_FRAME_OK = 0x00,

    // 未定义帧
    ROFU_SDO_FRAME_NO_DEFINED = 0x01,

    // 帧长度错误
    ROFU_SDO_FRAME_LEN_ERROR = 0x02,

    // pdo帧超时
    ROFU_SDO_FRAME_OUTTIME = 0x03,
} ROFU_SDO_STATE_T;
```

#### ROFU_SDO_ADDR_T (枚举)
SDO寄存器地址定义。
```c
typedef enum {
    ROFU_SDO_ADDR_NOUSED = 0x0000,

    // 电机SDO地址
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

    // 三环PID地址
    ROFU_SDO_ADDR_PID_ALL = 0x2000,
    ROFU_SDO_ADDR_PID_CUR_KP = 0x2001,
    ROFU_SDO_ADDR_PID_CUR_KI = 0x2002,
    ROFU_SDO_ADDR_PID_VEL_KP = 0x2003,
    ROFU_SDO_ADDR_PID_VEL_KI = 0x2004,
    ROFU_SDO_ADDR_PID_POS_KP = 0x2005,

    // 保护阈值地址
    ROFU_SDO_ADDR_THR_ALL = 0x2010,
    ROFU_SDO_ADDR_THR_VBUS_HIGH = 0x2011,
    ROFU_SDO_ADDR_THR_VBUS_LOW = 0x2012,
    ROFU_SDO_ADDR_THR_IPHASE_HIGH = 0x2013,
    ROFU_SDO_ADDR_THR_MOS_TEMP_MAX = 0x2014,
    ROFU_SDO_ADDR_THR_MOTOR_TEMP_MAX = 0x2015,
    ROFU_SDO_ADDR_THR_MOTOR_SPEED_MAX = 0x2016,
} ROFU_SDO_ADDR_T;
```
## 3. 电机控制协议
#### ROFU_MOTOR_CTRL_T (联合体)

```c
typedef union {
    struct {
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
```
### 3.2 控制模式说明

| 模式值 | 模式名称 | 描述 |
|--------|----------|------|
| 0      | MIT      | 力矩控制模式 |
| 1      | PP       | 位置轮廓控制模式 |
| 2      | PV       | 速度轮廓控制模式 |
| 3      | PT       | 力矩轮廓控制模式 |
| 4      | CSP      | 循环位置控制模式 |
| 5      | CSV      | 循环速度控制模式 |
| 6      | CST      | 循环力矩控制模式 |

### 3.3 电机头结构体

#### RX 头结构 (ROFU_MOTOR_RXHEAD_T)
```c
typedef struct {
    uint8_t id;                    // 电机ID号
    uint16_t seq;                  // 命令序列号
    ROFU_MOTOR_CTRL_T ctrl; // 控制字
} ROFU_MOTOR_RXHEAD_T;
```

#### TX 头结构 (ROFU_MOTOR_TXHEAD_T)
```c
typedef struct {
    uint8_t id;
    uint16_t seq;
    ROFU_MOTOR_CTRL_T state;
} ROFU_MOTOR_TXHEAD_T;
```

### 3.4 完整RXPDO结构

```c
typedef struct {
    ROFU_MOTOR_RXHEAD_T header;
    union {
        ROFU_MOTOR_RXPDO_MIT_T mit;
        ROFU_MOTOR_RXPDO_PT_T pt;
        ROFU_MOTOR_RXPDO_PV_T pv;
        ROFU_MOTOR_RXPDO_PP_T pp;
        ROFU_MOTOR_RXPDO_CST_T cst;
        ROFU_MOTOR_RXPDO_CSV_T csv;
        ROFU_MOTOR_RXPDO_CSP_T csp;
    };
} ROFU_MOTOR_RXPDO_T;
```

### 3.5 完整TXPDO结构

```c
typedef struct {
    ROFU_MOTOR_TXHEAD_T header;
    union {
        ROFU_MOTOR_TXPDO0_T pdo0;
    };
} ROFU_MOTOR_TXPDO_T;
```

### 3.6 RXPDO 数据结构

#### MIT 模式 (ROFU_MOTOR_RXPDO_MIT_T)
```c
typedef struct {
    int16_t kp;           // 比例增益
    int32_t pos_target;   // 目标位置
    int16_t kd;           // 微分增益
    int32_t vel_target;   // 目标速度
    int16_t taff;         // 扭矩补偿
} ROFU_MOTOR_RXPDO_MIT_T;
```

#### PT 模式 (ROFU_MOTOR_RXPDO_PT_T)
```c
typedef struct {
    int16_t tor_target;  // 目标扭矩
    int16_t tor_profile; // 扭矩斜率
} ROFU_MOTOR_RXPDO_PT_T;
```

#### PV 模式 (ROFU_MOTOR_RXPDO_PV_T)
```c
typedef struct {
    int32_t vel_target;  // 目标速度
    int32_t vel_max;     // 最高速度
    int32_t acc_profile; // 加速度斜率
    int32_t dec_profile; // 减速度斜率
} ROFU_MOTOR_RXPDO_PV_T;
```

#### PP 模式 (ROFU_MOTOR_RXPDO_PP_T)
```c
typedef struct {
    int32_t pos_target;  // 目标位置
    int32_t vel_target;  // 目标速度
    int32_t acc_profile; // 加速度斜率
    int32_t dec_profile; // 减速度斜率
} ROFU_MOTOR_RXPDO_PP_T;
```

#### CST 模式 (ROFU_MOTOR_RXPDO_CST_T)
```c
typedef struct {
    int16_t tor_target; // 目标扭矩
} ROFU_MOTOR_RXPDO_CST_T;
```

#### CSV 模式 (ROFU_MOTOR_RXPDO_CSV_T)
```c
typedef struct {
    int32_t vel_target; // 目标速度
} ROFU_MOTOR_RXPDO_CSV_T;
```

#### CSP 模式 (ROFU_MOTOR_RXPDO_CSP_T)
```c
typedef struct {
    int32_t pos_target; // 目标位置
} ROFU_MOTOR_RXPDO_CSP_T;
```

### 3.8 TXPDO 数据结构

#### 电机状态数据 (ROFU_MOTOR_TXPDO0_T)
```c
typedef struct {
    int32_t pos_real;     // 实际位置
    int32_t vel_real;     // 实际速度
    int16_t tor_real;     // 实际扭矩
    int16_t mos_temper;   // MOS温度
    int16_t motor_temper; // 电机温度
    int16_t error_code;   // 故障码
} ROFU_MOTOR_TXPDO0_T;
```

### 3.9 SDO 配置参数

#### PID 参数配置 (ROFU_MOTOR_SDO_PID_T)
```c
typedef struct {
    int16_t cur_kp;     // 电流KP增益
    int16_t cur_ki;     // 电流KI增益
    int16_t vel_kp;     // 速度KP增益
    int16_t vel_ki;     // 速度KI增益
    int16_t pos_kp;     // 位置KP增益
} ROFU_MOTOR_SDO_PID_T; // 三环PID参数
```

#### 电机参数配置 (ROFU_MOTOR_SDO_CONFIG_T)
```c
typedef struct {
    uint8_t pole;          // 极对数
    uint8_t phase_seq;     // 相序
    uint8_t redu_ratio;    // 减速比
    int16_t Rs;            // 定子电阻
    int16_t Lq;            // Lq电感
    int16_t Ld;            // Ld电感
    int16_t Kt;            // 转矩常数
    int16_t inertia;       // 转动惯量
    int16_t Ke;            // 反电动势常数
} ROFU_MOTOR_SDO_CONFIG_T; // 电机参数
```

#### 保护阈值配置 (ROFU_MOTOR_SDO_THR_T)
```c
typedef struct {
    uint16_t vbus_high;      // Vbus高阈值 
    uint16_t vbus_low;       // Vbus低阈值 
    uint16_t iphase_high;    // 相电流高阈值 
    uint16_t mos_temper;     // MOS温度阈值 
    uint16_t motor_temper;   // 电机温度阈值 
    uint16_t speed_max;      // 速度最大阈值 
} ROFU_MOTOR_SDO_THR_T; // 保护阈值
```

## 4. 传感器协议

#### IMU 传感器数据 (ROFU_IMU_TXPDO_T)
```c
typedef struct {
    uint8_t id;                 // 设备ID
    uint16_t seq;               // 序列号
    uint32_t sample_time_fine;  // 采样时间
    int16_t quaternion[4];      // 四元数 (w, x, y, z)
    int16_t acceleration[3];    // 加速度 (x, y, z)
    int16_t rate_of_turn[3];    // 角速度 (x, y, z)
    uint8_t state;              // 状态字
    uint8_t err_code;           // 错误码
} ROFU_IMU_TXPDO_T;
```
#### 力传感器数据 (ROFU_FT_TXPDO_T)
```c
typedef struct {
    uint8_t id;                 // 设备ID
    uint16_t seq;               // 序列号
    int16_t fx;                 // X轴力
    int16_t fy;                 // Y轴力
    int16_t fz;                 // Z轴力
    int16_t mx;                 // X轴力矩
    int16_t my;                 // Y轴力矩
    int16_t mz;                 // Z轴力矩
    uint8_t err_code;           // 错误码
} ROFU_FT_TXPDO_T;
```
#### 编码器数据 (ROFU_ENCODE_TXPDO_T)
```c
typedef struct {
    uint8_t id;                 // 设备ID
    uint16_t seq;               // 序列号
    int32_t pos;                // 位置值
    uint8_t state;              // 状态字
} ROFU_ENCODE_TXPDO_T;
```
## 5. 通信协议规范

### 5.1 数据包格式
所有数据包采用小端字节序，结构体打包对齐为1字节。

### 5.2 错误处理
- 错误码为32位无符号整数
- 错误码厂家自定义

### 5.3 通信时序
- 建议通信频率: 1kHz
- 超时时间: 100ms

## 6. 附录

### 6.1 版本历史
| 版本 | 日期 | 描述 |
|------|------|------|
| 1.0  | 2026-04-13 | 初始版本 |

### 6.2 技术说明
- 所有结构体使用 `#pragma pack(1)` 进行1字节对齐打包
- 协议支持PDO（过程数据对象）和SDO（服务数据对象）两种通信模式
- 电机控制支持8种不同的控制模式，每种模式有对应的数据结构
- 传感器数据包括IMU、力传感器和编码器的测量数据
- 所有数据都采用小端格式

### 6.3 编译环境
- Cubeide 版本:V1.18.0
- CubeMx 版本:V6.14.0
- 芯片型号：STM32H755

## 7. 联系方式
如需进一步了解RoboFusion技术，请发邮件到：RoboticsX@tencent.com