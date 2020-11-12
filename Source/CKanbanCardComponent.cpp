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
CKanbanCardComponent::CKanbanCardComponent(CKanbanColumnComponent* aOwner) : iIsDragging(false), iFlexItem(nullptr), iOwner(aOwner)
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
	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");

    g.setColour (juce::Colours::grey);
//	g.drawRect(getLocalBounds().reduced(m/2), 1);   // draw an outline around the component
	g.drawRect(getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText (name.length() == 0 ? "CKanbanCardComponent" : name, getLocalBounds(),
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
		dragContainer->startDragging(KanbanCardComponentDragDescription, this);
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

CKanbanColumnComponent* CKanbanCardComponent::getOwner()
{
	return iOwner;
}

void CKanbanCardComponent::setOwner(CKanbanColumnComponent* aOwner)
{
	iOwner = aOwner;
}


