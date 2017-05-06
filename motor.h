#ifndef MOTOR_H
#define MOTOR_H

int pwm_initializer();
int motor_stop(int);
int motor_forward(int, int);
int motor_back(int, int);
int motor_right(int, int);
int motor_left(int, int);

#endif
