#pragma once
#include "PluginProcessor.h"

inline juce::Colour bandColour (int i)
{
    static const juce::uint32 c[] = { 0xffff5f6d, 0xffff9f43, 0xfffeca57, 0xff1dd1a1,
                                      0xff48dbfb, 0xff54a0ff, 0xffa55eea, 0xfff368e0 };
    return juce::Colour (c[i & 7]);
}

//==============================================================================
class TearedLookAndFeel : public juce::LookAndFeel_V4
{
public:
    TearedLookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPos, float startAngle, float endAngle, juce::Slider&) override;
    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour&,
                               bool highlighted, bool down) override;
    void drawComboBox (juce::Graphics&, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox&) override;
    juce::Font getTextButtonFont (juce::TextButton&, int) override;
};

//==============================================================================
class TearedEQAudioProcessorEditor : public juce::AudioProcessorEditor,
                                     private juce::Timer
{
public:
    explicit TearedEQAudioProcessorEditor (TearedEQAudioProcessor&);
    ~TearedEQAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void mouseDown (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;
    void mouseUp (const juce::MouseEvent&) override;
    void mouseMove (const juce::MouseEvent&) override;
    void mouseDoubleClick (const juce::MouseEvent&) override;
    void mouseWheelMove (const juce::MouseEvent&, const juce::MouseWheelDetails&) override;

private:
    void timerCallback() override;
    void selectBand (int band);
    void updateSpectrum();
    void drawHeader (juce::Graphics&);
    void drawGraph (juce::Graphics&);

    float freqToX (float f) const;
    float xToFreq (float x) const;
    float gainToY (float g) const;
    float yToGain (float y) const;
    juce::Point<float> nodePosition (const teared::BandSettings&) const;
    int hitTestNode (juce::Point<float>) const;

    void setParam (const juce::String& id, float value);
    void beginGestures (int band);
    void endGestures (int band);

    TearedEQAudioProcessor& proc;
    TearedLookAndFeel laf;

    juce::Rectangle<float> graphArea, panelArea;

    // Band controls
    std::array<juce::TextButton, teared::numBands> chips;
    juce::TextButton onButton { "ON" };
    juce::ComboBox typeBox;
    juce::Slider freqSlider, gainSlider, qSlider, outSlider;
    juce::Label freqLabel, gainLabel, qLabel, outLabel;
    juce::TextButton analyzerButton { "ANALYZER" };

    using SliderAtt = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAtt  = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using ButtonAtt = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::unique_ptr<SliderAtt> freqAtt, gainAtt, qAtt, outAtt;
    std::unique_ptr<ComboAtt>  typeAtt;
    std::unique_ptr<ButtonAtt> onAtt, analyzerAtt;

    int selected = 2;
    int hoverBand = -1;
    int dragBand = -1;
    float dragStartQ = 1.0f, dragStartY = 0.0f;
    std::array<int, teared::numBands> chipState;

    // Spectrum
    static constexpr int fftOrder = 12;
    static constexpr int fftSize  = 1 << fftOrder;
    juce::dsp::FFT fft { fftOrder };
    juce::dsp::WindowingFunction<float> window { (size_t) fftSize, juce::dsp::WindowingFunction<float>::hann };
    std::array<float, fftSize * 2> fftData {};
    std::array<float, fftSize / 2 + 1> binDb {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TearedEQAudioProcessorEditor)
};
