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

const int KTitleHeight = 25;


CKanbanColumnComponent::CKanbanColumnComponent(int aColumnId, const String& aTitle, CKanbanBoardComponent& aOwner) : iOwner(aOwner), iColumnId(aColumnId), iMinimizedState(false), iIsFrameActive(false), iDueDateDone(false), iSortedAsc(false), iColumnTitle(aTitle), iViewportLayout(*this), iScrollBar(true), iAddCardButton("Add card", DrawableButton::ImageRaw), iSetupButton("Setup", DrawableButton::ImageRaw), iMouseTitleIsActive(false)
{
	//setInterceptsMouseClicks(false, true);
	//setRepaintsOnMouseActivity(true);
	setComponentID("1");

	iViewportLayout.addMouseListener(this, false);
	addAndMakeVisible(iViewportLayout);

	//iTitle.setText(aTitle, NotificationType::dontSendNotification);
	iTitle.addMouseListener(this,true);
	iTitle.setInterceptsMouseClicks(false, false);
	//iTitle.setColour(Label::backgroundColourId, Colours::red);
	addAndMakeVisible(iTitle);
	iTitle.setComponentID("2");

	iTitleCardsCount.addMouseListener(this, true);
	iTitleCardsCount.setInterceptsMouseClicks(false, false);
	iTitleCardsCount.setJustificationType(Justification::right);
	iTitleCardsCount.setColour(Label::textColourId, getLookAndFeel().findColour(juce::Label::textColourId).darker(0.6));
//	iTitleCardsCount.setColour(Label::textColourId, getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).brighter());
	iTitleCardsCount.setTooltip("Cards count");
	addAndMakeVisible(iTitleCardsCount);

	iTitleMinimalDueDate.addMouseListener(this, true);
	iTitleMinimalDueDate.setInterceptsMouseClicks(false, false);
	iTitleMinimalDueDate.setTooltip("Minimal due date");
	addAndMakeVisible(iTitleMinimalDueDate);
	iTitleMinimalDueDate.setVisible(false);

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
	iScrollBar.addMouseListener(this, false);
	addAndMakeVisible(iScrollBar);

	setOpaque(true);
}

CKanbanColumnComponent::~CKanbanColumnComponent()
{

}

void CKanbanColumnComponent::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));   // clear the background

	if (iMouseTitleIsActive )
	{
		if (iMinimizedState)
		{
			g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).brighter(0.08f));   // clear the background
		}
		else
		{
			g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).brighter(0.08f));   // clear the background)
			g.fillRect(iTitle.getBoundsInParent());
		}
	}
}

void CKanbanColumnComponent::paintOverChildren(Graphics & g)
{
	if (!iMinimizedState)
	{
		g.setColour(juce::Colours::grey);
		g.drawLine(0, iTitle.getBottom(), getWidth(), iTitle.getBottom(), 1);
	}

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

	if (iMinimizedState)
	{
//		iTitle.setSize(iTitle.getWidth(), r.getWidth());
		iTitle.setSize(r.getHeight(), r.getWidth());
		iTitle.setJustificationType(Justification::centredRight);
		iTitle.setTransform(AffineTransform::rotation(-MathConstants<float>::halfPi, 0, 0).translated(0, iTitle.getWidth() + KTitleHeight / 2));

		iViewportLayout.setBounds(r);

		iTitleCardsCount.setBounds(r.removeFromBottom(KTitleHeight));
		iTitleCardsCount.setJustificationType(Justification::centredTop);

		auto bb = iTitleCardsCount.getBounds();
		bb.translate(0, -KTitleHeight);
		iTitleMinimalDueDate.setBounds(bb);
		iTitleMinimalDueDate.setJustificationType(Justification::centredTop);
	}
	else
	{
		Rectangle<int> r1(r.removeFromTop(KTitleHeight));
		Rectangle<int> r1b = r1.removeFromRight(KTitleHeight);

		r1.removeFromRight(KTitleHeight);
		iTitle.setBounds(r1);
		iTitle.setTransform(AffineTransform());
		iTitle.setJustificationType(Justification::left);

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
		iTitleCardsCount.setJustificationType(Justification::right);

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
}

void CKanbanColumnComponent::mouseUp(const MouseEvent& event)
{
	if (event.eventComponent == &iScrollBar || event.originalComponent == &iScrollBar) return;

	if ( event.mods.isRightButtonDown() && getLocalBounds().contains( event.getPosition() ) )
	{
		PopupMenu menu;
		menu.addItem("Add card", [&]() 
		{
			if (iMinimizedState)
			{
				this->setMinimized(false, true);
			}
			this->iViewportLayout.createNewCard();
		});
		menu.addItem("Paste card", CKanbanCardComponent::getClipboardCard() != nullptr, false, [&]()
		{
			if (iMinimizedState)
			{
				this->setMinimized(false, true);
			}
			duplicateCard(CKanbanCardComponent::getClipboardCard());
		});
		/*menu.addSeparator();
		menu.addItem("Archive column", [&]()
		{
			this->archive();
		});*/
		menu.show();
	}
	else if (event.mods.isLeftButtonDown() )
	{
		if ( (!iMinimizedState && event.eventComponent == this && getLocalBounds().removeFromTop(KTitleHeight).contains(event.getPosition()) ||
			 (iMinimizedState && getLocalBounds().contains(event.getPosition()) )))
		{
			setMinimized(!iMinimizedState, true);
		}
	}
}

void CKanbanColumnComponent::mouseWheelMove(const MouseEvent & event, const MouseWheelDetails & details)
{
	static juce::int64 ms = 0;
	if ( ms < event.eventTime.toMilliseconds() )
	{		
		ms = event.eventTime.toMilliseconds();
		if (event.mods.isShiftDown())
		{
			auto e = event.getEventRelativeTo(&kanbanBoard());
			kanbanBoard().mouseWheelMove(e, details);
		}
		else if (iScrollBar.isVisible())
		{
			auto e = event.getEventRelativeTo(&iScrollBar);
			iScrollBar.mouseWheelMove(e, details);
		}
	}
}


void CKanbanColumnComponent::mouseMove(const MouseEvent & event)
{
	if (event.eventComponent == &iScrollBar || event.originalComponent == &iScrollBar) return;

	if ( iMinimizedState || ( !iMinimizedState && event.eventComponent == (juce::Component*)this) )
	{
		if (!iMouseTitleIsActive && iTitle.getBoundsInParent().contains(event.getPosition()))
		{
			iMouseTitleIsActive = true;
			repaint();
		}
		else if (iMouseTitleIsActive && !iTitle.getBoundsInParent().contains(event.getPosition()))
		{
			iMouseTitleIsActive = false;
			repaint();
		}
	}
}

void CKanbanColumnComponent::mouseExit(const MouseEvent & event)
{
	if (event.eventComponent == &iScrollBar || event.originalComponent == &iScrollBar) return;

	iMouseTitleIsActive = false;
	repaint();
}

bool CKanbanColumnComponent::keyPressed(const KeyPress & key)
{
	if (key.getKeyCode() == key.pageDownKey)
	{
		iScrollBar.moveScrollbarInPages(1);
		return true;
	}
	else if (key.getKeyCode() == key.pageUpKey) 
	{
		iScrollBar.moveScrollbarInPages(-1);
		return true;
	}
	return false;
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

bool CKanbanColumnComponent::isMinimized() const
{
	return iMinimizedState;
}

void CKanbanColumnComponent::setMinimized(bool aMinimized, bool aUpdateOwner)
{
	if (!aMinimized)
	{
		iMinimizedState = false;
		iViewportLayout.unhideAllCards();
		iViewportLayout.setAlpha(1);
		iAddCardButton.setVisible(true);
		iSetupButton.setVisible(true);
		iScrollBar.setVisible(true);
		iTitleMinimalDueDate.setVisible(false);
		//iTitle.addMouseListener(this, true);
		//iTitleCardsCount.addMouseListener(this, true);
		iOwner.updateSearch();
	}
	else
	{
		iMinimizedState = true;
		iViewportLayout.hideAllCards();
		iViewportLayout.setAlpha(0);
		iAddCardButton.setVisible(false);
		iSetupButton.setVisible(false);
		iScrollBar.setVisible(false);
		if (!iDueDateDone)
		{
			iTitleMinimalDueDate.setVisible(true);
		}
		//iTitle.removeMouseListener(this);
		//iTitleCardsCount.removeMouseListener(this);

		//Rectangle<int> r(getLocalBounds());
		//iViewportLayout.setBounds(r);
		updateColumnTitle();
	}

	if ( aUpdateOwner ) iOwner.updateColumnSize(this, iMinimizedState);
}

int CKanbanColumnComponent::getMaxWip() const
{
	if (iViewportLayout.isMaxWipSet()) return iViewportLayout.getCardsMaxWip();
	else return 0;
}

void CKanbanColumnComponent::setMaxWip(int aWip)
{
	iViewportLayout.setMaxWip(aWip);
}

void CKanbanColumnComponent::setGridItem(const GridItem & aGridItem)
{
	iGridItem = aGridItem;
}

const GridItem & CKanbanColumnComponent::getGridItem()
{
	return iGridItem;
}

bool CKanbanColumnComponent::isGridColumn(int aStartCol, int aEndCol)
{
	int s = iGridItem.column.start.getNumber();
	int e = iGridItem.column.end.getNumber();
	return ( s == aStartCol && e == aEndCol);
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
	menu.addItem("Minimize", true, false, [&]()
	{
		setMinimized(true, true);
	});
	menu.addItem("Set max WIP", [&]()
	{
		AlertWindow w("Set max WIP", "Provide maximum cards count for this column (Work In Progress)", AlertWindow::QuestionIcon);

		w.addTextEditor("text", (this->iViewportLayout.isMaxWipSet() ? String(this->iViewportLayout.getCardsMaxWip()) : "" ), "Max WIP:");
		w.addButton("OK", 1, KeyPress(KeyPress::returnKey, 0, 0));
		w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

		if (w.runModalLoop() != 0) // is they picked 'ok'
		{
			auto text = w.getTextEditorContents("text");
			int ti = text.getIntValue();
			if ( ti < 0 || ti >= 100 || ( ti == 0 && text.length() > 0 ) )
			{
				AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Max WIP", "Wrong Max WIP value", "OK");
			}
			else
			{ // set wip
				if ( ti > 0 && ti < this->iViewportLayout.getCardsCount())
				{
					AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Max WIP", "Column cards count is larger than specified WIP value. Firstly reduce cards count.", "OK");
				}
				else
				{
					this->iViewportLayout.setMaxWip(ti);
					this->updateColumnTitle();
				}
			}
		}
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

String CKanbanColumnComponent::getMinimalDueDate(juce::Colour* aColour)
{
	auto ar = iOwner.getCardsForColumn(this);

	juce::int64 min = INT64_MAX;
	String dd;

	if (ar.size() > 0)
	{
		for (int i = ar.size() - 1; i >= 0; --i)
		{
			if (ar[i]->isDueDateSet() && !ar[i]->isDone())
			{
				auto t = ar[i]->getDueDate().toMilliseconds();
				if (t < min)
				{
					dd = ar[i]->getDueDateAsString(aColour);
					min = t;
				}
			}
		}
	}

	return dd;
}

void CKanbanColumnComponent::updateColumnTitle()
{
	if (iViewportLayout.isMaxWipSet())
	{
		iTitleCardsCount.setText(" (" + String(iViewportLayout.getUnhiddenCardsCount() /*getCardsCount()*/) + "/" + String(iViewportLayout.getCardsMaxWip()) + ")", NotificationType::dontSendNotification);
	}
	else
	{
		iTitleCardsCount.setText(" (" + String(iViewportLayout.getUnhiddenCardsCount() /*getCardsCount()*/) + ")", NotificationType::dontSendNotification);
	}

	iTitle.setText(iColumnTitle, NotificationType::dontSendNotification);

	if (iTitleMinimalDueDate.isVisible())
	{
		juce::Colour c;
		auto s = getMinimalDueDate(&c);
		if (!s.isEmpty())
		{
			iTitleMinimalDueDate.setColour(Label::textColourId, c.darker(0));
		}
		iTitleMinimalDueDate.setText(s, NotificationType::dontSendNotification);
	}

//	iTitleCardsCount.setText("[" + String(iViewportLayout.getCardsCount()) +"]", NotificationType::dontSendNotification);
//	iTitle.setText(iColumnTitle + " | " + String(iViewportLayout.getCardsCount()) + "", NotificationType::dontSendNotification);
}

void CKanbanColumnComponent::scrollBarMoved(ScrollBar * scrollBarThatHasMoved, double newRangeStart)
{
	//Logger::outputDebugString("sc: " + String(newRangeStart));
	iViewportLayout.iScrollPos = newRangeStart;
	iViewportLayout.resized();
}

