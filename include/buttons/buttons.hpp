#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>

#include "buttonObserver.hpp"
#include <modes.hpp>

#define BUTTON_COUNT 8
#define MODE_BUTTON_COUNT 2

/*
    Pins used for step buttons:
    PB2, PB3, PB4, PB5, PC0, PC1, PC2, PC3

    Pins used for mode buttons:
    PD7, PD6
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
        }

        void tick(){
            for(int i = 0; i < 8; i++){
                buttons[i].tick();
            }

            // read mode buttons states
            bool stateA = modeButtons[0].read();
            bool stateB = modeButtons[1].read();

            int command = (stateA << 1 | stateB); // convert button states to a command
            // int command = (stateA << 2 | stateB << 1 | stateC); for a third button

            switch(command) {
                case 0: // 00: both buttons are not pressed
                    command = CMD_PRESS_STEP;
                    break;
                case 2: // 10: only button A is pressed
                    command = CMD_SET_MODE;
                    break;
                case 1: // 01: only button B is pressed
                    command = CMD_SELECT_STEP;
                    break;
                default:
                    // handle the case when both buttons are pressed
                    break;
            }

            notifyButtonTrigger(command);
        }


    private:
        PushButton buttons[8];
        PushButton modeButtons[2];

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
                notify(0, CMD_SHIFTA);
            }
            else if(modeButtons[1].isTriggered()){
                notify(1, CMD_SHIFTB);
            }
          
        }
    
};