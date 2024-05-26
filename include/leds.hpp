#pragma once

#define N_LEDS 8
#include <avr_ws2812.h>
#define CENTER_LED 4

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
                        setSpan(_stepper.getStep(_stepper.getSelectedStep())->getLength());
                        break;
                    case MODE_RATE:
                        setSpan(_stepper.getStep(_stepper.getSelectedStep())->getSampleRate());
                        break;
                    case MODE_OFFSET: // 0 is the start offset (center at 4)
                        setFromCenter(_stepper.getStep(_stepper.getSelectedStep())->getStartOffset());
                        break;
                    case MODE_BANK:
                        setSingle(getBankIndex());
                        break;
                    case MODE_GLOBAL:
                        // set LEDs on for global settings that are on
                        for(int i = 0; i < 8; i++){
                            if(glst[i].value){
                                leds[i] = _defaultColor;
                            } else {
                                leds[i] = _off;
                            }
                        }
                        break;


                    default:
                        break;
                }
                refresh();
            }
        }

        void setAllLedsOff() {
            for(int i = 0; i < N_LEDS; i++){
                leds[i] = _off;
            }
        }
        
        void setSelectedLedColor() {
            int selected = _stepper.getSelectedStep();
            bool nextStepActive = _stepper.getNextStepActive();
            leds[selected] = mixColors(leds[selected], _onStepColor);
            // if the next step is active, show it
            if(nextStepActive && selected == _stepper.getStepIndex()) {
                leds[selected] = mixColors(leds[selected], _activeColor);
            }
        }
        
        void setSpan(int end){
            setAllLedsOff();
            // set the selected led
            for(int i = 0; i < end; i++){
                leds[i] = _selectColor;
            }
            setSelectedLedColor();
            refresh();
        }
        
        void stepping(){
            int selected = _stepper.getSelectedStep();
            int activeStep = _stepper.getStepIndex();
            bool nextStepActive = _stepper.getNextStepActive();
            
            // show the steps that are active
            for(int i = 0; i < N_LEDS; i++){
                if(_stepper.getStep(i)->getState()) {
                    leds[i] = _onStepColor;
                } else {
                    leds[i] = _off;
                }
            }
            
            // if the next step is active, show it
            if(nextStepActive) {
                leds[activeStep] = _activeColor;
            }
            
            // show the selected step
            leds[selected] = _selectColor;
            
            // if the selected step is the active step, show it
            if(nextStepActive && activeStep == selected) {
                leds[selected] = mixColors(_activeColor, _selectColor);
            }
            refresh();
        }

        void setSingle(int index){
            setAllLedsOff();
            leds[index] = _selectColor;
            setSelectedLedColor();
            refresh();
        }
        
        void setFromCenter(unsigned int index){
            setAllLedsOff();
            int centeredValue = CENTER_LED + index;
        
            leds[CENTER_LED] = _selectColor;
            if(centeredValue > CENTER_LED){
                for(int i = CENTER_LED+1; i <= centeredValue; i++){
                    leds[i] = _selectColor;
                }
            }
            if(centeredValue < CENTER_LED){
                for(int i = CENTER_LED-1; i >= centeredValue; i--){
                    leds[i] = _selectColor;
                }
            }
        
            setSelectedLedColor();
            refresh();
        }

    private:
        void refresh(){
            ws2812_setleds(leds, N_LEDS);
        }
        cRGB mixColors(const cRGB &color1,const cRGB &color2){
            cRGB result;
            // OR colors together:
            result.r = color1.r | color2.r;
            result.g = color1.g | color2.g;
            result.b = color1.b | color2.b;
            return result;
        }
        ws2812_RGB_t leds[N_LEDS];
        const cRGB _selectColor = {0, 0, 1};
        const cRGB _activeColor = {0, 1, 0};
        const cRGB _defaultColor = {1, 1, 1};
        const cRGB _onStepColor = {1, 0, 0};
        const cRGB _off = {0, 0, 0};
        bool _doChange = false;

        Stepper &_stepper;
};