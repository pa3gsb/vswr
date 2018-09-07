/*
      	VSWR
		
		Reading ....
	  
	2018 Johan PA3GSB
*/

#include <stdlib.h>
#include <stdio.h>
#include <pigpio.h>
#include <unistd.h>

#define MAX11613_ADDRESS	0x34		

unsigned char data[8];
unsigned int i2c_bus;

int main(int argc, char **argv)
{
	int i2c_handler = 0;
	
	if (gpioInitialise() < 0) {
		fprintf(stderr,"gpio could not be initialized. \n");
		exit(-1);
	}
	
	i2c_bus = 1;
	i2c_handler = i2cOpen(i2c_bus, MAX11613_ADDRESS, 0);
	
	if (i2c_handler < 0)  return -1;
	
	unsigned char config[1];
	config[0] = 0x07;
	int writeResult = i2cWriteDevice(i2c_handler, config, 1);
	if (writeResult != 0 ) {
		printf ("adc config error");
		return -1;
	}

	while (1) {

		int result = i2cReadDevice(i2c_handler, data, 8);
		int rev = (int)(((data[0] & 0x0F) <<8) | data[1]);
		printf("data AIN0 (rev) = %d \n", rev);

		//sample = (int)(((data[2]&0x0F)<<8)  | data[3]);
		//printf("data AIN1 = %d \n", sample);
		
		//sample = (int)(((data[4]&0x0F)<<8) | data[5]);
		//printf("data AIN2 = %d \n", sample);
		
		int fwv = (int)(((data[6]&0x0F)<<8) | data[7]);
		printf("data AIN3 (fwv) = %d \n", fwv);
		

		printf("\n\n");
		
		double constant1=3.3;	//reference voltage
        double constant2=0.09;	//bridge voltage
		double v1;
		v1=((double)fwv/4095.0)*constant1;
		double fw_pwr=(v1*v1)/constant2;
		
		printf("forward pwr = %f \n", fw_pwr);

		double rev_pwr = 0.0;
		
		v1=((double)rev/4095.0)*constant1;
		rev_pwr=(v1*v1)/constant2;
		
		printf("rev pwr = %f \n", rev_pwr);
		
		double swr=(fw_pwr+rev_pwr)/(fw_pwr-rev_pwr);
		if(swr<0.0) swr=1.0;
		printf("SWR: %1.1f:1",swr);
		
		printf("\n\n");
		
		sleep (1);
	}
		
	i2cClose(i2c_handler);
	gpioTerminate();
}

