#pragma once
#include <stdint.h>
#include <stdlib.h>
#include "step.hpp"

#define STEPPER_STEPS_MAX 8 // max steps of a single sequence
#define STEPPER_STEPS_DIV 8 // ministeps per step
#define STEPPER_STEPS_TOT STEPPER_STEPS_MAX * STEPPER_STEPS_DIV // total steps

// ON / OFF / CONTINUE - gates
enum StepState{
    ON,
    OFF,
    CONTINUE
};

class Stepper {
public:
    Stepper(){
        for(uint8_t i = 0; i < STEPPER_STEPS_MAX; i++){
            _steps[i].setStepNumber(i*STEPPER_STEPS_DIV);
            _steps[i].setLength(STEPPER_STEPS_DIV/2);
        }
    }

    StepState step(){

        _step++;
        if(_step >= STEPPER_STEPS_TOT){
            _step = 0;
        }
        
        Step* currentStep = &_steps[_stepIndex];


        // step inactive (OFF)
        if(currentStep->getStepNumber() + currentStep->getStartOffset() + currentStep->getLength() == _step){
            _stepIndex++;
            if(_stepIndex >= STEPPER_STEPS_MAX){
                _stepIndex = 0;
            }
            return OFF; // sampled stops playing
        }

        // step active (ON)
        if((currentStep->getStepNumber() + currentStep->getStartOffset() == _step) && currentStep->getState()){

            return ON; // sample is played
        }

        return CONTINUE;
    }

    void setTempo(uint8_t tempo){
        _tempo = tempo;
    }

    uint8_t getStepSampleRate(){
        return _steps[_stepIndex].getSampleRate();
    }

    Step* getStep(uint8_t index){
        return &_steps[index];
    }

    uint8_t getStepIndex(){
        return _stepIndex;
    }



private:
    uint8_t _tempo;
    uint8_t _step = 0;
    Step _steps[STEPPER_STEPS_MAX];

    uint8_t _stepIndex = 0;
};



