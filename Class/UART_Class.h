#ifndef __UART_CLASS_H__
#define __UART_CLASS_H__

#include "user_main.h"

#define MAX_BUF_SIZE 256
#define MAX_SYNC_LEN 8 // 限制帧头/帧尾最大支持的字节数

enum calculate_type { CRC8, XOR };
enum Receive_State { WAIT_HEAD, RECEIVING };

// 定义回调函数类型
typedef void (*PacketCallback)(uint8_t* data, uint16_t len);

class MCU_UART {
protected:
    UART_HandleTypeDef* huart;
    Receive_State state;
    calculate_type crc_type;

    uint8_t frame_head[MAX_SYNC_LEN];
    uint8_t frame_tail[MAX_SYNC_LEN];
    uint8_t head_len;
    uint8_t tail_len;
    
    uint64_t head_window_val; 
    uint64_t frame_head_val;
    uint64_t head_mask;

    bool use_sync_bytes; // 透传模式标志位

    uint8_t tx_packet[MAX_BUF_SIZE];
    uint16_t tx_index;

    uint8_t rx_packet[MAX_BUF_SIZE];
    uint16_t rx_index;
    uint8_t rx_byte; // 硬件单字节缓冲

    PacketCallback _onPacket; // 回调函数成员

    uint8_t calculate_crc8(uint8_t *data, uint8_t size);
    uint8_t calculate_XOR(uint8_t *data, uint8_t size);

public:
    MCU_UART(UART_HandleTypeDef* huart, const uint8_t* head = nullptr, uint8_t h_len = 0,
         const uint8_t* tail = nullptr, uint8_t t_len = 0, calculate_type type = calculate_type::CRC8);
    ~MCU_UART();

    void setCallback(PacketCallback cb) { _onPacket = cb; }  //配合FIFO使用的回调函数设置（freertos streambuffer）
	// myUart.setCallback(my_packet_handler);
	// void my_packet_handler(uint8_t* data, uint16_t len) {
	//     xStreamBufferSendFromISR(xUartStreamBuffer, data, len, NULL);
	// }
    void start_receive_interrupt();
    void transmit(uint8_t* data, size_t size, uint32_t timeout = HAL_MAX_DELAY);
    void receive();
};

#endif