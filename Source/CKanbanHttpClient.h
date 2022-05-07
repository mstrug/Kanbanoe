/*
  ==============================================================================

    CKanbanHttpClient.h
    Created: 21 Sep 2021 12:01:36am
    Author:  michal.strug

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

using namespace juce;


class CHttpClient
{
public:

    static int Get(const String & aIp, uint16 aPort, const String& aUrl, const String& aBody, const String& aAdditionalHeaders, String& aRetrunCode, String& aRetrunMessage );
    
};
