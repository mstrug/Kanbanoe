/*
  ==============================================================================

    CKanbanCardComponent.h
    Created: 4 Nov 2020 5:20:44pm
    Author:  michal.strug

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
using namespace juce;

class CKanbanColumnContentComponent;

class CKanbanCardComponent  : public juce::Component
{
public:
    CKanbanCardComponent(CKanbanColumnContentComponent* aOwner);
    ~CKanbanCardComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
	void mouseDown(const MouseEvent& event) override;
	void mouseDrag(const MouseEvent& event) override;
	void mouseUp(const MouseEvent& event) override;
	void mouseDoubleClick(const MouseEvent& event) override;

	CKanbanColumnContentComponent* getOwner();
	void setOwner(CKanbanColumnContentComponent* aOwner);

	String name;
	FlexItem* iFlexItem;

private:

	void showProperties();


private:

	ComponentDragger iDragger;

	bool iIsDragging;

	TextButton iEditButton;

	CKanbanColumnContentComponent* iOwner;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CKanbanCardComponent)
};

const String KanbanCardComponentDragDescription = "KanbanCard";