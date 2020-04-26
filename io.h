#ifndef _IOH_ 
#define _IOH_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>


#define ADCREF_V     3.3
#define ADCMAXREAD   255   /* 8 bit ADC */
#define CONVRATE ((22+4.7)/4.7)

#define FULLCONV (ADCREF_V/ADCMAXREAD) * CONVRATE


/* --------- CONTROL INPUT ----------- */

void init_control(void);
void read_rotb(void);
void read_rots(void);


void read_pushb(void);


/* ------------ ADC ----------------- */
/* Max conversion rate = F_ADC/13.5 = 1.5MHz/13.5
 * = 111kHz (or 1 reading per 9us */

#define BUFSIZE 1000

void init_adc(void);
void delay_reading_us(int t);
void start_adc(void);

double ioGetMax(void);
double ioGetMin(void);
double ioGetAvr(void);
double ioGetdV(void);
double ioGetdT(void);

#endif
