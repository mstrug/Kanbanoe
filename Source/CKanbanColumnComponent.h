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
class CKanbanColumnComponent  : public juce::Component, public DragAndDropTarget, public ScrollBar::Listener
{
public:
    CKanbanColumnComponent();
    ~CKanbanColumnComponent() override;

    void paint (juce::Graphics&) override;
	void paintOverChildren(Graphics& g) override;
	void resized() override;

	void removeCard(CKanbanCardComponent* aCard);

public: // from DragAndDropTarget

	bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override;

	void itemDragEnter(const SourceDetails& dragSourceDetails) override;

	void itemDragMove(const SourceDetails& dragSourceDetails) override;

	void itemDragExit(const SourceDetails& dragSourceDetails) override;

	void itemDropped(const SourceDetails& dragSourceDetails) override;

public: // from ScrollBar::Listener

	void scrollBarMoved(ScrollBar *scrollBarThatHasMoved, double newRangeStart) override;

private:

	void addCard(CKanbanCardComponent* aCardComponent);

private:

	bool iDragTargetActive;

	bool iDragTargetPlaceholderActive;

	int iPlaceholderIndex;

	int iDraggedCardIndex;

	Rectangle< int > iPlaceholderActiveRect;

	FlexBox iLayout;

	OwnedArray< CKanbanColumnCardPlaceholderComponent > iPlaceholders;

	Label iTitle;

	Viewport iViewport;
	Component iViewportLayout;

	ScrollBar iScrollBar;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CKanbanColumnComponent)
};
