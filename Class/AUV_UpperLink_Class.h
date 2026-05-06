#ifndef __AUV_UPPERLINK_CLASS_H__
#define __AUV_UPPERLINK_CLASS_H__

#include "UART_Class.h"

struct AUV_Status_t {
    // 传感器数据
    float vol = 0.0f;
    float tem = 0.0f;
    float hum = 0.0f;
    uint8_t leak = 0;
    
    // 执行器反馈
    uint8_t magnet = 0;
    int led[2] = {0, 0};
    float servo_angle[2] = {0.0f, 0.0f};
    
    // 运动反馈
    struct {
        float pitch = 0.0f;
        float roll = 0.0f;
        float yaw = 0.0f;
    } imu;
    
    int pid_state[8] = {0};
    float thrust[6] = {0.0f};
};

class AUV_UpperLink_Class : public MCU_UART 
{
public:
    AUV_UpperLink_Class(UART_HandleTypeDef* huart);

    void reportDeviceManager(const AUV_Status_t& status);
    void reportMotionController(const AUV_Status_t& status);

    void parseJsonPacket(uint8_t* data, uint16_t len);

private:
    void dispatchCommand(const char* jsonStr);
    
    void handleMotorCmd(cJSON* value);
    void handleTargetPosCmd(cJSON* value);
    void handlePidParamCmd(cJSON* value);
    void handleServoCmd(cJSON* value);
    void handleLedCmd(cJSON* value);
};

#endif