//
//  PluginParameterListener.h
//  Squeeze
//
//  Created by Adam Jackson on 2/12/21.
//  Copyright © 2021 XDMakesMusic. All rights reserved.
//

#ifndef PluginParameterListener_h
#define PluginParameterListener_h

#include <JuceHeader.h>
#include <functional>

class PluginParameterListener : public juce::AudioProcessorParameter::Listener
{
 typedef std::function<void (float)> ListenerFunction;
 juce::AudioProcessorParameter *param;
 juce::NormalisableRange<float> const *paramRange;
 ListenerFunction lambda;

public:
 template <typename P>
 PluginParameterListener(P *p, ListenerFunction l):
 lambda(l)
 {
  param = p;
  paramRange = &(p->getNormalisableRange());
  p->addListener(this);
 }
 
 void sendInternalUpdate()
 {
  lambda(paramRange->convertFrom0to1(param->getValue()));
 }
 
 virtual void parameterValueChanged (int parameterIndex, float newValue)
 {
  lambda(paramRange->convertFrom0to1(newValue));
 }
  
 virtual void parameterGestureChanged (int parameterIndex, bool gestureIsStarting)
 {}
};

#endif /* PluginParameterListener_h */
