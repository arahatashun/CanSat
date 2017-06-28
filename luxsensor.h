#ifndef LUXSENSOR_H
#define LUXSENSOR_H

//C++から呼び出すため
#ifdef __cplusplus
extern "C" {
#endif

int luxsensor_initialize();
int luxsensor_close();
int getLux();
int isLight();

#ifdef __cplusplus
}
#endif

#endif
