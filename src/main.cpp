/* 
    ATmega328p - minimal musical stepper sequencer
    F_CPU = 16MHz
*/

#include <stepper.hpp>
#include <buttons/buttons.hpp>
#include <modeSelector.hpp>
#include <leds.hpp>
#include <data/buffer.hpp>
#include <clockSource.hpp>

Stepper stepper;
Leds leds(stepper);
ControlInterfaceButtons buttons;
ModeSelector selector(&stepper, &buttons);

int main(){
    sei();

    buffer_init();
    clockSource_init();

    while(true){

        // tick
        clockSource_tick();
        buttons.tick();
        buffer_tick();
        
        // stepping
        static int gateState;

        if(doStepNow()){
            leds.doChange();
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

        // tick leds
        leds.tick();
    }
}