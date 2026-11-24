#include "PluginProcessor.h"
#include "PluginEditor.h"

TearedEQAudioProcessor::TearedEQAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "TearedEQ", teared::createLayout())
{
    for (int b = 0; b < teared::numBands; ++b)
    {
        pOn[(size_t) b]   = apvts.getRawParameterValue (teared::paramId (b, "on"));
        pType[(size_t) b] = apvts.getRawParameterValue (teared::paramId (b, "type"));
        pFreq[(size_t) b] = apvts.getRawParameterValue (teared::paramId (b, "freq"));
        pGain[(size_t) b] = apvts.getRawParameterValue (teared::paramId (b, "gain"));
        pQ[(size_t) b]    = apvts.getRawParameterValue (teared::paramId (b, "q"));
    }
    pOut      = apvts.getRawParameterValue ("out");
    pAnalyzer = apvts.getRawParameterValue ("analyzer");
}

teared::BandSettings TearedEQAudioProcessor::readBand (int b) const
{
    teared::BandSettings s;
    const auto i = (size_t) b;
    s.on   = pOn[i]->load() > 0.5f;
    s.type = (int) (pType[i]->load() + 0.5f);
    s.freq = pFreq[i]->load();
    s.gain = pGain[i]->load();
    s.q    = pQ[i]->load();
    return s;
}

void TearedEQAudioProcessor::prepareToPlay (double sampleRate, int)
{
    currentSampleRate = sampleRate;

    teared::BandSettings init;
    auto c = teared::makeCoefficients (init, sampleRate);

    for (auto& band : filters)
        for (auto& f : band)
        {
            f.coefficients = c;
            f.reset();
        }

    haveLast.fill (false);
    lastOutGain = juce::Decibels::decibelsToGain (pOut->load());
    scope.fill (0.0f);
}

bool TearedEQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& out = layouts.getMainOutputChannelSet();
    if (out != juce::AudioChannelSet::mono() && out != juce::AudioChannelSet::stereo())
        return false;
    return out == layouts.getMainInputChannelSet();
}

void TearedEQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numCh      = juce::jmin (buffer.getNumChannels(), 2);
    const int numSamples = buffer.getNumSamples();

    for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear (i, 0, numSamples);

    juce::dsp::AudioBlock<float> block (buffer);

    for (int b = 0; b < teared::numBands; ++b)
    {
        const auto i = (size_t) b;
        const auto s = readBand (b);

        if (! haveLast[i] || s != lastSettings[i])
        {
            auto coeffs = teared::makeCoefficients (s, currentSampleRate);
            const bool toggled = ! haveLast[i] || s.on != lastSettings[i].on;

            for (auto& f : filters[i])
            {
                f.coefficients = coeffs;
                if (toggled) f.reset();
            }
            lastSettings[i] = s;
            haveLast[i] = true;
        }

        if (! s.on)
            continue;

        for (int ch = 0; ch < numCh; ++ch)
        {
            auto chBlock = block.getSingleChannelBlock ((size_t) ch);
            juce::dsp::ProcessContextReplacing<float> ctx (chBlock);
            filters[i][(size_t) ch].process (ctx);
        }
    }

    // Output gain (ramped to avoid zipper noise)
    const float g = juce::Decibels::decibelsToGain (pOut->load());
    if (std::abs (g - lastOutGain) > 1.0e-6f)
        buffer.applyGainRamp (0, numSamples, lastOutGain, g);
    else if (g != 1.0f)
        buffer.applyGain (g);
    lastOutGain = g;

    // Feed the spectrum analyser
    if (isAnalyzerOn() && numCh > 0)
    {
        int w = scopeWrite.load (std::memory_order_relaxed);
        const float inv = 1.0f / (float) numCh;
        for (int n = 0; n < numSamples; ++n)
        {
            float m = 0.0f;
            for (int ch = 0; ch < numCh; ++ch)
                m += buffer.getReadPointer (ch)[n];
            scope[(size_t) w] = m * inv;
            w = (w + 1) & (scopeSize - 1);
        }
        scopeWrite.store (w, std::memory_order_release);
    }
}

void TearedEQAudioProcessor::copyScope (float* dest, int n) const
{
    n = juce::jmin (n, scopeSize);
    const int w = scopeWrite.load (std::memory_order_acquire);
    for (int i = 0; i < n; ++i)
        dest[i] = scope[(size_t) ((w - n + i + scopeSize) & (scopeSize - 1))];
}

juce::AudioProcessorEditor* TearedEQAudioProcessor::createEditor()
{
    return new TearedEQAudioProcessorEditor (*this);
}

void TearedEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, destData);
}

void TearedEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TearedEQAudioProcessor();
}
