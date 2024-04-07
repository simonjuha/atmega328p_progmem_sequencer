#pragma once

// observer pattern for buttons
class ButtonObserver{
    public:
        virtual void onButtonPressed(int buttonIndex) = 0;
};