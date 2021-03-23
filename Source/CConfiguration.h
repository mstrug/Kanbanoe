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

const int KRecentlyOpenedMenuItemIdBase = 0xff001;
const int KRecentlyOpenedMenuItemIdCount = 8;
const int KRecentlyOpenedGroupMenuItemIdBase = 0xff011;
const int KRecentlyOpenedGroupMenuItemIdCount = 4;

class CConfiguration
{
private:
	CConfiguration();

	virtual ~CConfiguration();

public:

	void Destroy();

	PropertiesFile* getPropertiesFile();

	Array<String>& RecentlyOpened(bool aReturnGroup = false);
	void updateRecentlyOpenedMenu(PopupMenu& aMenu);
	String getRecentlyOpened(int aIdx, bool aReturnGroup = false);
	void addRecentlyOpened(const String& aFn, bool aReturnGroup = false);

	static CConfiguration& getInstance();

	static String getValue(StringRef aPropertyName);

	static int getIntValue(StringRef aPropertyName);

	static ColourPalette& getColourPalette();

	static int WeekOfYear();

	static String YearAndWeekOfYear();

private:

	PropertiesFile* iFile;

	ColourPalette* iPalette;

	Array<String> iRecentlyOpened;

	Array<String> iRecentlyOpenedGroup;

};

