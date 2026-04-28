#ifndef __CONTROL_ALGORITHM_H
#define __CONTROL_ALGORITHM_H

#include "user_main.h"
#include "arm_math.h"
#include "main.h"

enum PID_Mode { AUTOMATIC, MANUAL };
enum PID_Direction { DIRECT, REVERSE };    //正作用 反作用 
enum PID_ProportionalMode { P_ON_E, P_ON_M };// Proportional on Error;  Proportional on Measurement(输出平滑变化)

class PID
{
private:
    // 显示参数
	double dispKp;		
	double dispKi;
	double dispKd;
    
    //内部计算参数(经过采样时间缩放后的数值)
	double kp;
    double ki;
    double kd;

    PID_Mode m_mode;
	PID_Direction m_direction;
	PID_ProportionalMode m_pOn;

    double* m_Input;
    double* m_Output;
    double* m_Setpoint;

	unsigned long lastTime;
	double outputSum, lastInput;

	unsigned long SampleTime;
	double outMin, outMax;

public:
    bool Compute();
    void reset();

    double GetKp(){ return  dispKp; }
    double GetKi(){ return  dispKi; }
    double GetKd(){ return  dispKd; }
    PID_Mode GetMode(){ return  m_mode; }
    PID_Direction GetDirection(){ return m_direction; }
    PID_ProportionalMode GetPOn(){ return m_pOn; }

    void SetMode(PID_Mode mode);
    void SetDirection(PID_Direction direction);
    void SetSampleTime(unsigned long NewSampleTime);
		void SetOutputLimits(double Min, double Max);
    void SetTunings(double Kp, double Ki, double Kd, PID_ProportionalMode POn);
    void SetLinks(double* Input, double* Output, double* Setpoint);

    PID(double* Input, double* Output, double* Setpoint,
        double Kp, double Ki, double Kd, 
        double outMin, double outMax,
        unsigned long SampleTime = 100,	//default Controller Sample Time is 0.1 seconds;
        PID_Mode mode = PID_Mode::MANUAL, 
        PID_ProportionalMode POn = PID_ProportionalMode::P_ON_M, 
        PID_Direction direction = PID_Direction::REVERSE);
    ~PID();
};


class KalmanFilter
{
private:
	int stateSize; //state variable's dimenssion
	int measSize; //measurement variable's dimession
	int uSize; //control variables's dimenssion
	
	// arm_math double-precision matrix representations
	double* x;
	double* z;
	double* A;
	double* B;
	double* u;
	double* P; // covariance
	double* H;
	double* R; // measurement noise covariance
	double* Q; // process noise covariance

	// Matrix instances
	arm_matrix_instance_f64 mat_x;
	arm_matrix_instance_f64 mat_z;
	arm_matrix_instance_f64 mat_A;
	arm_matrix_instance_f64 mat_B;
	arm_matrix_instance_f64 mat_u;
	arm_matrix_instance_f64 mat_P;
	arm_matrix_instance_f64 mat_H;
	arm_matrix_instance_f64 mat_R;
	arm_matrix_instance_f64 mat_Q;

public:
	KalmanFilter(int stateSize_, int measSize_,int uSize_);
	~KalmanFilter();
	void init(double* x_, double* P_, double* R_, double* Q_);
	double* predict(double* A_);
	double* predict(double* A_, double* B_, double* u_);
	void update(double* H_, double* z_meas);
};

// 测试代码
// #include <fstream>

// using namespace std;
// #define N 1000
// #define T 0.01

// double func(double& x);
// double data_x[N],data_y[N];

// double func(double& x)
// {
// 	double res = 5*x*x;
// 	return res;
// }

// float sample(float x0, float v0, float acc, float t)
// {
// 	return x0 + v0*t + 1 / 2 * acc*t*t;
// }

// float GetRand()
// {
// 	return 0.5 * rand() / RAND_MAX - 0.25;
// }

// int main()
// {
// 	ofstream fout;
// 	fout.open("data.txt");
// 	float t;
// 	for (int i = 0; i < N; i++)
// 	{
// 		/*data_x[i] = i*T*10;
// 		data_y[i] = func(data_x[i]);*/
// 		t = i*T;
// 		data_x[i] = sample(0, -4, 0, t) + GetRand();
// 		data_y[i] = sample(0, 6.5, 0, t) + GetRand();
// 	}
// 	int stateSize = 6;
// 	int measSize = 2;
// 	int controlSize = 0;
// 	KalmanFilter kf(stateSize, measSize, controlSize);
// 	Eigen::MatrixXd A(stateSize, stateSize);
// 	A << 1, 0, T, 0, 1 / 2 * T*T, 0,
// 		0, 1, 0, T, 0, 1 / 2 * T*T,
// 		0, 0, 1, 0, T, 0, 
// 		0, 0, 0, 1, 0, T,
// 		0, 0, 0, 0, 1, 0,
// 		0, 0, 0, 0, 0, 1;
// 	//cout << A;
// 	Eigen::MatrixXd B(0,0);
// 	Eigen::MatrixXd H(measSize, stateSize);
// 	H << 1, 0, 0, 0, 0, 0,
// 		0, 1, 0, 0, 0, 0;
// 	//cout << H;
// 	Eigen::MatrixXd P(stateSize, stateSize);
// 	P.setIdentity();
// 	Eigen::MatrixXd R(measSize, measSize);
// 	R.setIdentity()*0.01;
// 	Eigen::MatrixXd Q(stateSize, stateSize);
// 	Q.setIdentity()*0.001;
// 	Eigen::VectorXd x(stateSize);
// 	Eigen::VectorXd u(0);
// 	Eigen::VectorXd z(measSize);
// 	z.setZero();
// 	Eigen::VectorXd res(stateSize);

// 	for (int i = 0; i < N; i++)
// 	{
// 		//cout << "state_" << i << ":\n";
// 		if (i == 0)
// 		{
// 			x << data_x[i], data_y[i], 0, 0, 0, 0;
// 			kf.init(x, P, R, Q);
// 		}
// 		res<< kf.predict(A);
// 		z << data_x[i], data_y[i];
// 		kf.update(H,z);
// 		fout << data_x[i] << " " << res[0] << " " << data_y[i] << " " << res[1] << " " << res[2] << " " << res[3] << " " << res[4] << " " << res[5] << endl;
// 	}
// 	fout.close();
// 	cout << "Done, use python script to draw the figure....\n";
// 	system("pause");
// 	return 0;
// }


#endif