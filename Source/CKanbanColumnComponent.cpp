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
#include "CKanbanBoard.h"



CKanbanColumnComponent::CKanbanColumnComponent(int aColumnId, const String& aTitle, CKanbanBoardComponent& aOwner) : iOwner(aOwner), iColumnId(aColumnId), iIsFrameActive(false), iViewportLayout(*this), iScrollBar(true)
{
	iViewportLayout.addMouseListener(this, false);
	addAndMakeVisible(iViewportLayout);

	iTitle.setText(aTitle, NotificationType::dontSendNotification);
	iTitle.addMouseListener(this,true);
	addAndMakeVisible(iTitle);

	iAddCardButton.setButtonText("+");
	addAndMakeVisible(iAddCardButton);

	int h = CConfiguration::getIntValue("KanbanCardHeight");
	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");
	iScrollBar.addListener(this);
	iScrollBar.setAlwaysOnTop(true);
	iScrollBar.setSingleStepSize( (h + m + m ) / 4);
	addAndMakeVisible(iScrollBar);

	setOpaque(true);
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
	Rectangle<int> r1(r.removeFromTop(25));
	//Rectangle<int> r1b = r1.removeFromRight(25);
	iTitle.setBounds(r1);
	//iAddCardButton.setBounds(r1b);

	Rectangle<int> r2(r);
	r2.setLeft(r2.getWidth() - 8);
	r2.setWidth(r2.getWidth() - 1);
	iScrollBar.setBounds(r2);

	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");
	r.removeFromTop(m / 2);

	iViewportLayout.setMinimumHeight(r.getHeight());
	iViewportLayout.setBounds(r);
	//iViewportLayout.updateSize();
	//Logger::outputDebugString("CminH: " + String(iViewportLayout.iMinimumHeight));
	//Logger::outputDebugString("CH: " + String(iViewportLayout.getHeight()));
}

void CKanbanColumnComponent::mouseUp(const MouseEvent& event)
{
	if (event.mods.isRightButtonDown())
	{
		PopupMenu menu;
		menu.addItem("Add card", [&]() 
		{ 
			this->iViewportLayout.createNewCard();
		});
		menu.addItem("Archive column", [&]()
		{
			if (this->iViewportLayout.getCardsCount() == 0)
			{
				AlertWindow::showMessageBoxAsync(AlertWindow::InfoIcon, "Column is empty!", "", "OK");
				return;
			}

			AlertWindow w("Archive column options",
				"Provide descriptive name for the archived column.\nProposed name is year and a weak number.\nArchived columns are accessible in menu Edit -> Show Archives.\n",
				AlertWindow::QuestionIcon);

			w.addTextEditor("text", CConfiguration::YearAndWeekOfYear(), "Archive name:");

			ToggleButton tb("Clear column after archivisation");
			tb.setComponentID("checkbox");
			tb.setSize(250, 36);
			tb.setName("");
			tb.setToggleState(true, NotificationType::dontSendNotification);
			w.addCustomComponent(&tb);
			w.addButton("OK", 1, KeyPress(KeyPress::returnKey, 0, 0));
			w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

			if (w.runModalLoop() != 0) // is they picked 'ok'
			{
				auto text = w.getTextEditorContents("text");
				bool checked = tb.getToggleState();
				if (text.isEmpty())
				{
					AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Archive column options", "Wrong archive name!", "OK");
				}
				else
				{
					if (iOwner.archiveColumn(this, text, checked))
					{

					}
				}
			}

			//this->iViewportLayout.createNewCard();
		});
		menu.show();
	}
}

void CKanbanColumnComponent::mouseWheelMove(const MouseEvent & event, const MouseWheelDetails & details)
{
	static juce::int64 ms = 0;
	if (iScrollBar.isVisible() && ms < event.eventTime.toMilliseconds() )
	{		
		ms = event.eventTime.toMilliseconds();
		auto e = event.getEventRelativeTo(&iScrollBar);
		iScrollBar.mouseWheelMove(e, details);
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
	//Logger::outputDebugString("minH: " + String(iViewportLayout.iMinimumHeight));
	//Logger::outputDebugString("H: " + String(iViewportLayout.getHeight()));

	iScrollBar.setRangeLimits(0, iViewportLayout.getHeight());


	iScrollBar.setCurrentRange(iScrollBar.getCurrentRangeStart(), iViewportLayout.iMinimumHeight);
}

CKanbanBoardComponent& CKanbanColumnComponent::kanbanBoard()
{
	return iOwner;
}

CKanbanColumnContentComponent& CKanbanColumnComponent::cardsLayout()
{
	return iViewportLayout;
}

void CKanbanColumnComponent::addCard(CKanbanCardComponent* aCard)
{
	iViewportLayout.addCard(aCard);
}

void CKanbanColumnComponent::removeCard(CKanbanCardComponent* aCard)
{
	iViewportLayout.removeCard(aCard);
	iViewportLayout.resized();
	iOwner.removeCard(aCard);
}

void CKanbanColumnComponent::removeAllCards()
{
	
}

void CKanbanColumnComponent::scrollToBottom()
{
	iScrollBar.scrollToBottom(NotificationType::sendNotificationSync);
}

void CKanbanColumnComponent::scrollToTop()
{
	iScrollBar.scrollToTop(NotificationType::sendNotificationSync);
}

void CKanbanColumnComponent::scrollEnsureVisible(CKanbanCardComponent * aCard)
{
	if (aCard)
	{
		int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");
		auto ran = iScrollBar.getCurrentRange();
		int top = aCard->getBounds().getTopLeft().getY() + ran.getStart();
		int bottom = aCard->getBounds().getBottom() + ran.getStart();
		//Logger::outputDebugString("range: " + String(ran.getStart()) + "  " + String(ran.getEnd()) + "    |  " + String(top) + "  " + String(bottom));
		if (!ran.contains(top))
		{
			iScrollBar.setCurrentRangeStart(top-m);
		}
		else if (!ran.contains(bottom))
		{
			int c = bottom + m - ran.getEnd() + iScrollBar.getCurrentRangeStart();
			if  (c >= 0 ) iScrollBar.setCurrentRangeStart(c);
			//Logger::outputDebugString("not contains bottom  " + String(c));
		}
	}
}

void CKanbanColumnComponent::search(const String & aString)
{
	if (aString.startsWith("tag:"))
	{
		String s = aString.substring(4);
		s = s.trim();
		if (s.length() == 0) return;
		Logger::outputDebugString("search tag: " + s);

		//for ( auto c : iOwner.getcar)
	}
}

String CKanbanColumnComponent::getTitle()
{
	return iTitle.getText();
}

int CKanbanColumnComponent::getColumnId() const
{
	return iColumnId;
}

void CKanbanColumnComponent::scrollBarMoved(ScrollBar * scrollBarThatHasMoved, double newRangeStart)
{
	//Logger::outputDebugString("sc: " + String(newRangeStart));
	iViewportLayout.iScrollPos = newRangeStart;
	iViewportLayout.resized();
}
