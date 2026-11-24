#include "PluginEditor.h"

using namespace teared;

namespace
{
const juce::uint32 kBg1 = 0xff090b10, kBg2 = 0xff131826, kPanel = 0xff0e1118,
                   kLine = 0xff1f2634, kGrey = 0xff8d97ab, kCyan = 0xff3de0ff, kViolet = 0xffa46bff;
}

//==============================================================================
// Look and feel
//==============================================================================
TearedLookAndFeel::TearedLookAndFeel()
{
    setColour (juce::TextButton::buttonColourId,   juce::Colour (0xff1b2030));
    setColour (juce::TextButton::buttonOnColourId, juce::Colour (kCyan));
    setColour (juce::TextButton::textColourOffId,  juce::Colour (kGrey));
    setColour (juce::TextButton::textColourOnId,   juce::Colour (kBg1));

    setColour (juce::ComboBox::textColourId,       juce::Colours::white);
    setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff1b2030));
    setColour (juce::ComboBox::outlineColourId,    juce::Colour (0xff2c3447));
    setColour (juce::ComboBox::arrowColourId,      juce::Colour (kGrey));

    setColour (juce::PopupMenu::backgroundColourId,            juce::Colour (0xff151922));
    setColour (juce::PopupMenu::textColourId,                  juce::Colours::white);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, juce::Colour (kCyan).withAlpha (0.25f));
    setColour (juce::PopupMenu::highlightedTextColourId,       juce::Colours::white);

    setColour (juce::Slider::textBoxTextColourId,       juce::Colours::white);
    setColour (juce::Slider::textBoxOutlineColourId,    juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxHighlightColourId,  juce::Colour (kCyan).withAlpha (0.4f));

    setColour (juce::Label::textColourId, juce::Colour (kGrey));
}

void TearedLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                          float pos, float startAngle, float endAngle, juce::Slider& s)
{
    auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) width, (float) height).reduced (4.0f);
    const float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const auto centre = bounds.getCentre();
    const float trackW = 4.0f;
    const float arcR = radius - trackW * 0.5f;
    const float angle = startAngle + pos * (endAngle - startAngle);
    const bool enabled = s.isEnabled();
    const auto fill = s.findColour (juce::Slider::rotarySliderFillColourId);

    juce::Path track;
    track.addCentredArc (centre.x, centre.y, arcR, arcR, 0.0f, startAngle, endAngle, true);
    g.setColour (juce::Colour (0xff232a3a));
    g.strokePath (track, juce::PathStrokeType (trackW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    float fromAngle = startAngle;
    if (s.getMinimum() < 0.0 && s.getMaximum() > 0.0)
        fromAngle = startAngle + (float) s.valueToProportionOfLength (0.0) * (endAngle - startAngle);

    juce::Path val;
    val.addCentredArc (centre.x, centre.y, arcR, arcR, 0.0f,
                       juce::jmin (fromAngle, angle), juce::jmax (fromAngle, angle), true);

    if (enabled)
    {
        g.setColour (fill.withAlpha (0.22f));
        g.strokePath (val, juce::PathStrokeType (trackW + 5.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
    g.setColour (enabled ? fill : juce::Colour (0xff3a4359));
    g.strokePath (val, juce::PathStrokeType (trackW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    const float bodyR = arcR - trackW - 5.0f;
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff2e3548), centre.x, centre.y - bodyR,
                                             juce::Colour (0xff141824), centre.x, centre.y + bodyR, false));
    g.fillEllipse (centre.x - bodyR, centre.y - bodyR, bodyR * 2.0f, bodyR * 2.0f);
    g.setColour (juce::Colour (0xff3a4359));
    g.drawEllipse (centre.x - bodyR, centre.y - bodyR, bodyR * 2.0f, bodyR * 2.0f, 1.0f);

    const auto p1 = centre.getPointOnCircumference (bodyR * 0.30f, angle);
    const auto p2 = centre.getPointOnCircumference (bodyR * 0.88f, angle);
    g.setColour (enabled ? juce::Colours::white : juce::Colour (kGrey));
    g.drawLine (juce::Line<float> (p1, p2), 2.5f);
}

void TearedLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& b, const juce::Colour& bg,
                                              bool highlighted, bool down)
{
    auto r = b.getLocalBounds().toFloat().reduced (0.5f);
    auto c = bg;
    if (highlighted) c = c.brighter (0.12f);
    if (down)        c = c.darker (0.12f);
    g.setColour (c);
    g.fillRoundedRectangle (r, 6.0f);
    if (! b.getToggleState())
    {
        g.setColour (juce::Colour (0xff2c3447));
        g.drawRoundedRectangle (r, 6.0f, 1.0f);
    }
}

void TearedLookAndFeel::drawComboBox (juce::Graphics& g, int w, int h, bool, int, int, int, int, juce::ComboBox&)
{
    auto r = juce::Rectangle<float> (0.0f, 0.0f, (float) w, (float) h).reduced (0.5f);
    g.setColour (juce::Colour (0xff1b2030));
    g.fillRoundedRectangle (r, 6.0f);
    g.setColour (juce::Colour (0xff2c3447));
    g.drawRoundedRectangle (r, 6.0f, 1.0f);

    juce::Path arrow;
    const float ax = (float) w - 16.0f, ay = (float) h * 0.5f;
    arrow.addTriangle (ax - 4.0f, ay - 2.0f, ax + 4.0f, ay - 2.0f, ax, ay + 3.0f);
    g.setColour (juce::Colour (kGrey));
    g.fillPath (arrow);
}

juce::Font TearedLookAndFeel::getTextButtonFont (juce::TextButton&, int)
{
    return juce::Font (juce::FontOptions (13.0f, juce::Font::bold));
}

//==============================================================================
// Editor
//==============================================================================
TearedEQAudioProcessorEditor::TearedEQAudioProcessorEditor (TearedEQAudioProcessor& p)
    : AudioProcessorEditor (&p), proc (p)
{
    setLookAndFeel (&laf);
    binDb.fill (-120.0f);
    chipState.fill (-1);

    // Band chips
    for (int i = 0; i < numBands; ++i)
    {
        auto& c = chips[(size_t) i];
        c.setButtonText (juce::String (i + 1));
        c.setClickingTogglesState (true);
        c.setRadioGroupId (101);
        c.setColour (juce::TextButton::buttonOnColourId, bandColour (i));
        c.setColour (juce::TextButton::textColourOnId, juce::Colour (kBg1));
        c.onClick = [this, i] { selectBand (i); };
        addAndMakeVisible (c);
    }

    addAndMakeVisible (onButton);
    onButton.setClickingTogglesState (true);

    addAndMakeVisible (typeBox);
    typeBox.addItemList (typeNames(), 1);
    typeBox.setJustificationType (juce::Justification::centredLeft);

    auto setupKnob = [this] (juce::Slider& s, juce::Label& l, const juce::String& text)
    {
        s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 96, 20);
        s.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (kCyan));
        addAndMakeVisible (s);
        l.setText (text, juce::dontSendNotification);
        l.setJustificationType (juce::Justification::centred);
        l.setFont (juce::Font (juce::FontOptions (12.0f, juce::Font::bold)));
        l.attachToComponent (&s, false);
    };
    setupKnob (freqSlider, freqLabel, "FREQ");
    setupKnob (gainSlider, gainLabel, "GAIN");
    setupKnob (qSlider,    qLabel,    "Q");
    setupKnob (outSlider,  outLabel,  "OUTPUT");

    outSlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (kViolet));
    outAtt = std::make_unique<SliderAtt> (proc.apvts, "out", outSlider);

    addAndMakeVisible (analyzerButton);
    analyzerButton.setClickingTogglesState (true);
    analyzerAtt = std::make_unique<ButtonAtt> (proc.apvts, "analyzer", analyzerButton);

    selectBand (selected);

    setResizeLimits (860, 560, 1800, 1100);
    setResizable (true, true);
    setSize (1000, 640);

    startTimerHz (30);
}

TearedEQAudioProcessorEditor::~TearedEQAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void TearedEQAudioProcessorEditor::selectBand (int b)
{
    selected = juce::jlimit (0, numBands - 1, b);
    chips[(size_t) selected].setToggleState (true, juce::dontSendNotification);

    freqAtt.reset(); gainAtt.reset(); qAtt.reset(); typeAtt.reset(); onAtt.reset();

    const auto col = bandColour (selected);
    freqSlider.setColour (juce::Slider::rotarySliderFillColourId, col);
    gainSlider.setColour (juce::Slider::rotarySliderFillColourId, col);
    qSlider.setColour    (juce::Slider::rotarySliderFillColourId, col);
    onButton.setColour   (juce::TextButton::buttonOnColourId, col);

    freqAtt = std::make_unique<SliderAtt> (proc.apvts, paramId (selected, "freq"), freqSlider);
    gainAtt = std::make_unique<SliderAtt> (proc.apvts, paramId (selected, "gain"), gainSlider);
    qAtt    = std::make_unique<SliderAtt> (proc.apvts, paramId (selected, "q"),    qSlider);
    typeAtt = std::make_unique<ComboAtt>  (proc.apvts, paramId (selected, "type"), typeBox);
    onAtt   = std::make_unique<ButtonAtt> (proc.apvts, paramId (selected, "on"),   onButton);

    repaint();
}

//==============================================================================
void TearedEQAudioProcessorEditor::timerCallback()
{
    if (proc.isAnalyzerOn())
        updateSpectrum();

    // Dim the gain knob when the band type has no gain
    const auto s = proc.readBand (selected);
    const bool usesGain = s.usesGain();
    if (gainSlider.isEnabled() != usesGain)
    {
        gainSlider.setEnabled (usesGain);
        gainSlider.setAlpha (usesGain ? 1.0f : 0.35f);
        gainLabel.setAlpha (usesGain ? 1.0f : 0.35f);
    }

    // Light up chips of enabled bands
    for (int i = 0; i < numBands; ++i)
    {
        const int on = proc.readBand (i).on ? 1 : 0;
        if (chipState[(size_t) i] != on)
        {
            chipState[(size_t) i] = on;
            chips[(size_t) i].setColour (juce::TextButton::textColourOffId,
                                         on ? bandColour (i) : juce::Colour (0xff566074));
        }
    }

    repaint (graphArea.toNearestInt());
}

void TearedEQAudioProcessorEditor::updateSpectrum()
{
    proc.copyScope (fftData.data(), fftSize);
    std::fill (fftData.begin() + fftSize, fftData.end(), 0.0f);
    window.multiplyWithWindowingTable (fftData.data(), (size_t) fftSize);
    fft.performFrequencyOnlyForwardTransform (fftData.data());

    for (int i = 0; i <= fftSize / 2; ++i)
    {
        const float level = fftData[(size_t) i] / (fftSize * 0.25f);
        const float db = juce::Decibels::gainToDecibels (level, -140.0f);
        auto& b = binDb[(size_t) i];
        b += (db - b) * (db > b ? 0.6f : 0.12f);
    }
}

//==============================================================================
float TearedEQAudioProcessorEditor::freqToX (float f) const
{
    return graphArea.getX() + graphArea.getWidth() * std::log (juce::jmax (20.0f, f) / 20.0f) / std::log (1000.0f);
}
float TearedEQAudioProcessorEditor::xToFreq (float x) const
{
    const float p = juce::jlimit (0.0f, 1.0f, (x - graphArea.getX()) / graphArea.getWidth());
    return 20.0f * std::pow (1000.0f, p);
}
float TearedEQAudioProcessorEditor::gainToY (float g) const
{
    return juce::jmap (g, -18.0f, 18.0f, graphArea.getBottom(), graphArea.getY());
}
float TearedEQAudioProcessorEditor::yToGain (float y) const
{
    return juce::jmap (y, graphArea.getBottom(), graphArea.getY(), -18.0f, 18.0f);
}

juce::Point<float> TearedEQAudioProcessorEditor::nodePosition (const BandSettings& s) const
{
    const float y = gainToY (s.usesGain() ? s.gain : 0.0f);
    return { freqToX (s.freq), juce::jlimit (graphArea.getY() + 4.0f, graphArea.getBottom() - 4.0f, y) };
}

int TearedEQAudioProcessorEditor::hitTestNode (juce::Point<float> p) const
{
    for (int b = numBands - 1; b >= 0; --b)
    {
        const auto s = proc.readBand (b);
        if (s.on && nodePosition (s).getDistanceFrom (p) < 12.0f)
            return b;
    }
    return -1;
}

//==============================================================================
void TearedEQAudioProcessorEditor::resized()
{
    auto r = getLocalBounds();
    auto header = r.removeFromTop (60);
    analyzerButton.setBounds (header.removeFromRight (130).reduced (10, 16));

    panelArea = r.removeFromBottom (210).reduced (14, 6).toFloat();
    graphArea = r.reduced (14, 4).toFloat();

    auto b = panelArea.toNearestInt().reduced (16, 12);

    auto chipRow = b.removeFromTop (30);
    for (auto& c : chips)
        c.setBounds (chipRow.removeFromLeft (50).reduced (3, 0));

    b.removeFromTop (10);

    auto outArea = b.removeFromRight (150);
    outSlider.setBounds (outArea.withTrimmedTop (20).reduced (14, 0));

    auto left = b.removeFromLeft (170);
    onButton.setBounds (left.removeFromTop (32).withTrimmedRight (30));
    left.removeFromTop (12);
    typeBox.setBounds (left.removeFromTop (32).withTrimmedRight (30));

    const int w = b.getWidth() / 3;
    freqSlider.setBounds (b.removeFromLeft (w).withTrimmedTop (20).reduced (12, 0));
    gainSlider.setBounds (b.removeFromLeft (w).withTrimmedTop (20).reduced (12, 0));
    qSlider.setBounds    (b.withTrimmedTop (20).reduced (12, 0));
}

//==============================================================================
void TearedEQAudioProcessorEditor::drawHeader (juce::Graphics& g)
{
    // teardrop logo
    const float cx = 36.0f, cy = 30.0f;
    juce::Path drop;
    drop.startNewSubPath (cx, cy - 15.0f);
    drop.cubicTo (cx + 3.0f, cy - 7.0f, cx + 10.0f, cy - 1.0f, cx + 10.0f, cy + 5.0f);
    drop.cubicTo (cx + 10.0f, cy + 11.0f, cx + 5.5f, cy + 15.0f, cx, cy + 15.0f);
    drop.cubicTo (cx - 5.5f, cy + 15.0f, cx - 10.0f, cy + 11.0f, cx - 10.0f, cy + 5.0f);
    drop.cubicTo (cx - 10.0f, cy - 1.0f, cx - 3.0f, cy - 7.0f, cx, cy - 15.0f);
    drop.closeSubPath();

    g.setColour (juce::Colour (kCyan).withAlpha (0.18f));
    g.fillPath (drop, juce::AffineTransform::scale (1.35f, 1.35f, cx, cy));
    g.setGradientFill (juce::ColourGradient (juce::Colour (kCyan), cx, cy - 15.0f,
                                             juce::Colour (kViolet), cx, cy + 15.0f, false));
    g.fillPath (drop);
    g.setColour (juce::Colours::white.withAlpha (0.55f));
    g.fillEllipse (cx - 5.5f, cy + 1.0f, 3.5f, 6.0f);

    const juce::Font big (juce::FontOptions (28.0f, juce::Font::bold));
    juce::GlyphArrangement ga;
    ga.addLineOfText (big, "Teared", 0.0f, 0.0f);
    const float tw = ga.getBoundingBox (0, -1, true).getWidth();

    g.setFont (big);
    g.setGradientFill (juce::ColourGradient (juce::Colour (kCyan), 58.0f, 0.0f,
                                             juce::Colour (kViolet), 58.0f + tw, 0.0f, false));
    g.drawText ("Teared", juce::Rectangle<int> (58, 8, (int) tw + 6, 36), juce::Justification::centredLeft, false);
    g.setColour (juce::Colours::white);
    g.drawText ("EQ", juce::Rectangle<int> (58 + (int) tw + 2, 8, 60, 36), juce::Justification::centredLeft, false);

    g.setColour (juce::Colour (kGrey));
    g.setFont (juce::Font (juce::FontOptions (10.5f, juce::Font::bold)));
    g.drawText ("8-BAND PARAMETRIC EQUALIZER", juce::Rectangle<int> (60, 40, 300, 14),
                juce::Justification::centredLeft, false);

    g.setColour (juce::Colour (0xff566074));
    g.setFont (juce::Font (juce::FontOptions (11.5f)));
    g.drawText ("Drag nodes  |  Scroll on a node = Q  |  Double-click = add / remove band",
                juce::Rectangle<int> (getWidth() - 140 - 520, 18, 520, 24),
                juce::Justification::centredRight, false);
}

void TearedEQAudioProcessorEditor::drawGraph (juce::Graphics& g)
{
    const auto area = graphArea;
    const double sr = proc.getCurrentSampleRate();

    g.setColour (juce::Colour (kPanel));
    g.fillRoundedRectangle (area, 10.0f);

    {
        juce::Graphics::ScopedSaveState ss (g);
        juce::Path clip;
        clip.addRoundedRectangle (area, 10.0f);
        g.reduceClipRegion (clip);

        // ---- grid ----
        g.setFont (juce::Font (juce::FontOptions (10.5f)));
        for (int dec : { 10, 100, 1000, 10000 })
            for (int m = 1; m <= 9; ++m)
            {
                const int f = dec * m;
                if (f < 20 || f > 20000) continue;
                const float x = freqToX ((float) f);
                const bool major = (m == 1 || m == 2 || m == 5);
                g.setColour (juce::Colour (kLine).withAlpha (major ? 1.0f : 0.5f));
                g.drawVerticalLine ((int) x, area.getY(), area.getBottom());
                if (major)
                {
                    g.setColour (juce::Colour (0xff566074));
                    const auto txt = f >= 1000 ? juce::String (f / 1000) + "k" : juce::String (f);
                    const float tx = juce::jlimit (area.getX() + 2.0f, area.getRight() - 42.0f, x - 20.0f);
                    g.drawText (txt, juce::Rectangle<float> (tx, area.getBottom() - 18.0f, 40.0f, 14.0f),
                                juce::Justification::centred, false);
                }
            }

        for (int db = -18; db <= 18; db += 6)
        {
            const float y = gainToY ((float) db);
            g.setColour (db == 0 ? juce::Colour (0xff343d52) : juce::Colour (kLine));
            g.drawHorizontalLine ((int) y, area.getX(), area.getRight());
            if (db != -18 && db != 18)
            {
                g.setColour (juce::Colour (0xff566074));
                g.drawText ((db > 0 ? "+" : "") + juce::String (db),
                            juce::Rectangle<float> (area.getX() + 6.0f, y - 14.0f, 30.0f, 12.0f),
                            juce::Justification::centredLeft, false);
            }
        }

        // ---- analyzer ----
        if (proc.isAnalyzerOn())
        {
            juce::Path p;
            bool started = false;
            const float fs = (float) sr;
            for (float x = area.getX(); x <= area.getRight() + 0.5f; x += 2.0f)
            {
                const float f0 = xToFreq (x - 1.0f), f1 = xToFreq (x + 1.0f);
                const float pos0 = f0 * (float) fftSize / fs, pos1 = f1 * (float) fftSize / fs;
                float db;
                if (pos1 - pos0 < 1.0f)
                {
                    const float pos = 0.5f * (pos0 + pos1);
                    const int i0 = juce::jlimit (0, fftSize / 2 - 1, (int) pos);
                    const float fr = pos - (float) i0;
                    db = binDb[(size_t) i0] * (1.0f - fr) + binDb[(size_t) i0 + 1] * fr;
                }
                else
                {
                    const int i0 = juce::jmax (1, (int) pos0), i1 = juce::jmin (fftSize / 2, (int) pos1);
                    float sum = 0.0f; int n = 0;
                    for (int i = i0; i <= i1; ++i) { sum += binDb[(size_t) i]; ++n; }
                    db = n > 0 ? sum / (float) n : -120.0f;
                }
                db += 4.5f * std::log2 (xToFreq (x) / 1000.0f); // pink-ish tilt for a natural look
                float y = juce::jmap (db, -90.0f, -10.0f, area.getBottom(), area.getY());
                y = juce::jlimit (area.getY(), area.getBottom(), y);
                if (! started) { p.startNewSubPath (x, y); started = true; }
                else p.lineTo (x, y);
            }
            juce::Path fill (p);
            fill.lineTo (area.getRight(), area.getBottom());
            fill.lineTo (area.getX(), area.getBottom());
            fill.closeSubPath();
            g.setGradientFill (juce::ColourGradient (juce::Colour (kViolet).withAlpha (0.30f), 0.0f, area.getY(),
                                                     juce::Colour (kCyan).withAlpha (0.02f), 0.0f, area.getBottom(), false));
            g.fillPath (fill);
            g.setColour (juce::Colour (kCyan).withAlpha (0.45f));
            g.strokePath (p.createPathWithRoundedCorners (3.0f), juce::PathStrokeType (1.2f));
        }

        // ---- EQ curve ----
        std::array<BandSettings, numBands> bs;
        std::array<Coeffs::Ptr, numBands> co;
        for (int b = 0; b < numBands; ++b)
        {
            bs[(size_t) b] = proc.readBand (b);
            if (bs[(size_t) b].on) co[(size_t) b] = makeCoefficients (bs[(size_t) b], sr);
        }

        juce::Path curve, bandCurve;
        bool first = true;
        const float zeroY = gainToY (0.0f);
        for (float x = area.getX(); x <= area.getRight() + 0.5f; x += 2.0f)
        {
            const double f = juce::jmin ((double) xToFreq (x), sr * 0.49);
            float total = 0.0f, sel = 0.0f;
            for (int b = 0; b < numBands; ++b)
                if (bs[(size_t) b].on)
                {
                    const float db = juce::Decibels::gainToDecibels (
                        (float) co[(size_t) b]->getMagnitudeForFrequency (f, sr), -60.0f);
                    total += db;
                    if (b == selected) sel = db;
                }
            const float y  = juce::jlimit (area.getY(), area.getBottom(), gainToY (total));
            const float ys = juce::jlimit (area.getY(), area.getBottom(), gainToY (sel));
            if (first) { curve.startNewSubPath (x, y); bandCurve.startNewSubPath (x, ys); first = false; }
            else       { curve.lineTo (x, y);          bandCurve.lineTo (x, ys); }
        }

        if (bs[(size_t) selected].on)
        {
            juce::Path bf (bandCurve);
            bf.lineTo (area.getRight(), zeroY);
            bf.lineTo (area.getX(), zeroY);
            bf.closeSubPath();
            g.setColour (bandColour (selected).withAlpha (0.16f));
            g.fillPath (bf);
            g.setColour (bandColour (selected).withAlpha (0.55f));
            g.strokePath (bandCurve, juce::PathStrokeType (1.2f));
        }

        juce::Path cf (curve);
        cf.lineTo (area.getRight(), zeroY);
        cf.lineTo (area.getX(), zeroY);
        cf.closeSubPath();
        g.setColour (juce::Colours::white.withAlpha (0.05f));
        g.fillPath (cf);

        juce::ColourGradient grad (juce::Colour (kCyan), area.getX(), 0.0f,
                                   juce::Colour (kViolet), area.getRight(), 0.0f, false);
        g.setGradientFill (grad);
        g.strokePath (curve, juce::PathStrokeType (5.0f));      // soft glow base
        g.setColour (juce::Colour (kBg1).withAlpha (0.55f));
        g.strokePath (curve, juce::PathStrokeType (3.2f));
        g.setGradientFill (grad);
        g.strokePath (curve, juce::PathStrokeType (2.2f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // ---- nodes ----
        g.setFont (juce::Font (juce::FontOptions (11.0f, juce::Font::bold)));
        for (int b = 0; b < numBands; ++b)
        {
            const auto& s = bs[(size_t) b];
            if (! s.on) continue;
            const auto pos = nodePosition (s);
            const bool sel = (b == selected), hov = (b == hoverBand || b == dragBand);
            const float r = sel ? 10.0f : (hov ? 9.5f : 8.0f);
            const auto c = bandColour (b);

            g.setColour (c.withAlpha (sel ? 0.30f : 0.18f));
            g.fillEllipse (pos.x - r * 1.9f, pos.y - r * 1.9f, r * 3.8f, r * 3.8f);
            g.setColour (juce::Colour (kBg1));
            g.fillEllipse (pos.x - r - 2.0f, pos.y - r - 2.0f, (r + 2.0f) * 2.0f, (r + 2.0f) * 2.0f);
            g.setColour (c);
            g.fillEllipse (pos.x - r, pos.y - r, r * 2.0f, r * 2.0f);
            if (sel)
            {
                g.setColour (juce::Colours::white);
                g.drawEllipse (pos.x - r - 3.0f, pos.y - r - 3.0f, (r + 3.0f) * 2.0f, (r + 3.0f) * 2.0f, 1.5f);
            }
            g.setColour (juce::Colour (kBg1));
            g.drawText (juce::String (b + 1), juce::Rectangle<float> (pos.x - r, pos.y - r, r * 2.0f, r * 2.0f),
                        juce::Justification::centred, false);
        }
    }

    g.setColour (juce::Colour (kLine));
    g.drawRoundedRectangle (area, 10.0f, 1.0f);
}

void TearedEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.setGradientFill (juce::ColourGradient (juce::Colour (kBg2), 0.0f, 0.0f,
                                             juce::Colour (kBg1), 0.0f, (float) getHeight(), false));
    g.fillAll();

    drawHeader (g);
    drawGraph (g);

    g.setColour (juce::Colour (kPanel));
    g.fillRoundedRectangle (panelArea, 10.0f);
    g.setColour (juce::Colour (kLine));
    g.drawRoundedRectangle (panelArea, 10.0f, 1.0f);

    // separator before the output knob
    const float sx = panelArea.getRight() - 16.0f - 150.0f;
    g.drawVerticalLine ((int) sx, panelArea.getY() + 56.0f, panelArea.getBottom() - 14.0f);
}

//==============================================================================
// Parameter helpers + mouse
//==============================================================================
void TearedEQAudioProcessorEditor::setParam (const juce::String& id, float value)
{
    if (auto* p = proc.apvts.getParameter (id))
        p->setValueNotifyingHost (p->convertTo0to1 (value));
}

void TearedEQAudioProcessorEditor::beginGestures (int b)
{
    for (auto* n : { "freq", "gain", "q" })
        if (auto* p = proc.apvts.getParameter (paramId (b, n))) p->beginChangeGesture();
}

void TearedEQAudioProcessorEditor::endGestures (int b)
{
    for (auto* n : { "freq", "gain", "q" })
        if (auto* p = proc.apvts.getParameter (paramId (b, n))) p->endChangeGesture();
}

void TearedEQAudioProcessorEditor::mouseDown (const juce::MouseEvent& e)
{
    dragBand = -1;
    if (! graphArea.contains (e.position)) return;

    const int b = hitTestNode (e.position);
    if (b < 0) return;

    dragBand = b;
    selectBand (b);
    dragStartQ = proc.readBand (b).q;
    dragStartY = e.position.y;
    beginGestures (b);
}

void TearedEQAudioProcessorEditor::mouseDrag (const juce::MouseEvent& e)
{
    if (dragBand < 0) return;

    const auto s = proc.readBand (dragBand);
    setParam (paramId (dragBand, "freq"), juce::jlimit (20.0f, 20000.0f, xToFreq (e.position.x)));

    if (s.usesGain())
        setParam (paramId (dragBand, "gain"), juce::jlimit (-18.0f, 18.0f, yToGain (e.position.y)));
    else
        setParam (paramId (dragBand, "q"),
                  juce::jlimit (0.1f, 18.0f, dragStartQ * std::pow (2.0f, (dragStartY - e.position.y) / 50.0f)));
}

void TearedEQAudioProcessorEditor::mouseUp (const juce::MouseEvent&)
{
    if (dragBand >= 0) endGestures (dragBand);
    dragBand = -1;
}

void TearedEQAudioProcessorEditor::mouseMove (const juce::MouseEvent& e)
{
    const int h = graphArea.contains (e.position) ? hitTestNode (e.position) : -1;
    if (h != hoverBand)
    {
        hoverBand = h;
        setMouseCursor (h >= 0 ? juce::MouseCursor::PointingHandCursor : juce::MouseCursor::NormalCursor);
    }
}

void TearedEQAudioProcessorEditor::mouseDoubleClick (const juce::MouseEvent& e)
{
    if (! graphArea.contains (e.position)) return;

    const int b = hitTestNode (e.position);
    if (b >= 0)
    {
        setParam (paramId (b, "on"), 0.0f);
        return;
    }

    for (int i = 0; i < numBands; ++i)
        if (! proc.readBand (i).on)
        {
            setParam (paramId (i, "freq"), juce::jlimit (20.0f, 20000.0f, xToFreq (e.position.x)));
            setParam (paramId (i, "gain"), 0.0f);
            setParam (paramId (i, "on"), 1.0f);
            selectBand (i);
            return;
        }
}

void TearedEQAudioProcessorEditor::mouseWheelMove (const juce::MouseEvent& e, const juce::MouseWheelDetails& w)
{
    const int b = graphArea.contains (e.position) ? hitTestNode (e.position) : -1;
    if (b < 0) return;

    const auto id = paramId (b, "q");
    if (auto* p = proc.apvts.getParameter (id))
    {
        const float q = juce::jlimit (0.1f, 18.0f, proc.readBand (b).q * std::pow (2.0f, w.deltaY * 2.0f));
        p->beginChangeGesture();
        p->setValueNotifyingHost (p->convertTo0to1 (q));
        p->endChangeGesture();
    }
}
