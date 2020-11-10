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
CKanbanColumnComponent::CKanbanColumnComponent() : iDragTargetActive(false), iDragTargetPlaceholderActive(false), iPlaceholderIndex(-1)
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
	
	int ps = CConfiguration::getIntValue("KanbanPlaceholderCardFrameSize");
	if (!iDragTargetPlaceholderActive && iDragTargetActive)
	{
		g.setColour(Colours::red);
		g.drawRect(getLocalBounds(), ps);
	}
	
	if ( !iDragTargetActive || iDragTargetPlaceholderActive )
	{
		g.setColour(juce::Colours::grey);
		g.drawRect(getLocalBounds(), 1);   // draw an outline around the component
	}


    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    //g.drawText ("CKanbanColumnComponent", getLocalBounds(), juce::Justification::centred, true);   // draw some placeholder text
}

void CKanbanColumnComponent::paintOverChildren(Graphics & g)
{
	int ps = CConfiguration::getIntValue("KanbanPlaceholderCardFrameSize");
	if (iDragTargetPlaceholderActive)
	{
		g.setColour(Colours::red);
		g.drawRect(iPlaceholderActiveRect, ps);
	}
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
	iPlaceholderIndex = -1;
	iDragTargetActive = true;
	repaint();
}

void CKanbanColumnComponent::itemDragMove(const SourceDetails & dragSourceDetails)
{
	bool tmp = iDragTargetPlaceholderActive;
	iDragTargetPlaceholderActive = false;
	int j = 0;
	for (auto& i : iLayout.items)
	{
		if (i.currentBounds.contains(dragSourceDetails.localPosition.toFloat()))
		{
			iDragTargetPlaceholderActive = true;

			iPlaceholderActiveRect = i.currentBounds.toNearestInt();
			if (dragSourceDetails.localPosition.y < i.currentBounds.getY() + i.currentBounds.getHeight() / 4)
			{
				iPlaceholderActiveRect.setBottom(iPlaceholderActiveRect.getTopLeft().y);
				iPlaceholderIndex = j;
			}
			else if (dragSourceDetails.localPosition.y > i.currentBounds.getY() + 3 * i.currentBounds.getHeight() / 4 )
			{
				iPlaceholderActiveRect.setTop(iPlaceholderActiveRect.getBottomLeft().y);
				iPlaceholderIndex = j + 1;
			}
			else
			{
				iPlaceholderIndex = -1;
				iDragTargetPlaceholderActive = false;
			}

			//iPlaceholderActiveRect = i.currentBounds.toNearestInt();
			iPlaceholderActiveRect.expand(2, 2);

			break;
		}
		j++;
	}

	if (tmp != iDragTargetPlaceholderActive || iDragTargetPlaceholderActive) repaint();
}

void CKanbanColumnComponent::itemDragExit(const SourceDetails & dragSourceDetails)
{
	iDragTargetActive = false;
	iDragTargetPlaceholderActive = false;
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
		auto col = dynamic_cast<CKanbanColumnComponent*>(card->getParentComponent());
		if ( col ) col->removeCard(card);
	//	addCard(static_cast<CKanbanCardComponent*>(dragSourceDetails.sourceComponent.get()));
	}


	int w = CConfiguration::getIntValue("KanbanCardWidth");
	int h = CConfiguration::getIntValue("KanbanCardHeight");

	FlexItem fi(w, h);
	fi.associatedComponent = dragSourceDetails.sourceComponent.get();

	if (iPlaceholderIndex == -1)
	{
		iLayout.items.add(fi);
	}
	else
	{
		iLayout.items.insert(iPlaceholderIndex, fi);
	}

	//auto& flexItem = iLayout.items.getReference(iLayout.items.size() - 1);

	addAndMakeVisible(dragSourceDetails.sourceComponent);
	resized();

	iDragTargetActive = false;
	iDragTargetPlaceholderActive = false;

	repaint();
}

void CKanbanColumnComponent::addCard(CKanbanCardComponent * aCardComponent)
{
	auto pc = new CKanbanColumnCardPlaceholderComponent();
	pc->attachCard(aCardComponent);
	iPlaceholders.add(pc);
	addAndMakeVisible(pc);
}


