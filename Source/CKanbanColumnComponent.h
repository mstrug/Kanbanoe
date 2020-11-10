/*
  ==============================================================================

    CKanbanColumnComponent.h
    Created: 9 Nov 2020 5:11:55pm
    Author:  michal.strug

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CKanbanColumnCardPlaceholderComponent.h"

using namespace juce;
//==============================================================================
/*
*/
class CKanbanColumnComponent  : public juce::Component, public DragAndDropTarget
{
public:
    CKanbanColumnComponent();
    ~CKanbanColumnComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

public: // from DragAndDropTarget

	bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override;

	void itemDragEnter(const SourceDetails& dragSourceDetails) override;

	void itemDragExit(const SourceDetails& dragSourceDetails) override;

	void itemDropped(const SourceDetails& dragSourceDetails) override;

private:

	bool iDragTargetActive;

	OwnedArray< CKanbanColumnCardPlaceholderComponent > iPlaceholders;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CKanbanColumnComponent)
};
