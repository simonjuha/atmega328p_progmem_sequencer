#pragma once
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <data/dataBanks.hpp>

#define BUFFER_SIZE 256

#define PWM_OUT PD5

// pointer to target bank
const uint8_t* sampleBank;
uint16_t sampleBank_size;
uint16_t sample_end;
uint16_t sample_start;

uint8_t buffer[BUFFER_SIZE];
volatile uint8_t head = 0;
volatile uint8_t tail = 0;

static bool sampleIsPlaying = false;
static int selectedBank = 0;

uint16_t mapSamplePosition(uint8_t input, uint16_t minVal, uint16_t maxVal) {
    if (input > 7) {
        return 7;
    }
    
    // Calculate the range
    uint16_t range = maxVal - minVal;
    
    // Map the input value to the percentage of the range
    uint16_t mappedValue = minVal + ((range * input) / 7);
    
    return mappedValue;
}

void setSampleEnd(uint8_t end){
    sample_end = mapSamplePosition(end, sample_start-1, sampleBank_size);

}

void setSampleStart(uint8_t start){
    sample_start = mapSamplePosition(start, 0, sample_end);
}

// reset buffer
void buffer_reset(){
    head = 0;
    tail = 0;
}

void setSamplePlayback(bool play){
    sampleIsPlaying = play;
}

int getBankIndex(){
    return selectedBank;
}

void setSampleBank(const uint8_t *bank, size_t size){
    sampleBank = bank;
    sampleBank_size = size;
}

void selectSampleBank(uint8_t bank){
    if(bank == selectedBank){
        return; // do nothing if the bank is already selected
    }

    //buffer_reset();

    switch(bank){
        case 0:
            selectedBank = 0;
            setSampleBank(random_chunk1, sizeof(random_chunk1)/sizeof(random_chunk1[0]));
            break;
        case 1:
            selectedBank = 1;
            setSampleBank(random_chunk2, sizeof(random_chunk2)/sizeof(random_chunk2[0]));
            break;
        case 2:
            selectedBank = 2;
            setSampleBank(tone1, sizeof(tone1)/sizeof(tone1[0]));
            break;
        default:
            break;
    }
}

// Add a value to the buffer
int buffer_put(uint8_t value) {
    if(!sampleIsPlaying){
        return 0;
    }
    uint8_t nextHead = (head + 1) % BUFFER_SIZE;
    if (nextHead != tail) { 
        buffer[head] = value;
        head = nextHead;
        return 1;
    }else{
        return 0;
    }
}

void buffer_init(){
    /* -------- 8-bit Timer interupt setup (Timer2) -------- */
    TCCR2A |= (1 << WGM21); // CTC mode
    TCCR2B |= (1 << CS20) | (1 << CS22);
    TIMSK2 |= (1 << OCIE2A);
    OCR2A = 255;
    // timer frequency = 16MHz / 8 / 256 = 7812.5 Hz

    /* -------- 8-bit PWM setup (Timer0) -------- */
    DDRD   |= (1 << PWM_OUT);
    TCCR0A |= (1 << COM0B1) | (1 << WGM00) | (1 << WGM01);
    TCCR0B |= (1 << CS00);

    setSampleBank(random_chunk1, sizeof(random_chunk1)/sizeof(random_chunk1[0]));
    setSampleStart(0);
    setSampleEnd(sampleBank_size);

}

void buffer_tick(){
    static int i_prog = 0;

    if(buffer_put(pgm_read_byte(&sampleBank[i_prog]))){
        // if buffer is full, do nothing
    }else{
        i_prog++;
        if(i_prog >= sample_end){
            i_prog = sample_start;
        }
    }
}

// Timer2 interrupt
ISR(TIMER2_COMPA_vect) {
    // Check if there is data in the buffer
    if(sampleIsPlaying){
        if (head != tail) {
            OCR0B = buffer[tail];
            tail = (tail + 1) % BUFFER_SIZE;
        }
        //OCR0B = buffer[tail]; // use previous value if buffer is empty - Sounds cool but not ideal, voids no sound when buffer is empty/underrun
    }
}


