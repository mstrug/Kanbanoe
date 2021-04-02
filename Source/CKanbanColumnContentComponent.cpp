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

	setOpaque(true);
}

CKanbanColumnContentComponent::~CKanbanColumnContentComponent()
{
}

void CKanbanColumnContentComponent::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));   // clear the background
	//g.fillAll(Colours::red);

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
	if (iDragTargetPlaceholderActive && !iOwner.isMinimized())
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
			iOwner.cardsCoutUpdated();
			break;
		}
	}
}

void CKanbanColumnContentComponent::addCard(CKanbanCardComponent* aCard, bool aLoadFromFile)
{
	//Logger::outputDebugString("add card");
	iPlaceholderIndex = -1;

	DragAndDropTarget::SourceDetails s(aLoadFromFile ? KanbanCardComponentLoadFromFileDescription : KanbanCardComponentDragDescription, aCard, Point<int>());
	itemDropped(s);
}

void CKanbanColumnContentComponent::hideCard(CKanbanCardComponent * aCard)
{
	for (auto& i : iLayout.items)
	{
		if (i.associatedComponent == aCard)
		{
			i.height = 0;
			i.minHeight = 0;
			i.margin = 0;
			resized();
			break;
		}
	}
}

void CKanbanColumnContentComponent::hideAllCards()
{
	for (auto& i : iLayout.items)
	{
		i.height = 0;
		i.minHeight = 0;
		i.margin = 0;
	}
	resized();
}

void CKanbanColumnContentComponent::unhideAllCards()
{
	int h = CConfiguration::getIntValue("KanbanCardHeight");
	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");

	for (auto& i : iLayout.items)
	{
//		if (i.height == 0)
		{
			i.height = h;
			i.minHeight = h;
			i.margin = m / 2;
		}
	}
	resized();
}

void CKanbanColumnContentComponent::createNewCard(const CKanbanCardComponent* aCardToCopyDataFrom )
{
	CKanbanCardComponent* c = iOwner.kanbanBoard().createCard();

	if (aCardToCopyDataFrom)
	{
		c->duplicateDataFrom(*aCardToCopyDataFrom);
	}

	addCard(c);
	iOwner.scrollToBottom();
	repaint();
	c->openPropertiesWindow();
}

void CKanbanColumnContentComponent::updateSize()
{
	if (iOwner.isMinimized()) return;
	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");
	int h = CConfiguration::getIntValue("KanbanCardHeight");

	int hh = m / 2;
	for (auto i : iLayout.items)
	{
		if ( i.height != 0 ) hh += (h + m);
	}
	//int hh = iLayout.items.size() * (h + m);
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

CKanbanColumnComponent& CKanbanColumnContentComponent::getOwner()
{
	return iOwner;
}

const FlexBox& CKanbanColumnContentComponent::getLayout()
{
	return iLayout;
}

int CKanbanColumnContentComponent::getCardsCount()
{
	return iLayout.items.size();
}

void CKanbanColumnContentComponent::moveCardTop(CKanbanCardComponent * aCard)
{
	int j = 0;
	for (auto& i : iLayout.items)
	{
		if (i.associatedComponent == aCard && j > 0 )
		{
			iLayout.items.move(j, 0);
			resized();
			iOwner.scrollEnsureVisible(aCard);
			break;
		}
		j++;
	}
}

void CKanbanColumnContentComponent::moveCardBottom(CKanbanCardComponent * aCard)
{
	int j = 0;
	for (auto& i : iLayout.items)
	{
		if (i.associatedComponent == aCard && j < iLayout.items.size() - 1)
		{
			iLayout.items.move(j, iLayout.items.size() - 1);
			resized();
			iOwner.scrollEnsureVisible(aCard);
			break;
		}
		j++;
	}
}

void CKanbanColumnContentComponent::updateDueDateDoneOnCards(bool aDone)
{
	for (auto& i : iLayout.items)
	{
		if (i.associatedComponent)
		{
			((CKanbanCardComponent*)i.associatedComponent)->setDone(aDone);
		}
	}
	repaint();
}

void CKanbanColumnContentComponent::sortCardsByColour(bool aAscending)
{
	std::sort( iLayout.items.begin(), iLayout.items.end(), [aAscending](const auto& lhs, const auto& rhs)
	{ 
		auto &pal = CConfiguration::getColourPalette();

		Colour c1 = static_cast<CKanbanCardComponent*>(lhs.associatedComponent)->getColour();
		Colour c2 = static_cast<CKanbanCardComponent*>(rhs.associatedComponent)->getColour();

		int idx1 = pal.getColourIndex(c1);
		int idx2 = pal.getColourIndex(c2);

		return (aAscending ? idx1 > idx2 : idx1 < idx2);
	}
	);

	resized();
	iOwner.scrollToTop();
}

void CKanbanColumnContentComponent::sortCardsByDueDate(bool aAscending)
{
	std::sort(iLayout.items.begin(), iLayout.items.end(), [aAscending](const auto& lhs, const auto& rhs)
	{
		auto d1 = static_cast<CKanbanCardComponent*>(lhs.associatedComponent)->getDueDate();
		auto d2 = static_cast<CKanbanCardComponent*>(rhs.associatedComponent)->getDueDate();

		auto t1 = d1.toMilliseconds();
		auto t2 = d2.toMilliseconds();

		if (!aAscending && t1 == 0) t1 = INT64_MAX;
		if (!aAscending && t2 == 0) t2 = INT64_MAX;

		return (aAscending ? t1 > t2 : t1 < t2);
	}
	);

	resized();
	iOwner.scrollToTop();
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
	iDraggedCardIndex = -1;
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
	card = static_cast<CKanbanCardComponent*>(dragSourceDetails.sourceComponent.get());
	if (dragSourceDetails.description == KanbanCardComponentDragDescription)
	{
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
	} 
	// else KanbanCardComponentLoadFromFileDescription
	card->setOwner(this);

	if (getOwner().isColumnDueDateDone()) card->setDone(true);
	else card->setDone(false);

	int w = CConfiguration::getIntValue("KanbanCardWidth");
	int h = CConfiguration::getIntValue("KanbanCardHeight");
	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");

	FlexItem fi(w, h);
	fi.associatedComponent = dragSourceDetails.sourceComponent.get();
	fi.margin = m / 2;

	if (iOwner.isMinimized())
	{
		fi.height = 0;
		fi.minHeight = 0;
		fi.margin = 0;
	}

	if (iPlaceholderIndex == -1 && iDraggedCardIndex != -1) iPlaceholderIndex = iDraggedCardIndex;

	if (iPlaceholderIndex == -1)
	{
		iLayout.items.add(fi);
	}
	else
	{
		iLayout.items.insert(iPlaceholderIndex, fi);
	}
	iOwner.cardsCoutUpdated();

	//auto& flexItem = iLayout.items.getReference(iLayout.items.size() - 1);

	addAndMakeVisible(dragSourceDetails.sourceComponent);

	if (dragSourceDetails.description != KanbanCardComponentLoadFromFileDescription )
	{
		resized();

		iDragTargetActive = false;
		iOwner.setActiveFrame(false);
		iOwner.scrollEnsureVisible(card);
		iDragTargetPlaceholderActive = false;
		iDraggedCardIndex = -1;

		repaint();
	}
}

