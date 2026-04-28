#include "UART_Class.h"
#include <string.h>

void MCU_UART::start_receive_interrupt() {
    if (huart != nullptr) {
        HAL_UART_Receive_IT(huart, &rx_byte, 1);
    }
}

void MCU_UART::transmit(uint8_t *data, size_t size, uint32_t timeout)
{
    if (!use_sync_bytes) {
        if (data != nullptr) {
            HAL_UART_Transmit(huart, data, size, timeout);
        }
        return;
    }
    if (size + head_len + tail_len + 1 > MAX_BUF_SIZE || data == nullptr) {
        return;
    }
    tx_index = 0;

    for (uint8_t i = 0; i < head_len; i++) {
        tx_packet[tx_index++] = frame_head[i];
    }
    for (size_t i = 0; i < size; i++) {
        tx_packet[tx_index++] = data[i];
    }
    tx_packet[tx_index++] = calculate_crc8(data, (uint8_t)size);
    for (uint8_t i = 0; i < tail_len; i++) {
        tx_packet[tx_index++] = frame_tail[i];
    }

    HAL_UART_Transmit(huart, tx_packet, tx_index, timeout);
}

void MCU_UART::receive() {
    // 透传模式：收到一个字节就直接触发回调
    if (!use_sync_bytes) {
        if (_onPacket != nullptr) {
            _onPacket(&rx_byte, 1);
        }
        HAL_UART_Receive_IT(huart, &rx_byte, 1);
        return;
    }
    switch (state) {
        case WAIT_HEAD:
        {
            // O(1) 移位操作：左移 8 位并拼入新字节
            head_window_val = (head_window_val << 8) | rx_byte;

            // 使用掩码比对有效位
            if ((head_window_val & head_mask) == frame_head_val) {
                state = RECEIVING;
                rx_index = 0;
                head_window_val = 0; // 清空防止下次误触发
            }
            break;
        }
        case RECEIVING:
            if (rx_index < MAX_BUF_SIZE) {
                rx_packet[rx_index++] = rx_byte;
            } else {
                state = WAIT_HEAD; // 溢出保护
                break;
            }
            if (rx_index >= tail_len) {
                bool tail_match = true;
                for (uint8_t i = 0; i < tail_len; i++) {
                    if (rx_packet[rx_index - tail_len + i] != frame_tail[i]) {
                        tail_match = false;
                        break;
                    }
                }
                if (tail_match) {
                    if (rx_index >= (uint16_t)(tail_len + 1)) {
                        uint16_t data_len = rx_index - tail_len - 1;
                        uint8_t rx_crc = rx_packet[data_len];

                        if (crc_type == calculate_type::CRC8) {
                            if (rx_crc == calculate_crc8(rx_packet, data_len) && _onPacket != nullptr) {
                                _onPacket(rx_packet, data_len);
                            }
                        } else if (crc_type == calculate_type::XOR) {
                            if (rx_crc == calculate_XOR(rx_packet, data_len) && _onPacket != nullptr) {
                                _onPacket(rx_packet, data_len);
                            }
                        }
                    }
                    state = WAIT_HEAD;
                }
            }
            break;
    }
    HAL_UART_Receive_IT(huart, &rx_byte, 1);
}

//CRC-8 校验
uint8_t MCU_UART::calculate_crc8(uint8_t *data, uint8_t size) {
    uint8_t crc = 0x00;
    for (uint8_t i = 0; i < size; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80) crc = (crc << 1) ^ 0x07;
            else crc <<= 1;
        }
    }
    return crc;
}

//异或校验
uint8_t MCU_UART::calculate_XOR(uint8_t *data, uint8_t size)
{
    uint8_t crc = 0x00;
    for (uint8_t i = 0; i < size; i++) {
        crc ^= data[i];
    }
    return crc;
}

MCU_UART::MCU_UART(UART_HandleTypeDef* huart, const uint8_t* head, uint8_t h_len, const uint8_t* tail, uint8_t t_len, calculate_type type) 
    : huart(huart), state(WAIT_HEAD), rx_index(0), tx_index(0), _onPacket(nullptr), crc_type(type)
{
    // 如果传入长度为0或空指针，则开启透传模式
    if (head == nullptr || h_len == 0 || tail == nullptr || t_len == 0) {
        use_sync_bytes = false;
        head_len = 0;
        tail_len = 0;
    } else {
        use_sync_bytes = true;
        head_len = (h_len > MAX_SYNC_LEN) ? MAX_SYNC_LEN : h_len;
        tail_len = (t_len > MAX_SYNC_LEN) ? MAX_SYNC_LEN : t_len;

        frame_head_val = 0;
        for (uint8_t i = 0; i < head_len; i++) {
            frame_head[i] = head[i]; 
            frame_head_val = (frame_head_val << 8) | frame_head[i];
        }
        
        head_window_val = 0;
        head_mask = (head_len >= 8) ? ~0ULL : ((1ULL << (head_len * 8)) - 1);

        memcpy(frame_tail, tail, tail_len);
    }

    memset(rx_packet, 0, MAX_BUF_SIZE);
    memset(tx_packet, 0, MAX_BUF_SIZE);
}

MCU_UART::~MCU_UART() {}