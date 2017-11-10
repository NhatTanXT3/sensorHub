/*
 * numManipulate.h
 *
 *  Created on: Sep 27, 2016
 *      Author: Nhat Tan
 */
#ifndef NUMMANIPULATE_H_
#define NUMMANIPULATE_H_

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

extern float Map_y(float x, float x_min, float x_max,float y_min,float y_max);
// gan gia tri choi so cho bien kp
extern void set_float_value (char *string,float *kp);
extern void set_int_value (char *string,int32_t *kp);

//extern char * float2num(float num);
extern void float2str(float num, char *str);
extern void int2num(int num,char *str);
//extern void set_position (char *string, int16_t *position);

#endif /* NUMMANIPULATE_H_ */
