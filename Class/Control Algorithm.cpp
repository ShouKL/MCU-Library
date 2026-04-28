#include "Control_Algorithm.h"

bool PID::Compute()
{
   if(m_mode == PID_Mode::MANUAL) return false;

   unsigned long now = HAL_GetTick();
   unsigned long timeChange = (now - lastTime);
   if (timeChange < SampleTime) return false;

   double input = *m_Input;
   double error = *m_Setpoint - input;
   double dInput = input - lastInput;

   outputSum += (ki * error);

   if(m_pOn == PID_ProportionalMode::P_ON_M) 
      outputSum -= kp * dInput;

   outputSum = (outputSum > outMax) ? outMax : 
               (outputSum < outMin ? outMin : outputSum);

   double output = 0;
   if(m_pOn == PID_ProportionalMode::P_ON_E) output = kp * error;

   output += outputSum - kd * dInput;

   output = (output > outMax) ? outMax : 
               (output < outMin ? outMin : output);

   *m_Output = output;

   lastInput = input;
   lastTime = now;
   return true;
}

void PID::SetTunings(double Kp, double Ki, double Kd, PID_ProportionalMode POn)
{
   if (Kp<0 || Ki<0 || Kd<0) return;

   dispKp = Kp;
   dispKi = Ki;
   dispKd = Kd;
   m_pOn = POn;

   double SampleTimeInSec = ((double)SampleTime)/1000;
   kp = Kp;
   ki = Ki * SampleTimeInSec;
   kd = Kd / SampleTimeInSec;

  if(m_direction == PID_Direction::REVERSE)
   {
      kp = (0 - kp);
      ki = (0 - ki);
      kd = (0 - kd);
   }
}

void PID::SetSampleTime(unsigned long NewSampleTime)
{
   double ratio  = (double)NewSampleTime / (double)SampleTime;
   ki *= ratio;
   kd /= ratio;
   SampleTime = NewSampleTime;
}

void PID::SetOutputLimits(double Min, double Max)
{
   if(Min >= Max) return;
   outMin = Min;
   outMax = Max;

   if(m_mode == PID_Mode::AUTOMATIC)
   {
	   if(*m_Output > outMax) *m_Output = outMax;
	   else if(*m_Output < outMin) *m_Output = outMin;

	   if(outputSum > outMax) outputSum= outMax;
	   else if(outputSum < outMin) outputSum= outMin;
   }
}

void PID::SetMode(PID_Mode Mode)
{
    if(m_mode == PID_Mode::MANUAL && Mode == PID_Mode::AUTOMATIC)
        reset();

    m_mode = Mode;
}

void PID::reset()
{
   outputSum = *m_Output;
   lastInput = *m_Input;
   if(outputSum > outMax) outputSum = outMax;
   else if(outputSum < outMin) outputSum = outMin;
}

void PID::SetDirection(PID_Direction direction)
{
   if(m_mode == PID_Mode::AUTOMATIC && m_direction != direction)
   {
	   kp = (0 - kp);
      ki = (0 - ki);
      kd = (0 - kd);
   }
   m_direction = direction;
}

void PID::SetLinks(double* Input, double* Output, double* Setpoint) {
    m_Input = Input;
    m_Output = Output;
    m_Setpoint = Setpoint;
}

PID::PID(double* Input, double* Output, double* Setpoint,
   double Kp, double Ki, double Kd,  
	double outMin, double outMax,
   unsigned long SampleTime,	//default Controller Sample Time is 0.1 seconds;
		PID_Mode mode, PID_ProportionalMode POn, PID_Direction direction)
   : m_Output(Output), m_Input(Input), m_Setpoint(Setpoint), 
     dispKp(Kp), dispKi(Ki), dispKd(Kd), SampleTime(SampleTime), 
     outMin(outMin), outMax(outMax), m_mode(mode), m_pOn(POn), m_direction(direction)
{
    SetTunings(Kp, Ki, Kd, POn);

    lastTime = HAL_GetTick() - SampleTime;
}

PID::~PID()
{
}

void KalmanFilter::init(double* x_, double* P_, double* R_, double* Q_)
{
	for(int i = 0; i < stateSize; i++) x[i] = x_[i];
	for(int i = 0; i < stateSize * stateSize; i++) P[i] = P_[i];
	for(int i = 0; i < measSize * measSize; i++) R[i] = R_[i];
	for(int i = 0; i < stateSize * stateSize; i++) Q[i] = Q_[i];
}
double* KalmanFilter::predict(double* A_, double* B_, double* u_)
{
	for(int i = 0; i < stateSize * stateSize; i++) A[i] = A_[i];
	for(int i = 0; i < stateSize * uSize; i++) B[i] = B_[i];
	for(int i = 0; i < uSize; i++) u[i] = u_[i];
	
	// x = A*x + B*u
	double temp_x[100];
	double temp_bu[100];
	arm_matrix_instance_f64 mat_temp_x = {(uint16_t)stateSize, 1, temp_x};
	arm_matrix_instance_f64 mat_temp_bu = {(uint16_t)stateSize, 1, temp_bu};
	
	arm_mat_mult_f64(&mat_A, &mat_x, &mat_temp_x);
	arm_mat_mult_f64(&mat_B, &mat_u, &mat_temp_bu);
	for(int i=0; i<stateSize; i++) x[i] = temp_x[i] + temp_bu[i];

	// P = A*P*A^T + Q
	double AT[400];
	arm_matrix_instance_f64 mat_AT = {(uint16_t)stateSize, (uint16_t)stateSize, AT};
	arm_mat_trans_f64(&mat_A, &mat_AT);

	double AP[400];
	arm_matrix_instance_f64 mat_AP = {(uint16_t)stateSize, (uint16_t)stateSize, AP};
	arm_mat_mult_f64(&mat_A, &mat_P, &mat_AP);
	
	double APAT[400];
	arm_matrix_instance_f64 mat_APAT = {(uint16_t)stateSize, (uint16_t)stateSize, APAT};
	arm_mat_mult_f64(&mat_AP, &mat_AT, &mat_APAT);
	
	for(int i=0; i<stateSize*stateSize; i++) P[i] = APAT[i] + Q[i];

	return x;
}

double* KalmanFilter::predict(double* A_)
{
	for(int i = 0; i < stateSize * stateSize; i++) A[i] = A_[i];
	
	// x = A*x
	double temp_x[100];
	arm_matrix_instance_f64 mat_temp_x = {(uint16_t)stateSize, 1, temp_x};
	arm_mat_mult_f64(&mat_A, &mat_x, &mat_temp_x);
	for(int i = 0; i < stateSize; i++) x[i] = temp_x[i];

	// P = A*P*A^T + Q
	double AT[400];
	arm_matrix_instance_f64 mat_AT = {(uint16_t)stateSize, (uint16_t)stateSize, AT};
	arm_mat_trans_f64(&mat_A, &mat_AT);

	double AP[400];
	arm_matrix_instance_f64 mat_AP = {(uint16_t)stateSize, (uint16_t)stateSize, AP};
	arm_mat_mult_f64(&mat_A, &mat_P, &mat_AP);
	
	double APAT[400];
	arm_matrix_instance_f64 mat_APAT = {(uint16_t)stateSize, (uint16_t)stateSize, APAT};
	arm_mat_mult_f64(&mat_AP, &mat_AT, &mat_APAT);
	
	for(int i=0; i<stateSize*stateSize; i++) P[i] = APAT[i] + Q[i];

	return x;
}

void KalmanFilter::update(double* H_, double* z_meas)
{
	for(int i = 0; i < measSize * stateSize; i++) H[i] = H_[i];
	
	// Ht = H^T
	double Ht[400];
	arm_matrix_instance_f64 mat_Ht = {(uint16_t)stateSize, (uint16_t)measSize, Ht};
	arm_mat_trans_f64(&mat_H, &mat_Ht);

	// temp1 = H*P*Ht + R
	double HP[400];
	arm_matrix_instance_f64 mat_HP = {(uint16_t)measSize, (uint16_t)stateSize, HP};
	arm_mat_mult_f64(&mat_H, &mat_P, &mat_HP);

	double HPHt[400];
	arm_matrix_instance_f64 mat_HPHt = {(uint16_t)measSize, (uint16_t)measSize, HPHt};
	arm_mat_mult_f64(&mat_HP, &mat_Ht, &mat_HPHt);

	double temp1[400];
	arm_matrix_instance_f64 mat_temp1 = {(uint16_t)measSize, (uint16_t)measSize, temp1};
	for(int i=0; i<measSize*measSize; i++) temp1[i] = HPHt[i] + R[i];

	// temp2 = temp1.inverse()
	double temp2[400];
	arm_matrix_instance_f64 mat_temp2 = {(uint16_t)measSize, (uint16_t)measSize, temp2};
	arm_mat_inverse_f64(&mat_temp1, &mat_temp2);

	// K = P*Ht*temp2
	double PHt[400];
	arm_matrix_instance_f64 mat_PHt = {(uint16_t)stateSize, (uint16_t)measSize, PHt};
	arm_mat_mult_f64(&mat_P, &mat_Ht, &mat_PHt);

	double K[400];
	arm_matrix_instance_f64 mat_K = {(uint16_t)stateSize, (uint16_t)measSize, K};
	arm_mat_mult_f64(&mat_PHt, &mat_temp2, &mat_K);

	// z = H*x
	arm_mat_mult_f64(&mat_H, &mat_x, &mat_z);

	// x = x + K*(z_meas - z)
	double z_diff[100];
	arm_matrix_instance_f64 mat_z_diff = {(uint16_t)measSize, 1, z_diff};
	for(int i=0; i<measSize; i++) z_diff[i] = z_meas[i] - z[i];
	
	double K_zdiff[100];
	arm_matrix_instance_f64 mat_K_zdiff = {(uint16_t)stateSize, 1, K_zdiff};
	arm_mat_mult_f64(&mat_K, &mat_z_diff, &mat_K_zdiff);
	
	for(int i=0; i<stateSize; i++) x[i] += K_zdiff[i];

	// P = (I - K*H)*P = P - K*H*P
	double KH[400];
	arm_matrix_instance_f64 mat_KH = {(uint16_t)stateSize, (uint16_t)stateSize, KH};
	arm_mat_mult_f64(&mat_K, &mat_H, &mat_KH);

	double KHP[400];
	arm_matrix_instance_f64 mat_KHP = {(uint16_t)stateSize, (uint16_t)stateSize, KHP};
	arm_mat_mult_f64(&mat_KH, &mat_P, &mat_KHP);

	for(int i=0; i<stateSize*stateSize; i++) P[i] -= KHP[i];
}

KalmanFilter::KalmanFilter(int stateSize_, int measSize_, int uSize_) :stateSize(stateSize_), measSize(measSize_), uSize(uSize_)
{
	if (stateSize == 0 || measSize == 0) return;

	x = new double[stateSize]{0};
	A = new double[stateSize * stateSize]{0};
	for(int i=0; i<stateSize; i++) A[i*stateSize+i] = 1.0; // identity matrix
	
	if(uSize > 0) {
		u = new double[uSize]{0};
		B = new double[stateSize * uSize]{0};
	} else {
		u = nullptr;
		B = nullptr;
	}

	z = new double[measSize]{0};
	P = new double[stateSize * stateSize]{0};
	for(int i=0; i<stateSize; i++) P[i*stateSize+i] = 1.0; 

	H = new double[measSize * stateSize]{0};
	R = new double[measSize * measSize]{0};
	Q = new double[stateSize * stateSize]{0};

	// init matrix structs
	arm_mat_init_f64(&mat_x, (uint16_t)stateSize, 1, x);
	arm_mat_init_f64(&mat_z, (uint16_t)measSize, 1, z);
	arm_mat_init_f64(&mat_A, (uint16_t)stateSize, (uint16_t)stateSize, A);
	if (uSize > 0) {
		arm_mat_init_f64(&mat_B, (uint16_t)stateSize, (uint16_t)uSize, B);
		arm_mat_init_f64(&mat_u, (uint16_t)uSize, 1, u);
	}
	arm_mat_init_f64(&mat_P, (uint16_t)stateSize, (uint16_t)stateSize, P);
	arm_mat_init_f64(&mat_H, (uint16_t)measSize, (uint16_t)stateSize, H);
	arm_mat_init_f64(&mat_R, (uint16_t)measSize, (uint16_t)measSize, R);
	arm_mat_init_f64(&mat_Q, (uint16_t)stateSize, (uint16_t)stateSize, Q);
}

KalmanFilter::~KalmanFilter()
{
	delete[] x; delete[] A;
	if (uSize > 0) { delete[] u; delete[] B; }
	delete[] z; delete[] P; delete[] H; delete[] R; delete[] Q;
}
