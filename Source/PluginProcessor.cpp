/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

static constexpr int PluginParameterVersion = 2;

//==============================================================================
PhaseRotatorAudioProcessor::PhaseRotatorAudioProcessor()
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
,
dsp(dspParam),
parameters(*this, nullptr, juce::Identifier("PhaseRotator"),
{
 std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("rotation", PluginParameterVersion), "Rotation", juce::NormalisableRange<float>(-180.,180.,1.0), 0., "deg"),
 std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("mode", PluginParameterVersion), "Mode", ModesList, 0)
})
{
 {
  // Rotation Parameter
  PluginParameterListener *listener = new PluginParameterListener(parameters.getParameter("rotation"), [&](float newValue)
  {
   dsp.rotator.rotationIn.setControl(newValue / 180. * M_PI);
  });
  parameters.getParameter("rotation")->addListener(listener);
  rotationListen = std::unique_ptr<PluginParameterListener>(listener);
 }

 {
  // Rotation Parameter
  PluginParameterListener *listener = new PluginParameterListener(parameters.getParameter("mode"), [&](float newValue)
  {
   dsp.setMode((int)newValue);
   switch ((int)newValue)
   {
    case 0:
    default:
     setLatencySamples(0);
     break;
     
    case 1:
     setLatencySamples(dsp.f63.DelayLength);
     break;
     
    case 2:
     setLatencySamples(dsp.f127.DelayLength);
     break;
     
    case 3:
     setLatencySamples(dsp.f255.DelayLength);
     break;
   }
  });
  parameters.getParameter("mode")->addListener(listener);
  modeListen = std::unique_ptr<PluginParameterListener>(listener);
 }
}

PhaseRotatorAudioProcessor::~PhaseRotatorAudioProcessor()
{
}

//==============================================================================
const juce::String PhaseRotatorAudioProcessor::getName() const
{
 return JucePlugin_Name;
}

bool PhaseRotatorAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
 return true;
#else
 return false;
#endif
}

bool PhaseRotatorAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
 return true;
#else
 return false;
#endif
}

bool PhaseRotatorAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
 return true;
#else
 return false;
#endif
}

double PhaseRotatorAudioProcessor::getTailLengthSeconds() const
{
 return 0.0;
}

int PhaseRotatorAudioProcessor::getNumPrograms()
{
 return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
 // so this should be at least 1, even if you're not really implementing programs.
}

int PhaseRotatorAudioProcessor::getCurrentProgram()
{
 return 0;
}

void PhaseRotatorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PhaseRotatorAudioProcessor::getProgramName (int index)
{
 return {};
}

void PhaseRotatorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PhaseRotatorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
 dspParam.setSampleRate(sampleRate);
 dspParam.setBufferSize(samplesPerBlock);
 monobuf.resize(samplesPerBlock);
 
 rotationListen->sendInternalUpdate();
}

void PhaseRotatorAudioProcessor::releaseResources()
{
 // When playback stops, you can use this as an opportunity to free up any
 // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PhaseRotatorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void PhaseRotatorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
 juce::ScopedNoDenormals noDenormals;
 if (buffer.getNumChannels() == 1)
 {
  dsp.floatInput.connect({buffer.getWritePointer(0), monobuf.data()});
  dsp.process(0, buffer.getNumSamples());
  dsp.rotator.signalOut.fastTransfer<float>({buffer.getWritePointer(0), monobuf.data()}, buffer.getNumSamples());
 }
 else
 {
  dsp.floatInput.connect({buffer.getWritePointer(0), buffer.getWritePointer(1)});
  dsp.process(0, buffer.getNumSamples());
  dsp.rotator.signalOut.fastTransfer<float>({buffer.getWritePointer(0), buffer.getWritePointer(1)}, buffer.getNumSamples());
 }
}

//==============================================================================
bool PhaseRotatorAudioProcessor::hasEditor() const
{
 return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PhaseRotatorAudioProcessor::createEditor()
{
 return new PhaseRotatorAudioProcessorEditor (*this, parameters);
// return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void PhaseRotatorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
 auto p = parameters.copyState();
 std::unique_ptr<juce::XmlElement> xml(p.createXml());
 copyXmlToBinary(*xml, destData);
}

void PhaseRotatorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
 std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
 
 if (xmlState.get() != nullptr)
 {
  if (xmlState->hasTagName (parameters.state.getType()))
  {
   juce::ValueTree p = juce::ValueTree::fromXml(*xmlState);
   parameters.replaceState(p);
  }
 }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
 return new PhaseRotatorAudioProcessor();
}
