/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class AlexK_MultibandCompressorAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    AlexK_MultibandCompressorAudioProcessorEditor (AlexK_MultibandCompressorAudioProcessor&);
    ~AlexK_MultibandCompressorAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AlexK_MultibandCompressorAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AlexK_MultibandCompressorAudioProcessorEditor)
};
