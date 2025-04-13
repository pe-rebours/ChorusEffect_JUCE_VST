/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ChorusEffect_VSTAudioProcessor::ChorusEffect_VSTAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
		//#if ! JucePlugin_IsMidiEffect
		 //#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
		//#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
		//#endif
	), apvts(*this, nullptr, "Parameters", createParameterLayout())
#else
	: apvts(*this, nullptr, "Parameters", createParameterLayout())
#endif
{
	apvts.addParameterListener("rate", this);
	apvts.addParameterListener("depth", this);
	apvts.addParameterListener("delay", this);
	apvts.addParameterListener("feedback", this);
	apvts.addParameterListener("chorusMix", this);
}

ChorusEffect_VSTAudioProcessor::~ChorusEffect_VSTAudioProcessor()
{
	apvts.removeParameterListener("rate", this);
	apvts.removeParameterListener("depth", this);
	apvts.removeParameterListener("delay", this);
	apvts.removeParameterListener("feedback", this);
	apvts.removeParameterListener("chorusMix", this);
}

//==============================================================================
const juce::String ChorusEffect_VSTAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool ChorusEffect_VSTAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool ChorusEffect_VSTAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool ChorusEffect_VSTAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double ChorusEffect_VSTAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int ChorusEffect_VSTAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int ChorusEffect_VSTAudioProcessor::getCurrentProgram()
{
	return 0;
}

void ChorusEffect_VSTAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String ChorusEffect_VSTAudioProcessor::getProgramName(int index)
{
	return {};
}

void ChorusEffect_VSTAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void ChorusEffect_VSTAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	float delay_max = apvts.getParameterRange("delay").end;
	float depth_max = apvts.getParameterRange("depth").end;
	int size_delay_buffer = int((depth_max * delay_max + depth_max) * sampleRate / 1000);

	delay_buffers[0].resize(size_delay_buffer, 0);
	delay_buffers[1].resize(size_delay_buffer, 0);

	current_sample = { 0,0 };

	juce::dsp::ProcessSpec spec;
	spec.maximumBlockSize = samplesPerBlock;
	spec.sampleRate = sampleRate;
	chorus.prepare(spec);
	chorus.setFeedback(0.2f);
	chorus.reset();
}

void ChorusEffect_VSTAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ChorusEffect_VSTAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
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

void ChorusEffect_VSTAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	float delay = apvts.getRawParameterValue("delay")->load();
	float chorus_mix = apvts.getRawParameterValue("chorusMix")->load();
	float depth = apvts.getRawParameterValue("depth")->load();

	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();
	auto sampleRate = getSampleRate();

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());



	inputBuffer.setSize(buffer.getNumChannels(), buffer.getNumSamples(), false, false, true);
	inputBuffer = buffer;
	// Chorus effect is applied to the samples
	juce::dsp::AudioBlock<float> sampleBlock(buffer);
	chorus.process(juce::dsp::ProcessContextReplacing<float>(sampleBlock));

	// Add stereo spread
	float spread = apvts.getRawParameterValue("spread")->load();;
	for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {

		float left = buffer.getSample(0, sample);
		float right = buffer.getSample(1, sample);

		// calculation of mid and side sample
		float mid = (left + right) * 0.5f;
		float side = (left - right) * 0.5f;

		// application of stereo spread
		side *= spread;

		// output to left and right channels
		buffer.setSample(0, sample, mid + side);
		buffer.setSample(1, sample, mid - side);


	}
}

//==============================================================================
bool ChorusEffect_VSTAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ChorusEffect_VSTAudioProcessor::createEditor()
{
	return new ChorusEffect_VSTAudioProcessorEditor(*this);
}

//==============================================================================
void ChorusEffect_VSTAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{

}

void ChorusEffect_VSTAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{

}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new ChorusEffect_VSTAudioProcessor();
}


juce::AudioProcessorValueTreeState::ParameterLayout ChorusEffect_VSTAudioProcessor::createParameterLayout()
{
	// Layout is created fo each parameters of the VST
	std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
	params.push_back(std::make_unique<juce::AudioParameterFloat>("delay", "Delay", 0.0f, 100.0f, 10.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>("rate", "LFO rate", 0.0f, 10.0f, 0.2f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>("feedback", "Feedback", -1.0f, 1.0f, 0.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>("depth", "Depth", 0.0f, 1.0f, 0.10f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>("chorusMix", "Chorus Mix", 0.0f, 1.0f, 0.5f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>("spread", "Spread", 0.0f, 2.0f, 1.0f));

	return { params.begin(), params.end() };
}


void ChorusEffect_VSTAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
	// Update parameters of chorus

	if (parameterID == "rate")
		chorus.setRate(newValue);

	if (parameterID == "depth")
		chorus.setDepth(newValue);

	if (parameterID == "delay")
		chorus.setCentreDelay(newValue);

	if (parameterID == "feedback")
		chorus.setFeedback(newValue);

	if (parameterID == "chorusMix")
		chorus.setMix(newValue);
}
void ChorusEffect_VSTAudioProcessor::reset()
{
	chorus.reset();
}