#include "NAV300_Class.h"

// 全局实例指针：用于将无上下文的 PacketCallback 转发到对象实例
// 注意：当前实现假设程序中只有一个 NAV300 实例。如果需要多实例，请修改 PacketCallback 签名支持上下文指针。
static NAV300* g_nav300_instance = nullptr;

// 接收回调（无捕获，匹配 PacketCallback）
static void nav300_rx_callback(uint8_t* data, uint16_t len)
{
    if (g_nav300_instance) g_nav300_instance->onPacket(data, len);
}

void NAV300::Init(void)
{
    // 启动独立接收 UART 的中断接收
    rx_uart.start_receive_interrupt();

    dvl_startup();
    imu_setmode();
}

void NAV300::imu_setmode() // 设置室内导航模式
{
    uint8_t BUFIMUSETMODE[10] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31};
    tx_uart.transmit(BUFIMUSETMODE, 10);
}

void NAV300::dvl_shutdown() // 关闭DVL
{
    uint8_t BUFDVLSHUTDOWN[10] = {0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30};
    tx_uart.transmit(BUFDVLSHUTDOWN, 10);
}

void NAV300::dvl_startup() // 打开DVL
{
    uint8_t BUFDVLPOWERON[10] = {0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31};
    tx_uart.transmit(BUFDVLPOWERON, 10);
}

IMU NAV300::ImuSolve()
{
    float imu_data[12]; // 前6位位置姿态矢量 后6位速度矢量

    // 使用本地回调缓存读取接收数据
    if (local_rx_len >= 84) { // 要读取到最多使用到的偏移（80+4），确保数据完整
        memcpy(&(imu_data[3]), local_rx_packet + 2, 12); // 姿态 (连续3个float)
        memcpy(&(imu_data[0]), local_rx_packet + 54, 8); // x y  (连续2个float)
        memcpy(&(imu_data[2]), local_rx_packet + 80, 4); // Z    (1个float)

        imu.imustate = local_rx_packet[66];              // 导航状态
        imu.dvlstate = local_rx_packet[67];
    } else {
        // 如果数据不足，置零或保持原值（此处清零以示异常）
        memset(imu_data, 0, sizeof(imu_data));
        imu.imustate = 0;
        imu.dvlstate = 0;
    }

    // 读取IMU数据
    imu.pos.y  = imu_data[0];
    imu.pos.x  = -imu_data[1];

    // 用深度计计算z轴位置，避免IMU的漂移
    //imu.pos.z  = imu_data[2];

    imu.pos.ry = imu_data[3];
    imu.pos.rx = imu_data[4];
    imu.pos.rz = imu_data[5];
    // if (imu_data[0] > -10 && imu_data[0] < 10) imu.pos.y = imu_data[0];
    // if (imu_data[1] > -10 && imu_data[1] < 10) imu.pos.x = -imu_data[1];
    // if (imu_data[2] > 0 && imu_data[2] < 5) imu.pos.z = imu_data[2];
    // if (imu_data[3] > -180 && imu_data[3] < 180) imu.pos.ry = imu_data[3];
    // if (imu_data[4] > -180 && imu_data[4] < 180) imu.pos.rx = imu_data[4];
    // if (imu_data[5] > -180 && imu_data[5] < 180) imu.pos.rz = imu_data[5];

    // imu.spd.x = -imu_data[6];
    // imu.spd.y = imu_data[7];
    // imu.spd.z = imu_data[8];
    return imu;
}

NAV300::NAV300(UART_HandleTypeDef* huart_rx, UART_HandleTypeDef* huart_tx,
                             const uint8_t* head_rx, uint8_t h_len_rx, const uint8_t* tail_rx, uint8_t t_len_rx, calculate_type type_rx,
                             const uint8_t* head_tx, uint8_t h_len_tx, const uint8_t* tail_tx, uint8_t t_len_tx, calculate_type type_tx)
        : tx_uart(huart_tx, head_tx, h_len_tx, tail_tx, t_len_tx, type_tx),
            rx_uart(huart_rx, head_rx, h_len_rx, tail_rx, t_len_rx, type_rx)
{
    local_rx_len = 0;
    g_nav300_instance = this;
    rx_uart.setCallback(nav300_rx_callback);
}

void NAV300::onPacket(uint8_t* data, uint16_t len)
{
    uint16_t copy_len = (len < MAX_BUF_SIZE) ? len : MAX_BUF_SIZE;
    memcpy(local_rx_packet, data, copy_len);
    local_rx_len = copy_len;
}

NAV300::~NAV300()
{
    dvl_shutdown();
}
