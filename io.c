#include "io.h"
#include "ui.h"

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

// New stuffs
volatile uint16_t vmax;
volatile uint16_t vmin;


/* ----------- ioPrefs ---------- */
void ioPrefsReset(ioPrefs * const me)
{
	me->scrollpos = 36;
	me->scale = 50;
	me->timebase = 0;
	me->offset = 120;
	me->trigger = 120;

	me->mcpos[0] = 95;
	me->mcpos[1] = 245;
	me->mcpos[2] = 55;
	me->mcpos[3] = 165;
}

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

void start_adc(uint16_t tb){
	for(adcn = 0; adcn < BUFSIZE; adcn++){
		ADCSRA |= _BV(ADSC);
		adcbuf[adcn] = ADCH;
		delay_reading_us(tb);
	}
	uint8_t max, min;
	max = 0, min = 255;
	for(int i=0; i < BUFSIZE; i++){
		if(adcbuf[i] > max) max = adcbuf[i];
		if(adcbuf[i] < min) min = adcbuf[i];
	}
	vmax = max, vmin = min;
}

double ioGetText(void)
{
    return 69;
}
