#include "UserInterface/CustomSlider.h"

#define SLIDER_INDICATOR_COLOUR_HEX 0xff508da1
#define SLIDER_INDICATOR_THICKNESS 2
#define SLIDER_INDICATOR_HEIGHT_PADDING 0.11

void CustomLookAndFeel::drawRotarySlider(juce::Graphics &g,
                                   int x,
                                   int y,
                                   int width,
                                   int height,
                                   float sliderPosProportional,
                                   float rotaryStartAngle,
                                   float rotaryEndAngle,
                                   juce::Slider & slider) {
    using juce::Rectangle, juce::jmap, juce::Colour, juce::Path, juce:: AffineTransform;

    juce::ignoreUnused(slider);

    auto bounds = Rectangle<float>(static_cast<float>(x),
                                   static_cast<float>(y),
                                   static_cast<float>(width),
                                   static_cast<float>(height));

    g.setColour(juce::Colour(SLIDER_INDICATOR_COLOUR_HEX));

    // Draw background
    juce::Image sliderBackground = ImageFileFormat::loadFrom(BinaryData::knob_png,
                                                             BinaryData::knob_pngSize);
    g.drawImage(sliderBackground,  // Image to draw
                bounds.toFloat(),  // Rectangle to draw within
                juce::RectanglePlacement::fillDestination,  // placementWithinTarget - default stretchToFit
                false);  // fillAlphaChannelWithCurrentBrush

    auto center = bounds.getCentre();

    Path p;

    Rectangle<float> r;

    r.setLeft(center.getX() - SLIDER_INDICATOR_THICKNESS);
    r.setRight(center.getX() + SLIDER_INDICATOR_THICKNESS);
    r.setTop(bounds.getY() + bounds.proportionOfHeight(SLIDER_INDICATOR_HEIGHT_PADDING));
    r.setBottom(center.getY());

    p.addRectangle(r);

    jassert(rotaryStartAngle < rotaryEndAngle);

    auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);

    p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));

    g.fillPath(p);
}

//======================================================================
void CustomRotarySlider::paint(juce::Graphics &g) {
    using juce::Rectangle, juce::jmap, juce::Colour, juce::degreesToRadians, juce::MathConstants;
    auto startAng = degreesToRadians(180.f + 45.f);
    auto endAng = degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;

    auto range = getRange();

    auto sliderBounds = getSliderBounds();

    // Draw boxes around bounds for debugging
    // g.setColour(Colours::red);
    // g.drawRect(getLocalBounds());
    // g.setColour(Colours::yellow);
    // g.drawRect(sliderBounds);

    getLookAndFeel().drawRotarySlider(g,
                                      sliderBounds.getX(),
                                      sliderBounds.getY(),
                                      sliderBounds.getWidth(),
                                      sliderBounds.getHeight(),
                                      jmap<float>(static_cast<float>(getValue()),
                                                  static_cast<float>(range.getStart()),
                                                  static_cast<float>(range.getEnd()),
                                                  0.0f,
                                                  1.0f),
                                      startAng,
                                      endAng,
                                      *this);

    auto center = sliderBounds.toFloat().getCentre();
    auto radius = sliderBounds.toFloat().getHeight() / 2.f;
    g.setColour(juce::Colours::black);
    g.setFont(static_cast<float>(getTextHeight()));
    auto numChoices = labels.size();
    for (int i = 0; i < numChoices; i++) {
        auto pos = labels[i].pos;
        if (pos >= 0.f && pos <= 1.f) {
            auto ang = jmap(pos, 0.f, 1.f, startAng, endAng);
            auto c = center.getPointOnCircumference(radius,
                                                    ang);

            Rectangle<float> r;
            auto str = labels[i].label;
            r.setSize(static_cast<float>(g.getCurrentFont().getStringWidth(str)), static_cast<float>(getTextHeight()));
            r.setCentre(c);
            r.setY(r.getY() + getTextHeight());
            g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);
        } else {
            Rectangle<float> r;
            auto str = labels[i].label;
            r.setSize(static_cast<float>(g.getCurrentFont().getStringWidth(str)), static_cast<float>(getTextHeight()));
            r.setCentre(getLocalBounds().toFloat().getWidth() / 2.f, getTextHeight() / 2.f);
            g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);
        }
    }
}

juce::Rectangle<int> CustomRotarySlider::getSliderBounds() const {
    auto bounds = getLocalBounds();
    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
    size -= getTextHeight() * 2;
    juce::Rectangle<int> r;
    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), 0);
    r.setY(getTextHeight());
    return r;
}

void CustomRotarySlider::addSliderLabels(juce::String minLabel, juce::String maxLabel, juce::String titleLabel) {
    labels.add({0.f, minLabel});
    labels.add({1.f, maxLabel});
    labels.add({2.f, titleLabel});
}
