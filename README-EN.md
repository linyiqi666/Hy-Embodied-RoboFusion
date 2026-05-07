# Robo Fusion Communication Protocol Specification

## Document Information
- **Protocol Version**: 1.0
- **Creation Date**: 2026-04-13
- **Last Updated**: 2026-04-14

## 1. Protocol Overview

Robo Fusion protocol is a UDP-based application layer communication protocol designed for robot control systems, supporting motor control, sensor data acquisition, and device status monitoring. The protocol uses a structured binary data format and supports multiple device types and data exchange modes.

- **PDO Communication**: Periodic transmission of real-time data such as motor control commands, status information, and sensor data, using event-driven mechanism
- **SDO Communication**: Client/server communication mode for transmitting critical configuration parameters and device status information, mainly used for initialization configuration, using request-response mechanism
- **Port Allocation**: Different services use independent port numbers for easy upper-layer application development and management

### 1.1 Protocol Features
- Supports multiple motor control modes (MIT, PP, PV, PT, CSP, CSV, CST)
- Supports multiple types of sensor data acquisition (IMU, force sensor, encoder)
- Uses compact binary format to ensure efficient data transmission
- Supports real-time status monitoring and fault diagnosis functions

### 1.2 Device Types
| Type Value | Device Type   | Description                                    |
|------------|---------------|------------------------------------------------|
| 0x00       | Unused        | Reserved type                                  |
| 0x80       | Sync Frame    | Synchronization frame for timing coordination  |
| 0x10       | Motor SDO     | Motor Service Data Object                      |
| 0x20       | Motor RXPDO   | Motor Receive Process Data Object              |
| 0x30       | Motor TXPDO   | Motor Transmit Process Data Object             |
| 0x60       | Force TXPDO   | Force sensor transmission data                 |
| 0x61       | IMU TXPDO     | Inertial Measurement Unit transmission data    |
| 0x62       | Encoder TXPDO | Encoder transmission data                      |

### 1.3 Protocol Format Specification

#### PDO (Process Data Object) Format
PDO is used for periodic transmission of real-time data, supporting multiple PDO types combined in a single UDP packet.

**PDO Packet Structure:**
```
+---------------+-------------------+-----------------+---------------+---------------+---------------------+
|  Type (1 byte)|  Length (2 bytes) |  Data (N bytes) |  Type (1 byte)|  Length (2 bytes) |  Data (N bytes) | ...
+---------------+-------------------+-----------------+---------------+---------------+---------------------+
```

**Field Description:**

| Field  | Length  | Description                                                                         |
|--------|---------|-------------------------------------------------------------------------------------|
| Type   | 1 byte  | PDO type identifier (0x20: Motor RXPDO, 0x30: Motor TXPDO, 0x60-0x62: Sensor TXPDO) |
| Length | 2 bytes | Length of subsequent data field (in bytes)                                          |
| Data   | N bytes | Specific PDO data content, format depends on PDO type                               |

#### SDO (Service Data Object) Format
SDO is used for configuration parameter transmission and device status query, using request-response mode.

**SDO Packet Structure:**
```
+---------------+-------------------+--------------+------------------+---------------------+--------------------------+-----------------+
|  Type (1 byte)|  Length (2 bytes) |  ID (1 byte) |  Opcode (1 byte) |  Op Length (1 byte) |  Register Addr (2 bytes) |  Data (N bytes) | ...
+---------------+-------------------+--------------+------------------+---------------------+--------------------------+-----------------+
```

**Field Description:**

| Field          | Length  | Description                                                                          |
|----------------|---------|--------------------------------------------------------------------------------------|
| Type           | 1 byte  | SDO type identifier (0x10)                                                         |
| Length         | 2 bytes | Length of subsequent data field (in bytes)                                           |
| ID             | 1 byte  | Device identifier                                                                    |
| Opcode         | 1 byte  | Operation type (0x01: Read, 0x02: Write, 0x03: Response, 0x04: Error)                |
| Op Length      | 1 byte  | Length of subsequent operation data field (in bytes)                                 |
| Register Addr  | 2 bytes | Target register address                                                              |
| Data           | N bytes | Read/write data content                                                              |

### 1.4 Communication Ports
| Port Number | Port Type | Description |
|-------------|-----------|-------------|
| 9110        | Motor PDO | Motor Process Data Object communication port |
| 8880        | Motor SDO | Motor Service Data Object communication port |
| 8887        | CM7 Debug | Cortex-M7 processor debug port |
| 8884        | CM4 Debug | Cortex-M4 processor debug port |

### 1.5 Communication Examples

**PDO Communication Example (multiple PDOs combined):**
```
UDP Packet: [0x30][0x0018][Motor TXPDO Data][0x61][0x0020][IMU Data]
```

**SDO Communication Example (parameter configuration):**
```
Write Operation: [0x10][0x000C][0x01][0x01][0x04][0x1000][0x3F800000] // Write motor parameters
Read Operation: [0x10][0x0008][0x01][0x00][0x04][0x1000][00000000] // Read motor parameters
```

## 2. Data Structure Definitions

### 2.1 Basic Data Types

#### ROFU_TYPE_T (Enum)
Defines device type identifiers used in the protocol.

```c
typedef enum {
    ROFU_NOUSED_TYPE = 0x00,

    // Synchronization frame
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
```

#### ROFU_PORT_T (Enum)
Communication port definitions.
```c
typedef enum {
    // Motor PDO communication port
    ROFU_MOTOR_PDO_PORT = 9110,
    // Motor SDO communication port
    ROFU_MOTOR_SDO_PORT = 8880,
    // CM7 debug port
    ROFU_DEBUG_M7_PORT = 8887,
    // CM4 debug port
    ROFU_DEBUG_M4_PORT = 8884,
} ROFU_PORT_T; // Port number
```

#### ROFU_PDO_T (Structure)
Generic Process Data Object structure.
```c
typedef struct {
    uint8_t type;      // Data type
    uint16_t len;      // Data length
    uint8_t *pdata;    // Data pointer
} ROFU_PDO_T;
```

#### ROFU_SDO_T (Structure)
Service Data Object structure.
```c
typedef struct {
    uint8_t type;      // Data type
    uint16_t len;      // Data length
    uint8_t id;        // Device ID
    uint8_t op;        // Operation code
    uint8_t op_len;    // Operation data length
    uint16_t addr;     // Register address
    uint8_t *pdata;    // Data pointer
} ROFU_SDO_T;
```

#### ROFU_SDO_OP_T (Enum)
SDO operation code definitions.
```c
typedef enum {
    ROFU_NOUSED_READ = 0x00,

    // SDO read operation
    ROFU_SDO_OP_READ = 0x01,

    // SDO write operation
    ROFU_SDO_OP_WRITE = 0x02,

    // SDO response
    ROFU_SDO_OP_RESPONSE = 0x03,

    // SDO error
    ROFU_SDO_OP_ERROR = 0x04,
} ROFU_SDO_OP_T;
```

#### ROFU_SDO_STATE_T (Enum)
SDO state definitions.
```c
typedef enum {
    ROFU_SDO_FRAME_OK = 0x00,

    // Undefined frame
    ROFU_SDO_FRAME_NO_DEFINED = 0x01,

    // Frame length error
    ROFU_SDO_FRAME_LEN_ERROR = 0x02,

    // PDO frame timeout
    ROFU_SDO_FRAME_OUTTIME = 0x03,
} ROFU_SDO_STATE_T;
```

#### ROFU_SDO_ADDR_T (Enum)
SDO register address definitions.
```c
typedef enum {
    ROFU_SDO_ADDR_NOUSED = 0x0000,

    // Motor SDO addresses
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

    // Three-loop PID addresses
    ROFU_SDO_ADDR_PID_ALL = 0x2000,
    ROFU_SDO_ADDR_PID_CUR_KP = 0x2001,
    ROFU_SDO_ADDR_PID_CUR_KI = 0x2002,
    ROFU_SDO_ADDR_PID_VEL_KP = 0x2003,
    ROFU_SDO_ADDR_PID_VEL_KI = 0x2004,
    ROFU_SDO_ADDR_PID_POS_KP = 0x2005,

    // Protection threshold addresses
    ROFU_SDO_ADDR_THR_ALL = 0x2010,
    ROFU_SDO_ADDR_THR_VBUS_HIGH = 0x2011,
    ROFU_SDO_ADDR_THR_VBUS_LOW = 0x2012,
    ROFU_SDO_ADDR_THR_IPHASE_HIGH = 0x2013,
    ROFU_SDO_ADDR_THR_MOS_TEMP_MAX = 0x2014,
    ROFU_SDO_ADDR_THR_MOTOR_TEMP_MAX = 0x2015,
    ROFU_SDO_ADDR_THR_MOTOR_SPEED_MAX = 0x2016,
} ROFU_SDO_ADDR_T;
```

## 3. Motor Control Protocol
#### ROFU_MOTOR_CTRL_T (Union)

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
### 3.2 Control Mode Description

| Mode Value | Mode Name | Description |
|------------|-----------|-------------|
| 0          | MIT       | Torque control mode |
| 1          | PP        | Position profile control mode |
| 2          | PV        | Velocity profile control mode |
| 3          | PT        | Torque profile control mode |
| 4          | CSP       | Cyclic position control mode |
| 5          | CSV       | Cyclic velocity control mode |
| 6          | CST       | Cyclic torque control mode |

### 3.3 Motor Header Structures

#### RX Header Structure (ROFU_MOTOR_RXHEAD_T)
```c
typedef struct {
    uint8_t id;                    // Motor ID number
    uint16_t seq;                  // Command sequence number
    ROFU_MOTOR_CTRL_T ctrl; // Control word
} ROFU_MOTOR_RXHEAD_T;
```

#### TX Header Structure (ROFU_MOTOR_TXHEAD_T)
```c
typedef struct {
    uint8_t id;
    uint16_t seq;
    ROFU_MOTOR_CTRL_T state;
} ROFU_MOTOR_TXHEAD_T;
```

### 3.4 Complete RXPDO Structure

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

### 3.5 Complete TXPDO Structure

```c
typedef struct {
    ROFU_MOTOR_TXHEAD_T header;
    union {
        ROFU_MOTOR_TXPDO0_T pdo0;
    };
} ROFU_MOTOR_TXPDO_T;
```

### 3.6 RXPDO Data Structures

#### MIT Mode (ROFU_MOTOR_RXPDO_MIT_T)
```c
typedef struct {
    int16_t kp;           // Proportional gain
    int32_t pos_target;   // Target position
    int16_t kd;           // Derivative gain
    int32_t vel_target;   // Target velocity
    int16_t taff;         // Torque compensation
} ROFU_MOTOR_RXPDO_MIT_T;
```

#### PT Mode (ROFU_MOTOR_RXPDO_PT_T)
```c
typedef struct {
    int16_t tor_target;  // Target torque
    int16_t tor_profile; // Torque slope
} ROFU_MOTOR_RXPDO_PT_T;
```

#### PV Mode (ROFU_MOTOR_RXPDO_PV_T)
```c
typedef struct {
    int32_t vel_target;  // Target velocity
    int32_t vel_max;     // Maximum velocity
    int32_t acc_profile; // Acceleration slope
    int32_t dec_profile; // Deceleration slope
} ROFU_MOTOR_RXPDO_PV_T;
```

#### PP Mode (ROFU_MOTOR_RXPDO_PP_T)
```c
typedef struct {
    int32_t pos_target;  // Target position
    int32_t vel_target;  // Target velocity
    int32_t acc_profile; // Acceleration slope
    int32_t dec_profile; // Deceleration slope
} ROFU_MOTOR_RXPDO_PP_T;
```

#### CST Mode (ROFU_MOTOR_RXPDO_CST_T)
```c
typedef struct {
    int16_t tor_target; // Target torque
} ROFU_MOTOR_RXPDO_CST_T;
```

#### CSV Mode (ROFU_MOTOR_RXPDO_CSV_T)
```c
typedef struct {
    int32_t vel_target; // Target velocity
} ROFU_MOTOR_RXPDO_CSV_T;
```

#### CSP Mode (ROFU_MOTOR_RXPDO_CSP_T)
```c
typedef struct {
    int32_t pos_target; // Target position
} ROFU_MOTOR_RXPDO_CSP_T;
```

### 3.8 TXPDO Data Structure

#### Motor Status Data (ROFU_MOTOR_TXPDO0_T)
```c
typedef struct {
    int32_t pos_real;     // Actual position
    int32_t vel_real;     // Actual velocity
    int16_t tor_real;     // Actual torque
    int16_t mos_temper;   // MOS temperature
    int16_t motor_temper; // Motor temperature
    int16_t error_code;   // Error code
} ROFU_MOTOR_TXPDO0_T;
```

### 3.9 SDO Configuration Parameters

#### PID Parameter Configuration (ROFU_MOTOR_SDO_PID_T)
```c
typedef struct {
    int16_t cur_kp;     // Current KP gain
    int16_t cur_ki;     // Current KI gain
    int16_t vel_kp;     // Velocity KP gain
    int16_t vel_ki;     // Velocity KI gain
    int16_t pos_kp;     // Position KP gain
} ROFU_MOTOR_SDO_PID_T; // Three-loop PID parameters
```

#### Motor Parameter Configuration (ROFU_MOTOR_SDO_CONFIG_T)
```c
typedef struct {
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
```

#### Protection Threshold Configuration (ROFU_MOTOR_SDO_THR_T)
```c
typedef struct {
    uint16_t vbus_high;      // Vbus high threshold 
    uint16_t vbus_low;       // Vbus low threshold 
    uint16_t iphase_high;    // Phase current high threshold 
    uint16_t mos_temper;     // MOS temperature threshold 
    uint16_t motor_temper;   // Motor temperature threshold 
    uint16_t speed_max;      // Maximum speed threshold 
} ROFU_MOTOR_SDO_THR_T; // Protection thresholds
```

## 4. Sensor Protocol

#### IMU Sensor Data (ROFU_IMU_TXPDO_T)
```c
typedef struct {
    uint8_t id;                 // Device ID
    uint16_t seq;               // Sequence number
    uint32_t sample_time_fine;  // Sample time
    int16_t quaternion[4];      // Quaternion (w, x, y, z)
    int16_t acceleration[3];    // Acceleration (x, y, z)
    int16_t rate_of_turn[3];    // Angular velocity (x, y, z)
    uint8_t state;              // Status word
    uint8_t err_code;           // Error code
} ROFU_IMU_TXPDO_T;
```
#### Force Sensor Data (ROFU_FT_TXPDO_T)
```c
typedef struct {
    uint8_t id;                 // Device ID
    uint16_t seq;               // Sequence number
    int16_t fx;                 // X-axis force
    int16_t fy;                 // Y-axis force
    int16_t fz;                 // Z-axis force
    int16_t mx;                 // X-axis moment
    int16_t my;                 // Y-axis moment
    int16_t mz;                 // Z-axis moment
    uint8_t err_code;           // Error code
} ROFU_FT_TXPDO_T;
```
#### Encoder Data (ROFU_ENCODE_TXPDO_T)
```c
typedef struct {
    uint8_t id;                 // Device ID
    uint16_t seq;               // Sequence number
    int32_t pos;                // Position value
    uint8_t state;              // Status word
} ROFU_ENCODE_TXPDO_T;
```
## 5. Communication Protocol Specification

### 5.1 Data Packet Format
All data packets use little-endian byte order, with structure packing aligned to 1 byte.

### 5.2 Error Handling
- Error codes are 32-bit unsigned integers
- Error codes are manufacturer-defined

### 5.3 Communication Timing
- Recommended communication frequency: 1kHz
- Timeout period: 100ms

## 6. Appendix

### 6.1 Version History
| Version | Date       | Description |
|---------|------------|-------------|
| 1.0     | 2026-04-13 | Initial version |

### 6.2 Technical Notes
- All structures use `#pragma pack(1)` for 1-byte alignment packing
- Protocol supports PDO (Process Data Object) and SDO (Service Data Object) communication modes
- Motor control supports 8 different control modes, each with corresponding data structures
- Sensor data includes IMU, force sensor, and encoder measurement data
- All data uses little-endian format

### 6.3 Compilation Environment
- CubeIDE Version: V1.18.0
- CubeMX Version: V6.14.0
- Chip Model: STM32H755

## 7. Contact Information
For more information about RoboFusion technology, please email RoboticsX@tencent.com.