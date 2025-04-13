/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/CustomLookAndFeel.h"

//==============================================================================
/**
*/

class ChorusEffect_VSTAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
	ChorusEffect_VSTAudioProcessorEditor(ChorusEffect_VSTAudioProcessor&);
	~ChorusEffect_VSTAudioProcessorEditor() override;

	//==============================================================================
	void paint(juce::Graphics&) override;
	void resized() override;


private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	ChorusEffect_VSTAudioProcessor& audioProcessor;

	// Slider
	juce::Slider delaySlider;
	juce::Slider depthSlider;
	juce::Slider chorusMixSlider;
	juce::Slider feedbackSlider;
	juce::Slider rateSlider;
	juce::Slider spreadSlider;

	// Custom LookAndFeel for sliders
	CustomLookAndFeel lookAndFeel;

	// Value Tree for sliders
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusMixAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> spreadAttachment;

	// Label for sliders
	juce::Label chorusMixLabel;
	juce::Label spreadLabel;
	juce::Label rateLabel;
	juce::Label depthLabel;
	juce::Label feedbackLabel;
	juce::Label delayLabel;

	juce::Image vst_logo;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChorusEffect_VSTAudioProcessorEditor)
};
