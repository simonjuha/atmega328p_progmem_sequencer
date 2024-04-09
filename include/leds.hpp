#pragma once

#define N_LEDS 8
#include <avr_ws2812.h>

class Leds{
    public:
        Leds(Stepper &stepper) : _stepper(stepper){
            for(int i = 0; i < N_LEDS; i++){
                leds[i] = _off;
            }
            refresh();
        }
        void doChange(){
            _doChange = true;
        }
        void tick(){
            if(_doChange){
                _doChange = false;
                switch(_stepper.getMode()){
                    case MODE_ACTIVE:
                        stepping();
                        break;
                    case MODE_LENGTH:
                        setSpan(2);
                        break;
                    default:
                        break;
                }
                refresh();
            }
        }

        void setSpan(int end){
            for(int i = 0; i < N_LEDS; i++){
                leds[i] = _off;
            }
            for(int i = 0; i < end; i++){
                leds[i] = _selectColor;
            }
            refresh();
        }
        void stepping(){
            int selected = _stepper.getSelectedStep();
            int activeStep = _stepper.getStepIndex();
            bool nextStepActive = _stepper.getNextStepActive();
            
            for(int i = 0; i < N_LEDS; i++){
                if(_stepper.getStep(i)->getState()) {
                    leds[i] = _deactivatedColor;
                } else {
                    leds[i] = _off;
                }
            }
            
            if(nextStepActive) {
                leds[activeStep] = _activeColor;
            }
            
            leds[selected] = _selectColor;
            
            if(nextStepActive && activeStep == selected) {
                leds[selected] = _activeAndSelectedColor;
            }
            refresh();
        }


    private:
        void refresh(){
            ws2812_setleds(leds, N_LEDS);
        }
        ws2812_RGB_t leds[N_LEDS];
        const cRGB _selectColor = {0, 0, 1};
        const cRGB _activeColor = {0, 1, 0};
        const cRGB _activeAndSelectedColor = {1, 1, 0};
        const cRGB _defaultColor = {1, 1, 1};
        const cRGB _deactivatedColor = {1, 0, 0};
        const cRGB _off = {0, 0, 0};
        bool _doChange = false;

        Stepper &_stepper;

        Mode _mode = MODE_ACTIVE;
        int _selectedStep = 0;
};