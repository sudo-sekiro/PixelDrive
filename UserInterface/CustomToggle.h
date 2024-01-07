#ifndef USERINTERFACE_CUSTOMTOGGLE_H_
#define USERINTERFACE_CUSTOMTOGGLE_H_

#include "LookAndFeel.h"

struct CustomToggleButton : juce::ToggleButton {
    CustomToggleButton() :
        juce::ToggleButton() {
        setLookAndFeel(&lnf);
    }
    explicit CustomToggleButton(juce::String label) :
        juce::ToggleButton(label) {
        setLookAndFeel(&lnf);
    }
    ~CustomToggleButton() {
        setLookAndFeel(nullptr);
    }
    // void paintButton (juce::Graphics&, bool, bool) override;
    int getTextHeight() const {return 14; }
    // juce::String label;
 private:
    CustomLookAndFeel lnf;
};

#endif  // USERINTERFACE_CUSTOMTOGGLE_H_
