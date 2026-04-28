#include "ThrusterAlloc_Model.h"
#include <math.h>


void ThrusterAllocModel_8::Init(void){
    for (int i = 0; i < 8; i++) {
        BLDC_Drivers[i]->Init();
    }
}

const float ThrusterAllocModel_8::allocationMatrix_data[48] = {
    // Surge    Sway      Heave     Roll      Pitch     Yaw
    -0.4393f,  0.2977f, -0.0044f,  0.0000f,  0.0000f,  0.1292f, // T0
     0.4393f,  0.3112f,  0.0044f,  0.0000f,  0.0000f, -0.1292f, // T1
    -0.4154f, -0.2977f, -0.0044f,  0.0000f,  0.0000f, -0.1292f, // T2
     0.4154f, -0.3112f,  0.0044f,  0.0000f,  0.0000f,  0.1292f, // T3
    -0.0370f,  0.0000f, -0.2592f,  0.0000f,  0.0000f,  0.0132f, // T4
     0.0370f,  0.0000f, -0.2888f,  0.0000f,  0.0000f, -0.0132f, // T5
     0.0000f,  0.0000f,  0.0000f,  0.0000f,  0.0000f,  0.0000f, // T6
     0.0000f,  0.0000f,  0.0000f,  0.0000f,  0.0000f,  0.0000f  // T7
};

void ThrusterAllocModel_8::ThrustAllocate(float *askedthrust) //askedthrust要求机器人对外推力
{
    float motor_output[8] = {0};
    
    arm_matrix_instance_f32 allocMat = {8, 6, (float *)allocationMatrix_data};
    arm_matrix_instance_f32 askedMat = {6, 1, askedthrust};
    arm_matrix_instance_f32 motorMat = {8, 1, motor_output};
    
    // T = AllocMatrix * F
    arm_mat_mult_f32(&allocMat, &askedMat, &motorMat);

    // 等比例缩放限幅保护
    float max_scale = 1.0f; 
    for (int i = 0; i < 8; i++) {
        if (motor_output[i] > BLDC_Drivers[i]->get_up_limit()) {
            max_scale = fmaxf(max_scale, motor_output[i] / BLDC_Drivers[i]->get_up_limit());
        } else if (motor_output[i] < BLDC_Drivers[i]->get_down_limit()) {
            max_scale = fmaxf(max_scale, motor_output[i] / BLDC_Drivers[i]->get_down_limit());
        }
    }
    if (max_scale > 1.0f) {
        for (int i = 0; i < 8; i++) motor_output[i] /= max_scale;
    }

    // 推力转换为 PWM 并下发给电机
    for (int i = 0; i < 8; i++) {
        BLDC_Drivers[i]->refresh_pwm(motor_output[i]);

        // 双速率平滑控制
        int current_step = (fabs(last_pwm[i] - BLDC_Drivers[i]->get_default_pwm()) < 200) ? 
                            Motor_PWM_Change_Speed_LOW : Motor_PWM_Change_Speed_HIGH;

        if (last_pwm[i] < BLDC_Drivers[i]->get_pwm() - current_step) last_pwm[i] += current_step;
        else if (last_pwm[i] > BLDC_Drivers[i]->get_pwm() + current_step) last_pwm[i] -= current_step;
        else last_pwm[i] = BLDC_Drivers[i]->get_pwm();

        BLDC_Drivers[i]->set_compare((uint16_t)last_pwm[i]);
    }
}

void ThrusterAllocModel_8::reset()
{
    for (int i = 0; i < 6; i++) {
        if (pid_controllers[i]) pid_controllers[i]->reset();
        pid_in[i]  = 0.0;
        pid_out[i] = 0.0;
        pid_set[i] = 0.0;
        g_thrust_vector[i] = 0.0f;
    }

    for (int i = 0; i < 8; i++) {
        last_pwm[i] = BLDC_Drivers[i]->get_default_pwm();
        if (BLDC_Drivers[i]) {
            BLDC_Drivers[i]->refresh_pwm(0.0f);
            BLDC_Drivers[i]->set_compare(BLDC_Drivers[i]->get_default_pwm());
        }
    }
}

void ThrusterAllocModel_8::ExecuteClosedLoopControl(Vector6d* target_pos, SINS_Data* sensor_data)
{
    if (!target_pos || !sensor_data) return; 
    for (int i = 0; i < 6; i++) if (!pid_controllers[i]) return;

    // 获取当前状态欧拉角三角函数值
    float cx = cosf(sensor_data->att[0]); // Roll
    float sx = sinf(sensor_data->att[0]);
    float cy = cosf(sensor_data->att[1]); // Pitch
    float sy = sinf(sensor_data->att[1]);
    float cz = cosf(sensor_data->att[2]); // Yaw
    float sz = sinf(sensor_data->att[2]);
    
    // 按 Z-Y-X 顺序生成当前机器人的欧拉角旋转矩阵 R_real (3x3)
    float R_real_data[9] = {
        cy*cz, cz*sx*sy - cx*sz, cx*cz*sy + sx*sz,
        cy*sz, cx*cz + sx*sy*sz, cx*sy*sz - cz*sx,
        -sy,   cy*sx,            cx*cy
    };
    
    // 生成仅有偏航角的地平坐标系旋转矩阵 R_level (3x3)
    float R_level_data[9] = {
        cz, -sz, 0.0f,
        sz,  cz, 0.0f,
        0.0f,0.0f,1.0f
    };

    // 计算位置误差(世界坐标系下差值)
    float pos_diff_data[3] = {
        target_pos->x - sensor_data->pos[0],
        target_pos->y - sensor_data->pos[1],
        target_pos->z - sensor_data->pos[2]
    };
    
    // 位置误差投影到地平系 (err_pos = R_level^T * pos_diff)
    float err_pos_data[3] = {0};
    arm_matrix_instance_f32 R_level_mat = {3, 3, R_level_data};
    
    arm_matrix_instance_f32 R_level_trans_mat;
    float R_level_trans_data[9];
    R_level_trans_mat.numRows = 3; R_level_trans_mat.numCols = 3; R_level_trans_mat.pData = R_level_trans_data;
    arm_mat_trans_f32(&R_level_mat, &R_level_trans_mat);
    
    arm_matrix_instance_f32 pos_diff_mat = {3, 1, pos_diff_data};
    arm_matrix_instance_f32 err_pos_mat = {3, 1, err_pos_data};
    arm_mat_mult_f32(&R_level_trans_mat, &pos_diff_mat, &err_pos_mat);
    
    // 计算姿态误差
    float err_att_data[3] = {
        target_pos->rx - sensor_data->att[0],
        target_pos->ry - sensor_data->att[1],
        target_pos->rz - sensor_data->att[2]
    };

    // 角度过零标准化处理
    for (int i = 0; i < 3; ++i) {
        err_att_data[i] = atan2f(sinf(err_att_data[i]), cosf(err_att_data[i]));
    }

    // PID控制量加载计算
    for(int i = 0; i < 6; i++) {
        pid_set[i] = 0.0;
        if (i < 3) pid_in[i] = -(double)err_pos_data[i];
        else pid_in[i] = -(double)err_att_data[i - 3];
        pid_controllers[i]->Compute(); 
    }

    // 取出 PID 输出(地平系下的推力和扭矩)
    float f_level_data[3] = {(float)pid_out[0], (float)pid_out[1], (float)pid_out[2]};
    float t_level_data[3] = {(float)pid_out[3], (float)pid_out[4], (float)pid_out[5]};

    // 计算从地平坐标系到机器人真实坐标系的最终旋转关系：R_trans = R_real^T * R_level
    arm_matrix_instance_f32 R_real_mat = {3, 3, R_real_data};
    arm_matrix_instance_f32 R_real_trans_mat;
    float R_real_trans_data[9];
    R_real_trans_mat.numRows = 3; R_real_trans_mat.numCols = 3; R_real_trans_mat.pData = R_real_trans_data;
    arm_mat_trans_f32(&R_real_mat, &R_real_trans_mat);
    
    arm_matrix_instance_f32 R_trans_mat;
    float R_trans_data[9];
    R_trans_mat.numRows = 3; R_trans_mat.numCols = 3; R_trans_mat.pData = R_trans_data;
    arm_mat_mult_f32(&R_real_trans_mat, &R_level_mat, &R_trans_mat);

    // 转化出的目标力与力矩施加在机器人自己的固定物理坐标系下 (g_thrust_vector = R_trans * level_data)
    arm_matrix_instance_f32 f_level_mat = {3, 1, f_level_data};
    arm_matrix_instance_f32 g_thrust_f_mat = {3, 1, &g_thrust_vector[0]};
    arm_mat_mult_f32(&R_trans_mat, &f_level_mat, &g_thrust_f_mat);
    
    arm_matrix_instance_f32 t_level_mat = {3, 1, t_level_data};
    arm_matrix_instance_f32 g_thrust_t_mat = {3, 1, &g_thrust_vector[3]};
    arm_mat_mult_f32(&R_trans_mat, &t_level_mat, &g_thrust_t_mat);

    ThrustAllocate(g_thrust_vector);
}

ThrusterAllocModel_8::ThrusterAllocModel_8(BLDC_Driver _drivers[8], PID _pids[6])
{
    for (int i = 0; i < 8; i++) {
        BLDC_Drivers[i] = &_drivers[i];
        last_pwm[i] = BLDC_Drivers[i]->get_default_pwm();
    }
    
    for (int i = 0; i < 6; i++) {
        pid_controllers[i] = &_pids[i];
        pid_in[i]  = 0.0;
        pid_out[i] = 0.0;
        pid_set[i] = 0.0;

        if (pid_controllers[i]) {
            // 物理绑定 PID 类的双精度内部缓存
            pid_controllers[i]->SetLinks(&pid_in[i], &pid_out[i], &pid_set[i]);
        }
    }
}

ThrusterAllocModel_8::~ThrusterAllocModel_8() {}