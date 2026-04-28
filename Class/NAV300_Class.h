#ifndef __NAV300_CLASS_H__
#define __NAV300_CLASS_H__

#include "user_main.h"
#include "UART_Class.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAV300_head_len 2
#define NAV300_tail_len 2
static const uint8_t default_head[] = {0xFC, 0xCF};
static const uint8_t default_tail[] = {0xFD, 0xDF};

typedef struct {
    float x;
    float y;
    float z;
    float rx;
    float ry;
    float rz;
} CoordinateVector;

#pragma pack(push, 1) // 强制 1 字节对齐，防止编译器插入填充字节
typedef struct {
    uint8_t imustate;
    uint8_t dvlstate;

    CoordinateVector pos;
    CoordinateVector spd;
} IMU;
#pragma pack(pop)     // 恢复默认对齐方式

//由于NAV300高频率串口的特殊性，采用两个串口独立收发数据
class NAV300
{
private:
    MCU_UART tx_uart;
    MCU_UART rx_uart;
    IMU imu;
    
    uint8_t local_rx_packet[MAX_BUF_SIZE];
    uint16_t local_rx_len;
public:
    void Init(void);

    MCU_UART* Get_rx_uart() { return &rx_uart; }
    MCU_UART* Get_tx_uart() { return &tx_uart; }
    void dvl_shutdown();
    void dvl_startup();
    void imu_setmode();
    IMU ImuSolve();

    // 双串口构造：支持为 RX/TX 分别指定帧头／帧尾和校验类型
    NAV300(UART_HandleTypeDef* huart_rx, UART_HandleTypeDef* huart_tx,
           const uint8_t* head_rx = default_head, uint8_t h_len_rx = NAV300_head_len,
           const uint8_t* tail_rx = default_tail, uint8_t t_len_rx = NAV300_tail_len,
           calculate_type type_rx = calculate_type::XOR,
           const uint8_t* head_tx = default_head, uint8_t h_len_tx = NAV300_head_len,
           const uint8_t* tail_tx = default_tail, uint8_t t_len_tx = NAV300_tail_len,
           calculate_type type_tx = calculate_type::XOR);

    // 回调入口：由 rx_uart 的 PacketCallback 调用
    void onPacket(uint8_t* data, uint16_t len);
    ~NAV300();
};

#endif


// while(1) {
//     // 1. 读取深度计
//     depthSensor.Read();
//     float current_z;
//     depthSensor.Depth(&current_z);

//     // 2. 解析 IMU 数据
//     // 注意：ImuSolve 应该在 UART 接收完成的回调中触发，或者轮询
//     IMU current_imu = nav300.ImuSolve();
    
//     // 3. 融合数据
//     current_imu.pos.z = current_z; // 用高精度的 MS5837 修正 Z 轴
    
//     // 4. 将结果交给控制算法（如 PID）
//     Do_Control(&current_imu);
    
//     Delay(10); 
// }