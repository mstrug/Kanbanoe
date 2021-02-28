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
class CKanbanCardComponent;

//==============================================================================
/*
*/
class CKanbanColumnComponent : public juce::Component, public ScrollBar::Listener
{
public:
	CKanbanColumnComponent(int aColumnId, const String& aTitle, CKanbanBoardComponent& aOwner);
	~CKanbanColumnComponent() override;

	void paint(juce::Graphics&) override;
	void paintOverChildren(Graphics & g) override;
	void resized() override;
	void mouseUp(const MouseEvent& event) override;
	void mouseWheelMove(const MouseEvent & event, const MouseWheelDetails & details) override;

	void setActiveFrame(bool aActive);
	void contentUpdated();
	CKanbanBoardComponent& kanbanBoard();
	CKanbanColumnContentComponent& cardsLayout();
	void addCard(CKanbanCardComponent* aCard);
	void removeCard(CKanbanCardComponent* aCard);
	void removeAllCards();
	void scrollToBottom();

	void search(const String& aString);

	String getTitle();
	int getColumnId() const;

public: // from ScrollBar::Listener

	void scrollBarMoved(ScrollBar* scrollBarThatHasMoved, double newRangeStart) override;

private:

	CKanbanBoardComponent& iOwner;

	int iColumnId;

	bool iIsFrameActive;

	Label iTitle;

	ScrollBar iScrollBar;

	TextButton iAddCardButton;

	CKanbanColumnContentComponent iViewportLayout;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CKanbanColumnComponent)
};


