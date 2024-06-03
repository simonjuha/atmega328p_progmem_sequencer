#pragma once
#include <avr/io.h>
#include <avr/interrupt.h>
#include <globalSettings.hpp>

static bool doStep = false;
static bool INT1_triggered = false;

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

void setStepClockActive(bool active){
    if(active){
        TIMSK1 |= (1 << OCIE1A); // Enable Timer1 interrupt
    } else {
        TIMSK1 &= ~(1 << OCIE1A); // Disable Timer1 interrupt
    }
}

bool doStepNow() {
    // check if a step should be executed
    if(doStep) {
        doStep = false;
        return true;
    }
    return false;
}

// Timer1 stepper interrupt
ISR(TIMER1_COMPA_vect) {
    // external clock
    if(glst[GLST_CLOCK_EXT].value && INT1_triggered) {
        doStep = true;
        INT1_triggered = false;
    }
    // internal clock
    else if(!glst[GLST_CLOCK_EXT].value) {
        doStep = true;
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