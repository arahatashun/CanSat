#ifndef MOTOR_RUN_H
#define MOTOR_RUN_H

extern int pwm_initializer();
extern int motor_stop(int);
extern int motor_forward(int, int);
extern int motor_back(int, int);
extern int motor_right(int, int);
extern int motor_left(int, int);

#endif
