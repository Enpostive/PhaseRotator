/*
  ==============================================================================

    DSP.h
    Created: 15 Jan 2024 7:20:30pm
    Author:  Adam Jackson

  ==============================================================================
*/

#pragma once

#include "XDDSP/XDDSP.h"










namespace XDDSP
{










template <typename SignalXIn, typename SignalYIn, typename RotationIn,
int StepSize = IntegerMaximum>
class Rotator : public Component<Rotator<SignalXIn, SignalYIn, RotationIn, StepSize>>
{
 // Private data members here
public:
 static constexpr int Count = SignalXIn::Count;
 
 // Specify your inputs as public members here
 SignalXIn signalXIn;
 SignalYIn signalYIn;
 RotationIn rotationIn;

 // Specify your outputs like this
 Output<Count> signalOut;
 
 // Include a definition for each input in the constructor
 Rotator(Parameters &p,
         SignalXIn _signalXIn,
         SignalYIn _signalYIn,
         RotationIn _rotationIn) :
 signalXIn(_signalXIn),
 signalYIn(_signalYIn),
 rotationIn(_rotationIn),
 signalOut(p)
 {}
 
 // This function is responsible for clearing the output buffers to a default state when
 // the component is disabled.
 void reset()
 {
  signalOut.reset();
 }
 
 // startProcess prepares the component for processing one block and returns the step
 // size. By default, it returns the entire sampleCount as one big step.
// int startProcess(int startPoint, int sampleCount)
// { return std::min(sampleCount, StepSize); }

 // stepProcess is called repeatedly with the start point incremented by step size
 void stepProcess(int startPoint, int sampleCount)
 {
  for (int c = 0; c < Count; ++c)
  {
   SampleType cc = cos(rotationIn(c, 0));
   SampleType cs = sin(rotationIn(c, 0));
   for (int i = startPoint, s = sampleCount; s--; ++i)
   {
    signalOut.buffer(c, i) = cc*signalXIn(c, i) + cs*signalYIn(c, i);
   }
  }
 }
 
 // finishProcess is called after the block has been processed
// void finishProcess()
// {}
};

 
 
 
 
 
 
 
 
 
class PhaseRotatorDSP : public Component<PhaseRotatorDSP>
{
 // Private data members here
public:
 static constexpr int Count = 2;
 
 // Specify your inputs as public members here
 BufferCoupler<float, 2> floatInput;
 
 SignalProbe<Connector<2>> inputProbe;
 
 IIRHilbertApproximator<Connector<2>> hil;
 ConvolutionHilbertFilter<Connector<2>, 255> f255;
 ConvolutionHilbertFilter<Connector<2>, 1023> f1023;
 ConvolutionHilbertFilter<Connector<2>, 2047> f2047;

 typedef Switch<4, 2> RotatorInputSwitch;
 
 Rotator<RotatorInputSwitch, RotatorInputSwitch, ControlConstant<2>> rotator;
 
 SignalProbe<Connector<2>> outputProbe;
 
 // Include a definition for each input in the constructor
 PhaseRotatorDSP(Parameters &p) :
 inputProbe(p, floatInput),
 hil(p, floatInput),
 f255(p, floatInput),
 f1023(p, floatInput),
 f2047(p, floatInput),
 rotator(p,
         {{&hil.inPhaseOut, &f255.inPhaseOut, &f1023.inPhaseOut, &f2047.inPhaseOut}},
         {{&hil.quadratureOut, &f255.quadratureOut, &f1023.quadratureOut, &f2047.quadratureOut}}, {0.}),
 outputProbe(p, rotator.signalOut)
 {}
 
 void setMode(int mode)
 {
  hil.setEnabled(mode == 0);
  f255.setEnabled(mode == 1);
  f1023.setEnabled(mode == 2);
  f2047.setEnabled(mode == 3);
  rotator.signalXIn.select(mode);
  rotator.signalYIn.select(mode);
 }
 
 // This function is responsible for clearing the output buffers to a default state when
 // the component is disabled.
 void reset()
 {
  inputProbe.reset();
  hil.reset();
  f255.reset();
  f1023.reset();
  f2047.reset();
  rotator.reset();
  outputProbe.reset();
 }
 
 // startProcess prepares the component for processing one block and returns the step
 // size. By default, it returns the entire sampleCount as one big step.
// int startProcess(int startPoint, int sampleCount)
// { return std::min(sampleCount, StepSize); }

 // stepProcess is called repeatedly with the start point incremented by step size
 void stepProcess(int startPoint, int sampleCount)
 {
  inputProbe.process(startPoint, sampleCount);
  hil.process(startPoint, sampleCount);
  f255.process(startPoint, sampleCount);
  f1023.process(startPoint, sampleCount);
  f2047.process(startPoint, sampleCount);
  rotator.process(startPoint, sampleCount);
  outputProbe.process(startPoint, sampleCount);
 }
 
 // finishProcess is called after the block has been processed
// void finishProcess()
// {}
};

 
 
 
 
 
 
 
 
 
}
