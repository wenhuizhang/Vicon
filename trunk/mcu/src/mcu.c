/**
 * \file   mcu.c
 * \author Tim Molloy
 *
 * $Author: tlmolloy $
 * $Date: 2010-06-10 23:59:05 +1000 (Thu, 10 Jun 2010) $
 * $Rev: 164 $
 * $Id: main.cpp 164 2010-06-10 13:59:05Z tlmolloy $
 *
 * Queensland University of Technology
 *
 * \section DESCRIPTION
 * Main file for the ATMEGA328P Mode Control Unit
 */

#define F_CPU 8000000UL // 16MHz scaled to 8 MHz

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Mode LED Indicator Defines
#define GREEN_LED 3
#define BLUE_LED 4
#define RED_LED 2

// ESC Commands
#define ESC1 OCR0A
#define ESC2 OCR0B
#define ESC3 OCR1A
#define ESC4 OCR1B

uint8_t escCommands[4] = {0, 0, 0, 0};

// PWM Defines
#define F_PWM ((double) F_CPU/(64.0*510.0))
#define PWM_MAX_US 2000
#define PWM_MIN_US 1000

// Boolian Defines
#define TRUE 1
#define FALSE 0



void init();
uint8_t InitialiseModeIndicator();

// Serial Comms
uint8_t InitialiseUSART();

// Timer Functions
uint8_t InitialiseTimer0();
uint8_t InitialiseTimer1();

// PWM Functions
uint8_t StartPWM();
uint8_t StopPWM();

// Pin Change Interupt
#define CHANNEL1 5
#define CHANNEL2 4
#define CHANNEL3 3
#define CHANNEL4 2
#define CHANNEL5 1
#define CHANNEL6 0

uint8_t InitialisePC();

void main (void)
{
  StopPWM();
  init();

  for ( ; ; )
  {
  }

  return;
}


void init()
{
  // Setup Clock - prescalar of 2 to 8MHz
  CLKPR = (1<<CLKPCE);
  CLKPR = (1<<CLKPS0);
  
  InitialiseModeIndicator();
  InitialiseTimer0();
  InitialiseTimer1();

  InitialisePC();

  if (InitialiseUSART())
  {
    // Start PWM
    StartPWM();
  }

  sei();

  return;
}


uint8_t InitialiseModeIndicator()
{
  DDRD |= (1<<RED_LED) | (1<<GREEN_LED) | (1<<BLUE_LED);
  
  uint8_t i = 0;
  for (i = 0; i < 4; ++i)
  {
    PORTD ^= (1<<RED_LED) | (1<<GREEN_LED) | (1<<BLUE_LED);
    _delay_ms(250);
  }

  return 1;
}

uint8_t InitialiseTimer0()
{
  // Set Initialisation of Timer
  OCR0A = 0;
  OCR0B = 0;
  
  // Register A 
  // COM0A = 10 for Non-inverting PWM
  // COM0B = 10
  // WGM0_210 = 001 for Normal PWM
  TCCR0A = (1 << COM0A1) | (0 << COM0A0) | 
           (1 << COM0B1) | (0 << COM0B0) | 
           (0 << WGM01) | (1 << WGM00);

  // Register B
  // No Clock Source Yet
  TCCR0B = (0 << WGM02); 

  // Outputs - OCR0A and OCR0B
  DDRD |= (1 << 6) | (1 << 5);

  return 1;
}

uint8_t InitialiseTimer1()
{  
  // Set Initialisation of Timer
  OCR1A = 0;
  OCR1B = 0;
  
  // Register A 
  // COM1A = 10 for Non-inverting PWM
  // COM1B = 10
  // WGM0_3210 = 0001 for 8bit Normal PWM
  TCCR1A = (1 << COM1A1) | (0 << COM1A0) | 
           (1 << COM1B1) | (0 << COM1B0) | 
           (0 << WGM11) | (1 << WGM10);

  // Register B
  // No Clock Source Yet
  TCCR1B = (0 << WGM13) | (0 << WGM12); 

  // Outputs - OCR1A and OCR1B
  DDRB |= (1 << 1) | (1 << 2);

  return 1;
}

uint8_t StopPWM()
{
  // ESC 1 and 2
  OCR0A = 0;
  OCR0B = 0;
  
  // ESC 3 and 4
  OCR1A = 0;
  OCR1B = 0;

  // Disable Clocks
  TCCR0B &= (0 << CS02) | (0 << CS01) | (0 << CS00);
  TCCR1B &= (0 << CS12) | (0 << CS11) | (0 << CS10);

  return 1;
}

uint8_t StartPWM()
{
  // CS0_210 = 011 for N = 64
  TCCR0B |= (0 << CS02) | (1 << CS01) | (1 << CS00);
  TCCR1B |= (0 << CS12) | (1 << CS11) | (1 << CS10);

  OCR0A = 150;
  OCR0B = 150;
  OCR1A = 150;
  OCR1B = 150;

  return 1;
}

uint8_t InitialiseUSART()
{
  uint8_t bRet = 1;

  return bRet;
}

uint8_t InitialisePC()
{
  // PC Interrupt Control Reg
  PCICR = (1 << PCIE1);

  // PC Interrupt MASK
  // Interrupts for the Pins all in PCMSK1
  PCMSK1 = (1 << PCINT13) | (1 << PCINT12) | (1 << PCINT11) | (1 << PCINT10) | (1 << PCINT9) | (1 << PCINT8);
   
  // Pins to Inputs to enable read of high or low
  DDRC = (1 << CHANNEL1) | (1 << CHANNEL2) | (1 << CHANNEL3) | (1 << CHANNEL4) | (1 << CHANNEL5) | (1 << CHANNEL6);
  
  return 1;
}

// Find the Pin that Changed
// Get Time that it changed
// if ( pin high )
//   log time
//   return and wait for low
// else if ( pin low )
//   calculate elapsed time
//   scale to range 0-255 for the pwm timers
// else
//   undefined
// endif

ISR(PCINT1_vect)
{
  
}
