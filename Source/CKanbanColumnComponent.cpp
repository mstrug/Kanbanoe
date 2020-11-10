/*
  ==============================================================================

    CKanbanColumnComponent.cpp
    Created: 9 Nov 2020 5:11:55pm
    Author:  michal.strug

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CKanbanColumnComponent.h"
#include "CConfiguration.h"

//==============================================================================
CKanbanColumnComponent::CKanbanColumnComponent() : iDragTargetActive(false)
{
	
	//iPlaceholders.add(new CKanbanColumnCardPlaceholderComponent());
	//addAndMakeVisible(iPlaceholders[0]);

	iLayout.alignContent = FlexBox::AlignContent::center;
	iLayout.alignItems = FlexBox::AlignItems::flexStart;
	iLayout.flexDirection = FlexBox::Direction::column;
	iLayout.flexWrap = FlexBox::Wrap::noWrap;
	iLayout.justifyContent = FlexBox::JustifyContent::flexStart;

}

CKanbanColumnComponent::~CKanbanColumnComponent()
{
}

void CKanbanColumnComponent::paint (juce::Graphics& g)
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

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    //g.drawText ("CKanbanColumnComponent", getLocalBounds(), juce::Justification::centred, true);   // draw some placeholder text
}

void CKanbanColumnComponent::resized()
{
	iLayout.performLayout(getLocalBounds().reduced(5));
//	iLayout.performLayout(getLocalBounds().reduced(10).toFloat());
	//if (iPlaceholders.size() > 0) iPlaceholders[0]->setBounds(5, 20, 100, 60);

}

void CKanbanColumnComponent::removeCard(CKanbanCardComponent * aCard)
{
	for (auto& i : iLayout.items)
	{
		if (i.associatedComponent == aCard)
		{
			iLayout.items.remove(&i);
			resized();
			break;
		}
	}
}

bool CKanbanColumnComponent::isInterestedInDragSource(const SourceDetails & dragSourceDetails)
{
	return true;
}

void CKanbanColumnComponent::itemDragEnter(const SourceDetails & dragSourceDetails)
{
	iDragTargetActive = true;
	repaint();
}

void CKanbanColumnComponent::itemDragExit(const SourceDetails & dragSourceDetails)
{
	iDragTargetActive = false;
	repaint();
}

void CKanbanColumnComponent::itemDropped(const SourceDetails & dragSourceDetails)
{
	/*dragSourceDetails.sourceComponent->getParentComponent()->removeChildComponent(dragSourceDetails.sourceComponent);
	addAndMakeVisible(dragSourceDetails.sourceComponent);
	dragSourceDetails.sourceComponent->setTopLeftPosition(0, 0);

	setSize(dragSourceDetails.sourceComponent->getWidth(), dragSourceDetails.sourceComponent->getHeight());
	*/

	if (dragSourceDetails.description == KanbanCardComponentDragDescription)
	{
		auto card = static_cast<CKanbanCardComponent*>(dragSourceDetails.sourceComponent.get());
		auto col = static_cast<CKanbanColumnComponent*>(card->getParentComponent());
		col->removeCard(card);
	//	addCard(static_cast<CKanbanCardComponent*>(dragSourceDetails.sourceComponent.get()));
	}


	int w = CConfiguration::getIntValue("KanbanCardWidth");
	int h = CConfiguration::getIntValue("KanbanCardHeight");

	FlexItem fi(w, h);
	fi.associatedComponent = dragSourceDetails.sourceComponent.get();

	if (dragSourceDetails.localPosition.x < 50)
	{
		iLayout.items.insert(0, fi);
	}
	else
	{
		iLayout.items.add(fi);
	}

	//auto& flexItem = iLayout.items.getReference(iLayout.items.size() - 1);

	addAndMakeVisible(dragSourceDetails.sourceComponent);
	resized();

	iDragTargetActive = false;
	repaint();
}

void CKanbanColumnComponent::addCard(CKanbanCardComponent * aCardComponent)
{
	auto pc = new CKanbanColumnCardPlaceholderComponent();
	pc->attachCard(aCardComponent);
	iPlaceholders.add(pc);
	addAndMakeVisible(pc);
}


