/*
  ==============================================================================

    CTimer.h
    Created: 23 Mar 2021 9:24:39pm
    Author:  michal.strug

  ==============================================================================
*/

#pragma once


#include <JuceHeader.h>


using namespace juce;


class MainComponent;


class CTimer: public Timer
{
	MainComponent& iOwner;

public:

	CTimer(MainComponent& aOwner);

	void Start();

	void timerCallback() override;

};

