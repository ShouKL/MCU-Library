#include "BLDC_Driver_Class.h"

void BLDC_Driver::Init(void)
{
    MCU_TIM::start_pwm();
    MCU_TIM::set_compare(default_pwm);
}

float BLDC_Driver::linear_interpolate(float x1, float y1, float x2, float y2, float input)
{
    float k = (y2 - y1) / (x2 - x1);
    return y1 + k * (input - x1);
}

float BLDC_Driver::quadratic_interpolate(float a, float b, float c, float x)
{
    return a * x * x + b * x + c;
}

void BLDC_Driver::refresh_pwm(float thrust)
{
    if (fabs(thrust) <= 1e-4) { pwm = default_pwm; }
    else if (thrust >= m_thrustcurve.up_thrustlimit) { pwm = high_pwm; }
    else if (thrust <= m_thrustcurve.down_thrustlimit) { pwm = low_pwm; }
    else if(m_thrustcurve.mode == CurveMode::SEGMENTED_LINEAR)
    {
        if (thrust > 0) {
            float mid_t = m_thrustcurve.up_thrustlimit / 2.0f; // 中点为推力上限的一半
            if (thrust < mid_t) {
                pwm = linear_interpolate(0, m_thrustcurve.pwm_parameter[3], mid_t, m_thrustcurve.pwm_parameter[4], thrust);
            } else {
                pwm = linear_interpolate(mid_t, m_thrustcurve.pwm_parameter[4], 
                    m_thrustcurve.up_thrustlimit, m_thrustcurve.pwm_parameter[5], thrust);
            }
        } 
        else if (thrust < 0) {
            float mid_t = m_thrustcurve.down_thrustlimit / 2.0f;
            if (thrust > mid_t) {
                pwm = linear_interpolate(mid_t, m_thrustcurve.pwm_parameter[1], 0, m_thrustcurve.pwm_parameter[2], thrust);
            } else {
                pwm = linear_interpolate(m_thrustcurve.down_thrustlimit, m_thrustcurve.pwm_parameter[0], 
                    mid_t, m_thrustcurve.pwm_parameter[1], thrust);
            }
        }
    }
    else if(m_thrustcurve.mode == CurveMode::QUADRATIC_CURVE){
        if (thrust < 0) pwm = quadratic_interpolate(m_thrustcurve.pwm_parameter[0], m_thrustcurve.pwm_parameter[1], 
                                                    m_thrustcurve.pwm_parameter[2], thrust);
        else if (thrust > 0) pwm = quadratic_interpolate(m_thrustcurve.pwm_parameter[3], m_thrustcurve.pwm_parameter[4], 
                                                    m_thrustcurve.pwm_parameter[5], thrust);
    }
}


void BLDC_Driver::calibrate()
{
    MCU_TIM::set_compare(high_pwm);
    MCU_TIM::set_compare(low_pwm);
}

BLDC_Driver::BLDC_Driver(TIM_HandleTypeDef* htim, uint32_t Channel, ThrustCurve thrustcurve, 
        uint16_t default_pwm, uint16_t high_pwm, uint16_t low_pwm)
        : MCU_TIM(htim, Channel), m_thrustcurve(thrustcurve), 
        default_pwm(default_pwm), high_pwm(high_pwm), low_pwm(low_pwm)
{
}

BLDC_Driver::~BLDC_Driver()
{
    MCU_TIM::stop_pwm();
}
