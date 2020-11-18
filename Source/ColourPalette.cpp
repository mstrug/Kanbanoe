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
	for (int i = 0; i < aCount; i++) iColors.add(Colour());
	for (int i = 0; i < aCount; i++) iColorsActive.add(Colour());
}

int ColourPalette::getColorCount()
{
	return iColors.size();
}

Colour ColourPalette::getColor(int aIdx)
{
	if (aIdx >= iColors.size()) return iColors[aIdx % iColors.size()];
	return iColors[aIdx];
}

void ColourPalette::setColor(int aIdx, const Colour& aColor)
{
	iColors.set( aIdx, aColor );
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
