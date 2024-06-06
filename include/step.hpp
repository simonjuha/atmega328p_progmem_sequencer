#pragma once
#include <stdint.h>
#include <stdlib.h>

class Step{
    public:
    Step(uint8_t stepNumber) : _stepNumber(stepNumber){
        _sampleRate = 8;
    }
    Step() : _stepNumber(0){
        _sampleRate = 8;
    }

    void setSampleStart(uint8_t sampleStart){
        _sampleStart = sampleStart;
    }

    void setSampleEnd(uint8_t sampleEnd){
        _sampleEnd = sampleEnd;
    }

    uint8_t getSampleStart(){
        return _sampleStart;
    }

    uint8_t getSampleEnd(){
        return _sampleEnd;
    }

    uint16_t getSampleRate(){
        return _sampleRate;
    }

    void setSampleRate(uint8_t sampleRate){
        _sampleRate = sampleRate;
    }

    void setStepNumber(uint8_t stepNumber){
        _stepNumber = stepNumber;
    }

    uint8_t getStepNumber(){
        return _stepNumber;
    }

    void setStartOffset(int8_t startOffset){
        _startOffset = startOffset;
    }

    int8_t getStartOffset(){
        return _startOffset;
    }

    uint8_t getLength(){
        return _length;
    }

    void setLength(uint8_t length){
        _length = length;
    }

    void setState(bool state){
        _active = state;
    }

    bool getState(){
        return _active;
    }

    uint8_t getBank(){
        return _bank;
    }

    void setBank(uint8_t bank){
        _bank = bank;
    }

    private:
        bool    _active = false;
        uint8_t _sampleRate;
        int8_t _startOffset = 0;
        uint8_t _length;
        uint8_t _stepNumber;
        uint8_t _bank = 0; // 0-7
        uint8_t _sampleEnd = 7;
        uint8_t _sampleStart = 0;

};