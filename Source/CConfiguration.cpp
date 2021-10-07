/*
  ==============================================================================

    CConfiguration.cpp
    Created: 10 Nov 2020 12:46:04pm
    Author:  michal.strug

  ==============================================================================
*/

#include "CConfiguration.h"

static CConfiguration* cfg = nullptr;


CConfiguration::CConfiguration()
{
	PropertiesFile::Options opt;
	opt.applicationName = "Test03";
	opt.filenameSuffix = ".cfg";
	opt.folderName = "Test03";
	opt.commonToAllUsers = false;
	opt.ignoreCaseOfKeyNames = true;
	opt.millisecondsBeforeSaving = 0;

	iFile = new PropertiesFile(opt);

	int cc = iFile->getIntValue("ColoursCount");

	iPalette = new ColourPalette(cc);
	for (int i = 0; i < cc; i++)
	{
		auto s = iFile->getValue("Colour_" + String(i));
		auto sname = iFile->getValue("ColourName_" + String(i));
		iPalette->setColor(i, Colour::fromString(s), sname);
	}

	for (int i = 0; i < KRecentlyOpenedMenuItemIdCount; i++)
	{
		auto s = iFile->getValue("RecentlyOpened_" + String(i));
		if (s.isNotEmpty())
		{
			iRecentlyOpened.add(s);
		}
	}

	for (int i = 0; i < KRecentlyOpenedGroupMenuItemIdCount; i++)
	{
		auto s = iFile->getValue("RecentlyOpenedGroup_" + String(i));
		if (s.isNotEmpty())
		{
			iRecentlyOpenedGroup.add(s);
		}
	}
}

CConfiguration::~CConfiguration()
{
	iFile->saveIfNeeded();
	delete iFile;
	delete iPalette;
}

void CConfiguration::Destroy()
{
	delete cfg;
	cfg = nullptr;
}

PropertiesFile * CConfiguration::getPropertiesFile()
{
	return iFile;
}

Array<String>& CConfiguration::RecentlyOpened(bool aReturnGroup)
{
	if (aReturnGroup) return iRecentlyOpenedGroup;
	return iRecentlyOpened;
}

void CConfiguration::updateRecentlyOpenedMenu(PopupMenu & aMenu)
{
	for (int i = 0; i < iRecentlyOpened.size(); i++)
	{
		aMenu.addItem(KRecentlyOpenedMenuItemIdBase + i, iRecentlyOpened[i]);
	} 
	if (iRecentlyOpenedGroup.size() > 0) aMenu.addSeparator();
	for (int i = 0; i < iRecentlyOpenedGroup.size(); i++)
	{
		aMenu.addItem(KRecentlyOpenedGroupMenuItemIdBase + i, iRecentlyOpenedGroup[i]);
	}
}

String CConfiguration::getRecentlyOpened(int aIdx, bool aReturnGroup)
{
	if (aReturnGroup)
	{
		if (aIdx < iRecentlyOpenedGroup.size()) return iRecentlyOpenedGroup[aIdx];
	}
	else if (aIdx < iRecentlyOpened.size()) return iRecentlyOpened[aIdx];
	return String();
}

void CConfiguration::addRecentlyOpened(const String & aFn, bool aReturnGroup)
{
	Array<String>* a = (aReturnGroup ? &iRecentlyOpenedGroup : &iRecentlyOpened);
	int idx = a->indexOf(aFn);
	if (idx == 0)
	{
		return;
	}
	else if ( idx > 0 )
	{
		a->move(idx, 0);
	}
	else
	{
		a->insert(0, aFn);
		a->removeRange((aReturnGroup ? KRecentlyOpenedGroupMenuItemIdCount : KRecentlyOpenedMenuItemIdCount), a->size());
	}

	for (int i = 0; i < a->size(); i++)
	{
		iFile->setValue((aReturnGroup ? "RecentlyOpenedGroup_" : "RecentlyOpened_" )+ String(i), (*a)[i]);
	}
	iFile->save();
}

CConfiguration & CConfiguration::getInstance()
{
	if (!cfg)
	{
		cfg = new CConfiguration();
	}
	return *cfg;
}

String CConfiguration::getValue(StringRef aPropertyName)
{
	CConfiguration& c = getInstance();
	return c.iFile->getValue(aPropertyName);
}

int CConfiguration::getIntValue(StringRef aPropertyName)
{
	CConfiguration& c = getInstance();
	return c.iFile->getIntValue(aPropertyName);
}

bool CConfiguration::getBoolValue(StringRef aPropertyName)
{
	CConfiguration& c = getInstance();
	return c.iFile->getBoolValue(aPropertyName);
}

ColourPalette& CConfiguration::getColourPalette()
{
	CConfiguration& c = getInstance();
	return *c.iPalette;
}

int CConfiguration::WeekOfYear()
{
	// basing on https://en.wikipedia.org/wiki/ISO_week_date
	juce::Time t = juce::Time::getCurrentTime();
	int doy = t.getDayOfYear() + 1;	//d
	int dow = t.getDayOfWeek(); //e
	if (dow == 0) dow = 7; // saturday in ISO is day no. 7
	int woy = (10 + doy - dow) / 7;
	if (woy == 0)
	{
		double y = t.getYear() - 1;
		int p = int(y + floor(y / 4.0f) - floor(y / 100.0f) + floor(y / 400.0f)) % 7;
		y = y - 1;
		int p_1 = int(y + floor(y / 4.0f) - floor(y / 100.0f) + floor(y / 400.0f)) % 7;
		int weeks = 52;
		if (p == 4 || p_1 == 3) weeks++;
		woy = weeks;
	}
	else if (woy >= 53)
	{
		double y = t.getYear();
		int p = int(y + floor(y / 4.0f) - floor(y / 100.0f) + floor(y / 400.0f)) % 7;
		y = y - 1;
		int p_1 = int(y + floor(y / 4.0f) - floor(y / 100.0f) + floor(y / 400.0f)) % 7;
		int weeks = 52;
		if (p == 4 || p_1 == 3) weeks++;
		if (woy > weeks) woy = 1;
	}

	return woy;
}

String CConfiguration::YearAndWeekOfYear()
{
	juce::Time t = juce::Time::getCurrentTime();
	int woy = CConfiguration::WeekOfYear();
	return String(t.getYear()) + " " + String::formatted("wk%02d", woy);
}

int CConfiguration::getColumnTypesCount()
{
	return 2;
}

StringArray CConfiguration::getColumnTypesNames()
{
	return{ "Normal column", "Gitlab integration" };
}
