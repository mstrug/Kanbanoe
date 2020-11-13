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
//==============================================================================
/*
*/
class CKanbanColumnComponent : public juce::Component, public ScrollBar::Listener
{
public:
	CKanbanColumnComponent();
	~CKanbanColumnComponent() override;

	void paint(juce::Graphics&) override;
	void paintOverChildren(Graphics & g) override;
	void resized() override;

	void setActiveFrame(bool aActive);
	void contentUpdated();

public: // from ScrollBar::Listener

	void scrollBarMoved(ScrollBar* scrollBarThatHasMoved, double newRangeStart) override;

private:

	bool iIsFrameActive;

	Label iTitle;

	ScrollBar iScrollBar;

	CKanbanColumnContentComponent iViewportLayout;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CKanbanColumnComponent)
};


