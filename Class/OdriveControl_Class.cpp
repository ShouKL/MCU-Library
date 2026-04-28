#include "OdriveControl.h"
#include <stdarg.h>
#include <string.h>

OdriveControl::OdriveControl(UART_HandleTypeDef* _huart)
    : MCU_UART(_huart, nullptr, 0, nullptr, 0)
{
}

void OdriveControl::sendAscii(const char* format, ...) {
    char buf[128];
    va_list args;
    va_start(args, format);
    int len = vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);

    if (len > 0) {
        MCU_UART::transmit((uint8_t*)buf, (uint16_t)len);
    }
}

void OdriveControl::SetTrapTraj(int motor, float destination) {
    sendAscii("t %d %.4f\n", motor, destination);
}

void OdriveControl::SetPosition(int motor, float pos) {
    sendAscii("q %d %.4f\n", motor, pos);
}

void OdriveControl::SetPosition(int motor, float pos, float vel_lim, float torque_lim) {
    // If either limit provided (non-zero), send full command so user intent is explicit
    if (vel_lim != 0.0f || torque_lim != 0.0f)
        sendAscii("q %d %.4f %.4f %.4f\n", motor, pos, vel_lim, torque_lim);
    else
        SetPosition(motor, pos);
}

void OdriveControl::SetVelocity(int motor, float velocity, float torque_ff) {
    sendAscii("v %d %.4f %.4f\n", motor, velocity, torque_ff);
}

void OdriveControl::SetTorque(int motor, float torque) {
    sendAscii("c %d %.4f\n", motor, torque);
}

void OdriveControl::SetAxisState(int motor, AxisState state) {
    sendAscii("w axis%d.requested_state %d\n", motor, (int)state);
}

void OdriveControl::ClearErrors() {
    sendAscii("sc\n");
}

void OdriveControl::Reboot() {
    sendAscii("sr\n");
}

void OdriveControl::SaveConfig() {
    sendAscii("ss\n");
}

void OdriveControl::RequestVbusVoltage() {
    // mark pending so incoming line can be parsed
    pending = REQ_VBUS;
    sendAscii("r vbus_voltage\n");
}

void OdriveControl::RequestEncoderPos(int motor) {
    pending = REQ_ENCODER;
    pending_encoder_index = motor;
    sendAscii("r axis%d.encoder.pos_estimate\n", motor);
}

void OdriveControl::RequestAxisError(int motor) {
    pending = REQ_ERROR;
    pending_encoder_index = motor;
    sendAscii("r axis%d.error\n", motor);
}

void OdriveControl::EmergencyStop(int motor) {
    SetAxisState(motor, AXIS_IDLE);
}

void OdriveControl::EraseConfig() {
    sendAscii("se\n");
}

// Append incoming RX byte; call this from UART RX IRQ/DMA handler for each received byte
void OdriveControl::processRxByte(uint8_t b)
{
    if (b == '\r') return; // ignore CR
    if (rx_len + 1 >= sizeof(rx_buf)) rx_len = 0; // overflow -> reset
    rx_buf[rx_len++] = (char)b;
    rx_buf[rx_len] = '\0';
    if (b == '\n') {
        // full line received
        parseLine(rx_buf);
        rx_len = 0;
        pending = REQ_NONE;
    }
}

void OdriveControl::parseLine(const char* line)
{
    // Trim leading spaces
    while (*line == ' ' || *line == '\t') ++line;
    if (strlen(line) == 0) return;

    // If we're expecting an integer error code, parse as int
    if (pending == REQ_ERROR) {
        int err = 0;
        if (sscanf(line, "%d", &err) == 1) {
            if (pending_encoder_index >= 0 && pending_encoder_index < 4)
                last_axis_error[pending_encoder_index] = err;
        }
        return;
    }

    // Try to parse a float from the line for other requests
    float v = 0.0f;
    if (sscanf(line, "%f", &v) == 1) {
        if (pending == REQ_VBUS) {
            last_vbus = v;
        } else if (pending == REQ_ENCODER) {
            if (pending_encoder_index >=0 && pending_encoder_index < 4)
                last_encoder_pos[pending_encoder_index] = v;
        }
    }
}

OdriveControl::~OdriveControl() {}