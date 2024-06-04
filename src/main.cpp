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
ModeSelector selector(&stepper, &buttons, &leds);

int main(){
    sei();

    buffer_init();
    clockSource_init();

    while(true){


        buttons.tick();
        buffer_tick();
           
        // stepping
        static int gateState;

        if(doStepNow()){
            leds.doChange();
            gateState = stepper.step();
            
            doStep = false;            
        }

        // play sample
   
        switch (gateState)
        {
            case ON:
            case LEGATO:
                setSamplePlayback(true);
                break;
            case OFF:
                // at this point the stepper has already moved to the next step
                setSamplePlayback(false);
                // set sample rate before next step
                OCR2A = (2<<stepper.getStepSampleRate())-1;
                // change bank to bank of next step
                selectSampleBank(stepper.getStepBank());
                // set sample start and end
                setSampleStart(stepper.getStep(stepper.getStepIndex())->getSampleStart());
                setSampleEnd(stepper.getStep(stepper.getStepIndex())->getSampleEnd());
                setStepClockActive(running);
                leds.doChange();
                break;
            default:
                break;
        }

        // tick leds
        leds.tick();
    }
}