#define F_CPU 8000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


uint8_t val_ch1;
uint8_t val_ch2;
uint8_t val_ch3;

int main()
{
	TCCR0A = 0;
	TCCR0B = (1 << CS01) | (0 << CS00);	// Prescaler = 1
	TIMSK = (1 << OCIE0B) | (1 << OCIE0A) | (1 << TOIE0);	// Enable compare 0A and OV0 interrupts

	TCCR1A = 0;
	TCCR1B = (1 << CS01) | (0 << CS00);
	TIMSK |= (1 << OCIE1A) | (1 << TOIE1);

	OCR0A = 128;

	sei();

	DDRA = 0xFF;
	PORTA = 0x00;
	
	while(1)
	{
		for(int i=0; i < 0xFF; i++)		
		{
			val_ch1 = i;
			_delay_ms(1);
		}
		val_ch1=0;
		for(int i=0; i < 0xFF; i++)		
		{
			val_ch2 = i;
			_delay_ms(1);
		}
		val_ch2=0;
		for(int i=0; i < 0xFF; i++)		
		{
			val_ch3 = i;
			_delay_ms(1);
		}
		val_ch3=0;

	}
	return 0;
}

ISR(TIM0_COMPA_vect)
{	
	PORTA &= ~(1 << 0);
}

ISR(TIM0_COMPB_vect)
{
	PORTA &= ~(1 << 1);
}

ISR(TIM1_COMPA_vect)
{
	PORTA &= ~(1 << 2);
}

ISR(TIM0_OVF_vect)
{
	if(val_ch1)
		PORTA |= (1 << 0);
	if(val_ch2)
		PORTA |= (1 << 1);
	if(val_ch3)
		PORTA |= (1 << 2);

	OCR0A = val_ch1;
	OCR0B = val_ch2;
	OCR1AH = 0;
	OCR1AL = val_ch3;
	TCNT1L = 0x00;
	TCNT1H = 0x00;
}
