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
CKanbanCardComponent::CKanbanCardComponent(CKanbanColumnContentComponent* aOwner) : iIsDragging(false), iOwner(aOwner), iMouseActive(false), iIsUrlSet(false), iIsUrlMouseActive(false)
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
		g.fillRect(4,4,2,getHeight()-8);

		if (iIsUrlSet)
		{
			g.setColour(juce::Colours::grey);
			if (iIsUrlMouseActive) g.setColour(juce::Colours::lightgrey);
			//g.fillRect(iRectUrl);
			g.drawArrow(iLineUrl, 2, 7, 7);
		}
	}
}

void CKanbanCardComponent::resized()
{
	Rectangle<int> r(getBounds());
	r.removeFromLeft(10);
	iLabel.setBounds(r);

	iRectUrl = getBounds().removeFromRight(15);
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
}

void CKanbanCardComponent::setText(const String& aString)
{
	iLabel.setText(aString, NotificationType::dontSendNotification);	
}

String CKanbanCardComponent::getText()
{
	return iLabel.getText();
}

void CKanbanCardComponent::setUrl(const String& aString)
{
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
	getProperties().set("tags", aString);
}

void CKanbanCardComponent::setColour(Colour aColor)
{
	iColorBar = aColor;
	repaint();
}

Colour CKanbanCardComponent::getColour()
{
	return iColorBar;
}

void CKanbanCardComponent::setNotes(const String& aString)
{
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
	String ret("{ \"text\":\"" + l + "\", \"colour\":\"" + c + "\", \"columnId\":" + String(iOwner->getOwner().getColumnId()));

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

	ret += " }";
	return ret;
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


