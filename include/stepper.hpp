#pragma once
#include <stdint.h>
#include <stdlib.h>
#include "step.hpp"
#include <modes.hpp>
#include <avr/io.h>
#include <globalSettings.hpp>

#define STEPPER_STEPS_MAX 8 // max steps of a single sequence
#define STEPPER_STEPS_DIV 8 // ministeps per step
#define STEPPER_STEPS_TOT STEPPER_STEPS_MAX * STEPPER_STEPS_DIV // total steps

// ON / OFF / CONTINUE - gates
enum StepState{
    ON,
    OFF,
    CONTINUE,
    LEGATO,
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

        clockOut(state == ON);

        // check if step is active or should continue with current state
        switch (state) {
            case ON:
                _nextStepActive = true;
                return ON;
            case OFF:
                moveToNextStep();
                _nextStepActive = false;
                return OFF;
            case LEGATO:
                moveToNextStep();
                _nextStepActive = true;
                return LEGATO;
            default:
                return CONTINUE;
        }
    }

    uint8_t getStepBank(){
        return _steps[_stepIndex].getBank();
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

    void setStepIndex(uint8_t index){
        _stepIndex = index;
        _step = _steps[_stepIndex].getStepNumber();
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

    uint16_t getStepSampleStart(){
        return _steps[_stepIndex].getSampleStart();
    }

    uint16_t getStepSampleEnd(){
        return _steps[_stepIndex].getSampleEnd();
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
    }

    StepState isStepActive(Step* currentStep, Step* nextStep){
        int currentStepStart = getStepStart(currentStep);
        int currentStepEnd = getStepEnd(currentStep);
        bool isCurrentStepActive = currentStep->getState();

        if(currentStepStart == _step && isCurrentStepActive){
            return ON;
        }

        bool isNextStepStartSameAsCurrentStepEnd = getStepStart(nextStep) == _step;
        if(currentStepEnd == _step){
            if(isNextStepStartSameAsCurrentStepEnd){
                return LEGATO;
            }
            return OFF;
        }

        return CONTINUE;
    }

    int getStepEnd(Step *step){
        int stepEnd = step->getStepNumber() + step->getStartOffset() + step->getLength();
        if (stepEnd >= STEPPER_STEPS_TOT) {
            stepEnd = STEPPER_STEPS_TOT - 1;
        }
        return stepEnd;
    }

    int getStepStart(Step *step){
        return step->getStepNumber() + step->getStartOffset();
    }

    // clock sync output
    void clockOut(bool stepState){
        // send even clock sync - ignoring offset
        if(!glst[GLST_CLOCK_OFFSET].value && (_step % STEPPER_STEPS_DIV == 0)){
            PORTD |= (1 << PD4); // high
            return;
        }
        if(glst[GLST_CLOCK_OFFSET].value && stepState){
            PORTD |= (1 << PD4); // high
            return;
        }
        PORTD &= ~(1 << PD4); // low

    }

    uint8_t _step = 0;
    Step _steps[STEPPER_STEPS_MAX];

    uint8_t _stepIndex = 0;
    bool _nextStepActive = false;
    
    int _selectedStep = 0;

    Mode _mode = MODE_ACTIVE;



};