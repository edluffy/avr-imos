#include "io.h"

volatile uint16_t scrollpos = 36;
volatile uint16_t scale = 50;
volatile uint16_t timebase = 0;
volatile uint16_t offset = 120;
volatile uint16_t trigger = 120;
volatile uint16_t mcpos[4] = {95, 245, 55, 165};

volatile uint8_t ioTrigState = 2;
volatile uint8_t ioCursor = 0;
volatile bool ioPaused = false;
volatile bool ioShowMeasure = false;

volatile uint16_t *rp;
volatile uint16_t rspec = 0;
volatile uint16_t rstep = 0;
volatile uint16_t rmax = 0;
volatile uint16_t rmin = 0;

volatile uint8_t pbstate = 1;
volatile uint8_t lastpb = 1;

volatile uint8_t lastportc = 0xFF;

volatile uint8_t adcbuf[BUFSIZE];
volatile uint16_t adcn = 0;

uint8_t cnt = 0;


void init_control(void)
{
	DDRC = 0x00; // enable input

	// enable pull ups on pins
	PORTC |= _BV(1) | _BV(2) | _BV(3) | _BV(6);

	// enable interrupt
	PCICR = _BV(PCIE2);
	PCMSK2 = _BV(1) | _BV(3) | _BV(6); // PC1, PC3, PC6


	// timer
	// normal mode!
	TIMSK0 |= _BV(TOIE0); // use overflow
	TCCR0B |= _BV(CS00); // no prescaler
}

ISR(TIMER0_OVF_vect)
{
	cnt++;
}

void read_rotb(void)
{ 
	if(rspec == 255)
		ioCursor = (ioCursor < 3) ? ioCursor+1 : 0;
	else if(rspec == 254)
		ioPaused = !ioPaused;
	else if(rspec == 253)
		ioShowMeasure = !ioShowMeasure;
	else
		*rp = rspec;

	_delay_ms(10); // debounce
}

void read_rots(void)
{
	if((PINC & _BV(2)) && *rp < rmax)
		*rp += rstep;
	else if(*rp > rmin)
		*rp -= rstep;
}

void read_pushb(void)
{
	lastpb = pbstate;
	pbstate = (pbstate < 6) ? pbstate+1 : 0;
	_delay_ms(10); // debounce
}

ISR(PCINT2_vect)
{
	uint8_t chbits = PINC ^ lastportc;
	lastportc = PINC;

	if(chbits & PINC & _BV(6))
		read_pushb();
	if(chbits & PINC & _BV(1))
		read_rotb();
	if(chbits & PINC & _BV(3))
		read_rots();
}

/* ------------ ADC ----------------- */

void init_adc(void)
{
	/* REFSx = 0 : Select AREF as reference
	 * ADLAR = 1 : Left shift result
 	 * MUXx  = 0 : Default to channel 0
	 */
	ADMUX = _BV(ADLAR);

	/*  ADTSx = 0 : Free running mode
	 *  ADATE = 1 : Enable auto trigger
	 *  ADIE  = 0 : ADC Interrupt Enable
	 *  ADPS2 = 1 : Configure ADC prescaler *  ADPS1 = 1 : F_ADC = F_CPU / 64 = 1.5MHz 
	 */
	ADCSRA = _BV(ADEN) | _BV(ADPS1) | _BV(ADPS0);

	/* Disable digital input */
	DIDR0 = _BV(0);

}

void delay_reading_us(int t){
	while(t--) _delay_us(1);
}

void start_adc(void){
	for(adcn = 0; adcn < BUFSIZE; adcn++){
		ADCSRA |= _BV(ADSC);
		adcbuf[adcn] = ADCH;
		delay_reading_us(timebase);
	}
/*
	uint8_t max, min;
	max = 0, min = 255;
	for(int i=0; i < BUFSIZE; i++){
		if(adcbuf[i] > max) max = adcbuf[i];
		if(adcbuf[i] < min) min = adcbuf[i];
	}
	vmax = max, vmin = min;
	*/
}

double ioGetMax(void)
{
	int i, max;
	for(i=max=0; i < BUFSIZE; i++)
		max = (adcbuf[i] > max) ? adcbuf[i] : max;
	return max * FULLCONV;
}

double ioGetMin(void)
{
	int i, min;
	for(i=0,min=ADCMAXREAD; i< BUFSIZE; i++)
		min = (adcbuf[i] < min) ? adcbuf[i] : min;
	return min * FULLCONV;
}

double ioGetAvr(void)
{
	int i, sum;
	for(i=sum=0; i< BUFSIZE; i++)
		sum += adcbuf[i];
	return sum/BUFSIZE * FULLCONV;
}

double ioGetdV(void)
{
	return (abs(mcpos[2]-mcpos[3])/((scale/100.0))) * FULLCONV;
}

double ioGetdT(void)
{
	return ((10.0+timebase)*abs(mcpos[0]-mcpos[1])) / 1000;
}

/*
uint16_t failcount = 0;
uint16_t wincount = 0;
void checkTrigger(void)
{
	uint8_t rising, above;
	rising = above = 0;

	for(int n=1; n < 10; n++)
		rising = (adcbuf[scrollpos*10] <= adcbuf[scrollpos*10+n]) ? 1 : rising;

	if(adcbuf[scrollpos*10]+(120-offset)>=(120-trigger)/(scale/100.0))
		above = 1, failcount = 0;


	if(above)
		wincount++;
	else
		failcount++;

	if(rising && above)
		ioTrigState = 0;
	else
		ioTrigState = 2;
	else if(failcount > 20)
		ioTrigState = 2;
	else
		ioTrigState = 1;
		
}
bool toggle = false;
void ioCheckTrigger(void)
{
	ADCSRA |= _BV(ADSC);
	uint8_t testsignal = ADCH;
	if(testsignal+(120-offset)>=(120-trigger)/(scale/100.0) && toggle)
		ioTrigState = 1;
	else
		ioTrigState = 0;

	toggle = !toggle;
}
*/

