/*
  ==============================================================================

    CKanbanCardComponent.cpp
    Created: 4 Nov 2020 5:20:44pm
    Author:  michal.strug

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CKanbanCardComponent.h"
#include "Cconfiguration.h"

//==============================================================================
CKanbanCardComponent::CKanbanCardComponent() : iIsDragging(false)
{
	int w = CConfiguration::getIntValue("KanbanCardWidth");
	int h = CConfiguration::getIntValue("KanbanCardHeight");

	setSize(w, h);
}

CKanbanCardComponent::~CKanbanCardComponent()
{
}

void CKanbanCardComponent::paint (juce::Graphics& g)
{
	if (iIsDragging) return;
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("CKanbanCardComponent", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void CKanbanCardComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void CKanbanCardComponent::mouseDown(const MouseEvent& event)
{
	//iDragger.startDraggingComponent(this, event);
	if (auto* dragContainer = DragAndDropContainer::findParentDragContainerFor(this))
	{
		dragContainer->startDragging("KanbanCard", this);
		iIsDragging = true;
		repaint();
	}
}

void CKanbanCardComponent::mouseDrag(const MouseEvent& event)
{
	//iDragger.dragComponent(this, event, nullptr);
}

void CKanbanCardComponent::mouseUp(const MouseEvent& event)
{
	if (iIsDragging)
	{
		iIsDragging = false;
		repaint();
	}
}


