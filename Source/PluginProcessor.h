#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <atomic>
#include "EQParams.h"

class TearedEQAudioProcessor : public juce::AudioProcessor
{
public:
    static constexpr int scopeSize = 8192; // power of two

    TearedEQAudioProcessor();
    ~TearedEQAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "TearedEQ"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Used by the editor
    teared::BandSettings readBand (int band) const;
    bool isAnalyzerOn() const { return pAnalyzer != nullptr && pAnalyzer->load() > 0.5f; }
    void copyScope (float* dest, int numSamples) const;
    double getCurrentSampleRate() const { return currentSampleRate; }

    juce::AudioProcessorValueTreeState apvts;

private:
    using Filter = juce::dsp::IIR::Filter<float>;

    std::array<std::array<Filter, 2>, teared::numBands> filters;
    std::array<teared::BandSettings, teared::numBands> lastSettings;
    std::array<bool, teared::numBands> haveLast {};

    std::array<std::atomic<float>*, teared::numBands> pOn {}, pType {}, pFreq {}, pGain {}, pQ {};
    std::atomic<float>* pOut = nullptr;
    std::atomic<float>* pAnalyzer = nullptr;

    float lastOutGain = 1.0f;
    double currentSampleRate = 44100.0;

    std::array<float, scopeSize> scope {};
    std::atomic<int> scopeWrite { 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TearedEQAudioProcessor)
};
