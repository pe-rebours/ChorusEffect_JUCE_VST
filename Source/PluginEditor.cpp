/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;

//==============================================================================
ChorusEffect_VSTAudioProcessorEditor::ChorusEffect_VSTAudioProcessorEditor(ChorusEffect_VSTAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
	// window size
	setSize(600, 530);

	// Initialization of the sliders
	delaySlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
	delaySlider.setRange(0.0, 100.0, 1.0);
	delaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, 0, 70, 30);
	delaySlider.setTextValueSuffix(" ms");
	delaySlider.setLookAndFeel(&lookAndFeel);
	addAndMakeVisible(&delaySlider);
	delayLabel.setJustificationType(juce::Justification::centred);
	delayLabel.setText(juce::String("Delay"), juce::NotificationType::dontSendNotification);
	delayLabel.setFont(juce::Font(juce::Font::getSystemUIFontName(), 25, 1));
	delayLabel.attachToComponent(&delaySlider, false);
	addAndMakeVisible(&delayLabel);

	chorusMixSlider.setSliderStyle(juce::Slider::LinearVertical);
	chorusMixSlider.setRange(0.0, 1.0, 0.1);
	chorusMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, 0, 70, 30);
	chorusMixSlider.setLookAndFeel(&lookAndFeel);
	addAndMakeVisible(&chorusMixSlider);
	chorusMixLabel.setJustificationType(juce::Justification::centred);
	chorusMixLabel.setText(juce::String("Chorus Mix"), juce::NotificationType::dontSendNotification);
	chorusMixLabel.setFont(juce::Font(juce::Font::getSystemUIFontName(), 25, 1));
	chorusMixLabel.attachToComponent(&chorusMixSlider, false);
	addAndMakeVisible(&chorusMixLabel);

	depthSlider.setSliderStyle(juce::Slider::Rotary);
	depthSlider.setRange(0.0, 1.0, 0.1);
	depthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, 0, 70, 30);
	depthSlider.setLookAndFeel(&lookAndFeel);
	addAndMakeVisible(&depthSlider);
	depthLabel.setJustificationType(juce::Justification::centred);
	depthLabel.setText(juce::String("Depth"), juce::NotificationType::dontSendNotification);
	depthLabel.setFont(juce::Font(juce::Font::getSystemUIFontName(), 25, 1));
	depthLabel.attachToComponent(&depthSlider, false);
	addAndMakeVisible(&depthLabel);

	feedbackSlider.setSliderStyle(juce::Slider::Rotary);
	feedbackSlider.setRange(-1.0, 1.0, 0.1);
	feedbackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, 0, 70, 30);
	feedbackSlider.setLookAndFeel(&lookAndFeel);
	addAndMakeVisible(&feedbackSlider);
	feedbackLabel.setJustificationType(juce::Justification::centred);
	feedbackLabel.setText(juce::String("Feedback"), juce::NotificationType::dontSendNotification);
	feedbackLabel.setFont(juce::Font(juce::Font::getSystemUIFontName(), 25, 1));
	feedbackLabel.attachToComponent(&feedbackSlider, false);
	addAndMakeVisible(&feedbackLabel);

	rateSlider.setSliderStyle(juce::Slider::Rotary);
	rateSlider.setRange(0.0, 10.0, 0.1);
	rateSlider.setTextValueSuffix(" Hz");
	rateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, 0, 70, 30);
	rateSlider.setLookAndFeel(&lookAndFeel);
	addAndMakeVisible(&rateSlider);
	rateLabel.setJustificationType(juce::Justification::centred);
	rateLabel.setText(juce::String("Rate"), juce::NotificationType::dontSendNotification);
	rateLabel.setFont(juce::Font(juce::Font::getSystemUIFontName(), 25, 1));
	rateLabel.attachToComponent(&rateSlider, false);
	addAndMakeVisible(&rateLabel);

	spreadSlider.setSliderStyle(juce::Slider::LinearVertical);
	spreadSlider.setRange(0.0, 2.0, 0.1);
	spreadSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, 0, 70, 30);
	spreadSlider.setLookAndFeel(&lookAndFeel);
	addAndMakeVisible(&spreadSlider);
	spreadLabel.setJustificationType(juce::Justification::centred);
	spreadLabel.setText(juce::String("Spread"), juce::NotificationType::dontSendNotification);
	spreadLabel.setFont(juce::Font(juce::Font::getSystemUIFontName(), 25, 1));
	spreadLabel.attachToComponent(&spreadSlider, false);
	addAndMakeVisible(&spreadLabel);

	// Intialization of SliderAttachment to maintain a connection betwwen sliders and parameters of the Value tree
	delayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "delay", delaySlider);
	depthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "depth", depthSlider);
	chorusMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "chorusMix", chorusMixSlider);
	rateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "rate", rateSlider);
	feedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "feedback", feedbackSlider);
	spreadAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "spread", spreadSlider);

	// Load logo of the VST
	const char* imageData = BinaryData::chorus_effect_logo_png;
	const int imageDataSize = BinaryData::chorus_effect_logo_pngSize;
	if (imageData != nullptr && imageDataSize > 0)
	{
		MemoryInputStream inputStream(imageData, imageDataSize, false);
		vst_logo = ImageFileFormat::loadFrom(inputStream);
	}
	else
	{
		DBG("Image file not found");
		vst_logo = Image();
	}
}

ChorusEffect_VSTAudioProcessorEditor::~ChorusEffect_VSTAudioProcessorEditor()
{
}

//==============================================================================
void ChorusEffect_VSTAudioProcessorEditor::paint(juce::Graphics& g)
{


	auto bounds = getLocalBounds().toFloat();
	auto centreX = bounds.getCentreX();

	// The bachground of the UI is a color gradient
	juce::Colour bottomColour = juce::Colours::black;
	juce::Colour topColour = juce::Colours::darkslategrey;
	juce::ColourGradient gradient(topColour, centreX, 0, bottomColour, centreX, bounds.getBottom(), 0);
	g.setFillType(gradient);
	g.fillRect(bounds);

	// Add logo to the UI
	float image_width = (float)vst_logo.getWidth();
	float image_height = (float)vst_logo.getHeight();
	float rate = image_height / image_width;
	int logo_width = 200;
	int logo_height = logo_width * rate;
	g.drawImage(vst_logo, centreX - logo_width / 2, bounds.getBottom() - logo_height - 10, logo_width, logo_height, 0, 0, image_width, image_height);


}

void ChorusEffect_VSTAudioProcessorEditor::resized()
{

	// UI elements are placed in the window bounds

	int linearSlider_width = 120;
	int rotarySlider_width = 170;
	int offsetX = (int)(getLocalBounds().getWidth() - (linearSlider_width * 2 + rotarySlider_width * 2)) / 5;
	int offsetY = 40;

	delaySlider.setBounds(spreadSlider.getX() + linearSlider_width + offsetX, offsetY, rotarySlider_width, rotarySlider_width);
	depthSlider.setBounds(spreadSlider.getX() + linearSlider_width + offsetX, delaySlider.getY() + rotarySlider_width + 55, rotarySlider_width, rotarySlider_width);
	feedbackSlider.setBounds(delaySlider.getX() + rotarySlider_width + offsetX, delaySlider.getY() + rotarySlider_width + 55, rotarySlider_width, rotarySlider_width);
	rateSlider.setBounds(delaySlider.getX() + rotarySlider_width + offsetX, offsetY, rotarySlider_width, rotarySlider_width);

	chorusMixSlider.setBounds(feedbackSlider.getX() + rotarySlider_width + offsetX, offsetY, linearSlider_width, getHeight() - 40);
	spreadSlider.setBounds(offsetX, offsetY, linearSlider_width, getHeight() - 40);

}
