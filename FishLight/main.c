#include <msp430g2553.h>

#define LED0 BIT0
#define LED1 BIT6
#define BUTTON BIT3


#define MCU_CLOCK           1100000
#define PWM_FREQUENCY       46      // In Hertz, ideally 50Hz.

#define SERVO_STEPS         180     // Maximum amount of steps in degrees (180 is common)
#define SERVO_MIN           700     // The minimum duty cycle for this servo
#define SERVO_MAX           3000    // The maximum duty cycle
unsigned int PWM_Period     = (MCU_CLOCK / PWM_FREQUENCY);  // PWM Period
unsigned int PWM_Duty       = 0;                            // %
int days = 0;
int onOff = 0;
int dummy = 0;

void controlServo(void)
{

    unsigned int servo_stepval, servo_stepnow;
    unsigned int servo_lut[ SERVO_STEPS+1 ];
    unsigned int i;

    // Calculate the step value and define the current step, defaults to minimum.
    servo_stepval   = ( (SERVO_MAX - SERVO_MIN) / SERVO_STEPS );
    servo_stepnow   = SERVO_MIN;

    // Fill up the LUT
    for (i = 0; i < SERVO_STEPS; i++) {
        servo_stepnow += servo_stepval;
        servo_lut[i] = servo_stepnow;
    }

    // Setup the PWM, etc.
    WDTCTL  = WDTPW + WDTHOLD;     // Kill watchdog timer
    TACCTL1 = OUTMOD_7;            // TACCR1 reset/set
    TACTL   = TASSEL_2 + MC_1;     // SMCLK, upmode
    TACCR0  = PWM_Period-1;        // PWM Period
    TACCR1  = PWM_Duty;            // TACCR1 PWM Duty Cycle
    P1DIR   |= BIT2;               // P1.2 = output
    P1SEL   |= BIT2;               // P1.2 = TA1 output

    // Main loop
    TACCR1 = servo_lut[0];
    __delay_cycles(1000000);
    TACCR1 = servo_lut[90];
    __delay_cycles(1000000);
	TACCR1 = servo_lut[0];
	__delay_cycles(1000000);
}
int main(void)
{
WDTCTL = WDTPW + WDTHOLD; 	// Stop watchdog timer
P1DIR |= (LED0 + LED1); 	// Set P1.0 to output direction
							// P1.3 must stay at input
P1OUT &= ~(LED0 + LED1); 	// set P1.0 to 0 (LED OFF)
P1IE |= BUTTON; 			// P1.3 interrupt enabled

P1IFG &= ~BUTTON; 			// P1.3 IFG cleared

__enable_interrupt(); 		// enable all interrupts


while(1){
//	onOff = 0;
	while(days < 7){//while fish tank doesn't need cleaning
		int i = 0;
		while(i < 5000) //wait for 1 day
		{
			i++;
			__delay_cycles(1000);
		}
		//feed the damn fish
		controlServo();
		dummy++;
		days++;
		onOff = 0;
	}

	P1DIR = 0x80;
	P1OUT = 0;
	while(onOff == 0)
	{
		P1OUT = 0x80;
		_delay_cycles(50000);
		P1OUT = 0;
		_delay_cycles(5000000);
		//do nothing
		//blink light
	}
	P1OUT = 0;
	//water has been changed, reset situation


	dummy++;
}

}


// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	onOff = 1;
	days = 0;
//P1OUT ^= (LED0 + LED1); 	// P1.0 = toggle
	P1IFG &= ~BUTTON; 			// P1.3 IFG cleared
	P1IES ^= BUTTON; // toggle the interrupt edge,
// the interrupt vector will be called
// when P1.3 goes from HitoLow as well as
// LowtoHigh
}




