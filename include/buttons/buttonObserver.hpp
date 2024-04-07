#pragma once

// observer pattern for buttons
class ButtonObserver{
    public:
        virtual void onButtonPressed(int buttonIndex, int mode) = 0;
};