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

class CKanbanCardComponent  : public juce::Component
{
public:
    CKanbanCardComponent();
    ~CKanbanCardComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
	void mouseDown(const MouseEvent& event) override;
	void mouseDrag(const MouseEvent& event) override;
	void mouseUp(const MouseEvent& event) override;

	String name;
private:

	ComponentDragger iDragger;

	bool iIsDragging;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CKanbanCardComponent)
};

const String KanbanCardComponentDragDescription = "KanbanCard";