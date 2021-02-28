/*
  ==============================================================================

    CKanbanColumnContentComponent.h
    Created: 13 Nov 2020 9:16:10am
    Author:  michal.strug

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CKanbanCardComponent.h"

using namespace juce;

class CKanbanColumnComponent;

//==============================================================================
/*
*/
class CKanbanColumnContentComponent : public juce::Component, public DragAndDropTarget
{
public:
	CKanbanColumnContentComponent(CKanbanColumnComponent& aOwner);
	~CKanbanColumnContentComponent() override;

	void paint(juce::Graphics&) override;
	void paintOverChildren(Graphics& g) override;
	void resized() override;

	void removeCard(CKanbanCardComponent* aCard);
	void addCard(CKanbanCardComponent* aCard);
	void hideCard(CKanbanCardComponent* aCard);
	void unhideAllCards();
	void createNewCard();
	void updateSize();
	void setMinimumHeight(int aHeight);
	CKanbanColumnComponent& getOwner();
	const FlexBox& getLayout();
	int getCardsCount();

public: // from DragAndDropTarget

	bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override;

	void itemDragEnter(const SourceDetails& dragSourceDetails) override;

	void itemDragMove(const SourceDetails& dragSourceDetails) override;

	void itemDragExit(const SourceDetails& dragSourceDetails) override;

	void itemDropped(const SourceDetails& dragSourceDetails) override;
	
private:

	CKanbanColumnComponent& iOwner;

	bool iDragTargetActive;

	bool iDragTargetPlaceholderActive;

	int iPlaceholderIndex;

	int iDraggedCardIndex;

	Rectangle< int > iPlaceholderActiveRect;

	FlexBox iLayout;
	
	int iMinimumHeight;

	int iScrollPos;

	friend class CKanbanColumnComponent;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CKanbanColumnContentComponent)
};


