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
#include "CKanbanHttpClient.h"

const int KTitleHeight = 25;
const int KEditModeMargin = 30;


void CKanbanColumnComponent::ComponentListenerEditButton::componentMovedOrResized(Component& component, bool wasMoved, bool wasResized)
{
	if (wasResized)
	{
		DrawablePath btnImg3;
		{
			Path p;
			int w = component.getWidth();
			int h = component.getHeight();
			int m = 4;
			p.addLineSegment(Line<float>(m, h / 2, w - m, h / 2), 1);
			p.addLineSegment(Line<float>(w / 2, h / 2 - (w - 2 * m) / 2, w / 2, h / 2 + (w - 2 * m) / 2 ), 1);
			p.addRectangle(-5, -5, w + 10, h + 10);
			btnImg3.setPath(p);
			PathStrokeType pt(1, PathStrokeType::curved);
			btnImg3.setStrokeType(pt);
//			btnImg3.setStrokeFill(FillType(component.getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).brighter(0.1)));
			btnImg3.setStrokeFill(FillType(component.getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId)));
			btnImg3.setFill(component.getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).brighter(0.04));
		}
		DrawablePath btnImgOver3(btnImg3);
//		btnImgOver3.setStrokeFill(FillType(Colours::whitesmoke));
		btnImgOver3.setStrokeFill(FillType(component.getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).brighter(0.2)));
		btnImgOver3.setFill(component.getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).brighter(0.08f));
		DrawablePath btnImgPushed3(btnImgOver3);
		btnImgPushed3.setFill(component.getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).darker());

		DrawableButton& b = static_cast<DrawableButton&>(component);
		b.setImages(&btnImg3, &btnImgOver3, &btnImgPushed3);

	}
}


CKanbanColumnComponent::CKanbanColumnComponent(int aColumnId, const String& aTitle, CKanbanBoardComponent& aOwner) : iOwner(aOwner), iColumnId(aColumnId), iMinimizedState(false), iIsFrameActive(false), iDueDateDone(false), iSortedAsc(false), iColumnTitle(aTitle), iViewportLayout(*this), iScrollBar(true), iAddCardButton("Add card", DrawableButton::ImageRaw), iSetupButton("Setup", DrawableButton::ImageRaw), iProgressBar(iProgressBarValue), iMouseTitleIsActive(false), iEditMode(false), iEditButtonRightVisible(false), iEditModeLeft("+", DrawableButton::ImageRaw), iEditModeRight("+", DrawableButton::ImageRaw), iRefreshOngoing(false)
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

	iProgressBarValue = -1;
	addAndMakeVisible(iProgressBar);
	iProgressBar.setVisible(false);

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
		p.addRectangle(-w, -w, 4 * w, 4 * w);
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

	iEditModeLeft.addComponentListener(&iEditModeButtonListener);
	addAndMakeVisible(iEditModeLeft);
	iEditModeLeft.onClick = [this]
	{
		this->kanbanBoard().addColumn(this, true);
	};
	iEditModeRight.addComponentListener(&iEditModeButtonListener);
	addAndMakeVisible(iEditModeRight);
	iEditModeRight.onClick = [this]
	{
		this->kanbanBoard().addColumn(this, false);
	};

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
			g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).brighter(0.08f));   // clear the background
			g.fillRect(getLocalBoundsForCardsSection());
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
	auto r = getLocalBoundsForCardsSection();

	if (!iMinimizedState)
	{
		g.setColour(juce::Colours::grey);

		g.drawLine( r.getTopLeft().x, iTitle.getBottom(), r.getRight(), iTitle.getBottom(), 1);
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

	// draw an outline around the component
	g.drawRect(r, ps);

	if (iEditMode)
	{
		g.setColour(juce::Colours::gold);
		g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).brighter(0.04f));   // clear the background

		//g.fillRoundedRectangle(getLocalBoundsForEditLeft().toFloat(), 2);
		//g.fillRoundedRectangle(getLocalBoundsForEditRight().toFloat(), 2);
	}
}

void CKanbanColumnComponent::resized()
{
	int m = CConfiguration::getIntValue("KanbanCardHorizontalMargin");

	if (iEditMode)
	{
		auto r2 = getLocalBoundsForEditLeft();
		r2.removeFromRight(m);
		iEditModeLeft.setBounds(r2);
		r2 = getLocalBoundsForEditRight();
		r2.removeFromLeft(m);
		iEditModeRight.setBounds(r2);
	}
	iEditModeLeft.setVisible(iEditMode);
	if (iEditButtonRightVisible || !iEditMode) iEditModeRight.setVisible(iEditMode);

	Rectangle<int> r(getLocalBoundsForCardsSection());

	if (iMinimizedState)
	{
		int a = 0;
		if (iEditMode) a = KEditModeMargin;
		iTitle.setBounds( a, 0, r.getHeight(), r.getWidth());
		iTitle.setJustificationType(Justification::centredRight);
		iTitle.setTransform(AffineTransform::rotation(-MathConstants<float>::halfPi, 0, 0).translated(a, iTitle.getWidth() + KTitleHeight / 2 + a));

		iViewportLayout.setBounds(r);

		iTitleCardsCount.setBounds(r.removeFromBottom(KTitleHeight));
		iTitleCardsCount.setJustificationType(Justification::centredTop);

		auto bb = iTitleCardsCount.getBounds();
		bb.translate(0, -KTitleHeight);
		iTitleMinimalDueDate.setBounds(bb);
		iTitleMinimalDueDate.setJustificationType(Justification::centredTop);

		iProgressBar.setVisible(false);
	}
	else
	{
		Rectangle<int> r1(r.removeFromTop(KTitleHeight));
		Rectangle<int> r3(r1);
		Rectangle<int> r1b = r1.removeFromRight(KTitleHeight);

		r1.removeFromRight(KTitleHeight);
		iTitle.setBounds(r1);
		iTitle.setTransform(AffineTransform());
		iTitle.setJustificationType(Justification::left);

		r3.setHeight(m - 1);
		r3.translate(0, KTitleHeight + 1);
		iProgressBar.setBounds(r3);
		iProgressBar.setVisible(iRefreshOngoing);

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
		r2.setLeft(r2.getRight() - 8);
		r2.setWidth(7);
		iScrollBar.setBounds(r2);

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
		menu.addSeparator();
		if (showRefreshMenuEntry())
		{
			menu.addItem("Refresh", !iRefreshOngoing, false, [&]()
			{
				iMouseTitleIsActive = false;
				if (!isMinimized()) iProgressBar.setVisible(true);
				repaint();
				Thread::launch([&]() { refreshThreadWorkerFunction(); }); // todo: exit application during running thread leaks this object
			});
		}
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

	if ( ( !iMinimizedState && event.eventComponent == (juce::Component*)this) )
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
	else if (iMinimizedState)
	{
		auto ev = event.getEventRelativeTo(this);
		auto r = getLocalBoundsForCardsSection();
		if (!iMouseTitleIsActive && r.contains(ev.getPosition()))
		{
			iMouseTitleIsActive = true;
			repaint();
		}
		else if (iMouseTitleIsActive && !r.contains(ev.getPosition()))
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
	//iOwner.
}

void CKanbanColumnComponent::updateScrollbars()
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

const GridItem & CKanbanColumnComponent::getGridItem() const
{
	return iGridItem;
}

bool CKanbanColumnComponent::isGridColumn(int aStartCol, int aEndCol)
{
	int s = iGridItem.column.start.getNumber();
	int e = iGridItem.column.end.getNumber();
	return ( s == aStartCol && e == aEndCol);
}

bool CKanbanColumnComponent::isColumnSameSize(const CKanbanColumnComponent & aColumn)
{
	auto g = aColumn.getGridItem();
	return (g.row.start.getNumber() == iGridItem.row.start.getNumber()) && (g.row.end.getNumber() == iGridItem.row.end.getNumber());
}

void CKanbanColumnComponent::moveGridItemRight()
{
	iGridItem.setArea((int)iGridItem.row.start.getNumber(), (int)iGridItem.column.start.getNumber() + 1, (int)iGridItem.row.end.getNumber(), (int)iGridItem.column.end.getNumber() + 1);
}


Rectangle<int> CKanbanColumnComponent::getLocalBoundsForCardsSection()
{
	auto r = getLocalBounds();
	if (iEditMode)
	{
		if ( iEditButtonRightVisible ) r.reduce(KEditModeMargin, 0);
		else r.setLeft(r.getTopLeft().x + KEditModeMargin);
	}
	return r;
}

Rectangle<int> CKanbanColumnComponent::getLocalBoundsForEditLeft()
{
	if (!iEditMode) return Rectangle<int>();
	
	auto r = getLocalBounds();
	r.removeFromRight(r.getWidth() - KEditModeMargin);
	return r;
}

Rectangle<int> CKanbanColumnComponent::getLocalBoundsForEditRight()
{
	if (!iEditMode || !iEditButtonRightVisible) return Rectangle<int>();

	auto r = getLocalBounds();
	r.removeFromLeft(r.getWidth() - KEditModeMargin);
	return r;
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
	menu.addSeparator();
	menu.addItem("Minimize", true, false, [&]()
	{
		setMinimized(true, true);
	});
	menu.addItem("Archive column", iViewportLayout.getCardsCount() > 0, false, [&]()
	{
		this->archive();
	});
	menu.addSeparator();
	menu.addItem("Due date done", true, iDueDateDone, [&]()
	{
		this->setColumnDueDateDone(!iDueDateDone);
	});
	menu.addItem("Set max WIP", [&]()
	{
		AlertWindow w("Set max WIP", "Provide maximum cards count for this column (Work In Progress)", AlertWindow::QuestionIcon);

		w.addTextEditor("text", (this->iViewportLayout.isMaxWipSet() ? String(this->iViewportLayout.getCardsMaxWip()) : ""), "Max WIP:");
		w.addButton("OK", 1, KeyPress(KeyPress::returnKey, 0, 0));
		w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

		auto txt = w.getTextEditor("text");
		txt->setExplicitFocusOrder(1);

		if (w.runModalLoop() != 0) // is they picked 'ok'
		{
			auto text = w.getTextEditorContents("text");
			int ti = text.getIntValue();
			if (ti < 0 || ti >= 100 || (ti == 0 && text.length() > 0))
			{
				AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Max WIP", "Wrong Max WIP value", "OK");
			}
			else
			{ // set wip
				if (ti > 0 && ti < this->iViewportLayout.getCardsCount())
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
	menu.addItem("Edit name", [&]()
	{
		AlertWindow w("Rename column", "Provide new name for this column", AlertWindow::QuestionIcon);

		w.addButton("OK", 1, KeyPress(KeyPress::returnKey, 0, 0));
		w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));
		w.addTextEditor("text", this->iColumnTitle);

		auto txt = w.getTextEditor("text");
		txt->setExplicitFocusOrder(1);

		if (w.runModalLoop() != 0) // is they picked 'ok'
		{
			auto text = w.getTextEditorContents("text");
			if (text.length() == 0)
			{
				AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Column", "Bad column name.", "OK");
			}
			else
			{
				this->iColumnTitle = text;
				this->updateColumnTitle();
			}
		}
	});
	if (showRefreshMenuEntry())
	{
		menu.addItem("Edit refresh settings", [&]()
		{
			refreshSetupFunction();
		});
	}
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

	auto txt = w.getTextEditor("text");
	txt->setExplicitFocusOrder(1);

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
					dd = ar[i]->getDueDateAsString(aColour, true);
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

void CKanbanColumnComponent::setEditMode(bool aLeftEnabled, bool aRightEnabled)
{
	iEditMode = aLeftEnabled || aRightEnabled;
	iEditButtonRightVisible = aRightEnabled;
}

bool CKanbanColumnComponent::isEditModeRightVisible()
{
	return iEditButtonRightVisible;
}

void CKanbanColumnComponent::setEditModeRightVisible(bool aVisible)
{
	iEditButtonRightVisible = aVisible;
	iEditModeRight.setVisible(iEditButtonRightVisible);
}

int CKanbanColumnComponent::getEditModeMargin()
{
	return KEditModeMargin;
}

String CKanbanColumnComponent::outputAsJson()
{
	String s;
	s << "{ \"title\":\"" + URL::addEscapeChars(getTitle(), false) + "\", \"id\":" + String(getColumnId());
	if (isColumnDueDateDone()) s << ", \"dueDateDone\":true ";
	if (isMinimized()) s << ", \"minimized\":true ";
	if (getMaxWip() > 0) s << ", \"wip\":" + String(getMaxWip());
	s << ", \"type\":" + String(getColumnTypeId());
	outputAdditionalDataToJson(s);
	s << " }";
	return s;
}

void CKanbanColumnComponent::scrollBarMoved(ScrollBar * scrollBarThatHasMoved, double newRangeStart)
{
	//Logger::outputDebugString("sc: " + String(newRangeStart));
	iViewportLayout.iScrollPos = newRangeStart;
	iViewportLayout.resized();
}

