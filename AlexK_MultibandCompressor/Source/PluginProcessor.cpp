/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
//This is the constructor for the AudioProcessor
AlexK_MultibandCompressorAudioProcessor::AlexK_MultibandCompressorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    //Parameter casting - retrieves audio parameters from pointers pointing to APVTS parameters and casts them to the correct type to be used in ProcessBlock
    //jasserts are for debugging, Helper functions are for making the code neater
    const auto& params = Params::GetParams();
    auto floatHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName) {
        param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(Params::GetParams().at(paramName)));
        jassert(param != nullptr);
    };
    auto boolHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName) {
        param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(Params::GetParams().at(paramName)));
        jassert(param != nullptr);
    };

    floatHelper(lowBandCompressor.threshold, Params::Names::Threshold_LowBand);
    floatHelper(lowBandCompressor.ratio, Params::Names::Ratio_LowBand);
    floatHelper(lowBandCompressor.upwardsRatio, Params::Names::UpwardsRatio_LowBand);
    floatHelper(lowBandCompressor.attack, Params::Names::Attack_LowBand);
    floatHelper(lowBandCompressor.release, Params::Names::Release_LowBand);
    boolHelper(lowBandCompressor.bypassed, Params::Names::Bypassed_LowBand);
    boolHelper(lowBandCompressor.mute, Params::Names::Mute_LowBand);
    boolHelper(lowBandCompressor.solo, Params::Names::Solo_LowBand);

    floatHelper(midBandCompressor.threshold, Params::Names::Threshold_MidBand);
    floatHelper(midBandCompressor.ratio, Params::Names::Ratio_MidBand);
    floatHelper(midBandCompressor.upwardsRatio, Params::Names::UpwardsRatio_MidBand);
    floatHelper(midBandCompressor.attack, Params::Names::Attack_MidBand);
    floatHelper(midBandCompressor.release, Params::Names::Release_MidBand);
    boolHelper(midBandCompressor.bypassed, Params::Names::Bypassed_MidBand);
    boolHelper(midBandCompressor.mute, Params::Names::Mute_MidBand);
    boolHelper(midBandCompressor.solo, Params::Names::Solo_MidBand);

    floatHelper(highBandCompressor.threshold, Params::Names::Threshold_HighBand);
    floatHelper(highBandCompressor.ratio, Params::Names::Ratio_HighBand);
    floatHelper(highBandCompressor.upwardsRatio, Params::Names::UpwardsRatio_HighBand);
    floatHelper(highBandCompressor.attack, Params::Names::Attack_HighBand);
    floatHelper(highBandCompressor.release, Params::Names::Release_HighBand);
    boolHelper(highBandCompressor.bypassed, Params::Names::Bypassed_HighBand);
    boolHelper(highBandCompressor.mute, Params::Names::Mute_HighBand);
    boolHelper(highBandCompressor.solo, Params::Names::Solo_HighBand);

    floatHelper(lowMidCrossover, Params::Names::LowMidCrossoverFreq);
    floatHelper(midHighCrossover, Params::Names::MidHighCrossoverFreq);

    floatHelper(gainInValue, Params::Names::Gain_In);
    floatHelper(gainOutValue, Params::Names::Gain_Out);

    lowPass1.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    allPassForLowBand.setType(juce::dsp::LinkwitzRileyFilterType::allpass);

    highPass1.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    lowPass2.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    highPass2.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
}

AlexK_MultibandCompressorAudioProcessor::~AlexK_MultibandCompressorAudioProcessor()
{
}

//==============================================================================
const juce::String AlexK_MultibandCompressorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AlexK_MultibandCompressorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AlexK_MultibandCompressorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AlexK_MultibandCompressorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AlexK_MultibandCompressorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AlexK_MultibandCompressorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AlexK_MultibandCompressorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AlexK_MultibandCompressorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AlexK_MultibandCompressorAudioProcessor::getProgramName (int index)
{
    return {};
}

void AlexK_MultibandCompressorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AlexK_MultibandCompressorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback initialisation that you need..
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;

    for (auto& compressor : compressors) {
        compressor.prepare(spec);
    }

    lowPass1.prepare(spec);
    allPassForLowBand.prepare(spec);

    highPass1.prepare(spec);
    lowPass2.prepare(spec);
    highPass2.prepare(spec);

    gainIn.prepare(spec);
    gainOut.prepare(spec);

    gainIn.setRampDurationSeconds(0.05);
    gainOut.setRampDurationSeconds(0.05);

    //Prepare filter buffers such that they aren't allocated when the input buffers are copied into them (???)
    for (auto& buffer : filterBuffers) {
        buffer.setSize(spec.numChannels, samplesPerBlock);
    }
}

void AlexK_MultibandCompressorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AlexK_MultibandCompressorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void AlexK_MultibandCompressorAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    //Clear any unused output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    //Check if all bands are bypassed
    bool allBandsBypassed = lowBandCompressor.bypassed->get() &&
        midBandCompressor.bypassed->get() &&
        highBandCompressor.bypassed->get();

    //Set gain values
    gainIn.setGainDecibels(gainInValue->get());
    gainOut.setGainDecibels(gainOutValue->get());

    //Apply input gain
    applyGain(buffer, gainIn);

    //If all bands are bypassed, skip all filtering and compression
    if (allBandsBypassed) {
        applyGain(buffer, gainOut);
        return;
    }

    //Update compressor settings
    for (auto& compressor : compressors) {
        compressor.updateSettings();
    }

    //Update filter cutoff frequencies
    auto lowMidCutoff = lowMidCrossover->get();
    lowPass1.setCutoffFrequency(lowMidCutoff);
    highPass1.setCutoffFrequency(lowMidCutoff);

    auto midHighCutoff = midHighCrossover->get();
    allPassForLowBand.setCutoffFrequency(midHighCutoff);
    lowPass2.setCutoffFrequency(midHighCutoff);
    highPass2.setCutoffFrequency(midHighCutoff);

    //Copy input buffer to filter buffers
    for (auto& fb : filterBuffers) {
        fb = buffer;
    }

    //Create audio blocks and contexts for filters
    auto fb0Block = juce::dsp::AudioBlock<float>(filterBuffers[0]);
    auto fb1Block = juce::dsp::AudioBlock<float>(filterBuffers[1]);
    auto fb2Block = juce::dsp::AudioBlock<float>(filterBuffers[2]);

    auto fb0Context = juce::dsp::ProcessContextReplacing<float>(fb0Block);
    auto fb1Context = juce::dsp::ProcessContextReplacing<float>(fb1Block);
    auto fb2Context = juce::dsp::ProcessContextReplacing<float>(fb2Block);

    //Apply filtering
    lowPass1.process(fb0Context);
    allPassForLowBand.process(fb0Context);

    highPass1.process(fb1Context);
    lowPass2.process(fb1Context);

    highPass1.process(fb2Context);
    highPass2.process(fb2Context);

    //Apply compression to each band
    for (int i = 0; i < filterBuffers.size(); ++i) {
        compressors[i].process(filterBuffers[i]);
    }

    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();

    //Clear the input buffer before recombining
    buffer.clear();

    //Helper function for adding filter bands back to the input buffer
    auto addFilterBand = [numChannels, numSamples](auto& inputBuffer, const auto& source) {
            for (int i = 0; i < numChannels; ++i) {
                inputBuffer.addFrom(i, 0, source, i, 0, numSamples);
            }
        };

    //Logic for soloed/muted bands: add only the soloed filter band to the buffer if one is soloed, else, add the filter band to the buffer if it's not muted
    int bandSoloed = -1;
    for (int i = 0; i < compressors.size(); ++i) {
        if (compressors[i].solo->get()) {
            bandSoloed = i;
            break;
        }
    }

    //Mix bands back together
    if (bandSoloed != -1) {
        addFilterBand(buffer, filterBuffers[bandSoloed]);
    } else {
        for (int i = 0; i < compressors.size(); ++i) {
            if (!compressors[i].mute->get()) {
                addFilterBand(buffer, filterBuffers[i]);
            }
        }
    }

    //Apply output gain
    applyGain(buffer, gainOut);
}

//==============================================================================
bool AlexK_MultibandCompressorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AlexK_MultibandCompressorAudioProcessor::createEditor()
{
    //custom GUI editor
    //return new AlexK_MultibandCompressorAudioProcessorEditor (*this);

    //automatically makes basic gui for each audio parameter
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
//next two functions save AudioProcessor state using APVTS
void AlexK_MultibandCompressorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void AlexK_MultibandCompressorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid()) {
        apvts.replaceState(tree);
    }
}

//Provides layout of audio parameters
juce::AudioProcessorValueTreeState::ParameterLayout AlexK_MultibandCompressorAudioProcessor::createParameterLayout() {
    APVTS::ParameterLayout layout;
    auto attackReleaseRange = juce::NormalisableRange<float>(5, 500, 1, 1);

    layout.add(std::make_unique<juce::AudioParameterFloat>(Params::GetParams().at(Params::Names::Gain_In), Params::GetParams().at(Params::Names::Gain_In), juce::NormalisableRange<float>(-24.f, 24.f, 0.f, 1.f), 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(Params::GetParams().at(Params::Names::Gain_Out), Params::GetParams().at(Params::Names::Gain_Out), juce::NormalisableRange<float>(-24.f, 24.f, 0.f, 1.f), 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(Params::GetParams().at(Params::Names::LowMidCrossoverFreq), Params::GetParams().at(Params::Names::LowMidCrossoverFreq), juce::NormalisableRange<float>(20, 999, 1, 1), 500));
    layout.add(std::make_unique<juce::AudioParameterFloat>(Params::GetParams().at(Params::Names::MidHighCrossoverFreq), Params::GetParams().at(Params::Names::MidHighCrossoverFreq), juce::NormalisableRange<float>(1000, 20000, 1, 1), 3000));

    layout.add(std::make_unique<juce::AudioParameterFloat>(Params::GetParams().at(Params::Names::Threshold_LowBand), Params::GetParams().at(Params::Names::Threshold_LowBand), juce::NormalisableRange<float>(-60, 12, 1, 1), 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(Params::GetParams().at(Params::Names::Ratio_LowBand), Params::GetParams().at(Params::Names::Ratio_LowBand), juce::NormalisableRange<float>(1, 100, 0, 0.3), 1.0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(Params::GetParams().at(Params::Names::UpwardsRatio_LowBand), Params::GetParams().at(Params::Names::UpwardsRatio_LowBand), juce::NormalisableRange<float>(1, 100, 0, 0.3), 1.0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(Params::GetParams().at(Params::Names::Attack_LowBand), Params::GetParams().at(Params::Names::Attack_LowBand), attackReleaseRange, 50));
    layout.add(std::make_unique<juce::AudioParameterFloat>(Params::GetParams().at(Params::Names::Release_LowBand), Params::GetParams().at(Params::Names::Release_LowBand), attackReleaseRange, 250));
    layout.add(std::make_unique<juce::AudioParameterBool>(Params::GetParams().at(Params::Names::Bypassed_LowBand), Params::GetParams().at(Params::Names::Bypassed_LowBand), false));
    layout.add(std::make_unique<juce::AudioParameterBool>(Params::GetParams().at(Params::Names::Mute_LowBand), Params::GetParams().at(Params::Names::Mute_LowBand), false));
    layout.add(std::make_unique<juce::AudioParameterBool>(Params::GetParams().at(Params::Names::Solo_LowBand), Params::GetParams().at(Params::Names::Solo_LowBand), false));

    layout.add(std::make_unique<juce::AudioParameterFloat>(Params::GetParams().at(Params::Names::Threshold_MidBand), Params::GetParams().at(Params::Names::Threshold_MidBand), juce::NormalisableRange<float>(-60, 12, 1, 1), 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(Params::GetParams().at(Params::Names::Ratio_MidBand), Params::GetParams().at(Params::Names::Ratio_MidBand), juce::NormalisableRange<float>(1, 100, 0, 0.3), 1.0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(Params::GetParams().at(Params::Names::UpwardsRatio_MidBand), Params::GetParams().at(Params::Names::UpwardsRatio_MidBand), juce::NormalisableRange<float>(1, 100, 0, 0.3), 1.0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(Params::GetParams().at(Params::Names::Attack_MidBand), Params::GetParams().at(Params::Names::Attack_MidBand), attackReleaseRange, 50));
    layout.add(std::make_unique<juce::AudioParameterFloat>(Params::GetParams().at(Params::Names::Release_MidBand), Params::GetParams().at(Params::Names::Release_MidBand), attackReleaseRange, 250));
    layout.add(std::make_unique<juce::AudioParameterBool>(Params::GetParams().at(Params::Names::Bypassed_MidBand), Params::GetParams().at(Params::Names::Bypassed_MidBand), false));
    layout.add(std::make_unique<juce::AudioParameterBool>(Params::GetParams().at(Params::Names::Mute_MidBand), Params::GetParams().at(Params::Names::Mute_MidBand), false));
    layout.add(std::make_unique<juce::AudioParameterBool>(Params::GetParams().at(Params::Names::Solo_MidBand), Params::GetParams().at(Params::Names::Solo_MidBand), false));

    layout.add(std::make_unique<juce::AudioParameterFloat>(Params::GetParams().at(Params::Names::Threshold_HighBand), Params::GetParams().at(Params::Names::Threshold_HighBand), juce::NormalisableRange<float>(-60, 12, 1, 1), 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(Params::GetParams().at(Params::Names::Ratio_HighBand), Params::GetParams().at(Params::Names::Ratio_HighBand), juce::NormalisableRange<float>(1, 100, 0, 0.3), 1.0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(Params::GetParams().at(Params::Names::UpwardsRatio_HighBand), Params::GetParams().at(Params::Names::UpwardsRatio_HighBand), juce::NormalisableRange<float>(1, 100, 0, 0.3), 1.0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(Params::GetParams().at(Params::Names::Attack_HighBand), Params::GetParams().at(Params::Names::Attack_HighBand), attackReleaseRange, 50));
    layout.add(std::make_unique<juce::AudioParameterFloat>(Params::GetParams().at(Params::Names::Release_HighBand), Params::GetParams().at(Params::Names::Release_HighBand), attackReleaseRange, 250));
    layout.add(std::make_unique<juce::AudioParameterBool>(Params::GetParams().at(Params::Names::Bypassed_HighBand), Params::GetParams().at(Params::Names::Bypassed_HighBand), false));
    layout.add(std::make_unique<juce::AudioParameterBool>(Params::GetParams().at(Params::Names::Mute_HighBand), Params::GetParams().at(Params::Names::Mute_HighBand), false));
    layout.add(std::make_unique<juce::AudioParameterBool>(Params::GetParams().at(Params::Names::Solo_HighBand), Params::GetParams().at(Params::Names::Solo_HighBand), false));
    
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AlexK_MultibandCompressorAudioProcessor();
}
