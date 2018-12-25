/*
 * DAC8532_test.c:
 *	Very simple program to test the serial port. Expects
 *	the port to be looped back to itself
 *
 */
 
/*
             define from bcm2835.h                       define from Board 
DVK511
                 3.3V | | 5V               ->                 3.3V | | 5V
    RPI_V2_GPIO_P1_03 | | 5V               ->                  SDA | | 5V 
    RPI_V2_GPIO_P1_05 | | GND              ->                  SCL | | GND
       RPI_GPIO_P1_07 | | RPI_GPIO_P1_08   ->                  IO7 | | TX
                  GND | | RPI_GPIO_P1_10   ->                  GND | | RX
       RPI_GPIO_P1_11 | | RPI_GPIO_P1_12   ->                  IO0 | | IO1
    RPI_V2_GPIO_P1_13 | | GND              ->                  IO2 | | GND
       RPI_GPIO_P1_15 | | RPI_GPIO_P1_16   ->                  IO3 | | IO4
                  VCC | | RPI_GPIO_P1_18   ->                  VCC | | IO5
       RPI_GPIO_P1_19 | | GND              ->                 MOSI | | GND
       RPI_GPIO_P1_21 | | RPI_GPIO_P1_22   ->                 MISO | | IO6
       RPI_GPIO_P1_23 | | RPI_GPIO_P1_24   ->                  SCK | | CE0
                  GND | | RPI_GPIO_P1_26   ->                  GND | | CE1

::if your raspberry Pi is version 1 or rev 1 or rev A
RPI_V2_GPIO_P1_03->RPI_GPIO_P1_03
RPI_V2_GPIO_P1_05->RPI_GPIO_P1_05
RPI_V2_GPIO_P1_13->RPI_GPIO_P1_13
::
*/
#include "adfunc.h"

#include <bcm2835.h>  
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>

//CS      -----   SPICS  
//DIN     -----   MOSI
//DOUT  -----   MISO
//SCLK   -----   SCLK
//DRDY  -----   ctl_IO     data  starting
//RST     -----   ctl_IO     reset






int  main(int argc, char *argv[])
{
   uint16_t   i,tmp;
   
	if (!bcm2835_init())
        return 1;

    // get parameters from command arguments
    float VDA0,VDA1;
    int duration;
    
    if(argc>3){
        duration = atoi(argv[3]);
        VDA0 = atof(argv[1]);
        VDA1 = atof(argv[2]);
    }else{
        duration = 100;
        VDA0 = 3;
        VDA1 = 3;
    }
        
    if(VDA0 < 0) VDA0 = 0;
    if(VDA1 < 0) VDA1 = 0;
    if(VDA0 > 5) VDA0 = 5;
    if(VDA1 > 5) VDA1 = 5;
    
   
    
    //initialize
    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);   //default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);                //default
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256);//default

    bcm2835_gpio_fsel(SPICS1, BCM2835_GPIO_FSEL_OUTP);// da?
    bcm2835_gpio_write(SPICS1, HIGH); 	
    bcm2835_gpio_fsel(DRDY, BCM2835_GPIO_FSEL_INPT);//ad?
    bcm2835_gpio_set_pud(DRDY, BCM2835_GPIO_PUD_UP); 


    // init AD module?
    uint8_t id;

    // DA output
    printf("Supposed DA output[V] DA0:%f DA1:%f \n",VDA0,VDA1);
    Write_DAC8532(channel_A, Voltage_Convert(5.0,VDA0));
    Write_DAC8532(channel_B, Voltage_Convert(5.0,VDA1));
    bsp_DelayUS(duration*1000); // sleep while

    // read start
  	ADS1256_CfgADC(ADS1256_GAIN_1, ADS1256_15SPS);
    ADS1256_StartScan(0);


    // Read AD Port
    int32_t adc0,adc1;
    float fV0,fV1;
    for(int k=0;k<10;k++){    
    while((ADS1256_Scan() == 0));
    }
    
    adc0 = ADS1256_GetAdc(0);
    adc1 = ADS1256_GetAdc(1);

    fV0 = (adc0*100.0)/167.0;
    fV1 = (adc1*100.0)/167.0;
    printf("Measured DA output V0 = %f, V1 = %f \n",fV0/1000000,fV1/1000000);
    bsp_DelayUS(5000);

    //Supposed magnitude
    float mag0,mag1;    
    mag0 = sqrt(fV0/VDA0/1000000);
    mag1 = sqrt(fV1/VDA1/1000000);
    printf("Supposed Vref = %f \nmagnitude offset: %f, %f\n",5.0*sqrt(mag0*mag1),mag0,mag1);
     
     
    
    bcm2835_spi_end();
    bcm2835_close();
	
    return 0;
}
