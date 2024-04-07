#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>

/*
    Pins used for buttons:
    PB2, PB3, PB4, PB5, PC0, PC1, PC2, PC3
*/

// observer pattern for buttons
class ButtonObserver{
    public:
        virtual void onButtonPressed(int buttonIndex) = 0;
};

class ButtonObservable{
    public:
        void addObserver(ButtonObserver *observer){
            observers[observerCount] = observer;
            observerCount++;
        }

        void notify(int buttonIndex){
            for(int i = 0; i < observerCount; i++){
                observers[i]->onButtonPressed(buttonIndex);
            }
        }

    private:
        ButtonObserver *observers[8];
        int observerCount = 0;
};

// button pin by port, name and number
class StepButton{
    public:
        StepButton(){}
        StepButton(volatile uint8_t *port, uint8_t pinName, volatile uint8_t *pinReg){
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

class StepButtonsGroup : public ButtonObservable{
    public:
        StepButtonsGroup(){
            buttons[0] = StepButton(&PORTB, PB2, &PINB);
            buttons[1] = StepButton(&PORTB, PB3, &PINB);
            buttons[2] = StepButton(&PORTB, PB4, &PINB);
            buttons[3] = StepButton(&PORTB, PB5, &PINB);
            buttons[4] = StepButton(&PORTC, PC0, &PINC);
            buttons[5] = StepButton(&PORTC, PC1, &PINC);
            buttons[6] = StepButton(&PORTC, PC2, &PINC);
            buttons[7] = StepButton(&PORTC, PC3, &PINC);
        }

        void tick(){
            for(int i = 0; i < 8; i++){
                buttons[i].tick();
            }

            for(int i = 0; i < 8; i++){
                if(buttons[i].isTriggered()){
                    notify(i);
                }
            }
        }

        bool read(int i){
            return buttons[i].read();
        }

    private:
        StepButton buttons[8];
        bool isTriggered(int i){
            return buttons[i].isTriggered();
        }
};

