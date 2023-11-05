#ifndef __USER_INTERFACE__CUSTOMTOGGLE_
#define __USER_INTERFACE__CUSTOMTOGGLE_

#include "LookAndFeel.h"

struct CustomToggleButton : juce::ToggleButton
{
    CustomToggleButton() :
        juce::ToggleButton()
    {
        setLookAndFeel(&lnf);
    }
    ~CustomToggleButton()
    {
        setLookAndFeel(nullptr);
    }
    int getTextHeight() const {return 14; }
private:
    LookAndFeel lnf;
};

#endif //__USER_INTERFACE__CUSTOMTOGGLE_
