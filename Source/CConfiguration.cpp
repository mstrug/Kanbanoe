/*
  ==============================================================================

    CConfiguration.cpp
    Created: 10 Nov 2020 12:46:04pm
    Author:  michal.strug

  ==============================================================================
*/

#include "CConfiguration.h"
#include "MainComponent.h"

static CConfiguration* cfg = nullptr;


CConfiguration::CConfiguration()
{
	PropertiesFile::Options opt;
	opt.applicationName = "Kanbanoe";
	opt.filenameSuffix = ".cfg";
	opt.folderName = "Kanbanoe";
	opt.commonToAllUsers = false;
	opt.ignoreCaseOfKeyNames = true;
	opt.millisecondsBeforeSaving = 0;

	iFile = new PropertiesFile(opt);
	verifyFile();

	//opt.getDefaultFile().getFullPathName()

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

String CConfiguration::getConfigurationFileLocation()
{
	return iFile->getFile().getFullPathName();
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
	if (aPropertyName == String("curl") && c.iFile->getValue(aPropertyName).isEmpty())
	{
		File pathToMyExecutable = File::getSpecialLocation(File::currentExecutableFile).getSiblingFile("curl.exe");
		return pathToMyExecutable.getFullPathName();
	}
	return c.iFile->getValue(aPropertyName);
}

int CConfiguration::getIntValue(StringRef aPropertyName)
{
	CConfiguration& c = getInstance();
	return c.iFile->getIntValue(aPropertyName);
}

float CConfiguration::getFloatValue(StringRef aPropertyName)
{
	CConfiguration& c = getInstance();
	return (float)c.iFile->getDoubleValue(aPropertyName);
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

// returns year and week number
std::pair<int, int> CConfiguration::WeekOfYear()
{
	// basing on https://en.wikipedia.org/wiki/ISO_week_date
	juce::Time t = juce::Time::getCurrentTime();
	int year = t.getYear();
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
		if (woy > weeks) {
			woy = 1;
			year++;
		}
	}

	return std::make_pair(year, woy);
}

String CConfiguration::YearAndWeekOfYear()
{
	std::pair<int, int> woy = CConfiguration::WeekOfYear();
	return String(woy.first) + " " + String::formatted("wk%02d", woy.second);
}

int CConfiguration::getColumnTypesCount()
{
	auto& c = getInstance();
	if (!c.iFile->containsKey("KanbanColumnTypesCount"))
	{
		return 1;
	}
	return c.iFile->getIntValue("KanbanColumnTypesCount");
}

StringArray CConfiguration::getColumnTypesNames()
{
	auto& c = getInstance();
	if (!c.iFile->containsKey("KanbanColumnTypesCount"))
	{
		return { "Normal column" };
	}
	int cnt = c.iFile->getIntValue("KanbanColumnTypesCount");
	if (cnt == 1)
	{
		return{ "Normal column" };
	}

	StringArray ret;
	for (int i = 0; i < cnt; i++)
	{
		auto s = c.iFile->getValue("KanbanColumnTypeName_" + String(i));
		ret.add(s);
	}
	return ret;  // { "Normal column", "Gitlab integration" };
}


void CConfiguration::showStatusbarMessage(StringRef aMessage)
{
	auto& c = CConfiguration::getInstance();
	c.iStatusbarLock.enter();
	c.iStatusbarMsg = aMessage;
	c.iStatusbarLock.exit();
	JUCEApplication::getInstance()->invokeDirectly(MainComponent::CommandIDs::statusbarMessage, true);
}

String CConfiguration::getStatusbarMessage()
{
	String tmp;
	auto& c = CConfiguration::getInstance();
	c.iStatusbarLock.enter();
	tmp = c.iStatusbarMsg;
	c.iStatusbarLock.exit();
	return tmp;
}

void CConfiguration::verifyFile()
{
	if (!iFile->containsKey("KanbanCardWidth"))
	{
		iFile->setValue("KanbanCardWidth", "240");
	}
	if (!iFile->containsKey("KanbanCardHeight"))
	{
		iFile->setValue("KanbanCardHeight", "40");
	}
	if (!iFile->containsKey("KanbanCardHorizontalMargin"))
	{
		iFile->setValue("KanbanCardHorizontalMargin", "10");
	}
	if (!iFile->containsKey("KanbanPlaceholderCardHeight"))
	{
		iFile->setValue("KanbanPlaceholderCardHeight", "4");
	}
	if (!iFile->containsKey("KanbanPlaceholderCardFrameSize"))
	{
		iFile->setValue("KanbanPlaceholderCardFrameSize", "2");
	}
	if (!iFile->containsKey("KanbanColumnMinimizedWidth"))
	{
		iFile->setValue("KanbanColumnMinimizedWidth", "34");
	}
	if (!iFile->containsKey("ColoursCount"))
	{
		iFile->setValue("ColoursCount", 6);
		iFile->setValue("Colour_0", "ffc71518");
		iFile->setValue("Colour_1", "ffffd700");
		iFile->setValue("Colour_2", "ff32cd32");
		iFile->setValue("Colour_3", "ff1e90ff");
		iFile->setValue("Colour_4", "fff5f5f5");
		iFile->setValue("Colour_5", "00000000");
		iFile->setValue("ColourName_0", "red");
		iFile->setValue("ColourName_1", "yellow");
		iFile->setValue("ColourName_2", "green");
		iFile->setValue("ColourName_3", "blue");
		iFile->setValue("ColourName_4", "white");
		iFile->setValue("ColourName_5", "none");
	}
	if (!iFile->containsKey("ConfigSearchCaseInsensitive"))
	{
		iFile->setValue("ConfigSearchCaseInsensitive", "1");
	}
	if (!iFile->containsKey("ConfigSearchDynamic"))
	{
		iFile->setValue("ConfigSearchDynamic", "1");
	}
	if (!iFile->containsKey("ConfigAutosave"))
	{
		iFile->setValue("ConfigAutosave", "0");
	}
	if (!iFile->containsKey("UiFontTypeFace"))
	{
	    // if empty in config -> use default system typeface
		//iFile->setValue("UiFontTypeFace", "Arial");
	}
	if (!iFile->containsKey("KanbanCardFontTypeFace"))
	{
	    // if empty in config -> use default system typeface
		//iFile->setValue("KanbanCardFontTypeFace", "Roboto");
	}
	if (!iFile->containsKey("KanbanCardFontSize"))
	{
		iFile->setValue("KanbanCardFontSize", "12.0");
	}
	if (!iFile->containsKey("KanbanCardDataFontSize"))
	{
		iFile->setValue("KanbanCardDataFontSize", "12.0");
	}
	if (!iFile->containsKey("KanbanColumnTypesCount"))
	{
		iFile->setValue("KanbanColumnTypesCount", "3");
	}
	if (!iFile->containsKey("KanbanColumnTypeName_0"))
	{
		iFile->setValue("KanbanColumnTypeName_0", "Normal column");
	}
	if (!iFile->containsKey("KanbanColumnTypeName_1"))
	{
		iFile->setValue("KanbanColumnTypeName_1", "Gitlab integration");
	}
	if (!iFile->containsKey("KanbanColumnTypeName_2"))
	{
		iFile->setValue("KanbanColumnTypeName_2", "Github integration");
	}
	iFile->saveIfNeeded();
}
