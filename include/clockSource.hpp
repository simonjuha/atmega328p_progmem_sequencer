#pragma once
#include <avr/io.h>
#include <avr/interrupt.h>

static bool doStep = false;
static bool INT1_triggered = false;
static bool externalClock = true;

void clockSource_init(){
    /* -------- 16-bit Timer interupt setup (Timer1) -------- */
    TCCR1B |= (1 << WGM12) | (1 << CS10) | (1 << CS11);
    OCR1A = 6000;
    TIMSK1 |= (1 << OCIE1A);// Enable Timer1 interrupt
    TIFR1 |= (1 << OCF1A); 

    // PD4 as clock sync output
    DDRD &= ~(1 << PD4);
    PORTD |= (1 << PD4); // pull-up resistor

    // PD3 clock interrupt input
    DDRD &= ~(1 << PD3);
    //PORTD |= (1 << PD3);
    EICRA |= (1 << ISC11) | (1 << ISC10); // rising edge
    EIMSK |= (1 << INT1);
}

void setExternalClock(bool state) {
    externalClock = state;
}

void handleClockSelection() {
    doStep = true;
    if(externalClock) {
        INT1_triggered = false;
    }
}

void clockSource_tick() {
    if(!(PIND & (1 << PD3))){
        
    }else{
        
    }
}

bool doStepNow() {
    if(doStep) {
        doStep = false;
        return true;
    }
    return false;
}

// Timer1 stepper interrupt
ISR(TIMER1_COMPA_vect) {
    if(externalClock && INT1_triggered) {
        handleClockSelection();
    }

    // internal clock
    if(!externalClock) {
        handleClockSelection();
    }
}

// External clock interrupt
ISR(INT1_vect) {
    INT1_triggered = true;
}

// set tempo
void setTempo(uint16_t tempo) {
    OCR1A = tempo;
}