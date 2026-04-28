#include "Servo_Class.h"

void Servo::Init(void) {
    MCU_TIM::start_pwm();

    set_angle(current_angle);
}

void Servo::set_angle(float angle)
{
    if (angle > 180)
    {
        angle = 180;
    }
    else if (angle < 0)
    {
        angle = 0;
    }
    current_angle = angle;

    uint16_t pulse = (uint16_t)((current_angle * (SERVO_MAX_PULSE - SERVO_MIN_PULSE) / 
                                (SERVO_MAX_ANGLE - SERVO_MIN_ANGLE)) + SERVO_MIN_PULSE);
    MCU_TIM::set_compare(pulse);
}

Servo::Servo(TIM_HandleTypeDef *htim, uint32_t Channel, float angle) : MCU_TIM(htim, Channel), current_angle(angle)
{
}

Servo::~Servo()
{
    MCU_TIM::stop_pwm();
}