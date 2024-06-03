#pragma once

#include <buttons/buttonObserver.hpp>
#include <stepper.hpp>
#include <data/buffer.hpp>
#include <globalSettings.hpp>
#include <leds.hpp>

volatile static bool running = true;

class ModeSelector : public ButtonObserver {
public:
    ModeSelector(Stepper *stepper, ButtonObservable *buttons, Leds *leds): _stepper(stepper) {
        buttons->addObserver(this);
        buttons->addObserver(leds);
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
            case CMD_SHIFTC:
                running = !running;
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
            case MODE_SAMPLE_START:
                _stepper->getStep(_stepper->getSelectedStep())->setSampleStart(buttonIndex);
                break;
            case MODE_SAMPLE_END:
                _stepper->getStep(_stepper->getSelectedStep())->setSampleEnd(buttonIndex);
                break;
            case MODE_BANK:
                _stepper->getStep(_stepper->getSelectedStep())->setBank(buttonIndex);
                break;
            case MODE_GLOBAL:
                switch(buttonIndex) {
                    case GLST_CLOCK_EXT:
                        glst[GLST_CLOCK_EXT].value = !glst[GLST_CLOCK_EXT].value;
                        break;
                    case GLST_CLOCK_OFFSET:
                        glst[GLST_CLOCK_OFFSET].value = !glst[GLST_CLOCK_OFFSET].value;
                        break;
                    default:
                        break;
                }
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
            case MODE_SAMPLE_START:
                _stepper->setMode(MODE_SAMPLE_START);
                break;
            case MODE_SAMPLE_END:
                _stepper->setMode(MODE_SAMPLE_END);
                break;
            case MODE_BANK:
                _stepper->setMode(MODE_BANK);
                break;
            case MODE_GLOBAL:
                _stepper->setMode(MODE_GLOBAL);
                break;
            default:
                break;
        }
    }

    Stepper *_stepper;
};