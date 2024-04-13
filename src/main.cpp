// ATmega328p
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdlib.h>

#include <stepper.hpp>

#include <buttons/buttons.hpp>
#include <options.hpp>
#include <leds.hpp>

#include <data/buffer.cpp>

#define F_CPU 16000000UL


Stepper stepper;
Leds leds(stepper);
ControlInterfaceButtons buttons;
ModeSelector selector(&stepper, &buttons);

bool doStep = false;

int main(){



    /* -------- 16-bit Timer interupt setup (Timer1) -------- */
    TCCR1B |= (1 << WGM12) | (1 << CS10) | (1 << CS11);
    OCR1A = 6000;
    TIMSK1 |= (1 << OCIE1A);// Enable Timer1 interrupt
    TIFR1 |= (1 << OCF1A); 
    
    sei();

    buffer_init();

    while(1){

        buttons.tick();
        buffer_tick();
        
        // stepping
        static int gateState;

        if(doStep){
            gateState = stepper.step();
            doStep = false;
            // change sample rate
            OCR2A = (1<<stepper.getStepSampleRate())-1;
        }

        // play sample
        switch (gateState)
        {
            case ON:
                setSamplePlayback(true);
                break;
            case OFF:
                setSamplePlayback(false);
                break;
            case LEGATO:
                setSamplePlayback(true);
                break;
            default:
                break;
        }

        leds.tick();
    }
}

// Timer1 stepper interrupt
ISR(TIMER1_COMPA_vect) {
    doStep = true;
    leds.doChange();
}
