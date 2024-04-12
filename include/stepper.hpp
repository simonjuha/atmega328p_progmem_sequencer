#pragma once
#include <stdint.h>
#include <stdlib.h>
#include "step.hpp"
#include <modes.hpp>

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
            _steps[i].setLength(STEPPER_STEPS_DIV/4);
        }
    }

    StepState step(){

        incrementStep();
        
        Step* currentStep = &_steps[_stepIndex];
        Step* nextStep = &_steps[(_stepIndex + 1) % STEPPER_STEPS_MAX];

        StepState state = isStepActive(currentStep, nextStep);

        if(state == ON){
            _nextStepActive = true;
            return ON;
        }
        if(state == OFF){
            moveToNextStep();
            return OFF;
        }
        return CONTINUE;
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

    bool getNextStepActive(){
        return _nextStepActive;
    }

    int getSelectedStep(){
        return _selectedStep;
    }

    void setSelectedStep(int step){
        _selectedStep = step;
    }

    Mode getMode(){
        return _mode;
    }

    void setMode(Mode mode){
        _mode = mode;
    }

private:
    void incrementStep(){
        _step++;
        if(_step >= STEPPER_STEPS_TOT){
            _step = 0;
        }
    }

    void moveToNextStep(){
        _stepIndex++;
        if(_stepIndex >= STEPPER_STEPS_MAX){
            _stepIndex = 0;
        }
        _nextStepActive = false;
    }

    StepState isStepActive(Step* currentStep, Step* nextStep){
        if((getStepStart(currentStep) == _step) && currentStep->getState()){
            return ON;
        }
        if(getStepEnd(currentStep) == _step){
            return OFF;
        }
        return CONTINUE;
    }

    int getStepEnd(Step *step){
        return step->getStepNumber() + step->getStartOffset() + step->getLength();
    }

    int getStepStart(Step *step){
        return step->getStepNumber() + step->getStartOffset();
    }

    uint8_t _step = 0;
    Step _steps[STEPPER_STEPS_MAX];

    uint8_t _stepIndex = 0;
    bool _nextStepActive = false;
    
    int _selectedStep = 0;

    Mode _mode = MODE_ACTIVE;


};