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
    explicit CustomToggleButton(juce::String label) :
        juce::ToggleButton(label)
    {
        setLookAndFeel(&lnf);
    }
    ~CustomToggleButton()
    {
        setLookAndFeel(nullptr);
    }
    // void paintButton (juce::Graphics&, bool, bool) override;
    int getTextHeight() const {return 14; }
    // juce::String label;
private:
    LookAndFeel lnf;
};

#endif //__USER_INTERFACE__CUSTOMTOGGLE_
