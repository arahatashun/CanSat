#ifndef MOTOR_H
#define MOTOR_H

int pwm_initializer();
int motor_stop();
int motor_forward(int);
int motor_back(int);
int motor_right(int);
int motor_left(int);
int motor_rotate(int);

int motor_stack();
#endif
