#pragma once

#include <buttons/buttonObserver.hpp>
#include <stepper.hpp>

class ModeSelector : public ButtonObserver {
public:
    ModeSelector(Stepper *stepper, ButtonObservable *buttons): _stepper(stepper) {
        buttons->addObserver(this);
    }

    void onButtonPressed(int buttonIndex, int command) {
        switch(command) {
            case CMD_PRESS_STEP:
                handlePressStep(buttonIndex);
                break;
            case CMD_SELECT_STEP:
                handleSelectStep(buttonIndex);
                break;
            case CMD_SET_MODE:
                handleSetMode(buttonIndex);
                break;
            default:
                break;
        }
    }

private:
    void handlePressStep(int buttonIndex) {
        switch(_stepper->getMode()) {
            case MODE_ACTIVE:
                _stepper->getStep(buttonIndex)->setState(!_stepper->getStep(buttonIndex)->getState());
                break;
            case MODE_LENGTH:
                _stepper->getStep(_stepper->getSelectedStep())->setLength(buttonIndex + 1);
                break;
            case MODE_RATE:
                _stepper->getStep(_stepper->getSelectedStep())->setSampleRate((buttonIndex + 1));
                break;
            case MODE_OFFSET:
                _stepper->getStep(_stepper->getSelectedStep())->setStartOffset(buttonIndex-4);
                break;
            default:
                break;
        }
    }

    void handleSelectStep(int buttonIndex) {
        _stepper->setSelectedStep(buttonIndex);
    }

    void handleSetMode(int buttonIndex) {
        switch(buttonIndex) {
            case MODE_ACTIVE:
                _stepper->setMode(MODE_ACTIVE);
                break;
            case MODE_LENGTH:
                _stepper->setMode(MODE_LENGTH);
                break;
            case MODE_RATE:
                _stepper->setMode(MODE_RATE);
                break;
            case MODE_OFFSET:
                _stepper->setMode(MODE_OFFSET);
                break;
            default:
                break;
        }
    }

    Stepper *_stepper;
};