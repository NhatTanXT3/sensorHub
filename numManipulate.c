/*
 * numManipulate.c
 *
 *  Created on: Sep 27, 2016
 *      Author: Nhat Tan
 */
/*-----system include----------------*/
#include <stdint.h>
//#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>


/*-----my include-------------------*/
#include "numManipulate.h"

float Map_y(float x, float x_min, float x_max,float y_min,float y_max)
{
	float tan_alpha;
	tan_alpha=(y_max-y_min)/(x_max-x_min);
	return(tan_alpha*(x-x_min)+y_min);
}

void float2str(float num, char *str){
	unsigned char numNeg,numID;
	long numDem;
	long numInt;

	numID=0;

	if (num<0)
	{
		num=-num;
		numNeg=1;
	}
	else
		numNeg=0;



	char a[10];

	numInt=num*100;

	a[numID++]=numInt%10+48;
	a[numID++]=numInt/10%10+48;
	a[numID++]='.';

	numInt=num;
	if(num<10)
	{
		a[numID++]=numInt+48;
	}
	else
	{
		a[numID++]=numInt%10+48;

		numDem=10;
		while(num>=numDem)
		{
			numInt=num/numDem;
			numDem*=10;
			a[numID++]=numInt%10+48;
		}
	}

	if(numNeg==1)
		a[numID++]='-';
	a[numID]='\0';

	//	char b[10];
	//	signed char i;
	//	char numInx=0;
	//	for (i=numID-1;i>=0;i--)
	//	{
	//		b[numInx++]=a[i];
	//	}
	//	b[numInx]='\0';

	//	char str[10];
	signed char i;
	char numInx=0;
	for (i=numID-1;i>=0;i--)
	{
		*(str+(numInx++))=a[i];
		//		numInx++;
	}
	*(str+numInx)='\0';

}

// hai lôi trong cu phap, (se bao loi doi voi compiler C99 stric)
//  + return con tro vao bien cuc bo
//  + dung index cho array co kieu la char
void int2num(int num,char *str)
{
	unsigned char numNeg,numID;
	long numDem;
	long numInt;

	numID=0;

	if (num<0)
	{
		num=-num;
		numNeg=1;
	}
	else
		numNeg=0;



	char a[10];

	//	numInt=num*100;

	//	a[numID++]=numInt%10+48;
	//	a[numID++]=numInt/10%10+48;
	//	a[numID++]='.';

	numInt=num;
	//	if(num<10)
	//	{
	//		a[numID++]=numInt+48;
	//	}
	//	else
	//	{
	a[numID++]=numInt%10+48;

	numDem=10;
	while(num>=numDem)
	{
		numInt=num/numDem;
		numDem*=10;
		a[numID++]=numInt%10+48;
	}
	//	}

	if(numNeg==1)
		a[numID++]='-';
	a[numID]='\0';


	signed char i;
	char numInx=0;
	for (i=0;i<numID;i++)
	{
		*(str+(numInx++))=a[numID-1-i];
	}
	*(str+numInx)='\0';
}

void set_float_value (char *string,float *kp)
{
	while (!(isdigit(*string) || *string == '-')) (string++);
	*kp=atof(string);
}

void set_int_value (char *string,int32_t *kp)
{
	while (! (isdigit(*string) || *string == '-')) (string++);
	//	while (! (isdigit(*string))) (string++);
	*kp=atoi(string);
}

//void set_position (char *string, int16_t *position)
//{
//	while(*string!='\0')
//	{
//
//		switch(*string){
//		case 'x':
//			*position=atoi(string+1);
//			break;
//		case 'y':
//			*(position+1)=atoi(string+1);
//			break;
//		case 'z':
//			*(position+2)=atoi(string+1);
//			break;
//		default:
//			break;
//		}
//		string++;
//	}
//
//}
