#define F_CPU 8000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void i2cAck();
void i2cNak();
void i2cInit();

uint8_t i2cWritecount = 0;

uint8_t chdata[3];	// Channel data
uint8_t chbuf[3];	// Receive buffer

int main()
{
	TCCR0A = 0;
	TCCR0B = (1 << CS01) | (0 << CS00);	// Prescaler = 1
	TIMSK = (1 << OCIE0B) | (1 << OCIE0A) | (1 << TOIE0);	// Enable compare 0A and OV0 interrupts

	TCCR1A = 0;
	TCCR1B = (1 << WGM12) | (1 << CS01) | (0 << CS00);
	TIMSK |= (1 << OCIE1B) | (1 << OCIE1A);
	OCR1A = 255;

	i2cInit();

	sei();

	DDRA |= 0b111;	// Enable ch1-3
	PORTA = 0x00;
	

	while(1)
	{

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
	if(chdata[2])
		PORTA |= (1 << 2);

	OCR1B = chdata[2];
}

ISR(TIM1_COMPB_vect)
{
	if(chdata[2] != 0xFF)
		PORTA &= ~(1 << 2);
}

ISR(TIM0_OVF_vect)
{
	if(chdata[0])
		PORTA |= (1 << 0);
	if(chdata[1])
		PORTA |= (1 << 1);

	OCR0A = chdata[0];
	OCR0B = chdata[1];
}

void i2cInit()
{
	TWSA = 0b10101010;	// Slave address (lsb should be 0)
	TWSCRA = (1 << TWEN) | (1 << TWSIE) | (1 << TWASIE) | (1 << TWDIE);
}

void i2cAck()
{
	TWSCRB = 0b011;
}

void i2cNak()
{
	TWSCRB = 0b110;	
}

ISR(TWI_SLAVE_vect)
{
	// Determine cause of interrupt
	if(TWSSRA & (1 << TWASIF))	// Address/Stop interrupt	
	{
		if(TWSSRA & (1 << TWAS)) // Address match
		{
			i2cWritecount = 0;
			i2cAck();	// Acknowledge address			
		}
		else	// Stop condition
		{
			i2cNak();
			for(int i = 0; i < 3; i++)	// Transfer buf to data array
			{
				chdata[i] = chbuf[i];
			}
		}
	}
	else if(TWSSRA & (1 << TWDIF))	// Data interrupt
	{
		if(TWSSRA & (1 << TWDIR))	// Master read
		{
			TWSD = chdata[i2cWritecount++];	// Write data from data array

		}
		else	// Master write
		{
			chbuf[i2cWritecount++] = TWSD;	// Receive data into buffer
			//chdata[i2cWritecount++] = TWSD;
			if(i2cWritecount > 2)
			{	
				i2cNak();	// Pls no more data senpai~
			}
			else
			{
				i2cAck();	// Request more data
			}
		}
	}
	else if(TWSSRA & (1 << TWBE))	// Bus error
	{
		i2cNak();
	}
}
