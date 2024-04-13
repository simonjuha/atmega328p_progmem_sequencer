#pragma once
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <data/dataBanks.hpp>

#define BUFFER_SIZE 256

#define PWM_OUT PD5

// pointer to target bank
const uint8_t* sampleBank;
uint8_t sampleBank_size;

uint8_t buffer[BUFFER_SIZE];
volatile uint8_t head = 0;
volatile uint8_t tail = 0;

static bool sampleIsPlaying = false;
static int selectedBank = 0;

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
    switch(bank){
        case 0:
            selectedBank = 0;
            setSampleBank(random_chunk1, sizeof(random_chunk1)/sizeof(random_chunk1[0]));
            break;
        case 1:
            selectedBank = 1;
            setSampleBank(random_chunk2, sizeof(random_chunk2)/sizeof(random_chunk2[0]));
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
    TCCR2B |= (1 << CS21) | (1 << CS20);
    TIMSK2 |= (1 << OCIE2A);
    OCR2A = 255;

    /* -------- 8-bit PWM setup (Timer0) -------- */
    DDRD   |= (1 << PWM_OUT);
    TCCR0A |= (1 << COM0B1) | (1 << WGM00) | (1 << WGM01);
    TCCR0B |= (1 << CS00);

    setSampleBank(random_chunk1, sizeof(random_chunk1)/sizeof(random_chunk1[0]));
}

void buffer_tick(){
    static int i_prog = 0;

    if(buffer_put(pgm_read_byte(&sampleBank[i_prog]))){
        // if buffer is full, do nothing
    }else{
        i_prog++;
        if(i_prog >= sampleBank_size){
            i_prog = 0;
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
    }
}


