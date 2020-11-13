/*
  ==============================================================================

    CKanbanColumnComponent.h
    Created: 9 Nov 2020 5:11:55pm
    Author:  michal.strug

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CKanbanColumnContentComponent.h"

using namespace juce;

class CKanbanBoardComponent;

//==============================================================================
/*
*/
class CKanbanColumnComponent : public juce::Component, public ScrollBar::Listener
{
public:
	CKanbanColumnComponent(CKanbanBoardComponent& aOwner);
	~CKanbanColumnComponent() override;

	void paint(juce::Graphics&) override;
	void paintOverChildren(Graphics & g) override;
	void resized() override;
	void mouseUp(const MouseEvent& event) override;

	void setActiveFrame(bool aActive);
	void contentUpdated();
	CKanbanBoardComponent& kanbanBoard();

public: // from ScrollBar::Listener

	void scrollBarMoved(ScrollBar* scrollBarThatHasMoved, double newRangeStart) override;

private:

	CKanbanBoardComponent& iOwner;

	bool iIsFrameActive;

	Label iTitle;

	ScrollBar iScrollBar;

	CKanbanColumnContentComponent iViewportLayout;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CKanbanColumnComponent)
};


