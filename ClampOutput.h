/**
 * Silences the buffer if bad or loud values are detected in the output buffer.
 * Use this during debugging to avoid blowing out your eardrums on headphones.
 * If the output value is out of the range [-1, +1] it will be hard clipped.
 * Credit to: https://gist.github.com/hollance
 */
inline void protectYourEars(juce::AudioBuffer<float>& buffer, int sampleCount, int numChannels)
{
    bool firstWarning = true;
    for (int channel = 0; channel < numChannels; channel++) {
        if (buffer.getMagnitude(channel, 0, sampleCount) > 2.f) {
            DBG("!!! WARNING: sample out of range, silencing !!!");
            buffer.clear();
            return;
        }
        for (int i = 0; i < sampleCount; ++i) {
            float x = buffer.getSample(channel, i);
            if (std::isnan(x)) {
                DBG("!!! WARNING: nan detected in audio buffer, silencing !!!");
            } else if (std::isinf(x)) {
                DBG("!!! WARNING: inf detected in audio buffer, silencing !!!");
            } else if (x < -1.0f) {
                if (firstWarning) {
                    DBG("!!! WARNING: sample out of range, clamping !!!");
                    firstWarning = false;
                }
                buffer.setSample(channel, i, -1.f);
            } else if (x > 1.0f) {
                if (firstWarning) {
                    DBG("!!! WARNING: sample out of range, clamping !!!");
                    firstWarning = false;
                }
                buffer.setSample(channel, i, 1.f);
            }
        }
    }
}
