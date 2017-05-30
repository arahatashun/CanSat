//luxsensor.h

#ifndef LUXSENSOR_H
#define LUXSENSOR_H

int luxsensor_initializer();
int luxsensor_close();
double calcuateLux();
int getLux();
int islight();

#endif
