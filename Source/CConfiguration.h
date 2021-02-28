/*
  ==============================================================================

    CConfiguration.h
    Created: 10 Nov 2020 12:46:04pm
    Author:  michal.strug

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ColourPalette.h"

using namespace juce;

class CConfiguration
{
private:
	CConfiguration();

	virtual ~CConfiguration();

public:

	void Destroy();

	PropertiesFile* getPropertiesFile();

	static CConfiguration& getInstance();

	static String getValue(StringRef aPropertyName);

	static int getIntValue(StringRef aPropertyName);

	static ColourPalette& getColourPalette();

	static int WeekOfYear();

	static String YearAndWeekOfYear();

private:

	PropertiesFile* iFile;

	ColourPalette* iPalette;

};

