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



CKanbanColumnComponent::CKanbanColumnComponent(int aColumnId, const String& aTitle, CKanbanBoardComponent& aOwner) : iOwner(aOwner), iColumnId(aColumnId), iIsFrameActive(false), iDueDateDone(false), iSortedAsc(false), iColumnTitle(aTitle), iViewportLayout(*this), iScrollBar(true), iAddCardButton("Add card", DrawableButton::ImageRaw), iSetupButton("Setup", DrawableButton::ImageRaw)
{
	iViewportLayout.addMouseListener(this, false);
	addAndMakeVisible(iViewportLayout);

	//iTitle.setText(aTitle, NotificationType::dontSendNotification);
	iTitle.addMouseListener(this,true);
	addAndMakeVisible(iTitle);

	iTitleCardsCount.addMouseListener(this, true);
	iTitleCardsCount.setJustificationType(Justification::right);
	iTitleCardsCount.setColour(Label::textColourId, getLookAndFeel().findColour(juce::Label::textColourId).darker(0.6));
//	iTitleCardsCount.setColour(Label::textColourId, getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).brighter());
	iTitleCardsCount.setTooltip("Cards count");
	addAndMakeVisible(iTitleCardsCount);

	updateColumnTitle();

	DrawablePath btnImg;
	{
		Path p;
		int w = 10, dh = 4;
		p.addLineSegment(Line<float>(0, 0, w, 0), 1);
		p.addLineSegment(Line<float>(0, dh, w, dh), 1);
		p.addLineSegment(Line<float>(0, 2*dh, w, 2*dh), 1);
		AffineTransform af = AffineTransform::translation(7, 8);
		p.applyTransform(af);
		p.addRectangle(-5, -5, 35, 35);
		btnImg.setPath(p);
		PathStrokeType pt(1, PathStrokeType::curved);
		btnImg.setStrokeType(pt);
		btnImg.setStrokeFill(FillType(Colours::lightgrey));
		btnImg.setFill(Colours::transparentWhite);
	}
	DrawablePath btnImgOver(btnImg);
	btnImgOver.setStrokeFill(FillType(Colours::whitesmoke));
	btnImgOver.setFill(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).brighter(0.08f));
	DrawablePath btnImgPushed(btnImgOver);
	btnImgPushed.setFill(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).darker());

	iSetupButton.setImages(&btnImg, &btnImgOver, &btnImgPushed);
	iSetupButton.setTooltip("Column options");
	iSetupButton.onClick = [this]
	{// todo: clear mouse over state
		iSetupButton.setState(Button::buttonDown);
		this->showSetupMenu();
	};
	iSetupButton.setTooltip("Column menu");
	addAndMakeVisible(iSetupButton);


	DrawablePath btnImg2;
	{
		Path p;
		int w = 10;
		p.addLineSegment(Line<float>(0, w/2, w, w/2), 1);
		p.addLineSegment(Line<float>(w/2, 0, w/2, w), 1);
		AffineTransform af = AffineTransform::translation(7, 7);
		p.applyTransform(af);
		p.addRectangle(-5, -5, 35, 35);
		btnImg2.setPath(p);
		PathStrokeType pt(1, PathStrokeType::curved);
		btnImg2.setStrokeType(pt);
		btnImg2.setStrokeFill(FillType(Colours::lightgrey));
		btnImg2.setFill(Colours::transparentWhite);
	}
	DrawablePath btnImgOver2(btnImg2);
	btnImgOver2.setStrokeFill(FillType(Colours::whitesmoke));
	btnImgOver2.setFill(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).brighter(0.08f));
	DrawablePath btnImgPushed2(btnImgOver2);
	btnImgPushed2.setFill(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).darker());
	iAddCardButton.setImages(&btnImg2, &btnImgOver2, &btnImgPushed2);
	iAddCardButton.onClick = [this]
	{ // todo: clear mouse over state
		iAddCardButton.setState(Button::buttonNormal);
		this->iViewportLayout.createNewCard();
	};
	iAddCardButton.setTooltip("Add card");
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
	Rectangle<int> r1b = r1.removeFromRight(25);

	r1.removeFromRight(25);
	iTitle.setBounds(r1);

	r1b.translate(0, 1);
	r1b.setHeight(r1b.getHeight() - 1);
	iSetupButton.setBounds(r1b);

	r1b.translate(-r1b.getWidth(), 0);
	iAddCardButton.setBounds(r1b);

	r1b.translate(-50, 0);
	r1b.setWidth(50);
	r1b.setTop(r1.getTopLeft().y);
	r1b.setBottom(r1.getBottom());
	iTitleCardsCount.setBounds(r1b);

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
		menu.addItem("Paste card", CKanbanCardComponent::getClipboardCard(), false, [&]()
		{
			duplicateCard(CKanbanCardComponent::getClipboardCard());
		});
		/*menu.addSeparator();
		menu.addItem("Archive column", [&]()
		{
			this->archive();
		});*/
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

void CKanbanColumnComponent::cardsCoutUpdated()
{
	updateColumnTitle();
}

CKanbanBoardComponent& CKanbanColumnComponent::kanbanBoard()
{
	return iOwner;
}

CKanbanColumnContentComponent& CKanbanColumnComponent::cardsLayout()
{
	return iViewportLayout;
}

void CKanbanColumnComponent::duplicateCard(const CKanbanCardComponent* aCard)
{
	iViewportLayout.createNewCard(aCard);
}

void CKanbanColumnComponent::addCard(CKanbanCardComponent* aCard, bool aLoadFromFile)
{
	iViewportLayout.addCard(aCard, aLoadFromFile);
}

void CKanbanColumnComponent::removeCard(CKanbanCardComponent* aCard)
{
	iViewportLayout.removeCard(aCard);
	iViewportLayout.resized();
	iOwner.removeCard(aCard);
}

void CKanbanColumnComponent::removeAllCards()
{
	int ret = AlertWindow::showYesNoCancelBox(AlertWindow::QuestionIcon, "Confirmation", "Do you really want to remove all cards from this column?");
	if (ret == 1)
	{
		auto ar = iOwner.getCardsForColumn(this);

		if (ar.size() > 0)
		{
			for (int i = ar.size() - 1; i >= 0; --i)
			{
				removeCard(ar[i]);
			}
		}
	}
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

String CKanbanColumnComponent::getTitle()
{
	return iTitle.getText();
}

int CKanbanColumnComponent::getColumnId() const
{
	return iColumnId;
}

bool CKanbanColumnComponent::isColumnDueDateDone() const
{
	return iDueDateDone;
}

void CKanbanColumnComponent::setColumnDueDateDone(bool aDueDateDone)
{
	iDueDateDone = aDueDateDone;
	iViewportLayout.updateDueDateDoneOnCards(iDueDateDone);
}

void CKanbanColumnComponent::showSetupMenu()
{
	PopupMenu menu;
	menu.addItem("Paste card", CKanbanCardComponent::getClipboardCard(), false, [&]()
	{
		this->duplicateCard(CKanbanCardComponent::getClipboardCard());
	});
	menu.addItem("Sort by colour", iViewportLayout.getCardsCount() > 1, false, [&]()
	{
		this->iViewportLayout.sortCardsByColour(iSortedAsc);
		iSortedAsc = !iSortedAsc;
	});
	menu.addItem("Sort by due date", iViewportLayout.getCardsCount() > 1, false, [&]()
	{
		this->iViewportLayout.sortCardsByDueDate(iSortedAsc);
		iSortedAsc = !iSortedAsc;
	});
	menu.addItem("Remove all cards", iViewportLayout.getCardsCount() > 0, false, [&]()
	{
		this->removeAllCards();
	});
	menu.addItem("Archive column", iViewportLayout.getCardsCount() > 0, false, [&]()
	{
		this->archive();
	});
	menu.addSeparator();
	/*menu.addItem("Edit name",  [&]()
	{
		
	});*/
	menu.addItem("Due date done", true, iDueDateDone, [&]()
	{
		this->setColumnDueDateDone(!iDueDateDone);
	});
	menu.show(0,0,0,0, new BtnMenuHandler(*this));
}

void CKanbanColumnComponent::archive()
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
}

void CKanbanColumnComponent::updateColumnTitle()
{
	iTitleCardsCount.setText(" (" + String(iViewportLayout.getCardsCount()) + ")", NotificationType::dontSendNotification);
	iTitle.setText(iColumnTitle, NotificationType::dontSendNotification);

//	iTitleCardsCount.setText("[" + String(iViewportLayout.getCardsCount()) +"]", NotificationType::dontSendNotification);
//	iTitle.setText(iColumnTitle + " | " + String(iViewportLayout.getCardsCount()) + "", NotificationType::dontSendNotification);
}

void CKanbanColumnComponent::scrollBarMoved(ScrollBar * scrollBarThatHasMoved, double newRangeStart)
{
	//Logger::outputDebugString("sc: " + String(newRangeStart));
	iViewportLayout.iScrollPos = newRangeStart;
	iViewportLayout.resized();
}

