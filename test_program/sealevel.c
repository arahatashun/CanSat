#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <wiringPiI2C.h>
#include "../bme280.h"

static const float ALTITUDE = 21.0; //現在地の高度

int main() {
  int fd = wiringPiI2CSetup(BME280_ADDRESS);
  if(fd < 0) {
    printf("Device not found");
    return -1;
  }

while(1){
  bme280_calib_data cal;
  readCalibrationData(fd, &cal);

  wiringPiI2CWriteReg8(fd, 0xf2, 0x01);   // humidity oversampling x 1
  wiringPiI2CWriteReg8(fd, 0xf4, 0x25);   // pressure and temperature oversampling x 1, mode normal

  bme280_raw_data raw;

  delay(1000);
  getRawData(fd, &raw);

  int32_t t_fine = getTemperatureCalibration(&cal, raw.temperature);
  float t = compensateTemperature(t_fine); // C
  float p = compensatePressure(raw.pressure, &cal, t_fine) / 100; // hPa
  float h = compensateHumidity(raw.humidity, &cal, t_fine);       // %
  float p_0 = cal_sealevel_pressure(p,t,ALTITUDE);

  printf("pressure:%.2f,temperature:%.2f,sealevel_pressure:%.2f\n",
    p, t, p_0);
}
  return 0;
}
