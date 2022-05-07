/*
  ==============================================================================

    ColourPalette.cpp
    Created: 13 Dec 2018 11:47:16pm
    Author:  Michał

  ==============================================================================
*/

#include "ColourPalette.h"



ColourPalette::ColourPalette(int aCount)
{
	for (int i = 0; i < aCount; i++)
	{
		iColors.add(Colour());
		iColorsActive.add(Colour());
		iColorsNames.add(String());
	}
}

int ColourPalette::getColorCount()
{
	return iColors.size();
}

Colour ColourPalette::getColor(int aIdx)
{
	if (aIdx >= iColors.size()) return iColors[aIdx % iColors.size()];
	if (aIdx >= 0 && aIdx < iColors.size()) return iColors[aIdx];
	else return Colour();
}

Colour ColourPalette::getLastColor()
{
	if (iColors.size() > 0)
	{
		return iColors[iColors.size() - 1];
	}
	return Colour();
}

int ColourPalette::getColourIndex(const Colour& aColor)
{
	return iColors.indexOf(aColor);
}

void ColourPalette::setColor(int aIdx, const Colour& aColor, const String& aName)
{
	iColors.set( aIdx, aColor );
	iColorsNames.set(aIdx, aName);
}

Colour ColourPalette::getColorActive(int aIdx)
{
	if (aIdx >= iColorsActive.size()) return iColorsActive[aIdx % iColors.size()];
	return iColorsActive[aIdx];
}

void ColourPalette::setColorActive(int aIdx, const Colour& aColor)
{
	iColorsActive.set(aIdx, aColor);
}

void ColourPalette::setColorsActiveBrighter(float aValue)
{
	for (int i = 0; i < iColorsActive.size(); i++)
	{
		setColorActive(i, iColors[i].brighter(aValue));
	}
}

String ColourPalette::getColourName(const Colour & aColor)
{
	int idx = iColors.indexOf(aColor);
	return getColourName(idx);
}

String ColourPalette::getColourName(int aIdx)
{
	if (aIdx >= iColorsNames.size()) return iColorsNames[aIdx % iColorsNames.size()];
	return iColorsNames[aIdx];
}
