#ifndef __ODRIVECONTROL_CLASS_H__
#define __ODRIVECONTROL_CLASS_H__

#include "UART_Class.h"
#include <stdio.h>

enum AxisState : uint8_t {
    AXIS_UNDEFINED = 0,
    AXIS_IDLE = 1,
    AXIS_CLOSED_LOOP_CONTROL = 8,
    AXIS_FULL_CALIBRATION_SEQUENCE = 3
};

enum OdrivePendingRequest : uint8_t {
    REQ_NONE = 0,
    REQ_VBUS,
    REQ_ENCODER,
    REQ_ERROR
};

class OdriveControl : public MCU_UART
{
private:
    void sendAscii(const char* format, ...);
    void parseLine(const char* line);

    // RX parsing
    char rx_buf[128];
    size_t rx_len = 0;
    OdrivePendingRequest pending = REQ_NONE;
    int pending_encoder_index = 0;

    // last values
    float last_vbus = 0.0f;
    float last_encoder_pos[4] = {0};
    int last_axis_error[4] = {0};
    
public:
    OdriveControl(UART_HandleTypeDef* huart);

    void SetTrapTraj(int motor, float destination);
    void SetPosition(int motor, float pos); // position-only
    void SetPosition(int motor, float pos, float vel_lim, float torque_lim); // full
    void SetVelocity(int motor, float velocity, float torque_ff = 0);
    void SetTorque(int motor, float torque);
    
    void SetAxisState(int motor, AxisState state);
    void ClearErrors();
    void Reboot();
    void SaveConfig();

    void RequestVbusVoltage();
    void RequestEncoderPos(int motor);
    void RequestAxisError(int motor);

    void EmergencyStop(int motor);
    void EraseConfig();

    // Call this from UART RX ISR/DMA byte handler to accumulate and parse responses
    void processRxByte(uint8_t b);

    float getLastVbus() const { return last_vbus; }
    float getLastEncoderPos(int motor) const { return last_encoder_pos[motor]; }

    ~OdriveControl();

};

#endif