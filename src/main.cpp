// ATmega328p
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdlib.h>

#include <stepper.hpp>

#include <buttons/buttonObserver.hpp>
#include <buttons/buttons.hpp>
#include <options.hpp>
#include <leds.hpp>

#define F_CPU 16000000UL
#define PWM_OUT PD5

#define LED1 PB0

#define BUFFER_SIZE 256

uint8_t buffer[BUFFER_SIZE];
volatile uint8_t head = 0;
volatile uint8_t tail = 0;


const PROGMEM uint8_t random_chunk1[] = {
0x9C, 0x14, 0xD1, 0x58, 0x5A, 0x59, 0x19, 0xF2, 0x07, 0xBC, 0xDA, 0x13, 0xF5, 0x45, 0x64, 0x53, 
0x5C, 0x57, 0x5A, 0x19, 0xB3, 0xDF, 0x50, 0x5E, 0x17, 0xF3, 0x46, 0x23, 0xAD, 0xA2, 0xA8, 0xA5, 
0xA7, 0xA6, 0xE6, 0x0D, 0xF9, 0x43, 0x64, 0x13, 0xF5, 0x45, 0x24, 0xED, 0x09, 0xBB, 0xDB, 0x52, 
0x5C, 0x57, 0x1A, 0xB2, 0xE0, 0x10, 0xF7, 0x44, 0x64, 0x13, 0xB5, 0x9E, 0xAA, 0xA4, 0xE7, 0x0C, 
0xF9, 0x03, 0xFE, 0x40, 0x26, 0xAC, 0xE3, 0x0E, 0xB8, 0x9D, 0xAB, 0xA4, 0xE7, 0x0C, 0xB9, 0x9C, 
0xEB, 0x4A, 0x21, 0xAE, 0xA2, 0xE8, 0x4C, 0x20, 0xEF, 0x48, 0x62, 0x15, 0xB5, 0x9E, 0xEA, 0x0B, 
0xFA, 0x03, 0xBE, 0xD9, 0x53, 0x1C, 0xB1, 0xE0, 0x50, 0x5E, 0x17, 0xB4, 0xDF, 0x11, 0xF7, 0x04, 
0xFD, 0x01, 0xBF, 0xD9, 0x14, 0xB5, 0xDE, 0x11, 0xF6, 0x04, 0xFD, 0x01, 0xBF, 0x99, 0xED, 0x49, 
0x21, 0xEE, 0x49, 0x22, 0xEE, 0x49, 0x61, 0x15, 0xB5, 0x9E, 0xEA, 0x4B, 0x60, 0x15, 0xF4, 0x45, 
0x63, 0x14, 0xF5, 0x45, 0x63, 0x53, 0x5C, 0x18, 0xB3, 0x9F, 0xAA, 0xE4, 0x4E, 0x5F, 0x16, 0xF4, 
0x06, 0xBC, 0x9A, 0xAC, 0xA3, 0xE8, 0x0C, 0xF9, 0x09, 0xED, 0xEB, 0x0D, 0x33, 0x48, 0x2D, 0x0D, 
0xFA, 0xF1, 0xDA, 0xEF, 0x05, 0x0F, 0x35, 0x45, 0x3F, 0x41, 0x41, 0x30, 0xFB, 0xD6, 0x01, 0x3A, 
0x33, 0x0A, 0x0C, 0x26, 0xFF, 0xC4, 0xBB, 0xBF, 0xBE, 0xBE, 0xCF, 0xF4, 0x04, 0x0F, 0x35, 0x35, 
0x0A, 0x0C, 0x26, 0x0F, 0xFA, 0xF0, 0xDB, 0x00, 0x3B, 0x43, 0x2F, 0xFB, 0xD6, 0xF1, 0x05, 0x0F, 
0x35, 0x35, 0xF9, 0xC6, 0xBB, 0xBF, 0xCF, 0xF4, 0x04, 0xFE, 0x00, 0x10, 0x24, 0xFF, 0xD5, 0xF1, 
0xF4, 0xC8, 0xBA, 0xBF, 0xCF, 0xF4, 0xF3, 0xC9, 0xCB, 0x06, 0x28, 0xFE, 0xC5, 0xCC, 0x06, 0x28, 
0x0E, 0x0B, 0x37, 0x34, 0xF9, 0xC6, 0xCC, 0xF5, 0x03, 0xFE, 0xEF, 0xDB, 0x00, 0x2A, 0xFD, 0xD5, 
0x02, 0x3A, 0x33, 0xFA, 0xD7, 0xF1, 0x05, 0xFE, 0x00, 0x00, 0xEF, 0xDB, 0xEF, 0xF4, 0xD9, 0xEF, 
0x05, 0xFE, 0x00, 0x00, 0xEF, 0xCA, 0xCA, 0x06, 0x28, 0x0E, 0x0B, 0x26, 0x0F, 0x0A, 0x37, 0x34, 
0xF9, 0xC6, 0xCC, 0x05, 0x39, 0x33, 0x0A, 0x0C, 0x36, 0x34, 0x0A, 0x0C, 0x36, 0x45, 0x40, 0x30, 
0xFB, 0xC6, 0xBB, 0xD0, 0x04, 0x39, 0x33, 0x0A, 0xFC, 0xF0, 0xCA, 0xBA, 0xC0, 0xCF, 0xF4, 0x04, 
0x00, 0xFE, 0xF3, 0xF8, 0x12, 0x29, 0x2D, 0x1A, 0x05, 0xFA, 0xEF, 0xEB, 0xF8, 0x06, 0x14, 0x29, 
0x31, 0x2F, 0x2F, 0x2B, 0x16, 0xF5, 0xED, 0x0D, 0x27, 0x1C, 0x09, 0x0F, 0x10, 0xF2, 0xD4, 0xCE, 
0xD0, 0xD0, 0xD4, 0xE5, 0xFA, 0x05, 0x14, 0x25, 0x1C, 0x09, 0x0E, 0x15, 0x07, 0xF9, 0xEF, 0xEF, 
0x0D, 0x2B, 0x2C, 0x15, 0xF5, 0xE9, 0xF8, 0x06, 0x14, 0x25, 0x18, 0xF0, 0xD4, 0xCF, 0xD4, 0xE5, 
0xFA, 0x01, 0xFF, 0x04, 0x10, 0x10, 0xF6, 0xE9, 0xF4, 0xED, 0xD5, 0xCE, 0xD5, 0xE5, 0xF5, 0xEC, 
0xDA, 0xE7, 0x0B, 0x12, 0xF2, 0xD8, 0xE8, 0x0B, 0x16, 0x0B, 0x13, 0x25, 0x18, 0xF0, 0xD8, 0xE3, 
0xFA, 0x01, 0xFB, 0xEF, 0xEF, 0x09, 0x12, 0xF6, 0xED, 0x0D, 0x27, 0x17, 0xF4, 0xE9, 0xF8, 0x01, 
0xFF, 0xFF, 0xFB, 0xEF, 0xEB, 0xF4, 0xF1, 0xEA, 0xF8, 0x02, 0xFF, 0xFF, 0xFB, 0xEA, 0xDA, 0xE7, 
0x0B, 0x16, 0x0B, 0x0E, 0x15, 0x0B, 0x12, 0x25, 0x18, 0xF0, 0xD8, 0xE8, 0x0F, 0x27, 0x1C, 0x09, 
0x13, 0x25, 0x1C, 0x09, 0x13, 0x2A, 0x31, 0x2B, 0x16, 0xF0, 0xD4, 0xD3, 0xE9, 0x0F, 0x27, 0x1C, 
0x05, 0xFA, 0xEB, 0xD6, 0xCE, 0xD4, 0xE5, 0xFA, 0x02, 0xFF, 0xFB, 0xF8, 0x01, 0x12, 0x1E, 0x1C, 
0x0E, 0x01, 0xF9, 0xF3, 0xF4, 0xFD, 0x08, 0x14, 0x1F, 0x23, 0x22, 0x21, 0x1A, 0x08, 0xF7, 0xFA, 
0x0F, 0x19, 0x10, 0x09, 0x0A, 0x04, 0xEF, 0xDF, 0xDC, 0xDD, 0xDE, 0xE4, 0xF1, 0xFE, 0x07, 0x12, 
0x18, 0x10, 0x09, 0x0C, 0x0B, 0x02, 0xF9, 0xF4, 0xFB, 0x10, 0x1F, 0x1B, 0x08, 0xF6, 0xF3, 0xFD, 
0x08, 0x12, 0x17, 0x08, 0xEE, 0xDF, 0xDD, 0xE4, 0xF1, 0xFC, 0x00, 0x01, 0x06, 0x0B, 0x05, 0xF6, 
0xF2, 0xF5, 0xEC, 0xE0, 0xDD, 0xE4, 0xF0, 0xF5, 0xED
};

const int random_chunk1_size = sizeof(random_chunk1)/sizeof(random_chunk1[0]);

bool sampleIsPlaying = false;
bool doStep = false;

bool ledChange = false;

int buffer_put(uint8_t value);
void playSample(uint16_t period);

Stepper stepper;
Leds leds(stepper);
ControlInterfaceButtons buttons;
ModeSelector selector(&stepper, &buttons);

int main(){


    // enable interrupt on button 4 (PD3 / INT1)
    EIMSK |= (1 << INT1);  // Enable INT1 (PD3)
    EICRA |= (1 << ISC10); // Trigger on falling edge

    /* -------- 8-bit PWM setup (Timer0) -------- */
    DDRD   |= (1 << PWM_OUT);
    TCCR0A |= (1 << COM0B1) | (1 << WGM00) | (1 << WGM01);
    TCCR0B |= (1 << CS00);

    /* -------- 8-bit Timer interupt setup (Timer2) -------- */
    TCCR2A |= (1 << WGM21); // CTC mode
    TCCR2B |= (1 << CS21) | (1 << CS20);
    TIMSK2 |= (1 << OCIE2A);
    OCR2A = 255;

    /* -------- 16-bit Timer interupt setup (Timer1) -------- */
    TCCR1B |= (1 << WGM12) | (1 << CS10) | (1 << CS11);
    OCR1A = 8000;
    TIMSK1 |= (1 << OCIE1A);// Enable Timer1 interrupt
    TIFR1 |= (1 << OCF1A); 

    /* -------- ADC setup -------- */
    //ADMUX |= (1 << REFS0);
    //ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADSC) | (1 << ADATE);

    // LED
    DDRB |= (1 << LED1);
    
    sei();

    while(1){

        buttons.tick();
        
        
        static int i = 0;

        if(buffer_put(pgm_read_byte(&random_chunk1[i]))){
            // if buffer is full, do nothing
        }else{
            i++;
            if(i >= random_chunk1_size){
                i = 0;
            }
        }
        
        // stepping
        static int gateState;

        if(doStep){
            gateState = stepper.step();
            doStep = false;
        }
        switch (gateState)
        {
            case ON:
                sampleIsPlaying = true;
                break;
            case OFF:
                sampleIsPlaying = false;
                break;
            default:
                break;
        }
        OCR2A = stepper.getStepSampleRate();

        leds.tick();

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

// Timer1 interrupt (trigger once)
ISR(TIMER1_COMPA_vect) {
    // toggle led
    //PORTB ^= (1 << LED1);
    // do step
    doStep = true;

    leds.doChange();
}

// trigger sample interrupt
ISR(INT1_vect) {
    //if(!(PIND & (1 << BTN4))){
       // sampleIsPlaying = false;
    //}
    //else{
        //sampleIsPlaying = true;
    //}
}


