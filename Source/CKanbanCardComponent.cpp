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
#include "Cconfiguration.h"
#include "CKanbanCardPropertiesComponent.h"


//==============================================================================
CKanbanCardComponent::CKanbanCardComponent(CKanbanColumnContentComponent* aOwner) : iIsDragging(false), iOwner(aOwner), iMouseActive(false)
{
	int w = CConfiguration::getIntValue("KanbanCardWidth");
	int h = CConfiguration::getIntValue("KanbanCardHeight");

	iColorBar = CConfiguration::getColourPalette().getColor(2);
	//	juce::Colours::coral;// gold);
	//iColorBar.fromRGBA(0, 0, 0, 0);

	iLabel.setText("test", NotificationType::dontSendNotification);
	addAndMakeVisible(iLabel);
	iLabel.addMouseListener(this,true);

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
	}
}

void CKanbanCardComponent::resized()
{
	Rectangle<int> r(getBounds());
	r.removeFromLeft(10);
	iLabel.setBounds(r);
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
		showProperties();

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

void CKanbanCardComponent::mouseExit(const MouseEvent& event)
{
	iMouseActive = false;
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

void CKanbanCardComponent::setupFromJson(const String& aLabel, const String& aNotes, const String& aColour)
{
	iLabel.setText(URL::removeEscapeChars(aLabel), NotificationType::dontSendNotification);
	iColorBar = Colour::fromString(URL::removeEscapeChars(aColour));
	iNotes = URL::removeEscapeChars(aNotes);
}

void CKanbanCardComponent::setText(const String& aString)
{
	iLabel.setText(aString, NotificationType::dontSendNotification);	
}

String CKanbanCardComponent::getText()
{
	return iLabel.getText();
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

String CKanbanCardComponent::toJson()
{
	String l = URL::addEscapeChars(iLabel.getText().toUTF8(), false);
	String n = URL::addEscapeChars(iNotes.toUTF8(), false);
	String c = URL::addEscapeChars(iColorBar.toString().toUTF8(), false);
	//String::fromUTF8(URL::removeEscapedChars(url));
	String ret("{ \"text\":\"" + l + "\", \"notes\":\"" + n + "\", \"colour\":\"" + c + "\", \"columnId\":" + String(iOwner->getOwner().getColumnId()) + " }");
	return ret;
}


void CKanbanCardComponent::showProperties()
{
	auto comp = std::make_unique<CKanbanCardPropertiesComponent>(*this);
	CallOutBox* box = &CallOutBox::launchAsynchronously(std::move(comp), this->getScreenBounds(), nullptr);
	box->setAlwaysOnTop(true);
}

void CKanbanCardComponent::setChildrenVisibility(bool aHidden)
{
	iLabel.setVisible(!aHidden);
}


