#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <cmath>

namespace teared
{
constexpr int numBands = 8;

enum BandType { Bell = 0, LowShelf, HighShelf, LowCut, HighCut, Notch };

inline const juce::StringArray& typeNames()
{
    static const juce::StringArray n { "Bell", "Low Shelf", "High Shelf", "Low Cut", "High Cut", "Notch" };
    return n;
}

inline juce::String paramId (int band, const juce::String& name)
{
    return "b" + juce::String (band + 1) + "_" + name;
}

struct BandSettings
{
    bool  on   = false;
    int   type = Bell;
    float freq = 1000.0f;
    float gain = 0.0f;
    float q    = 1.0f;

    bool usesGain() const { return type == Bell || type == LowShelf || type == HighShelf; }

    bool operator== (const BandSettings& o) const
    {
        return on == o.on && type == o.type && freq == o.freq && gain == o.gain && q == o.q;
    }
    bool operator!= (const BandSettings& o) const { return ! (*this == o); }
};

using Coeffs = juce::dsp::IIR::Coefficients<float>;

inline Coeffs::Ptr makeCoefficients (const BandSettings& s, double sr)
{
    const float f = juce::jlimit (10.0f, (float) (sr * 0.49), s.freq);
    const float q = juce::jmax (0.05f, s.q);
    const float g = juce::Decibels::decibelsToGain (s.gain);

    switch (s.type)
    {
        case LowShelf:  return Coeffs::makeLowShelf  (sr, f, q, g);
        case HighShelf: return Coeffs::makeHighShelf (sr, f, q, g);
        case LowCut:    return Coeffs::makeHighPass  (sr, f, q);
        case HighCut:   return Coeffs::makeLowPass   (sr, f, q);
        case Notch:     return Coeffs::makeNotch     (sr, f, q);
        case Bell:
        default:        return Coeffs::makePeakFilter (sr, f, q, g);
    }
}

struct BandDefault { int type; float freq; float q; bool on; };

inline const std::array<BandDefault, numBands>& bandDefaults()
{
    static const std::array<BandDefault, numBands> d {{
        { LowCut,    30.0f,    0.71f, false },
        { LowShelf,  100.0f,   0.71f, true  },
        { Bell,      250.0f,   1.0f,  true  },
        { Bell,      650.0f,   1.0f,  true  },
        { Bell,      1600.0f,  1.0f,  true  },
        { Bell,      4000.0f,  1.0f,  true  },
        { HighShelf, 9000.0f,  0.71f, true  },
        { HighCut,   18000.0f, 0.71f, false }
    }};
    return d;
}

inline juce::NormalisableRange<float> logRange (float lo, float hi)
{
    return juce::NormalisableRange<float> (
        lo, hi,
        [] (float s, float e, float p) { return s * std::pow (e / s, p); },
        [] (float s, float e, float v) { return std::log (v / s) / std::log (e / s); },
        [] (float s, float e, float v) { return juce::jlimit (s, e, v); });
}

inline juce::String formatFreq (float v)
{
    if (v >= 1000.0f) return juce::String (v / 1000.0f, 2) + " kHz";
    return juce::String (v, v < 100.0f ? 1 : 0) + " Hz";
}

inline float parseFreq (const juce::String& t)
{
    float v = t.getFloatValue();
    if (t.containsIgnoreCase ("k")) v *= 1000.0f;
    return v;
}

inline juce::String formatDb (float v)
{
    return (v > 0.005f ? "+" : "") + juce::String (v, 1) + " dB";
}

inline juce::AudioProcessorValueTreeState::ParameterLayout createLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    for (int b = 0; b < numBands; ++b)
    {
        const auto d = bandDefaults()[(size_t) b];
        const auto n = "Band " + juce::String (b + 1) + " ";

        layout.add (std::make_unique<juce::AudioParameterBool> (
            juce::ParameterID { paramId (b, "on"), 1 }, n + "On", d.on));

        layout.add (std::make_unique<juce::AudioParameterChoice> (
            juce::ParameterID { paramId (b, "type"), 1 }, n + "Type", typeNames(), d.type));

        layout.add (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { paramId (b, "freq"), 1 }, n + "Freq", logRange (20.0f, 20000.0f), d.freq,
            juce::AudioParameterFloatAttributes()
                .withStringFromValueFunction ([] (float v, int) { return formatFreq (v); })
                .withValueFromStringFunction ([] (const juce::String& t) { return parseFreq (t); })));

        layout.add (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { paramId (b, "gain"), 1 }, n + "Gain",
            juce::NormalisableRange<float> (-18.0f, 18.0f, 0.01f), 0.0f,
            juce::AudioParameterFloatAttributes()
                .withStringFromValueFunction ([] (float v, int) { return formatDb (v); })
                .withValueFromStringFunction ([] (const juce::String& t) { return t.getFloatValue(); })));

        layout.add (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { paramId (b, "q"), 1 }, n + "Q", logRange (0.1f, 18.0f), d.q,
            juce::AudioParameterFloatAttributes()
                .withStringFromValueFunction ([] (float v, int) { return juce::String (v, 2); })
                .withValueFromStringFunction ([] (const juce::String& t) { return t.getFloatValue(); })));
    }

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "out", 1 }, "Output",
        juce::NormalisableRange<float> (-24.0f, 24.0f, 0.01f), 0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction ([] (float v, int) { return formatDb (v); })
            .withValueFromStringFunction ([] (const juce::String& t) { return t.getFloatValue(); })));

    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { "analyzer", 1 }, "Analyzer", true));

    return layout;
}
} // namespace teared
