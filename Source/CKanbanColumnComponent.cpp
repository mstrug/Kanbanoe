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
CKanbanColumnComponent::CKanbanColumnComponent() : iDragTargetActive(false), iDragTargetPlaceholderActive(false), iPlaceholderIndex(-1), iDraggedCardIndex(-1)
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

/*	if (!iDragTargetPlaceholderActive && iDragTargetActive)
	{
		g.setColour(Colours::red);
		g.drawRect(getLocalBounds(), ps);
	}
	
	if ( !iDragTargetActive || iDragTargetPlaceholderActive )
	{
		g.setColour(juce::Colours::grey);
		g.drawRect(getLocalBounds(), 1);   // draw an outline around the component
	}*/

	if (iDragTargetActive)
	{
		g.setColour(Colours::red);
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
	int j = 0;
	iDraggedCardIndex = -1;
	for (auto& i : iLayout.items)
	{
		if (i.associatedComponent == dragSourceDetails.sourceComponent.get())
		{
			iDraggedCardIndex = j;
			break;
		}
		j++;
	}
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
			if (dragSourceDetails.localPosition.y < i.currentBounds.getY() + i.currentBounds.getHeight() / 4 )
			{
				iPlaceholderActiveRect.setBottom(iPlaceholderActiveRect.getTopLeft().y);
				iPlaceholderIndex = j;
				//std::cout << "j1: " << j << std::endl;
				// printf("j1: %\n", j);
				Logger::outputDebugString("j1: " + String(iPlaceholderIndex) + "  k: " + String(iDraggedCardIndex));
			}
			else if (dragSourceDetails.localPosition.y > i.currentBounds.getY() + 3 * i.currentBounds.getHeight() / 4 )
			{
				iPlaceholderActiveRect.setTop(iPlaceholderActiveRect.getBottomLeft().y);
				iPlaceholderIndex = j + 1;
				//std::cout << "j2: " << j << std::endl;
				//printf("j2: %\n", j);
				Logger::outputDebugString("j2: " + String(iPlaceholderIndex) + "  k: " + String(iDraggedCardIndex));
			}
			else
			{
				iPlaceholderIndex = -1;
				iDragTargetPlaceholderActive = false;
				//std::cout << "j3: " << j << std::endl;
				//printf("j3: %\n", j);
				Logger::outputDebugString("j3: " + String(iPlaceholderIndex) + "  k: " + String(iDraggedCardIndex));
			}

			if ( (iDraggedCardIndex != -1 ) && ( iDraggedCardIndex == iPlaceholderIndex || iDraggedCardIndex + 1 == iPlaceholderIndex) )
			{
				iPlaceholderIndex = -1;
				iDragTargetPlaceholderActive = false;
				Logger::outputDebugString("j4: " + String(iPlaceholderIndex) + "  k: " + String(iDraggedCardIndex));
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

	CKanbanCardComponent* card = nullptr;
	CKanbanColumnComponent* col = nullptr;
	if (dragSourceDetails.description == KanbanCardComponentDragDescription)
	{
		card = static_cast<CKanbanCardComponent*>(dragSourceDetails.sourceComponent.get());
		col = dynamic_cast<CKanbanColumnComponent*>(card->getParentComponent());

		/*if (iPlaceholderIndex == -1 && card->getParentComponent() == this )
		{
			iDragTargetActive = false;
			iDragTargetPlaceholderActive = false;
			iDraggedCardIndex = -1;

			repaint();
			return;
		}*/

		if (col)
		{
			col->removeCard(card);
			if (iDraggedCardIndex != -1 && iPlaceholderIndex > iDraggedCardIndex) iPlaceholderIndex--;
		}

	//	addCard(static_cast<CKanbanCardComponent*>(dragSourceDetails.sourceComponent.get()));
	}


	int w = CConfiguration::getIntValue("KanbanCardWidth");
	int h = CConfiguration::getIntValue("KanbanCardHeight");

	FlexItem fi(w, h);
	fi.associatedComponent = dragSourceDetails.sourceComponent.get();

	if (iPlaceholderIndex == -1 && iDraggedCardIndex != -1) iPlaceholderIndex = iDraggedCardIndex;
		
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
	iDraggedCardIndex = -1;

	repaint();
}

void CKanbanColumnComponent::addCard(CKanbanCardComponent * aCardComponent)
{
	auto pc = new CKanbanColumnCardPlaceholderComponent();
	pc->attachCard(aCardComponent);
	iPlaceholders.add(pc);
	addAndMakeVisible(pc);
}


