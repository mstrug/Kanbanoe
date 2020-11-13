/*
  ==============================================================================

    CKanbanBoard.h
    Created: 12 Nov 2020 4:50:24pm
    Author:  michal.strug

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CKanbanCardComponent.h"
#include "CKanbanColumnComponent.h"

using namespace juce;

//==============================================================================
/*
*/
class CKanbanBoard  : public juce::Component
{
public:
    CKanbanBoard();
    ~CKanbanBoard() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:

	OwnedArray< CKanbanCardComponent > iKanbanCards;

	OwnedArray< CKanbanColumnComponent > iKanbanColumns;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CKanbanBoard)
};
