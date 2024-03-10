/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PhaseRotatorAudioProcessorEditor::PhaseRotatorAudioProcessorEditor (PhaseRotatorAudioProcessor& p, juce::AudioProcessorValueTreeState &vts)
: AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vts)
{
 // Make sure that before the constructor has finished, you've set the
 // editor's size to whatever you need it to be.
 setSize (400, 100);
 
 lookAndFeel = std::make_unique<XDLookAndFeel>();
 
 addAndMakeVisible(rotationSlider);
 rotationSlider.setBounds(150, 15, 100, 85);
 rotationAttachment.reset(new SliderAttachment(valueTreeState, "rotation", rotationSlider));
 rotationSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
 rotationSlider.setLookAndFeel(lookAndFeel.get());
 rotationSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 48, 15);
 
 addAndMakeVisible(modeSelector);
 modeSelector.setBounds(150, 0, 100, 15);
 modeAttachment.reset(new ComboBoxAttachment(valueTreeState, "mode", modeSelector));
 modeSelector.addItemList(audioProcessor.ModesList, 1);
 modeSelector.setLookAndFeel(lookAndFeel.get());
 juce::RangedAudioParameter *pr = valueTreeState.getParameter("mode");
 int s = pr->getNormalisableRange().convertFrom0to1(pr->getValue());
 modeSelector.setSelectedId(s + 1, juce::dontSendNotification);

 addAndMakeVisible(inputMinimum);
 inputMinimum.setBounds(5, 60, 145, 20);
 addAndMakeVisible(inputMaximum);
 inputMaximum.setBounds(5, 20, 145, 20);
 addAndMakeVisible(outputMinimum);
 outputMinimum.setBounds(255, 60, 145, 20);
 addAndMakeVisible(outputMaximum);
 outputMaximum.setBounds(255, 20, 145, 20);

 startTimerHz(10);
}

PhaseRotatorAudioProcessorEditor::~PhaseRotatorAudioProcessorEditor()
{
}

juce::String formatLabel(float x)
{
 return juce::String(x, 2) + juce::String("dB");
}

void PhaseRotatorAudioProcessorEditor::timerCallback()
{
 float t;
 
 t = audioProcessor.dsp.inputProbe.getMinimumValue(0) + audioProcessor.dsp.inputProbe.getMinimumValue(1);
 inputMinimum.setText(formatLabel(XDDSP::linear2dB(-0.5*t)), juce::dontSendNotification);
 
 t = audioProcessor.dsp.inputProbe.getMaximumValue(0) + audioProcessor.dsp.inputProbe.getMaximumValue(1);
 inputMaximum.setText(formatLabel(XDDSP::linear2dB(0.5*t)), juce::dontSendNotification);
 
 t = audioProcessor.dsp.outputProbe.getMinimumValue(0) + audioProcessor.dsp.outputProbe.getMinimumValue(1);
 outputMinimum.setText(formatLabel(XDDSP::linear2dB(-0.5*t)), juce::dontSendNotification);
 
 t = audioProcessor.dsp.outputProbe.getMaximumValue(0) + audioProcessor.dsp.outputProbe.getMaximumValue(1);
 outputMaximum.setText(formatLabel(XDDSP::linear2dB(0.5*t)), juce::dontSendNotification);
 
 audioProcessor.dsp.inputProbe.reset();
 audioProcessor.dsp.outputProbe.reset();
}

//==============================================================================
void PhaseRotatorAudioProcessorEditor::paint (juce::Graphics& g)
{
 // (Our component is opaque, so we must completely fill the background with a solid colour)
 g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void PhaseRotatorAudioProcessorEditor::resized()
{
 // This is generally where you'll want to lay out the positions of any
 // subcomponents in your editor..
}
