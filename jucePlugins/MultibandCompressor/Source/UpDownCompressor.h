/*
  ==============================================================================

    UpDownCompressor.h
    Created: 30 Apr 2025 1:19:52pm
    Author:  alexk
    I copied from juce_Compressor.h, the header file for the DSP compressor, and made changes such that both upwards and downwards compression can be applied.

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace juce::dsp
{

    template <typename SampleType>
    class UpDownCompressor
    {
    public:
        //==============================================================================
        /** Constructor. */
        UpDownCompressor();

        //==============================================================================
        /** Sets the threshold in dB of the compressor.*/
        void setThreshold(SampleType newThreshold);

        /** Sets the downwards compression ratio of the compressor (must be higher or equal to 1).*/
        void setRatio(SampleType newRatio);

        /** Sets the upwards compression ratio of the compressor (must be higher or equal to 1).*/
        void setUpwardsRatio(SampleType newUpwardsRatio);

        /** Sets the attack time in milliseconds of the compressor.*/
        void setAttack(SampleType newAttack);

        /** Sets the release time in milliseconds of the compressor.*/
        void setRelease(SampleType newRelease);

        //==============================================================================
        /** Initialises the processor. */
        void prepare(const ProcessSpec& spec);

        /** Resets the internal state variables of the processor. */
        void reset();

        //==============================================================================
        /** Processes the input and output samples supplied in the processing context. */
        template <typename ProcessContext>
        void process(const ProcessContext& context) noexcept
        {
            const auto& inputBlock = context.getInputBlock();
            auto& outputBlock = context.getOutputBlock();
            const auto numChannels = outputBlock.getNumChannels();
            const auto numSamples = outputBlock.getNumSamples();

            jassert(inputBlock.getNumChannels() == numChannels);
            jassert(inputBlock.getNumSamples() == numSamples);

            if (context.isBypassed)
            {
                outputBlock.copyFrom(inputBlock);
                return;
            }

            for (size_t channel = 0; channel < numChannels; ++channel)
            {
                auto* inputSamples = inputBlock.getChannelPointer(channel);
                auto* outputSamples = outputBlock.getChannelPointer(channel);

                for (size_t i = 0; i < numSamples; ++i)
                    outputSamples[i] = processSample((int)channel, inputSamples[i]);
            }
        }

        /** Performs the processing operation on a single sample at a time. */
        SampleType processSample(int channel, SampleType inputValue);

    private:
        //==============================================================================
        void update();

        //==============================================================================
        SampleType threshold, thresholdInverse, ratioInverse, upwardsRatioInverse;
        BallisticsFilter<SampleType> envelopeFilter;

        double sampleRate = 44100.0;
        SampleType thresholddB = 0.0, ratio = 1.0, upwardsRatio = 1.0, attackTime = 1.0, releaseTime = 100.0;
    };

} // namespace juce::dsp
