#pragma once
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <data/dataBanks.cpp>

#define BUFFER_SIZE 256

// pointer to target bank
const uint8_t* sampleBank;
uint8_t sampleBank_size;

uint8_t buffer[BUFFER_SIZE];
volatile uint8_t head = 0;
volatile uint8_t tail = 0;

static bool sampleIsPlaying = false;

void setSamplePlayback(bool play){
    sampleIsPlaying = play;
}

void setSampleBank(const uint8_t *bank, size_t size){
    sampleBank = bank;
    sampleBank_size = size;
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


