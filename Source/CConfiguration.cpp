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
		iPalette->setColor(i, Colour::fromString(s));
	}
}

CConfiguration::~CConfiguration()
{
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

ColourPalette& CConfiguration::getColourPalette()
{
	CConfiguration& c = getInstance();
	return *c.iPalette;
}
