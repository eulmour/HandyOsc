
#ifndef HANDYOSC_BIT_REDUCTION_HPP
#define HANDYOSC_BIT_REDUCTION_HPP

#include "JuceHeader.h"

inline float drywet(float dry, float wet, float mix) {
    return (wet * mix) + (dry * (1 - mix));
}

class BitReduction {
public:
    BitReduction() = default;

    template <typename ProcessContext>
    void process(const ProcessContext& context, float amount) noexcept {
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();

        const auto numInputChannels = inputBlock.getNumChannels();
        const auto numOutputChannels = outputBlock.getNumChannels();
        const auto numSamples = outputBlock.getNumSamples();

        jassert (inputBlock.getNumSamples() == numSamples);
        juce::ignoreUnused (numSamples);

        if (numOutputChannels != 2 || numInputChannels == 0 || numInputChannels > 2)
            return;

        if (numInputChannels == 2)
        {
            outputBlock.copyFrom (inputBlock);
        }
        else
        {
            outputBlock.getSingleChannelBlock (0).copyFrom (inputBlock);
            outputBlock.getSingleChannelBlock (1).copyFrom (inputBlock);
        }

        if (context.isBypassed)
            return;

        for (size_t channel = 0; channel < numInputChannels; ++channel)
        {
            auto* inputSamples = inputBlock.getChannelPointer (channel);
            auto* outputSamples = outputBlock.getChannelPointer (channel);

            for (size_t i = 0; i < numSamples; ++i) {
                outputSamples[i] = this->mix(inputSamples[i],
                    processSample (inputSamples[i], amount)
                );
            }
        }
    }

    void setMixAmount(float amount) { this->mixAmount = amount; }

protected:
    inline float processSample(float x, float amount) noexcept {
        return static_cast<float>(static_cast<int>(x * amount) / amount);
    }

    inline float mix(float dry, float wet) const noexcept {
        return (wet * this->mixAmount) + (dry * (1 - this->mixAmount));
    }

    float mixAmount = 1.f;
};

#endif // HANDYOSC_BIT_REDUCTION_HPP