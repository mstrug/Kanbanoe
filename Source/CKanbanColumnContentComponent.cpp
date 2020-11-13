/*
  ==============================================================================

    CKanbanColumnContentComponent.cpp
    Created: 13 Nov 2020 9:16:10am
    Author:  michal.strug

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CKanbanColumnContentComponent.h"
#include "CKanbanColumnComponent.h"
#include "CKanbanBoard.h"
#include "CConfiguration.h"

//==============================================================================
CKanbanColumnContentComponent::CKanbanColumnContentComponent(CKanbanColumnComponent& aOwner) : iOwner(aOwner), iDragTargetActive(false), iDragTargetPlaceholderActive(false), iPlaceholderIndex(-1), iDraggedCardIndex(-1), iScrollPos(0)
{
	iLayout.alignContent = FlexBox::AlignContent::center;
	iLayout.alignItems = FlexBox::AlignItems::flexStart;
	iLayout.flexDirection = FlexBox::Direction::column;
	iLayout.flexWrap = FlexBox::Wrap::noWrap;
	iLayout.justifyContent = FlexBox::JustifyContent::flexStart;
}

CKanbanColumnContentComponent::~CKanbanColumnContentComponent()
{
}

void CKanbanColumnContentComponent::paint(juce::Graphics& g)
{
	//g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

	int ps = CConfiguration::getIntValue("KanbanPlaceholderCardFrameSize");

	g.setColour(juce::Colours::grey);
	//g.drawLine(0, iTitle.getBottom(), getWidth(), iTitle.getBottom(),1);

	if (iDragTargetActive)
	{
		//g.setColour(Colours::red);
		//g.drawRect(getLocalBounds(), ps);
	}
	else
	{
		g.setColour(juce::Colours::grey);
		//g.drawRect(getLocalBounds(), 1);   // draw an outline around the component
	}

	g.setColour(juce::Colours::white);
	g.setFont(14.0f);
	//g.drawText ("CKanbanColumnComponent", getLocalBounds(), juce::Justification::centred, true);   // draw some placeholder text

	//int ps = CConfiguration::getIntValue("KanbanPlaceholderCardFrameSize");
	if (iDragTargetPlaceholderActive)
	{
		Rectangle<int> r(iPlaceholderActiveRect);
		//r += Point<int>(0, iViewport.getY());
		g.setColour(Colours::red);
		g.drawRect(r, ps);
	}
}

void CKanbanColumnContentComponent::paintOverChildren(Graphics & g)
{

}

void CKanbanColumnContentComponent::resized()
{
	Rectangle<int> r(0, -iScrollPos, getWidth(), getHeight());
	iLayout.performLayout(r);
	updateSize();
	iOwner.contentUpdated();
}

void CKanbanColumnContentComponent::removeCard(CKanbanCardComponent * aCard)
{
	for (auto& i : iLayout.items)
	{
		if (i.associatedComponent == aCard)
		{
			iLayout.items.remove(&i);
			break;
		}
	}
}

void CKanbanColumnContentComponent::addCard()
{
	Logger::outputDebugString("add card");
	iPlaceholderIndex = -1;

	auto c = iOwner.kanbanBoard().createCard();
	DragAndDropTarget::SourceDetails s(KanbanCardComponentDragDescription, c, Point<int>());
	itemDropped(s);
}

void CKanbanColumnContentComponent::updateSize()
{
	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");
	int h = CConfiguration::getIntValue("KanbanCardHeight");
	int hh = iLayout.items.size() * (h + m);
	if (hh < iMinimumHeight)
	{
		hh = iMinimumHeight;
	}

	setSize(getWidth(), hh);
}

void CKanbanColumnContentComponent::setMinimumHeight(int aHeight)
{
	iMinimumHeight = aHeight;
}

bool CKanbanColumnContentComponent::isInterestedInDragSource(const SourceDetails & dragSourceDetails)
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

void CKanbanColumnContentComponent::itemDragEnter(const SourceDetails & dragSourceDetails)
{
	iPlaceholderIndex = -1;
	iDragTargetActive = true;
	iOwner.setActiveFrame(true);
	repaint();
}

void CKanbanColumnContentComponent::itemDragMove(const SourceDetails & dragSourceDetails)
{
	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");

	bool tmp = iDragTargetPlaceholderActive;
	iDragTargetPlaceholderActive = false;
	int j = 0;

	Point<int> p = dragSourceDetails.localPosition;
	//p.addXY(0, -iViewport.getY());

	for (auto& i : iLayout.items)
	{
		if (i.currentBounds.expanded(i.margin.left, i.margin.top).contains(p.toFloat()) ||
			iLayout.items.size() == j + 1) // last item
		{
			iDragTargetPlaceholderActive = true;

			iPlaceholderActiveRect = i.currentBounds.toNearestInt();
			if (p.y < i.currentBounds.getY() + i.currentBounds.getHeight() / 4 &&
				p.y > i.currentBounds.getY() - i.currentBounds.getHeight() / 4)
			{
				iPlaceholderActiveRect.setBottom(iPlaceholderActiveRect.getTopLeft().y);
				iPlaceholderActiveRect -= Point<int>(0, m / 2);
				iPlaceholderIndex = j;
			}
			else if (p.y > i.currentBounds.getY() + 3 * i.currentBounds.getHeight() / 4 &&
				( p.y < i.currentBounds.getY() + 5 * i.currentBounds.getHeight() / 4 || iLayout.items.size() == j + 1)) // last item
			{
				iPlaceholderActiveRect.setTop(iPlaceholderActiveRect.getBottomLeft().y);
				iPlaceholderActiveRect += Point<int>(0, m / 2);
				iPlaceholderIndex = j + 1;
			}
			else
			{
				iPlaceholderIndex = -1;
				iDragTargetPlaceholderActive = false;
			}

			if ((iDraggedCardIndex != -1) && (iDraggedCardIndex == iPlaceholderIndex || iDraggedCardIndex + 1 == iPlaceholderIndex))
			{
				iPlaceholderIndex = -1;
				iDragTargetPlaceholderActive = false;
				//				Logger::outputDebugString("j4: " + String(iPlaceholderIndex) + "  k: " + String(iDraggedCardIndex));
			}

			//iPlaceholderActiveRect = i.currentBounds.toNearestInt();
			iPlaceholderActiveRect.expand(2, 2);

			break;
		}
		j++;
	}

	if (tmp != iDragTargetPlaceholderActive || iDragTargetPlaceholderActive) repaint();
}

void CKanbanColumnContentComponent::itemDragExit(const SourceDetails & dragSourceDetails)
{
	iDragTargetActive = false;
	iOwner.setActiveFrame(false);
	iDragTargetPlaceholderActive = false;
	repaint();
}

void CKanbanColumnContentComponent::itemDropped(const SourceDetails & dragSourceDetails)
{
	/*dragSourceDetails.sourceComponent->getParentComponent()->removeChildComponent(dragSourceDetails.sourceComponent);
	addAndMakeVisible(dragSourceDetails.sourceComponent);
	dragSourceDetails.sourceComponent->setTopLeftPosition(0, 0);

	setSize(dragSourceDetails.sourceComponent->getWidth(), dragSourceDetails.sourceComponent->getHeight());
	*/

	CKanbanCardComponent* card = nullptr;
	if (dragSourceDetails.description == KanbanCardComponentDragDescription)
	{
		card = static_cast<CKanbanCardComponent*>(dragSourceDetails.sourceComponent.get());
		CKanbanColumnContentComponent* col = card->getOwner();

		if (col)
		{
			col->removeCard(card);
			if (col != this)
			{
				col->resized();
				//col->updateSize();
				//col->iOwner.contentUpdated();
			}
			if (iDraggedCardIndex != -1 && iPlaceholderIndex > iDraggedCardIndex) iPlaceholderIndex--;
		}
		card->setOwner(this);

	}

	int w = CConfiguration::getIntValue("KanbanCardWidth");
	int h = CConfiguration::getIntValue("KanbanCardHeight");
	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");

	FlexItem fi(w, h);
	fi.associatedComponent = dragSourceDetails.sourceComponent.get();
	fi.margin = m / 2;

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
	iOwner.setActiveFrame(false);
	iDragTargetPlaceholderActive = false;
	iDraggedCardIndex = -1;

	repaint();
}

