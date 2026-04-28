#ifndef __BLDC_DRIVER_CLASS_H__
#define __BLDC_DRIVER_CLASS_H__

#include "user_main.h"
#include "TIM_Class.h"

#define Motor_PWM_Change_Speed_HIGH 160
#define Motor_PWM_Change_Speed_LOW 40
#define Default_PWM 1500
#define Motor_High_PWM 2000
#define Motor_Low_PWM 1000


// 推力曲线 
enum CurveMode{
    SEGMENTED_LINEAR,
    QUADRATIC_CURVE
};

//1.  2*反向线段 + 死区 + 2*正向线段
//2.  1*反向二次曲线 + 1*正向二次曲线
typedef struct {
    CurveMode mode;
    float pwm_parameter[6];       // 1. PWM 值 （六点） 2.  a1 b1 c1, a2 b2 c2
    float up_thrustlimit; // 推力上限
    float down_thrustlimit; // 推力下限
} ThrustCurve;

class BLDC_Driver : public MCU_TIM
{
private:
    uint16_t default_pwm;
    uint16_t high_pwm;
    uint16_t low_pwm;

    ThrustCurve m_thrustcurve;
    uint16_t pwm;

private:
    float linear_interpolate(float x1, float y1, float x2, float y2, float input);
    float quadratic_interpolate(float a, float b, float c, float x);
    
public:
    void Init(void);

    void refresh_pwm(float thrust);
    uint16_t get_pwm() const { return pwm; }

    float get_default_pwm() const { return default_pwm; }
    float get_high_pwm() const { return high_pwm; }
    float get_low_pwm() const { return low_pwm; }
    float get_up_limit() const { return m_thrustcurve.up_thrustlimit; }
    float get_down_limit() const { return m_thrustcurve.down_thrustlimit; }

    void calibrate();

    BLDC_Driver(TIM_HandleTypeDef* htim, uint32_t Channel, ThrustCurve thrustcurve, 
        uint16_t default_pwm = Default_PWM, uint16_t high_pwm = Motor_High_PWM, uint16_t low_pwm = Motor_Low_PWM);
    ~BLDC_Driver();

};

#endif
