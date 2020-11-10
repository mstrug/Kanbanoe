/*
  ==============================================================================

    CKanbanColumnCardPlaceholderComponent.cpp
    Created: 9 Nov 2020 5:20:24pm
    Author:  michal.strug

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CKanbanColumnCardPlaceholderComponent.h"
#include "CConfiguration.h"

//==============================================================================
CKanbanColumnCardPlaceholderComponent::CKanbanColumnCardPlaceholderComponent() : iDragTargetActive(false)
{
	int cw = CConfiguration::getIntValue("KanbanCardWidth");
	int ch = CConfiguration::getIntValue("KanbanCardHeight");
	int ph = CConfiguration::getIntValue("KanbanPlaceholderCardHeight");
	int ps = CConfiguration::getIntValue("KanbanPlaceholderCardFrameSize");

	setSize(cw, ph);
}

CKanbanColumnCardPlaceholderComponent::~CKanbanColumnCardPlaceholderComponent()
{
}

void CKanbanColumnCardPlaceholderComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

	if (iDragTargetActive)
	{
		g.setColour(Colours::red);
		int ps = CConfiguration::getIntValue("KanbanPlaceholderCardFrameSize");
		g.drawRect(getLocalBounds(), ps);
	}
	else
	{
		g.setColour(juce::Colours::grey);
		g.drawRect(getLocalBounds(), 1);   // draw an outline around the component
	}

    //g.setColour (juce::Colours::white);
    //g.setFont (14.0f);
    //g.drawText ("CKanbanColumnCardPlaceholderComponent", getLocalBounds(), juce::Justification::centred, true);   // draw some placeholder text
}

void CKanbanColumnCardPlaceholderComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void CKanbanColumnCardPlaceholderComponent::childrenChanged()
{
	if (getNumChildComponents() == 0)
	{
		int ph = CConfiguration::getIntValue("KanbanPlaceholderCardHeight");
		setSize(getWidth(), ph);
		repaint();
	}
}

void CKanbanColumnCardPlaceholderComponent::attachCard(CKanbanCardComponent * aCardComponent)
{
	aCardComponent->getParentComponent()->removeChildComponent(aCardComponent);
	addAndMakeVisible(aCardComponent);
	aCardComponent->setTopLeftPosition(0, 0);

	setSize(aCardComponent->getWidth(), aCardComponent->getHeight());
}

void CKanbanColumnCardPlaceholderComponent::attachCard(const SourceDetails & dragSourceDetails)
{
	if (dragSourceDetails.description == KanbanCardComponentDragDescription)
	{
		attachCard(static_cast<CKanbanCardComponent*>(dragSourceDetails.sourceComponent.get()));
	}
}

bool CKanbanColumnCardPlaceholderComponent::isInterestedInDragSource(const SourceDetails & dragSourceDetails)
{
	return (getNumChildComponents() == 0);
}

void CKanbanColumnCardPlaceholderComponent::itemDragEnter(const SourceDetails & dragSourceDetails)
{
	iDragTargetActive = true;
	repaint();
}

void CKanbanColumnCardPlaceholderComponent::itemDragExit(const SourceDetails & dragSourceDetails)
{
	iDragTargetActive = false;
	repaint();
}

void CKanbanColumnCardPlaceholderComponent::itemDropped(const SourceDetails & dragSourceDetails)
{
	/*dragSourceDetails.sourceComponent->getParentComponent()->removeChildComponent(dragSourceDetails.sourceComponent);
	addAndMakeVisible(dragSourceDetails.sourceComponent);
	dragSourceDetails.sourceComponent->setTopLeftPosition(0, 0);

	setSize(dragSourceDetails.sourceComponent->getWidth(), dragSourceDetails.sourceComponent->getHeight());
	*/
	attachCard(dragSourceDetails);

	iDragTargetActive = false;
	repaint();
}
