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
class CKanbanColumnContentComponent : public juce::Component, public DragAndDropTarget//, public ScrollBar::Listener
{
public:
	CKanbanColumnContentComponent();
    ~CKanbanColumnContentComponent() override;

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

	void scrollBarMoved1(ScrollBar *scrollBarThatHasMoved, double newRangeStart) ;

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



	ScrollBar iScrollBar;

	friend class CKanbanColumnComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CKanbanColumnContentComponent)
};




class CKanbanColumnComponent : public juce::Component
{
public:
	CKanbanColumnComponent();
	~CKanbanColumnComponent() override;

	void paint(juce::Graphics&) override;
	void paintOverChildren(Graphics & g) override;
	void resized() override;

private:

	Label iTitle;

	Viewport iViewport;
	CKanbanColumnContentComponent iViewportLayout;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CKanbanColumnComponent)
};


