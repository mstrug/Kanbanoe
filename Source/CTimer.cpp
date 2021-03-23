/*
  ==============================================================================

    CTimer.cpp
    Created: 23 Mar 2021 9:24:39pm
    Author:  michal.strug

  ==============================================================================
*/

#include "CTimer.h"
#include "MainComponent.h"



CTimer::CTimer(MainComponent& aOwner): iOwner(aOwner)
{
}

void CTimer::Start()
{
	Time t(Time::getCurrentTime());
	Time t2(t.getYear(), t.getMonth(), t.getDayOfMonth(), t.getHours(), t.getMinutes());
	int deltasec = 24 * 3600 - (t.getHours() * 3600 + t.getMinutes() * 60 + t.getSeconds());

	Logger::outputDebugString("timer started [sec]: " + String(deltasec));

	startTimer((deltasec + 2) * 1000);
}

void CTimer::timerCallback()
{
	startTimer(24 * 3600 * 1000); // call again after 24h
	Logger::outputDebugString("timer called: " + Time::getCurrentTime().toString(true, true, true, true));

	iOwner.updateTimer24h();
}
