#ifndef __SEROV_CLASS_H
#define __SEROV_CLASS_H

#include "user_main.h"
#include "TIM_Class.h"

#define SERVO_MIN_ANGLE        0          // 最小角度
#define SERVO_MAX_ANGLE        180        // 最大角度
#define SERVO_MIN_PULSE        500        // 最小角度对应的脉宽(us)
#define SERVO_MAX_PULSE        2500       // 最大角度对应的脉宽(us)

class Servo : public MCU_TIM
{
private:
    float current_angle;
public:
    void Init(void);
    void set_angle(float angle);

    Servo(TIM_HandleTypeDef *htim, uint32_t Channel, float angle);
    ~Servo();
};

#endif