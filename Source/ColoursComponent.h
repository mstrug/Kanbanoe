/*
  ==============================================================================

    ColoursComponent.h
    Created: 13 Dec 2018 10:55:21pm
    Author:  Michał

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ColourPalette.h"


using namespace juce;

//==============================================================================
/*
*/
class ColoursComponentListener
{
public:
	virtual void ColorChanged(int aSelectedColorIdx) = 0;
};


class ColoursComponent    : public Component
{
public:
    ColoursComponent( int xColourCount, int yColourCount, ColourPalette& aPalette, int aIdxSelected );
    ~ColoursComponent();

	int getSelectedColourIdx();
	void setListener(ColoursComponentListener* aListener);
	
	void paintOverChildren(Graphics&) override;
    void resized() override;

	int getColoursWidth();

private:

	ColoursComponentListener* iListener;

	OwnedArray< OwnedArray< TextButton > > iButtons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ColoursComponent)
};
