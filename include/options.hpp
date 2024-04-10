#pragma once

#include <buttons/buttonObserver.hpp>
#include <stepper.hpp>


// statemachine for mannaging LEDs and stepper options
class ModeSelector : public ButtonObserver{
    public:
        ModeSelector(Stepper *stepper, ButtonObservable *buttons): _stepper(stepper){
            buttons->addObserver(this);
        }
        void onButtonPressed(int buttonIndex, int command){
            
            switch(command){
                case CMD_PRESS_STEP: // turn on/off steps
                    _stepper->getStep(buttonIndex)->setState(!_stepper->getStep(buttonIndex)->getState());
                    break;
                case CMD_SELECT_STEP: // select step
                    _stepper->setSelectedStep(buttonIndex);
                    break;
                case CMD_SET_MODE: // set mode
                    break;
                default:
                    break;
            }
    }
    private:
        Stepper *_stepper;
};

