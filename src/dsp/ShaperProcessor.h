#pragma once

#include <juce_dsp/juce_dsp.h>

enum OversamplingRate
{
    X1 = -1,
    X2 = 0,
    X4 = 1,
    X8 = 2,
    X16 = 3
};

using OversampledWaveShaperEventHandler = std::function<void (juce::AudioBuffer<float>&)>;

template <typename SampleType>
class ShaperProcessor : public juce::dsp::ProcessorBase
{

public:
    using ProcessContextReplacing = juce::dsp::ProcessContextReplacing<SampleType>;
    using ProcessorSpec = juce::dsp::ProcessSpec;
    using AudioBuffer = juce::AudioBuffer<SampleType>;
    using AudioBlock = juce::dsp::AudioBlock<SampleType>;

    ShaperProcessor() noexcept
        : clipper2x (1), clipper4x (2), clipper8x (3), clipper16x (4), clipper (&clipper2x)
    {
    }

    ~ShaperProcessor() override
    {
    }

    //==============================================================================
    /** Inheritet implemented methods */
    void prepare (const ProcessorSpec& spec) noexcept override
    {
        /** Setup input gain */
        inputGain.prepare (spec);
        inputGain.setRampDurationSeconds (RAMP_DURATION_SEC);
        setInputGain (DEFAULT_INPUT_GAIN);

        /** Setup clipper */
        clipper1x.prepare (spec);
        clipper2x.prepare (spec);
        clipper4x.prepare (spec);
        clipper8x.prepare (spec);
        clipper16x.prepare (spec);

        /** Setup output gain */
        outputGain.prepare (spec);
        outputGain.setRampDurationSeconds (RAMP_DURATION_SEC);
        setOutputGain (DEFAULT_OUTPUT_GAIN);
    }

    void reset() override
    {
        inputGain.reset();
        clipper1x.reset();
        clipper2x.reset();
        clipper4x.reset();
        clipper8x.reset();
        clipper16x.reset();
        outputGain.reset();
    }

    void process (ProcessContextReplacing const& context) override
    {
        inputGain.process (context);
        if (clipper)
        {
            clipper->process (context);
        }
        outputGain.process (context);
    }

    void process (AudioBuffer& buffer) noexcept
    {
        auto ioBlock = AudioBlock (buffer);
        ProcessContextReplacing context (ioBlock);

        inputGain.process (context);
        if (mPostInputGainHandler)
        {
            mPostInputGainHandler (buffer);
        }
        clipper->process (context);
        if (mPostCeilingHandler)
        {
            mPostCeilingHandler (buffer);
        }
        outputGain.process (context);
        if (mPostOutputGainHandler)
        {
            mPostOutputGainHandler (buffer);
        }
    }

    // ==============================================================================
    /** Public interface */
    void onPostInputGain (OversampledWaveShaperEventHandler const& handler)
    {
        mPostInputGainHandler = handler;
    }

    void onPostCeiling (OversampledWaveShaperEventHandler const& handler)
    {
        mPostCeilingHandler = handler;
    }

    void onPostOutputGain (OversampledWaveShaperEventHandler const& handler)
    {
        mPostOutputGainHandler = handler;
    }

    void unsubscribeFromAll()
    {
        mPostInputGainHandler = nullptr;
        mPostCeilingHandler = nullptr;
        mPostOutputGainHandler = nullptr;
    }

    void setInputGain (float gainDbValue) noexcept
    {
        inputGain.setGainDecibels (gainDbValue);
    }

    void setOutputGain (float gainDbValue) noexcept
    {
        outputGain.setGainDecibels (gainDbValue);
    }

    void setCeiling (float ceilingDbValue) noexcept
    {
        clipper1x.setThreshold (ceilingDbValue);
        clipper2x.setCeiling (ceilingDbValue);
        clipper4x.setCeiling (ceilingDbValue);
        clipper8x.setCeiling (ceilingDbValue);
        clipper16x.setCeiling (ceilingDbValue);
    }

    void setClippingType (ClippingType clippingType) noexcept
    {
        clipper1x.setClippingType (clippingType);
        clipper2x.setClippingType (clippingType);
        clipper4x.setClippingType (clippingType);
        clipper8x.setClippingType (clippingType);
        clipper16x.setClippingType (clippingType);
    }

    void setOversamplingRate (OversamplingRate oversamplingRate) noexcept
    {
        switch (oversamplingRate)
        {
            case X1:
                clipper = &clipper1x;
                break;
            case X2:
                clipper = &clipper2x;
                break;
            case X4:
                clipper = &clipper4x;
                break;
            case X8:
                clipper = &clipper8x;
                break;
            case X16:
                clipper = &clipper16x;
                break;
            default:
                clipper = &clipper1x;
                break;
        }
    }

private:
    /** Default values */
    constexpr float DEFAULT_INPUT_GAIN = 0.0f;
    constexpr float DEFAULT_OUTPUT_GAIN = 0.0f;
    constexpr float RAMP_DURATION_SEC = 0.1f;

    /** DSP */
    juce::dsp::Gain<SampleType> inputGain;
    juce::dsp::Gain<SampleType> outputGain;
    Clipper<SampleType> clipper1x;
    OversampledClipper<SampleType> clipper2x;
    OversampledClipper<SampleType> clipper4x;
    OversampledClipper<SampleType> clipper8x;
    OversampledClipper<SampleType> clipper16x;

    juce::dsp::ProcessorBase* clipper; // Current processor

    /* Handlers */
    OversampledWaveShaperEventHandler mPostInputGainHandler;
    OversampledWaveShaperEventHandler mPostCeilingHandler;
    OversampledWaveShaperEventHandler mPostOutputGainHandler;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShaperProcessor)
};