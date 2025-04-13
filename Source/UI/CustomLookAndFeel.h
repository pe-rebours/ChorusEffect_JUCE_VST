#pragma once

#include <JuceHeader.h>

using namespace juce;

class CustomLookAndFeel : public LookAndFeel_V4
{
public:
	CustomLookAndFeel()
	{
		// Load cursor image for linear slider
		const char* imageData = BinaryData::slider_cursor_png;
		const int imageDataSize = BinaryData::slider_cursor_pngSize;
		if (imageData != nullptr && imageDataSize > 0)
		{
			MemoryInputStream inputStream(imageData, imageDataSize, false);
			cursorImage = ImageFileFormat::loadFrom(inputStream);
			setColour(Slider::trackColourId, Colours::dodgerblue);
		}
		else
		{
			DBG("Image file not found");
			cursorImage = Image();
			setColour(Slider::thumbColourId, Colours::blue);
		}

		// Load cursor image for rotary slider
		const char* imageData2 = BinaryData::rotary_slider_cursor_png;
		const int imageDataSize2 = BinaryData::rotary_slider_cursor_pngSize;
		if (imageData2 != nullptr && imageDataSize2 > 0)
		{
			MemoryInputStream inputStream2(imageData2, imageDataSize2, false);
			round_cursorImage = ImageFileFormat::loadFrom(inputStream2);
		}
		else
		{
			DBG("Image file not found");
			round_cursorImage = Image();
			setColour(Slider::thumbColourId, Colours::blue);
		}

		// tack color is set as blue
		setColour(Slider::trackColourId, Colours::dodgerblue);



	}

	Font getLabelFont(Label& label) override
	{
		//return label.getFont();
		return Font(Font::getSystemUIFontName(), 25, 0);
	}

	void drawLabel(Graphics& g, Label& label) override
	{
		g.fillAll(label.findColour(Label::backgroundColourId));

		if (!label.isBeingEdited())
		{
			auto alpha = label.isEnabled() ? 1.0f : 0.5f;
			const Font font(getLabelFont(label));

			g.setColour(label.findColour(Label::textColourId).withMultipliedAlpha(alpha));
			g.setFont(font);

			auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());

			g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
				jmax(1, (int)((float)textArea.getHeight() / font.getHeight())),
				label.getMinimumHorizontalScale());

			g.setColour(label.findColour(Label::outlineColourId).withMultipliedAlpha(alpha));
		}
		else if (label.isEnabled())
		{
			g.setColour(label.findColour(Label::outlineColourId));
		}
	}

	void drawLinearSlider(Graphics& g, int x, int y, int width, int height,
		float sliderPos,
		float minSliderPos,
		float maxSliderPos,
		const Slider::SliderStyle style, Slider& slider) override
	{

		auto trackWidth = jmin(6.0f, slider.isHorizontal() ? (float)height * 0.25f : (float)width * 0.25f);



		Point<float> startPoint(slider.isHorizontal() ? (float)x : (float)x + (float)width * 0.5f,
			slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));

		Point<float> endPoint(slider.isHorizontal() ? (float)(width + x) : startPoint.x,
			slider.isHorizontal() ? startPoint.y : (float)y);




		Path backgroundTrack;
		backgroundTrack.startNewSubPath(startPoint);
		backgroundTrack.lineTo(endPoint);
		g.setColour(slider.findColour(Slider::backgroundColourId));
		g.strokePath(backgroundTrack, { trackWidth * 2, PathStrokeType::curved, PathStrokeType::rounded });

		Path valueTrack;
		Point<float> minPoint, maxPoint, thumbPoint;


		auto kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
		auto ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;
		minPoint = startPoint;
		maxPoint = { kx, ky };


		auto thumbWidth = getSliderThumbRadius(slider);

		valueTrack.startNewSubPath(minPoint);
		valueTrack.lineTo(maxPoint);
		g.setColour(slider.findColour(Slider::trackColourId));
		g.strokePath(valueTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

		float image_width = (float)cursorImage.getWidth();
		float image_height = (float)cursorImage.getHeight();
		float rate = image_height / image_width;

		int cursor_width = trackWidth * 6;
		int cursor_height = cursor_width * rate;

		g.drawImage(cursorImage, x + width / 2 - cursor_width / 2, sliderPos - cursor_height / 2, cursor_width, cursor_height, 0, 0, cursorImage.getWidth(), cursorImage.getHeight());


	}

	void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
		const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
	{
		auto outline = slider.findColour(Slider::rotarySliderOutlineColourId);
		auto track_color = slider.findColour(Slider::trackColourId);
		auto fill = slider.findColour(Slider::rotarySliderFillColourId);

		auto bounds = Rectangle<int>(x, y, width, height).toFloat().reduced(10);

		auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
		auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
		auto lineW = jmin(6.0f, radius * 0.5f);
		auto arcRadius = radius - lineW * 0.5f;

		Path backgroundArc;
		backgroundArc.addCentredArc(bounds.getCentreX(),
			bounds.getCentreY(),
			arcRadius,
			arcRadius,
			0.0f,
			rotaryStartAngle,
			rotaryEndAngle,
			true);

		g.setColour(outline);
		g.strokePath(backgroundArc, PathStrokeType(lineW * 2, PathStrokeType::curved, PathStrokeType::rounded));

		if (slider.isEnabled())
		{
			Path valueArc;
			valueArc.addCentredArc(bounds.getCentreX(),
				bounds.getCentreY(),
				arcRadius,
				arcRadius,
				0.0f,
				rotaryStartAngle,
				toAngle,
				true);

			g.setColour(track_color);
			g.strokePath(valueArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));
		}

		auto thumbWidth = lineW * 2.0f;
		Point<float> thumbPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - MathConstants<float>::halfPi),
			bounds.getCentreY() + arcRadius * std::sin(toAngle - MathConstants<float>::halfPi));

		g.setColour(slider.findColour(Slider::thumbColourId));


		float image_width = (float)round_cursorImage.getWidth();
		float image_height = (float)round_cursorImage.getHeight();
		float rate = image_height / image_width;

		int cursor_width = thumbWidth * 2;
		int cursor_height = cursor_width * rate;


		g.drawImage(round_cursorImage, thumbPoint.getX() - cursor_width / 2, thumbPoint.getY() - cursor_height / 2, cursor_width, cursor_height, 0, 0, image_width, image_height);
	}

private:
	Image cursorImage;
	Image round_cursorImage;
};
