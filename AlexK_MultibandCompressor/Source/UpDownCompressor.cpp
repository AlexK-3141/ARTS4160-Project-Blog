/*
  ==============================================================================

    UpDownCompressor.cpp
    Created: 30 Apr 2025 1:19:52pm
    Author:  alexk

  ==============================================================================
*/

#include "UpDownCompressor.h"
namespace juce::dsp
{

    //==============================================================================
    template <typename SampleType>
    UpDownCompressor<SampleType>::UpDownCompressor()
    {
        update();
    }

    //==============================================================================
    template <typename SampleType>
    void UpDownCompressor<SampleType>::setThreshold(SampleType newThreshold)
    {
        thresholddB = newThreshold;
        update();
    }

    template <typename SampleType>
    void UpDownCompressor<SampleType>::setRatio(SampleType newRatio)
    {
        jassert(newRatio >= static_cast<SampleType> (1.0));

        ratio = newRatio;
        update();
    }

    template <typename SampleType>
    void UpDownCompressor<SampleType>::setUpwardsRatio(SampleType newUpwardsRatio)
    {
        jassert(newUpwardsRatio >= static_cast<SampleType> (1.0));

        upwardsRatio = newUpwardsRatio;
        update();
    }

    template <typename SampleType>
    void UpDownCompressor<SampleType>::setAttack(SampleType newAttack)
    {
        attackTime = newAttack;
        update();
    }

    template <typename SampleType>
    void UpDownCompressor<SampleType>::setRelease(SampleType newRelease)
    {
        releaseTime = newRelease;
        update();
    }

    //==============================================================================
    template <typename SampleType>
    void UpDownCompressor<SampleType>::prepare(const ProcessSpec& spec)
    {
        jassert(spec.sampleRate > 0);
        jassert(spec.numChannels > 0);

        sampleRate = spec.sampleRate;

        envelopeFilter.prepare(spec);

        update();
        reset();
    }

    template <typename SampleType>
    void UpDownCompressor<SampleType>::reset()
    {
        envelopeFilter.reset();
    }

    //==============================================================================
    template <typename SampleType>
    SampleType UpDownCompressor<SampleType>::processSample(int channel, SampleType inputValue)
    {
        // Ballistics filter with peak rectifier
        auto env = envelopeFilter.processSample(channel, inputValue);

        // VCA - upwards and downwards compression
        auto gain = (env < threshold)
            ? std::pow(env * thresholdInverse, upwardsRatioInverse - static_cast<SampleType>(1.0))
            : std::pow(env * thresholdInverse, ratioInverse - static_cast<SampleType>(1.0));

        // Output
        return gain * inputValue;
    }

    template <typename SampleType>
    void UpDownCompressor<SampleType>::update()
    {
        threshold = Decibels::decibelsToGain(thresholddB, static_cast<SampleType>(-200.0));
        thresholdInverse = static_cast<SampleType> (1.0) / threshold;
        ratioInverse = static_cast<SampleType> (1.0) / ratio;
        upwardsRatioInverse = static_cast<SampleType> (1.0) / upwardsRatio;

        envelopeFilter.setAttackTime(attackTime);
        envelopeFilter.setReleaseTime(releaseTime);
    }

    //==============================================================================
    template class UpDownCompressor<float>;
    template class UpDownCompressor<double>;

} // namespace juce::dsp