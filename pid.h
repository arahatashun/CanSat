#ifndef PID_H
#define PID_H

typedef struct {
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
} pid;

int pid_limiter(pid* pid_limits);

int compute_output(pid* pid_comp);

int pid_initialize(pid* pid_init);


#endif
