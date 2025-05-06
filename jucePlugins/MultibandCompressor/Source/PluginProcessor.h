/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "UpDownCompressor.h"

//For looking up audio parameters more easily
namespace Params {
    enum Names {
        LowMidCrossoverFreq,
        MidHighCrossoverFreq,

        Threshold_LowBand,
        Threshold_MidBand,
        Threshold_HighBand,

        Ratio_LowBand,
        Ratio_MidBand,
        Ratio_HighBand,

        UpwardsRatio_LowBand,
        UpwardsRatio_MidBand,
        UpwardsRatio_HighBand,

        Attack_LowBand,
        Attack_MidBand,
        Attack_HighBand,

        Release_LowBand,
        Release_MidBand,
        Release_HighBand,

        Bypassed_LowBand,
        Bypassed_MidBand,
        Bypassed_HighBand,

        Mute_LowBand,
        Mute_MidBand,
        Mute_HighBand,

        Solo_LowBand,
        Solo_MidBand,
        Solo_HighBand,

        Gain_In,
        Gain_Out
    };

    inline const std::map<Names, juce::String>& GetParams() {
        static std::map<Names, juce::String> params = {
            {LowMidCrossoverFreq, "Low-Mid Crossover Freq"},
            {MidHighCrossoverFreq, "Mid-High Crossover Freq"},

            {Threshold_LowBand, "Threshold Low Band"},
            {Threshold_MidBand, "Threshold Mid Band"},
            {Threshold_HighBand, "Threshold High Band"},

            {Ratio_LowBand, "Downwards Compression Ratio Low Band"},
            {Ratio_MidBand, "Downwards Compression Ratio Mid Band"},
            {Ratio_HighBand, "Downwards Compression Ratio High Band"},

            {UpwardsRatio_LowBand, "Upwards Compression Ratio Low Band"},
            {UpwardsRatio_MidBand, "Upwards Compression Ratio Mid Band"},
            {UpwardsRatio_HighBand, "Upwards Compression Ratio High Band"},

            {Attack_LowBand, "Attack Low Band"},
            {Attack_MidBand, "Attack Mid Band"},
            {Attack_HighBand, "Attack High Band"},

            {Release_LowBand, "Release Low Band"},
            {Release_MidBand, "Release Mid Band"},
            {Release_HighBand, "Release High Band"},

            {Bypassed_LowBand, "Bypassed Low Band"},
            {Bypassed_MidBand, "Bypassed Mid Band"},
            {Bypassed_HighBand, "Bypassed High Band"},

            {Mute_LowBand, "Mute Low Band"},
            {Mute_MidBand, "Mute Mid Band"},
            {Mute_HighBand, "Mute High Band"},

            {Solo_LowBand, "Solo Low Band"},
            {Solo_MidBand, "Solo Mid Band"},
            {Solo_HighBand, "Solo High Band"},

            {Gain_In, "Gain In"},
            {Gain_Out, "Gain Out"}
        };

        return params;
    }
}

//Cleaner code for compressors
struct CompressorBand {
public:
    juce::AudioParameterFloat* threshold{ nullptr };
    juce::AudioParameterFloat* ratio{ nullptr };
    juce::AudioParameterFloat* upwardsRatio{ nullptr };
    juce::AudioParameterFloat* attack{ nullptr };
    juce::AudioParameterFloat* release{ nullptr };
    juce::AudioParameterBool* bypassed{ nullptr };
    juce::AudioParameterBool* mute{ nullptr };
    juce::AudioParameterBool* solo{ nullptr };

    void prepare(const juce::dsp::ProcessSpec& spec) {
        compressor.prepare(spec);
    }

    //Sets parameters of compressor as the public variables in constructor
    void updateSettings() {
        compressor.setThreshold(threshold->get());
        compressor.setAttack(attack->get());
        compressor.setRelease(release->get());
        compressor.setRatio(ratio->get());
        compressor.setUpwardsRatio(upwardsRatio->get());
    }

    //Creates AudioBlock that points to data in the buffer parameter, then uses it to create a variable storing context info, which is needed for the compressor to process it
    void process(juce::AudioBuffer<float>& buffer) {
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto context = juce::dsp::ProcessContextReplacing<float>(block);
        context.isBypassed = bypassed->get();
        compressor.process(context);
    }
private:
    juce::dsp::UpDownCompressor<float> compressor;
};

//==============================================================================
/**
*/
class AlexK_MultibandCompressorAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    AlexK_MultibandCompressorAudioProcessor();
    ~AlexK_MultibandCompressorAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //APVTS synchronizes audio parameters with GUI by managing the AudioProcessor's entire state
    using APVTS = juce::AudioProcessorValueTreeState;
    static APVTS::ParameterLayout createParameterLayout();

    APVTS apvts{ *this, nullptr, "Parameters", createParameterLayout() };

private:
    //1 compressor for each band
    std::array<CompressorBand, 3> compressors;
    CompressorBand& lowBandCompressor = compressors[0];
    CompressorBand& midBandCompressor = compressors[1];
    CompressorBand& highBandCompressor = compressors[2];

    //lowPass1 and highPass1 are used to split the audio into 2 bands with their crossover freqs being lowMidCrossover
    //The higher band is split into 2 more bands, a mid and a high band, by lowPass2 and highPass2, with their crossover freqs being midHighCrossover
    //The lowest band (made by lowPass1 and highPass1) is processed by allPassForLowBand with its crossover freq being midHighCrossover
    juce::dsp::LinkwitzRileyFilter<float> lowPass1, highPass1,
        allPassForLowBand, lowPass2, highPass2;
    juce::AudioParameterFloat* lowMidCrossover{ nullptr };
    juce::AudioParameterFloat* midHighCrossover{ nullptr };

    std::array<juce::AudioBuffer<float>, 3> filterBuffers;

    juce::dsp::Gain<float> gainIn, gainOut;
    juce::AudioParameterFloat* gainInValue{ nullptr };
    juce::AudioParameterFloat* gainOutValue{ nullptr };

    template<typename T, typename U>
    void applyGain(T& buffer, U& gain) {
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto context = juce::dsp::ProcessContextReplacing<float>(block);
        gain.process(context);
    }
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AlexK_MultibandCompressorAudioProcessor)
};
