//
//  LookAndFeel.h
//  Squeeze
//
//  Created by Adam Jackson on 10/12/21.
//  Copyright © 2021 XDMakesMusic. All rights reserved.
//

#ifndef LookAndFeel_h
#define LookAndFeel_h

#include <JuceHeader.h>
#include <random>



static constexpr float knobStartAngle = 230./180.*M_PI;
static constexpr float knobEndAngle = 490./180.*M_PI;










class XDLookAndFeel : public juce::LookAndFeel_V4
{
 virtual juce::Font getLabelFont(juce::Label &l) override
 {
  return juce::Font("Arial", 10., juce::Font::plain);
 }
 
 virtual void drawLinearSlider (juce::Graphics& g,
                                int x,
                                int y,
                                int width,
                                int height,
                                float sliderPos,
                                float minSliderPos,
                                float maxSliderPos,
                                const juce::Slider::SliderStyle style,
                                juce::Slider& slider) override
 {
  auto thickness = 2.5;
  float fx = static_cast<float>(x);
  float fy = static_cast<float>(y);
  float fWidth = static_cast<float>(width);
  float fHeight = static_cast<float>(height);

  juce::Point<float> startPoint(slider.isHorizontal() ?
                                fx :
                                fx + 0.5*fWidth,
                                slider.isHorizontal() ?
                                fy + 0.5*fHeight :
                                fHeight + fy);
  juce::Point<float> endPoint(slider.isHorizontal() ?
                              fWidth + fx : startPoint.x,
                              slider.isHorizontal() ?
                              startPoint.y : fy);
  
  {
   juce::Path runner;
   juce::PathStrokeType s(thickness);
   runner.startNewSubPath(startPoint);
   runner.lineTo(endPoint);
   g.setColour(juce::Colours::white.withBrightness(0.4));
   g.strokePath(runner, s);
  }
  
  {
   juce::Point<float> nub (slider.isHorizontal() ? sliderPos : startPoint.x,
                           slider.isHorizontal() ? startPoint.y : sliderPos);
   
   juce::Point<float> nubStart(slider.isHorizontal() ? 0 : -0.5*fHeight,
                               slider.isHorizontal() ? -0.5*fWidth : 0);
   juce::Point<float> nubEnd(slider.isHorizontal() ? 0 : 0.5*fHeight,
                             slider.isHorizontal() ? 0.5*fWidth : 0);
   nubStart = nubStart + nub;
   nubEnd = nubEnd + nub;
   juce::Path p;
   juce::PathStrokeType s(thickness);

   p.startNewSubPath(nubStart);
   p.lineTo(nubEnd);
   g.setColour(juce::Colours::white);
   g.strokePath(p, s);
  }
 }
 
 virtual void drawRotarySlider (juce::Graphics& g,
                                int x,
                                int y,
                                int width,
                                int height,
                                float sliderPos,
                                const float rotaryStartAngle,
                                const float rotaryEndAngle,
                                juce::Slider&)
 override
 {
  auto radius = (float) juce::jmin (width / 2, height / 2) - 4.0f;
  auto thickness = 2.5;
  auto centreX = (float) x + (float) width  * 0.5f;
  auto centreY = (float) y + (float) height * 0.5f;
//  auto rx = centreX - radius;
//  auto ry = centreY - radius;
//  auto rw = radius * 2.0f;
  auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
  
  {
   juce::Path p;
   juce::PathStrokeType s(thickness);
   p.addCentredArc(centreX, centreY, radius - 0.5*thickness, radius - 0.5*thickness, 0., knobStartAngle, knobEndAngle, true);
   g.setColour(juce::Colours::white.withBrightness(0.4));
   g.strokePath(p, s);
  }
  
  {
   juce::Path p;
   auto pointerLength = 6.f;
   auto pointerThickness = 3.0f;
   p.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
   p.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));
   g.setColour(juce::Colours::white);
   g.fillPath(p);
  }
 }
 
 virtual void drawComboBox (juce::Graphics& g,
                            int width,
                            int height,
                            const bool isButtonDown,
                            int buttonX, int buttonY,
                            int buttonW, int buttonH,
                            juce::ComboBox& box)
 override
 {
//  g.fillAll (juce::Colours::black);
  juce::Path p;
  p.addRectangle(1., 1., box.getWidth() - 2., box.getHeight() - 2.);
  g.setColour(juce::Colours::white.withBrightness(0.4));
  g.fillPath(p);
 }
 
 virtual void positionComboBoxText (juce::ComboBox &box,
                                    juce::Label &label)
 override
 {
  label.setBounds (1, 1, box.getWidth() - 2, box.getHeight() - 2);
  label.setFont (getComboBoxFont (box));
 }
 
 virtual void drawToggleButton (juce::Graphics &g,
                                juce::ToggleButton &b,
                                bool shouldDrawButtonAsHighlighted,
                                bool shouldDrawButtonAsDown)
 override
 {
  juce::Path p;
  p.addRectangle(1., 1., b.getWidth() - 2., b.getHeight() - 2.);
  g.setColour(b.getToggleState() ? juce::Colours::yellow : juce::Colours::white.withBrightness(0.4));
  g.fillPath(p);
//  g.fillAll();

  g.setColour (b.getToggleState() ? juce::Colours::black : juce::Colours::white);
  
  g.setFont (juce::jmin (15.0f, b.getHeight() * 0.6f));
  
  if (! b.isEnabled())
   g.setOpacity (0.5f);
  
  const int textX = 1;
  
  g.drawFittedText (b.getButtonText(),
                    textX, 4,
                    b.getWidth() - textX - 2, b.getHeight() - 8,
                    juce::Justification::centredLeft, 10); }
};










template <int BufferSize>
class WaveformDisplay : public juce::Component
{
 std::array<float, BufferSize> wave;
 
public:
 juce::Colour waveformColour {juce::Colours::white};
 
 WaveformDisplay()
 {
  wave.fill(0);
 }
 
 template <typename Iterator>
 void changeWave(Iterator start, Iterator end)
 {
  wave.fill(0.);
  
  int i = 0;
  for (Iterator it = start; i < BufferSize && it < end; ++i, ++it)
  {
   wave[i] = *it;
  }
  
  repaint();
 }
 
 void paint (juce::Graphics& g) override
 {
  float width = getWidth();
  
  float xScale = width / BufferSize;
  float yScale = 0.5*getHeight();
  float yMid = 0.5*getHeight();
  
  g.setColour(juce::Colours::black);
  g.fillRect(0, 0, getWidth(), getHeight());
  
  float x = 0;
  float y = yMid - wave[0]*yScale;
  
  g.setColour(waveformColour);
  for (int i = 1; i < BufferSize; ++i)
  {
   float x2 = static_cast<float>(i)*xScale;
   float y2 = yMid - wave[i]*yScale;
   g.drawLine(x, y, x2, y2);
   x = x2;
   y = y2;
  }
 }
};










class LambdaTextSlider : public juce::Slider
{
public:
 typedef std::function<juce::String (double)> TextFunction;
 
private:
 TextFunction textFunction;
 
public:
 LambdaTextSlider()
 {
  textFunction = [&](double x) -> juce::String
  {
   return juce::Slider::getTextFromValue(x);
  };
 }
 
 void setTextFunction(const TextFunction &f)
 {
  if (f) textFunction = f;
 }
 
 virtual juce::String getTextFromValue(double value) override
 {
  return textFunction(value);
 }
};










class SliderValueCallout : public juce::Label, public juce::Slider::Listener
{
 virtual void sliderValueChanged (juce::Slider *slider)
 {
  setText(slider->getTextFromValue(slider->getValue()), juce::dontSendNotification);
 }
};










#endif /* LookAndFeel_h */
