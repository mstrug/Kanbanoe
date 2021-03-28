/*
  ==============================================================================

    ColourPalette.h
    Created: 13 Dec 2018 11:47:16pm
    Author:  Michał

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


using namespace juce;


class ColourPalette
{
public:
	
	ColourPalette(int aCount);

	int getColorCount();

	Colour getColor(int aIdx);

	Colour getLastColor();

	int getColourIndex(const Colour& aColor);

	void setColor(int aIdx, const Colour& aColor, const String& aName = "");

	Colour getColorActive(int aIdx);

	void setColorActive(int aIdx, const Colour& aColor);

	void setColorsActiveBrighter(float aValue);

	String getColourName(const Colour& aColor);

	String getColourName(int aIdx);

private:

	Array< Colour > iColors;
	Array< Colour > iColorsActive;
	StringArray iColorsNames;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColourPalette)
};
