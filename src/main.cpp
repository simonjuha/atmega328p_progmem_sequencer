/* 
    ATmega328p - minimal musical stepper sequencer
    F_CPU = 16MHz
*/

#include <stepper.hpp>
#include <buttons/buttons.hpp>
#include <modeSelector.hpp>
#include <leds.hpp>
#include <data/buffer.hpp>

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

    while(true){

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
            case LEGATO:
                setSamplePlayback(true);
                break;
            case OFF:
                setSamplePlayback(false);
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
