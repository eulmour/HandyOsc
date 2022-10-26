#ifndef HANDYOSC_DSP_BIQUAD_FILTER_HPP
#define HANDYOSC_DSP_BIQUAD_FILTER_HPP

#include "JuceHeader.h"

class BiquadFilter
{
public:
    BiquadFilter() = default;
    BiquadFilter(float freq, float res);

    enum class FilterType
    {
        Off,
        LowPass,
        HighPass,
        BandPass,
        Notch,
        Peak,
        LowShelf,
        HighShelf,
        LowShelfNoQ,
        HighShelfNoQ,
        AllPass
    };

    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept
    {
        auto& outputBlock = context.getOutputBlock();

        const auto numOutputChannels = outputBlock.getNumChannels();
        const auto numSamples = outputBlock.getNumSamples();

        jassert (outputBlock.getNumSamples() == numSamples);
        juce::ignoreUnused (numSamples);

        if (numOutputChannels != 2 || numOutputChannels == 0 || numOutputChannels > 2)
            return;

        if (context.isBypassed)
            return;

        for (size_t channel = 0; channel < numOutputChannels; ++channel)
        {
            auto* outputSamples = outputBlock.getChannelPointer (channel);

            for (size_t i = 0; i < numSamples; ++i)
            {
                 outputSamples[i] = this->filter(outputSamples[i], channel);
            }
        }
    }

    void setSampleRate (double sampleRate) { mSampleRate = sampleRate; }
    void clear();

    void setFilterType (FilterType type);

    void setFrequency(double f);
    void setResonance(double q);

//    void copyCoeffFrom (BiquadFilter& other);
    bool usesGain() const { return mType == FilterType::Peak || mType == FilterType::HighShelf || mType == FilterType::LowShelf; }
    bool usesQ() { return true; }

    inline float filter (float in,  int channel) {
        double out = in * this->coeff[channel].mA0 + this->coeff[channel].mZ1;
        this->coeff[channel].mZ1 = in * this->coeff[channel].mA1 + this->coeff[channel].mZ2 - this->coeff[channel].mB1 * out;
        this->coeff[channel].mZ2 = in * this->coeff[channel].mA2 - this->coeff[channel].mB2 * out;
        return static_cast<float>(out);
    }

    float mF;
    float mQ;
    float mDbGain;
    FilterType mType;

protected:

    double mSampleRate;

    struct Coefficients {
        double mA0;
        double mA1;
        double mA2;
        double mB1;
        double mB2;
        double mZ1;
        double mZ2;

        void updateFilterCoeff(FilterType type, float freq, float res, float dbGain, double sampleRate);
        float getMagnitudeResponseAt (float f, double sampleRate) const;

    } coeff[2] = {};

    void updateCoefficients();
};

#endif // HANDYOSC_DSP_BIQUAD_FILTER_HPP