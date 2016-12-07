#ifndef MadgwickAHRS_h
#define MadgwickAHRS_h

/* algorithm gain */
extern volatile float beta;

void ahrs_update(
	float gx, float gy, float gz,
	float ax, float ay, float az,
	float mx, float my, float mz,
	float* q0_o, float* q1_o, float* q2_o, float* q3_o
);

void ahrs_update_imu(
	float gx, float gy, float gz,
	float ax, float ay, float az,
	float* q0_o, float* q1_o, float* q2_o, float* q3_o
);

#endif