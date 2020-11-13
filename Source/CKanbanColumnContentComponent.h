/*
  ==============================================================================

    CKanbanColumnContentComponent.h
    Created: 13 Nov 2020 9:16:10am
    Author:  michal.strug

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class CKanbanColumnContentComponent  : public juce::Component
{
public:
    CKanbanColumnContentComponent();
    ~CKanbanColumnContentComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CKanbanColumnContentComponent)
};
