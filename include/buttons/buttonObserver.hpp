#pragma once

// observer pattern for buttons
class ButtonObserver{
    public:
        virtual void onButtonPressed(int buttonIndex, int command) = 0;
};

class ButtonObservable{
    public:
        void addObserver(ButtonObserver *observer){
            observers[observerCount] = observer;
            observerCount++;
        }

        void notify(int buttonIndex, int mode){
            for(int i = 0; i < observerCount; i++){
                observers[i]->onButtonPressed(buttonIndex, mode);
            }
        }

    private:
        ButtonObserver *observers[8];
        int observerCount = 0;
};