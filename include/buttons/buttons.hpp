#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>

#include "buttonObserver.hpp"
#include <modes.hpp>

#define BUTTON_COUNT 8
#define MODE_BUTTON_COUNT 3

/*
    Pins used for step buttons:
    PB2, PB3, PB4, PB5, PC0, PC1, PC2, PC3

    Pins used for mode buttons:
    PD7, PD6, PB0
*/

// button pin by port, name and number
class PushButton{
    public:
        PushButton(){}
        PushButton(volatile uint8_t *port, uint8_t pinName, volatile uint8_t *pinReg){
        this->port = port;
        this->pinName = pinName;
        this->pinReg = pinReg;

        // enable pullup resistor
        *port |= (1 << pinName);
        }

        void tick(){
            bool state = read();
            if(state && !lastState){
                triggered = true;
            }
            lastState = state;
        }

        bool isTriggered(){
            if(triggered){
                triggered = false;
                return true;
            }
            return false;
        }

        bool read(){
            return !(*pinReg & (1 << pinName));
        }

    private:
        volatile uint8_t *port;
        volatile uint8_t *pinReg;
        uint8_t pinName;

        bool triggered = false;
        bool lastState = false;
};

class ControlInterfaceButtons : public ButtonObservable{
    public:
        ControlInterfaceButtons(){
            buttons[0] = PushButton(&PORTB, PB2, &PINB);
            buttons[1] = PushButton(&PORTB, PB3, &PINB);
            buttons[2] = PushButton(&PORTB, PB4, &PINB);
            buttons[3] = PushButton(&PORTB, PB5, &PINB);
            buttons[4] = PushButton(&PORTC, PC0, &PINC);
            buttons[5] = PushButton(&PORTC, PC1, &PINC);
            buttons[6] = PushButton(&PORTC, PC2, &PINC);
            buttons[7] = PushButton(&PORTC, PC3, &PINC);

            modeButtons[0] = PushButton(&PORTD, PD6, &PIND);
            modeButtons[1] = PushButton(&PORTD, PD7, &PIND);
            modeButtons[2] = PushButton(&PORTB, PB0, &PINB);
        }

        void tick(){
            for(int i = 0; i < BUTTON_COUNT; i++){
                buttons[i].tick();
            }

            for(int i = 0; i < MODE_BUTTON_COUNT; i++){
                modeButtons[i].tick();
            }

            // read mode buttons states
            bool stateA = modeButtons[0].read();
            bool stateB = modeButtons[1].read();
            bool stateC = modeButtons[2].read();

            int command = (stateA << 2 | stateB << 1 | stateC);

            switch(command) {
                case 0: // 000: no buttons are pressed
                    command = CMD_PRESS_STEP;
                    break;
                case 4: // 100: only button A is pressed
                    command = CMD_SET_MODE;
                    break;
                case 2: // 010: only button B is pressed
                    command = CMD_SELECT_STEP;
                    break;
                case 1: // 001: only button C is pressed
                    command = CMD_SET_GLOBAL_MODE;
                    break;
                case 3: // 011: buttons B and C are pressed
                case 5: // 101: buttons A and C are pressed
                case 6: // 110: buttons A and B are pressed
                    // set random value for current step
                    command = CMD_RANDOMIZE;
                    break;
                case 7: // 111: all buttons are pressed
                    break;
                default:
                    break;
            }

            notifyButtonTrigger(command);
        }


    private:
        PushButton buttons[BUTTON_COUNT];
        PushButton modeButtons[MODE_BUTTON_COUNT];

        bool read(int i){
            return buttons[i].read();
        }

        bool isTriggered(int i){
            return buttons[i].isTriggered();
        }

        void notifyButtonTrigger(int command){
            for(int i = 0; i < BUTTON_COUNT; i++){
                if(buttons[i].isTriggered()){
                    notify(i, command);
                    return;
                }
            }

            // if no button was triggered, check mode buttons
            if(modeButtons[0].isTriggered()){
                if(modeButtons[1].read()){
                    notify(1, CMD_RANDOMIZE);
                }
                else
                notify(0, CMD_SHIFTA);
            }
            else if(modeButtons[1].isTriggered()){
                notify(1, CMD_SHIFTB);
            }
            else if(modeButtons[2].isTriggered()){
                notify(2, CMD_STARTSTOP);
            }
          
        }
    
};