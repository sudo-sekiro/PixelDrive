#include "UserInterface/CustomSlider.h"

void LookAndFeel::drawRotarySlider(juce::Graphics &g,
                                   int x,
                                   int y,
                                   int width,
                                   int height,
                                   float sliderPosProportional,
                                   float rotaryStartAngle,
                                   float rotaryEndAngle,
                                   juce::Slider & slider)
{
    using namespace juce;

    juce::ignoreUnused(slider);

    auto bounds = Rectangle<float>((float)x, (float)y, (float)width, (float)height);

    g.setColour(Colour(255u, 154u, 1u));
    g.drawEllipse(bounds, 1.f);

    auto center = bounds.getCentre();

    Path p;

    Rectangle<float> r;

    r.setLeft(center.getX() - 2);
    r.setRight(center.getX() + 2);
    r.setTop(bounds.getY());
    r.setBottom(center.getY());

    p.addRectangle(r);

    jassert(rotaryStartAngle < rotaryEndAngle);

    auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);

    p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));

    g.fillPath(p);
}

//======================================================================
void CustomRotarySlider::paint(juce::Graphics &g)
{
    using namespace juce;
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
                                      jmap<float>((float)getValue(), (float)range.getStart(), (float)range.getEnd(), 0.0f, 1.0f),
                                      startAng,
                                      endAng,
                                      *this);

    auto center = sliderBounds.toFloat().getCentre();
    auto radius = sliderBounds.toFloat().getHeight() / 2.f;
    g.setColour(Colour(0u, 172u, 1u));
    g.setFont((float)getTextHeight());
    auto numChoices = labels.size();
    for (int i = 0; i < numChoices; i++)
    {
        auto pos = labels[i].pos;
        if (pos >= 0.f && pos <= 1.f) {
            auto ang = jmap(pos, 0.f, 1.f, startAng, endAng);
            auto c = center.getPointOnCircumference(radius,
                                                    ang);

            Rectangle<float> r;
            auto str = labels[i].label;
            r.setSize((float)g.getCurrentFont().getStringWidth(str), (float)getTextHeight());
            r.setCentre(c);
            r.setY(r.getY() + getTextHeight());
            g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);
        } else {
            Rectangle<float> r;
            auto str = labels[i].label;
            r.setSize((float)g.getCurrentFont().getStringWidth(str), (float)getTextHeight());
            r.setCentre(getLocalBounds().toFloat().getWidth() / 2.f, getTextHeight() / 2.f);
            g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);
        }
    }
}

juce::Rectangle<int> CustomRotarySlider::getSliderBounds() const
{
    auto bounds = getLocalBounds();
    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
    size -= getTextHeight() * 2;
    juce::Rectangle<int> r;
    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), 0);
    r.setY(getTextHeight());
    return r;
}
