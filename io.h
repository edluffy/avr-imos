#ifndef _IOH_ 
#define _IOH_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdlib.h>

#define ADCREF_V     3.3
#define ADCMAXREAD   255   /* 8 bit ADC */
#define CONVRATE ((22+4.7)/4.7)

#define FULLCONV (ADCREF_V/ADCMAXREAD) * CONVRATE

volatile uint16_t scrollpos;
volatile uint16_t scale;
volatile uint16_t timebase;
volatile uint16_t offset;
volatile uint16_t trigger;
volatile uint16_t mcpos[4];

volatile uint8_t ioTrigState;
volatile uint8_t ioCursor;
volatile bool ioPaused;
volatile bool ioShowMeasure;


/* --------- CONTROL INPUT ----------- */
volatile uint16_t *rp;
volatile uint16_t rspec;
volatile uint16_t rstep;
volatile uint16_t rmax;
volatile uint16_t rmin;

volatile uint8_t pbstate;
volatile uint8_t lastpb;

volatile uint8_t lastportc;

uint8_t cnt;

void init_control(void);
void read_rotb(void);
void read_rots(void);

void read_pushb(void);


/* ------------ ADC ----------------- */
/* Max conversion rate = F_ADC/13.5 = 1.5MHz/13.5
 * = 111kHz (or 1 reading per 9us */

#define BUFSIZE 1000

volatile uint8_t adcbuf[BUFSIZE];
volatile uint16_t adcn;

void init_adc(void);
void delay_reading_us(int t);
void start_adc(void);

double ioGetMax(void);
double ioGetMin(void);
double ioGetAvr(void);
double ioGetdV(void);
double ioGetdT(void);

#endif
