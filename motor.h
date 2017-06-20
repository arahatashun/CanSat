#ifndef MOTOR_H
#define MOTOR_H
//C++から呼び出すため
#ifdef __cplusplus
extern "C" {
#endif

int pwm_initialize();
int motor_stop();
int motor_forward(int);
int motor_back(int);
int motor_right(int);
int motor_left(int);
int motor_rotate(int);
int motor_stack();

#ifdef __cplusplus
}
#endif

#endif
