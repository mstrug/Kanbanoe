/*
  ==============================================================================

    CKanbanBoardArchive.h
    Created: 29 Mar 2021 1:35:44am
    Author:  michal.strug

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class CKanbanBoardArchive  : public juce::Component
{
public:
    CKanbanBoardArchive();
    ~CKanbanBoardArchive() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CKanbanBoardArchive)
};
