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



CKanbanColumnComponent::CKanbanColumnComponent(CKanbanBoardComponent& aOwner) : iOwner(aOwner), iIsFrameActive(false), iViewportLayout(*this), iScrollBar(true)
{
	iViewportLayout.addMouseListener(this, false);
	addAndMakeVisible(iViewportLayout);

	iTitle.setText("Column Name", NotificationType::dontSendNotification);
	iTitle.addMouseListener(this,true);
	addAndMakeVisible(iTitle);

	iScrollBar.addListener(this);
	iScrollBar.setAlwaysOnTop(true);
	addAndMakeVisible(iScrollBar);
}

CKanbanColumnComponent::~CKanbanColumnComponent()
{

}

void CKanbanColumnComponent::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));   // clear the background


}

void CKanbanColumnComponent::paintOverChildren(Graphics & g)
{
	g.setColour(juce::Colours::grey);
	g.drawLine(0, iTitle.getBottom(), getWidth(), iTitle.getBottom(), 1);

	int ps = 1;
	if (iIsFrameActive)
	{
		ps = CConfiguration::getIntValue("KanbanPlaceholderCardFrameSize");
		g.setColour(juce::Colours::red);
	}
	else
	{
		g.setColour(juce::Colours::grey);
	}
	g.drawRect(getLocalBounds(), ps);   // draw an outline around the component
}

void CKanbanColumnComponent::resized()
{
	Rectangle<int> r(getLocalBounds());
	iTitle.setBounds(r.removeFromTop(25));

	Rectangle<int> r2(r);
	r2.setLeft(r2.getWidth() - 8);
	r2.setWidth(r2.getWidth() - 1);
	iScrollBar.setBounds(r2);

	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");
	r.removeFromTop(m / 2);

	iViewportLayout.setMinimumHeight(r.getHeight());
	iViewportLayout.setBounds(r);
	//iViewportLayout.updateSize();
	Logger::outputDebugString("CminH: " + String(iViewportLayout.iMinimumHeight));
	Logger::outputDebugString("CH: " + String(iViewportLayout.getHeight()));
}

void CKanbanColumnComponent::mouseUp(const MouseEvent& event)
{
	if (event.mods.isRightButtonDown())
	{
		PopupMenu menu;
		menu.addItem("Add card", [&]() 
		{ 
			this->iViewportLayout.addCard();
		});
		menu.addItem(1,"Archive column");
		menu.show();
	}
}

void CKanbanColumnComponent::setActiveFrame(bool aActive)
{
	if (aActive != iIsFrameActive)
	{
		iIsFrameActive = aActive;
		repaint();
	}
}

void CKanbanColumnComponent::contentUpdated()
{
	Logger::outputDebugString("minH: " + String(iViewportLayout.iMinimumHeight));
	Logger::outputDebugString("H: " + String(iViewportLayout.getHeight()));

	iScrollBar.setRangeLimits(0, iViewportLayout.getHeight());


	iScrollBar.setCurrentRange(iScrollBar.getCurrentRangeStart(), iViewportLayout.iMinimumHeight);
}

CKanbanBoardComponent& CKanbanColumnComponent::kanbanBoard()
{
	return iOwner;
}

void CKanbanColumnComponent::scrollBarMoved(ScrollBar * scrollBarThatHasMoved, double newRangeStart)
{
	Logger::outputDebugString("sc: " + String(newRangeStart));
	iViewportLayout.iScrollPos = newRangeStart;
	iViewportLayout.resized();
}
