#pragma once

#include <buttons/buttonObserver.hpp>
#include <stepper.hpp>

// statemachine for mannaging LEDs and stepper options
class Options : public ButtonObserver{
    public:
        Options(Stepper *stepper): _stepper(stepper){}
        void onButtonPressed(int buttonIndex, int mode){
            switch(mode){
                case 0: // turn on/off steps
                    _stepper->getStep(buttonIndex)->setState(!_stepper->getStep(buttonIndex)->getState());
                    break;
                default:
                    break;
            }
    }
    private:
        Stepper *_stepper;
};

