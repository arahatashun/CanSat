#ifndef PID_H
#define PID_H
//C++から呼び出すため
#ifdef __cplusplus
extern "C" {
#endif

typedef struct pid{
    double input;//入力値
    double setpoint;//目標値
    int output; //操作量(milliseconds) -100~100
    unsigned int lastTime;//前回の時間
    double Kp;//propotional constant
    double Ki; //integral constant
    double Kd;//differential constant
    double integral;
    double differential;
    double prev_error;//前の偏差
} Pid;

int pid_limiter(Pid* pid_limits);

int compute_output(Pid* pid_comp);

int pid_initialize(Pid* pid_init);

int pid_const_initialize(Pid* pid_init, double setpoint,
  double kp_value, double ki_value, double kd_value);

#ifdef __cplusplus
}
#endif
#endif
