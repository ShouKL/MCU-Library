#ifndef __THRUSTERALLOC_MODEL_H__
#define __THRUSTERALLOC_MODEL_H__


#include "user_main.h"
#include "arm_math.h"
#include "BLDC_Driver_Class.h"
#include "Control_Algorithm.h"
typedef struct {
    float pos[3]; // 空间位置: x, y, z (通常为北东地NED坐标系 或 自定义全局坐标系)
    float spd[3]; // 线速度: vx, vy, vz
    float att[3]; // 欧拉角姿态: rx (Roll横滚), ry (Pitch俯仰), rz (Yaw偏航)
} SINS_Data;

typedef struct {
    float x;
    float y;
    float z;
    float rx;
    float ry;
    float rz;
} Vector6d;

class ThrusterAllocModel_8 {
public:
    void Init(void);

    void ThrustAllocate(float *askedthrust);
    void reset();
    void ExecuteClosedLoopControl(Vector6d* target_pos, SINS_Data* sensor_data);

    ThrusterAllocModel_8(BLDC_Driver drivers[8], PID pid_controllers[6]);
    ~ThrusterAllocModel_8();
    
private:
    BLDC_Driver* BLDC_Drivers[8];
    PID* pid_controllers[6];       // X,Y,Z, Roll,Pitch,Yaw

    double pid_in[6];              // PID 当前输入
    double pid_set[6];             // PID 目标设定值
    double pid_out[6];             // PID 运算输出量

    float last_pwm[8] = {0};       // 记录上一次发给电机的PWM占空比，用于双速率平滑输出
    float g_thrust_vector[6];      // 系统解算完毕的 6DOF 整体目标推力与扭矩向量
    Vector6d uuv_real;

    // 8x6 的推力分配矩阵
    static const float allocationMatrix_data[48];
};

#endif
