/*
  ==============================================================================

    CKanbanCardComponent.cpp
    Created: 4 Nov 2020 5:20:44pm
    Author:  michal.strug

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CKanbanCardComponent.h"
#include "CKanbanColumnContentComponent.h"
#include "CKanbanColumnComponent.h"
#include "CKanbanBoard.h"
#include "Cconfiguration.h"
#include "CKanbanCardPropertiesComponent.h"


//==============================================================================
CKanbanCardComponent::CKanbanCardComponent(CKanbanColumnContentComponent* aOwner) : iIsDragging(false), iOwner(aOwner), iMouseActive(false), iIsUrlSet(false), iIsUrlMouseActive(false), iIsDueDateSet(false), iCreationDate(juce::Time::getCurrentTime())
{
	int w = CConfiguration::getIntValue("KanbanCardWidth");
	int h = CConfiguration::getIntValue("KanbanCardHeight");

	iColorBar = CConfiguration::getColourPalette().getLastColor();
	//	juce::Colours::coral;// gold);
	//iColorBar.fromRGBA(0, 0, 0, 0);

	iLabel.setText("test", NotificationType::dontSendNotification);
	addAndMakeVisible(iLabel);
	iLabel.addMouseListener(this,true);
	iLabel.setMinimumHorizontalScale(1);

	iButtonUrl.setButtonText("+");
	addAndMakeVisible(iButtonUrl);
	

	//setOpaque(true);

	setSize(w, h);

	iLastUpdateDate = iCreationDate;
}

CKanbanCardComponent::~CKanbanCardComponent()
{
}

void CKanbanCardComponent::paint (juce::Graphics& g)
{
	if (iIsDragging)
	{
		//g.fillAll(juce::Colours::darkred);
		g.setColour(juce::Colours::darkgrey);
		g.drawRect(getLocalBounds(), 1);   // draw an outline around the component
	}
	else
	{
		if (iMouseActive)
		{
			g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).brighter(0.08f));   // clear the background
		}
		else
		{
			g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));   // clear the background
		}

		g.setColour (juce::Colours::grey);
		g.drawRect(getLocalBounds(), 1);   // draw an outline around the component

		g.setColour(iColorBar);
		if (getHeight()>8) g.fillRect(4,4,2,getHeight()-8);

		if (iIsUrlSet)
		{
			g.setColour(juce::Colours::grey);
			if (iIsUrlMouseActive) g.setColour(juce::Colours::lightgrey);
			//g.fillRect(iRectUrl);
			g.drawArrow(iLineUrl, 2, 7, 7);
		}

		if (iIsDueDateSet)
		{
			auto b = getLocalBounds();
			auto b1 = b.removeFromRight(50);
			auto b2 = b1.removeFromTop(20);
			b2.translate(-1, 0);
			g.setColour(Colours::lightgrey);
			g.setFont(juce::Font(12.0f));

			
			auto ct = Time::getCurrentTime();
			RelativeTime d = Time( iDueDate.getYear(), iDueDate.getMonth(), iDueDate.getDayOfMonth(),0,0) - Time(ct.getYear(), ct.getMonth(), ct.getDayOfMonth(), 0, 0);
			double dval = d.inDays();
			String s;

			if (dval > 14) s = String((int)ceil(d.inWeeks())) + "wks";
			else if (dval == 0) s = "tooday";
			else if (dval == 1) s = "tomorrow";
			else if (dval == -1) s = "yesterday";
			else if (dval < -365) s = String((int)ceil(d.inWeeks()) / 54) + "y";
			else if (dval < -14) s = String((int)ceil(d.inWeeks())) + "wks";
			else s = String((int)ceil(dval)) + "d";
			g.drawText( s, b2, Justification::topRight, false);
		}
	}
}

void CKanbanCardComponent::resized()
{
	Rectangle<int> r(getLocalBounds());
	if (r.getHeight() == 0) return;
		
	r.removeFromLeft(10);
	r.removeFromRight(10);
	iLabel.setBounds(r);

	iRectUrl = getLocalBounds().removeFromRight(15);
	iRectUrl = iRectUrl.removeFromBottom(15);
	iRectUrl -= Point<int>(1, 1);

	iLineUrl.setStart(iRectUrl.getTopLeft().x + 2, iRectUrl.getBottom() - 2);
	iLineUrl.setEnd(iRectUrl.getRight() - 2, iRectUrl.getTopLeft().y + 2);
}

void CKanbanCardComponent::mouseDown(const MouseEvent& event)
{
}

void CKanbanCardComponent::mouseDrag(const MouseEvent& event)
{
	if ( !iIsDragging && event.mods.isLeftButtonDown())
	{
		if (auto* dragContainer = DragAndDropContainer::findParentDragContainerFor(this))
		{
			dragContainer->startDragging(KanbanCardComponentDragDescription, this);
			iIsDragging = true;
			setChildrenVisibility(iIsDragging);
			repaint();
		}
	}
}

void CKanbanCardComponent::mouseUp(const MouseEvent& event)
{
	if (iIsDragging)
	{
		iIsDragging = false;
		setChildrenVisibility(iIsDragging);
		repaint();
	}
	else if (event.mods.isLeftButtonDown())
	{
		URL u(getProperties()["url"]);
		if (iIsUrlSet &&
			((event.eventComponent == this && iRectUrl.contains(event.getPosition())) ||
			(event.eventComponent != this && iRectUrl.contains(event.getEventRelativeTo(this).getPosition()))))
		{
			//u.isWellFormed() // todo
			u.launchInDefaultBrowser();
		}
		else
		{
			showProperties();
		}

		/*PopupMenu menu;
		menu.addItem("Edit card", [&]()
		{
			auto comp = std::make_unique<CKanbanCardPropertiesComponent>(*this);
			CallOutBox* box = &CallOutBox::launchAsynchronously(std::move(comp), this->getScreenBounds(), nullptr);
			box->setAlwaysOnTop(true);
		});
		menu.addItem("Remove card", [&]()
		{
		});
		menu.show();*/
	}
	else if (event.mods.isRightButtonDown())
	{
		PopupMenu menu;
		menu.addItem("Remove", [&]()
		{
			this->getOwner()->getOwner().removeCard(this);
		});
		menu.addSeparator();
		menu.addItem("Move top", [&]()
		{
			this->iMouseActive = false;
			this->getOwner()->moveCardTop(this);
			this->repaint();
		});
		menu.addItem("Move bottom", [&]()
		{
			this->iMouseActive = false;
			this->getOwner()->moveCardBottom(this);
			this->repaint();
		});
		menu.show();
	}
}

void CKanbanCardComponent::mouseDoubleClick(const MouseEvent& event)
{
	Logger::outputDebugString("DB");
}

void CKanbanCardComponent::mouseEnter(const MouseEvent& event)
{
	iMouseActive = true;
	repaint();
}

void CKanbanCardComponent::mouseMove(const MouseEvent & event)
{
	if (iMouseActive && iIsUrlSet)
	{
		bool tmp = iIsUrlMouseActive;
		//Logger::outputDebugString("pos: " + String(event.getPosition().x) + ", " + String(event.getPosition().y));
		//Logger::outputDebugString("x: " + String(iRectUrl.getTopLeft().x) + "  y:" + String(iRectUrl.getTopLeft().y));
		//Logger::outputDebugString(event.eventComponent == this ? "ok" : "nok");
		if (event.eventComponent == this)
		{
			iIsUrlMouseActive = iRectUrl.contains(event.getPosition());
		}
		else
		{
			auto p = event.getEventRelativeTo(this).getPosition();
			//Logger::outputDebugString("pos2: " + String(p.x) + ", " + String(p.y));
			iIsUrlMouseActive = iRectUrl.contains(p);	
		}
		if ( tmp != iIsUrlMouseActive ) repaint();
	}
}

void CKanbanCardComponent::mouseExit(const MouseEvent& event)
{
	iMouseActive = false;
	iIsUrlMouseActive = false;
	repaint();
}


CKanbanColumnContentComponent* CKanbanCardComponent::getOwner()
{
	return iOwner;
}

void CKanbanCardComponent::setOwner(CKanbanColumnContentComponent* aOwner)
{
	iOwner = aOwner;
}

void CKanbanCardComponent::openPropertiesWindow()
{
	showProperties();
}

void CKanbanCardComponent::setupFromJson(const NamedValueSet& aValues) //const String& aLabel, const String& aNotes, const String& aColour)
{
	iLabel.setText(URL::removeEscapeChars(aValues["text"]), NotificationType::dontSendNotification);
	iColorBar = Colour::fromString(URL::removeEscapeChars(aValues["colour"]));
	iNotes = URL::removeEscapeChars(aValues["notes"]);
	setUrl(URL::removeEscapeChars(aValues["url"]));
	setTags(URL::removeEscapeChars(aValues["tags"]));
	iIsDueDateSet = aValues["dueDateSet"];
	iCreationDate = Time(aValues["creationDate"].toString().getLargeIntValue());
	iDueDate = Time(aValues["dueDate"].toString().getLargeIntValue());
	iLastUpdateDate = Time(aValues["lastUpdateDate"].toString().getLargeIntValue());

	// todo:  wa for versions prev 0.25: (remove in future)
	if (iCreationDate.toMilliseconds() == 0) iCreationDate = Time(2021, 0, 1, 12, 00);
	if (iLastUpdateDate.toMilliseconds() == 0) iLastUpdateDate = Time(2021, 0, 1, 12, 00);
	if (iIsDueDateSet && iDueDate.toMilliseconds() == 0) iDueDate = Time(2021, 0, 1, 12, 00);
}

void CKanbanCardComponent::setText(const String& aString)
{
	updateLastUpdateDate();
	iLabel.setText(aString, NotificationType::dontSendNotification);
}

String CKanbanCardComponent::getText()
{
	return iLabel.getText();
}

void CKanbanCardComponent::setUrl(const String& aString)
{
	updateLastUpdateDate();
	if ( aString.contains("://") || aString.indexOf("//") == 0 )
	{
		getProperties().set("url", aString);
		iIsUrlSet = true;
	}
	else if ( aString.length() > 0 )
	{
		getProperties().set("url", "http://"+aString);
		iIsUrlSet = true;
	}
	else
	{
		getProperties().remove("url");
		iIsUrlSet = false;
	}
	/*iButtonUrl.setVisible(aString.length() > 0);
	iButtonUrl.setURL(aString);
	resized();*/
}

void CKanbanCardComponent::setTags(const String& aString)
{
	updateLastUpdateDate();
	getProperties().set("tags", aString);
}

bool CKanbanCardComponent::isDueDateSet()
{
	return iIsDueDateSet;
}

void CKanbanCardComponent::setDueDate(bool aIsSet, juce::Time& aDueDate)
{
	updateLastUpdateDate();
	iIsDueDateSet = aIsSet;
	if (aIsSet) iDueDate = aDueDate;
	else iDueDate = Time(0);
}

juce::Time CKanbanCardComponent::getCreationDate()
{
	return iCreationDate;
}

juce::Time CKanbanCardComponent::getLastUpdateDate()
{
	return iLastUpdateDate;
}

juce::Time CKanbanCardComponent::getDueDate()
{
	return iDueDate;
}

void CKanbanCardComponent::updateLastUpdateDate()
{
	iLastUpdateDate = Time::getCurrentTime();
}

void CKanbanCardComponent::setColour(Colour aColor)
{
	updateLastUpdateDate();
	iColorBar = aColor;
	repaint();
}

Colour CKanbanCardComponent::getColour()
{
	return iColorBar;
}

void CKanbanCardComponent::setNotes(const String& aString)
{
	updateLastUpdateDate();
	iNotes = aString;
}

String CKanbanCardComponent::getNotes()
{
	return iNotes;
}

void CKanbanCardComponent::deselect()
{
	iMouseActive = false;
	repaint();
}

String CKanbanCardComponent::toJson()
{
	String l = URL::addEscapeChars(iLabel.getText().toUTF8(), false);
	String c = URL::addEscapeChars(iColorBar.toString().toUTF8(), false);
	//String::fromUTF8(URL::removeEscapedChars(url));
	String ret("{ \"text\":\"" + l + "\", \"colour\":\"" + c + "\", \"columnId\":" + String(getOwnerColumnId()));

	String n = iNotes.toUTF8();
	if (n.length() > 0)
	{
		n = URL::addEscapeChars(n, false);
		ret += ", \"notes\":\"" + n + "\"";
	}

	String url = getProperties()["url"];
	if (url.length() > 0)
	{
		url = URL::addEscapeChars(url, false);
		ret += ", \"url\":\"" + url + "\"";
	}

	String tags = getProperties()["tags"];
	if (tags.length() > 0)
	{
		tags = URL::addEscapeChars(tags, false);
		ret += ", \"tags\":\"" + tags + "\"";
	}

	ret += ", \"dueDateSet\":" + String(iIsDueDateSet ? "true" : "false");
	ret += ", \"creationDate\":" + String(iCreationDate.toMilliseconds());
	ret += ", \"dueDate\":" + String(iDueDate.toMilliseconds());
	ret += ", \"lastUpdateDate\":" + String(iLastUpdateDate.toMilliseconds());

	ret += " }";
	return ret;
}

int CKanbanCardComponent::getOwnerColumnId() const
{
	if (!iOwner) return -1;
	return iOwner->getOwner().getColumnId();
}


void CKanbanCardComponent::showProperties()
{
	iMouseActive = true;
	auto comp = std::make_unique<CKanbanCardPropertiesComponent>(*this);
	CallOutBox* box = &CallOutBox::launchAsynchronously(std::move(comp), this->getScreenBounds(), nullptr);
	box->setAlwaysOnTop(true);
}

void CKanbanCardComponent::setChildrenVisibility(bool aHidden)
{
	iLabel.setVisible(!aHidden);
}


