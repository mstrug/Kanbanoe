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
CKanbanColumnComponent::CKanbanColumnComponent() : iDragTargetActive(false), iDragTargetPlaceholderActive(false), iPlaceholderIndex(-1), iDraggedCardIndex(-1), iScrollBar(true)
{
	iTitle.setText("Column Name", NotificationType::dontSendNotification);
	addAndMakeVisible(iTitle);

	addAndMakeVisible(iViewport);
	iViewport.setViewedComponent(&iViewportLayout);

	//iPlaceholders.add(new CKanbanColumnCardPlaceholderComponent());
	//addAndMakeVisible(iPlaceholders[0]);

	//addAndMakeVisible(iScrollBar);
	iScrollBar.setRangeLimits(0, 800);
	iScrollBar.setCurrentRange(20, 100);
	iScrollBar.setAutoHide(false);
	iScrollBar.setVisible(true);
	//iScrollBar.addListener(this);

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

	g.setColour(juce::Colours::grey);
	g.drawLine(0, iTitle.getBottom(), getWidth(), iTitle.getBottom(),1);

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

	//int ps = CConfiguration::getIntValue("KanbanPlaceholderCardFrameSize");
	if (iDragTargetPlaceholderActive)
	{
		Rectangle<int> r(iPlaceholderActiveRect);
		r += Point<int>(0, iViewport.getY());
		g.setColour(Colours::red);
		g.drawRect(r, ps);
	}
}

void CKanbanColumnComponent::paintOverChildren(Graphics & g)
{

}

void CKanbanColumnComponent::resized()
{
//	Rectangle<int> r(getLocalBounds().reduced(5));
//	iTitle.setBounds(r.removeFromTop(25));

	Rectangle<int> r(getLocalBounds());
	iTitle.setBounds(r.removeFromTop(25));

	Rectangle<int> r2(r);
	r2.setLeft(r2.getWidth() - 8);
	r2.setWidth(r2.getWidth() - 1);
	//iScrollBar.setBounds(r2);

	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");
	r.removeFromTop(m / 2);

	iViewport.setBounds(r);
	int h = CConfiguration::getIntValue("KanbanCardHeight");
	iViewportLayout.setSize(r.getWidth(), iLayout.items.size() * (h + m));

	//iLayout.performLayout(iViewportLayout.getBoundsInParent());

	//r.setTop(iScrollBar.getCurrentRangeStart());
	iLayout.performLayout(iViewportLayout.getBounds());

	//iScrollBar.setRangeLimits()
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
	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");

	bool tmp = iDragTargetPlaceholderActive;
	iDragTargetPlaceholderActive = false;
	int j = 0;

	Point<int> p = dragSourceDetails.localPosition;
	p.addXY(0, -iViewport.getY());

	for (auto& i : iLayout.items)
	{
		if (i.currentBounds.expanded(i.margin.left, i.margin.top).contains(p.toFloat()) ||
			iLayout.items.size() == j + 1 ) // last item
		{
			iDragTargetPlaceholderActive = true;

			iPlaceholderActiveRect = i.currentBounds.toNearestInt();
			if (p.y < i.currentBounds.getY() + i.currentBounds.getHeight() / 4 &&
				p.y > i.currentBounds.getY() - i.currentBounds.getHeight() / 4)
			{
				iPlaceholderActiveRect.setBottom(iPlaceholderActiveRect.getTopLeft().y);
				iPlaceholderActiveRect -= Point<int>(0, m/2);
				iPlaceholderIndex = j;
			}
			else if (p.y > i.currentBounds.getY() + 3 * i.currentBounds.getHeight() / 4 &&
				     p.y < i.currentBounds.getY() + 5 * i.currentBounds.getHeight() / 4)
			{
				iPlaceholderActiveRect.setTop(iPlaceholderActiveRect.getBottomLeft().y);
				iPlaceholderActiveRect += Point<int>(0, m/2);
				iPlaceholderIndex = j + 1;
			}
			else
			{
				iPlaceholderIndex = -1;
				iDragTargetPlaceholderActive = false;
			}

			if ( (iDraggedCardIndex != -1 ) && ( iDraggedCardIndex == iPlaceholderIndex || iDraggedCardIndex + 1 == iPlaceholderIndex) )
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
	if (dragSourceDetails.description == KanbanCardComponentDragDescription)
	{
		card = static_cast<CKanbanCardComponent*>(dragSourceDetails.sourceComponent.get());
		CKanbanColumnComponent* col = card->getOwner();

		if (col)
		{
			col->removeCard(card);
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
	iScrollBar.setRangeLimits(0, iLayout.items.size() * (h+m));

	//auto& flexItem = iLayout.items.getReference(iLayout.items.size() - 1);

	iViewportLayout.addAndMakeVisible(dragSourceDetails.sourceComponent);
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

void CKanbanColumnComponent::scrollBarMoved1(ScrollBar *scrollBarThatHasMoved, double newRangeStart)
{
	Rectangle<int> r(getLocalBounds());
	r.removeFromTop(25);
	r.setTop(-iScrollBar.getCurrentRangeStart());
	iLayout.performLayout(r);

	repaint();
}

