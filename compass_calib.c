#include "compass.h"
#include "compass_calib.h"
#include "motor.h"
#include <stdio.h>
#include <wiringPi.h>

static const int turn_calib_power = 40;//地磁気補正時turnするpower
static const int turn_calib_milliseconds = 100;//地磁気補正時turnするmilliseconds

int compass_offset_initialize(Cmps_offset *compass_offset)
{
	compass_offset->compassx_offset_max = 0;
	compass_offset->compassx_offset_min = 0;
	compass_offset->compassy_offset_max = 0;
	compass_offset->compassy_offset_min = 0;
	compass_offset->compassz_offset_max = 0;
	compass_offset->compassz_offset_min = 0;
	compass_offset->compassx_offset = 0;
	compass_offset->compassx_offset = 0;
	compass_offset->compassy_offset = 0;
	return 0;
}

static int rotate_to_calib(Cmps *compass_data)
{
	compass_value_initialize(compass_data);
	motor_right(turn_calib_power);
	delay(turn_calib_milliseconds);
	motor_stop();
	compass_read(compass_data);
	printf( "compass_x= %f, compass_y= %f, compass_z=%f",compass_data->compassx_value
	        ,compass_data->compassy_value
	        ,compass_data->compassz_value);
	return 0;
}

int cal_maxmin_compass(Cmps_offset *compass_offset,Cmps *compass_data)
{
	compass_offset_initialize(compass_offset);
	for(i = 0; i<10; i++)
	{
		rotate_to_calib(ompass_data);
		maxmin_compass(compass_offset,compass_data);
	}
	mean_compass_offset(compass_offset);
	return 0;
}

static int maxmin_compass(Cmps *compass_offset, Cmps *compass_data)
{
	if(compass_data->compassx_value > compass_offset->compassx_offset_max)
	{
		compass_offset->compassx_offset_max = compass_data->compassx_value;
	}
	else if(compass_data->compassx_value < compass_offset->compassx_offset_min)
	{
		compass_offset->compassx_offset_min = compass_data->compassx_value;
	}
	if(compass_data->compassy_value > compass_offset->compassy_offset_max)
	{
		compass_offset->compassy_offset_max = compass_data->compassy_value;
	}
	else if(compass_data->compassy_value < compass_offset->compassy_offset_min)
	{
		compass_offset->compassy_offset_min = compass_data->compassy_value;
	}
	if(compass_data->compassz_value > compass_offset->compassz_offset_max)
	{
		compass_offset->compassz_offset_max = compass_data->compassz_value;
	}
	else if(compass_data->compassz_value < compass_offset->compassz_offset_min)
	{
		compass_offset->compassz_offset_min = compass_data->compassz_value;
	}
	return 0;
}

static int mean_compass_offset(Cmps *compass_offset)
{
	compass_offset->compassx_offset = (compass_offset->compassx_offset_max + compass_offset->compassx_offset_min)/2;
	compass_offset->compassy_offset = (compass_offset->compassy_offset_max + compass_offset->compassy_offset_min)/2;
	compass_offset->compassz_offset = (compass_offset->compassz_offset_max + compass_offset->compassz_offset_min)/2;
	printf("x_offset=%f, y_offset=%f, z_offset=%f", compass_offset->compassx_offset
	       ,compass_offset->compassy_offset
	       ,compass_offset->compassz_offset);
	return 0;
}
