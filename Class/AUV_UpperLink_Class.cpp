#include "AUV_UpperLink_Class.h"

void AuvLink_Callback_Glue(uint8_t* data, uint16_t len) 
{
    AuvLink.parseJsonPacket(data, len);
}

AUV_UpperLink_Class::AUV_UpperLink_Class(UART_HandleTypeDef* huart) 
    : MCU_UART(huart, nullptr, 0, nullptr, 0) 
{ 
    this->setCallback(AuvLink_Callback_Glue);
}

void AUV_UpperLink_Class::reportDeviceManager(const AUV_Status_t& s) 
{
    char buf[256];
    // 使用 snprintf 拼接，避开 cJSON 大量申请内存的开销，提高实时性
    int len = snprintf(buf, sizeof(buf), 
        "{\"name\":\"device_manager\",\"value\":{\"vol\":%.1f,\"tem\":%.1f,\"hum\":%.1f,"
        "\"leak\":%d,\"magnet\":%d,\"led\":[%d,%d],\"angle\":[%.2f,%.2f]}}\n",
        s.vol, s.tem, s.hum, s.leak, s.magnet, s.led[0], s.led[1], s.servo_angle[0], s.servo_angle[1]);
    
    if (len > 0) this->transmit((uint8_t*)buf, (size_t)len);
}

void AUV_UpperLink_Class::reportMotionController(const AUV_Status_t& s) 
{
    char buf[256];
    int len = snprintf(buf, sizeof(buf),
        "{\"name\":\"motion_controller\",\"value\":{\"imu\":{\"pitch\":%.2f,\"roll\":%.2f,\"yaw\":%.2f},"
        "\"pidstate\":[%d,%d,%d,%d,%d,%d,%d,%d],\"thrust\":[%.2f,%.2f,%.2f,%.2f,%.2f,%.2f]}}\n",
        s.imu.pitch, s.imu.roll, s.imu.yaw,
        s.pid_state[0], s.pid_state[1], s.pid_state[2], s.pid_state[3],
        s.pid_state[4], s.pid_state[5], s.pid_state[6], s.pid_state[7],
        s.thrust[0], s.thrust[1], s.thrust[2], s.thrust[3], s.thrust[4], s.thrust[5]);

    if (len > 0) this->transmit((uint8_t*)buf, (size_t)len);
}

void AUV_UpperLink_Class::parseJsonPacket(uint8_t* data, uint16_t len) 
{
    // 确保字符串以 null 结尾
    static char json_buffer[MAX_BUF_SIZE];
    uint16_t safe_len = (len >= MAX_BUF_SIZE) ? MAX_BUF_SIZE - 1 : len;
    memcpy(json_buffer, data, safe_len);
    json_buffer[safe_len] = '\0';

    this->dispatchCommand(json_buffer);
}

void AUV_UpperLink_Class::dispatchCommand(const char* jsonStr) 
{
    cJSON* root = cJSON_Parse(jsonStr);
    if (root == nullptr) return;

    cJSON* name = cJSON_GetObjectItem(root, "name");
    cJSON* value = cJSON_GetObjectItem(root, "value");

    if (name && cJSON_IsString(name) && value) {
        const char* cmdName = name->valuestring;

        if (strcmp(cmdName, "motor") == 0)             handleMotorCmd(value);
        else if (strcmp(cmdName, "target_pos") == 0)   handleTargetPosCmd(value);
        else if (strcmp(cmdName, "pid_param") == 0)    handlePidParamCmd(value);
        else if (strcmp(cmdName, "servo") == 0)        handleServoCmd(value);
        else if (strcmp(cmdName, "led") == 0)          handleLedCmd(value);
    }
    
    cJSON_Delete(root); // 必须释放内存
}

void AUV_UpperLink_Class::handleMotorCmd(cJSON* v) 
{
    // 提取 6 自由度控制量
    float x = (float)cJSON_GetObjectItem(v, "x")->valuedouble;
    float y = (float)cJSON_GetObjectItem(v, "y")->valuedouble;
    float rz = (float)cJSON_GetObjectItem(v, "rz")->valuedouble;
    // TODO: 调用 H750 的动力分配函数
}

void AUV_UpperLink_Class::handlePidParamCmd(cJSON* v) 
{
    const char* axis = cJSON_GetObjectItem(v, "axis")->valuestring;
    float p = (float)cJSON_GetObjectItem(v, "p")->valuedouble;
    float i = (float)cJSON_GetObjectItem(v, "i")->valuedouble;
    // TODO: 更新 PID 结构体变量
}

void AUV_UpperLink_Class::handleServoCmd(cJSON* v) 
{
    int id = cJSON_GetObjectItem(v, "id")->valueint;
    float angle = (float)cJSON_GetObjectItem(v, "angle")->valuedouble;
    // TODO: F407 控制 PWM 输出
}

void AUV_UpperLink_Class::handleLedCmd(cJSON* v) 
{
    int led0 = cJSON_GetObjectItem(v, "led0")->valueint;
    // HAL_GPIO_WritePin(...)
}

void AUV_UpperLink_Class::handleTargetPosCmd(cJSON* v) 
{
    float x = (float)cJSON_GetObjectItem(v, "x")->valuedouble;
    // TODO: 更新期望位姿
}
