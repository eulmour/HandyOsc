#ifndef HANDYOSC_WAVE_WARP_HPP
#define HANDYOSC_WAVE_WARP_HPP

#include "JuceHeader.h"

class WaveWarp {
public:
    WaveWarp() = default;

    template <typename ProcessContext>
    void process(const ProcessContext& context, float amount) noexcept {

        auto& outputBlock = context.getOutputBlock();

        const auto numOutputChannels = outputBlock.getNumChannels();
        const auto numSamples = outputBlock.getNumSamples();

        jassert (outputBlock.getNumSamples() == numSamples);
        juce::ignoreUnused (numSamples);

        if (numOutputChannels != 2 || numOutputChannels == 0 || numOutputChannels > 2)
            return;

        if (context.isBypassed)
            return;

        switch (juce::roundToInt(this->type->load())) {

            case 0:

                for (size_t channel = 0; channel < numOutputChannels; ++channel) {
                    auto* outputSamples = outputBlock.getChannelPointer (channel);
                    for (size_t i = 0; i < numSamples; ++i) {

                        if (std::abs(outputSamples[i]) > std::abs(this->floatSample[channel])) {
                            this->floatSample[channel] = outputSamples[i];
                        }
                        else {
                            if (this->floatSample[channel] > 0)
                                this->floatSample[channel] -= amount;
                            else if (this->floatSample[channel] < 0)
                                this->floatSample[channel] += amount;
                        }

                        outputSamples[i] = this->mix(outputSamples[i], this->floatSample[channel]);
                    }
                }

                break;
            case 1:

                for (size_t channel = 0; channel < numOutputChannels; ++channel) {
                    auto* outputSamples = outputBlock.getChannelPointer (channel);
                    for (size_t i = 0; i < numSamples; ++i) {

                        if (outputSamples[i] > std::abs(this->floatSample[channel])) {
                            this->floatSample[channel] = outputSamples[i];
                        }
                        else {
                            if (this->floatSample[channel] > 0)
                                this->floatSample[channel] -= amount;
                            else if (this->floatSample[channel] < 0)
                                this->floatSample[channel] += amount;
                        }

                        outputSamples[i] = this->mix(outputSamples[i], this->floatSample[channel]);
                    }
                }

                break;
            case 2:

                for (size_t channel = 0; channel < numOutputChannels; ++channel) {
                    auto* outputSamples = outputBlock.getChannelPointer (channel);
                    for (size_t i = 0; i < numSamples; ++i) {

                        if (outputSamples[i] > this->floatSample[channel]) {
                            this->floatSample[channel] = outputSamples[i];
                        }
                        else {
                            if (this->floatSample[channel] > 0)
                                this->floatSample[channel] -= amount;
                            else if (this->floatSample[channel] < 0)
                                this->floatSample[channel] += amount;
                        }

                        outputSamples[i] = this->mix(outputSamples[i], this->floatSample[channel]);
                    }
                }

                break;

            default:
                break;
        }
    }

    void setMixAmount(float amount) { this->mixAmount = amount; }

    std::atomic<float>* type;

private:

    [[nodiscard]] inline float mix(float dry, float wet) const noexcept {
        return (wet * this->mixAmount) + (dry * (1 - this->mixAmount));
    }

    float floatSample[2] = { 0.f };
    float mixAmount = 1.f;
};
#endif // HANDYOSC_WAVE_WARP_HPP