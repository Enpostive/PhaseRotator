/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"

//==============================================================================
/**
 */
class PhaseRotatorAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
 PhaseRotatorAudioProcessorEditor (PhaseRotatorAudioProcessor&, juce::AudioProcessorValueTreeState&);
 ~PhaseRotatorAudioProcessorEditor() override;
 
 //==============================================================================
 void paint (juce::Graphics&) override;
 void resized() override;
 
 virtual void timerCallback() override;
 
private:
 typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
 typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
 typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
 // This reference is provided as a quick way for your editor to
 // access the processor object that created it.
 PhaseRotatorAudioProcessor& audioProcessor;
 juce::AudioProcessorValueTreeState& valueTreeState;
 
 std::unique_ptr<XDLookAndFeel> lookAndFeel;
 
 juce::Label inputMinimum;
 juce::Label inputMaximum;
 juce::Label outputMinimum;
 juce::Label outputMaximum;
 
 juce::Slider rotationSlider;
 std::unique_ptr<SliderAttachment> rotationAttachment;
 
 juce::ComboBox modeSelector;
 std::unique_ptr<ComboBoxAttachment> modeAttachment;
 
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhaseRotatorAudioProcessorEditor)
};
