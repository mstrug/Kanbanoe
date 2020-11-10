/*
  ==============================================================================

    CKanbanColumnCardPlaceholderComponent.h
    Created: 9 Nov 2020 5:20:24pm
    Author:  michal.strug

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

using namespace juce;
//==============================================================================
/*
*/
class CKanbanColumnCardPlaceholderComponent  : public juce::Component, public DragAndDropTarget
{
public:
    CKanbanColumnCardPlaceholderComponent();
    ~CKanbanColumnCardPlaceholderComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
	void childrenChanged() override;

public: // from DragAndDropTarget

	bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override;

	void itemDragEnter(const SourceDetails& dragSourceDetails) override;

	void itemDragExit(const SourceDetails& dragSourceDetails) override;

	void itemDropped(const SourceDetails& dragSourceDetails) override;


private:

	bool iDragTargetActive;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CKanbanColumnCardPlaceholderComponent)
};
