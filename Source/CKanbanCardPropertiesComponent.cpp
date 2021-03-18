/*
  ==============================================================================

    PropertiesComponent.cpp
    Created: 13 Dec 2018 12:00:05am
    Author:  Michał

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CKanbanCardPropertiesComponent.h"
#include "CKanbanCardComponent.h"
#include "CConfiguration.h"
#include "ColourPalette.h"


//==============================================================================
CKanbanCardPropertiesComponent::CKanbanCardPropertiesComponent(CKanbanCardComponent &aOwner) : iOwner(aOwner)
{
	setViewportIgnoreDragFlag(true);
	addKeyListener(this);

	int w = 360;
	int wl = 60;
	int wm = 20;

	//iBar->getProperties().set("name", "test");
	addAndMakeVisible(iTextName);
	iTextName.addKeyListener(this);
	iTextName.setBounds(10, 10, w, 24);
	iTextName.setSelectAllWhenFocused(true);
	iTextName.setText(aOwner.getText());
	iTextName.onTextChange = [this]
	{
		this->changesApply();
	};

	int yofs = iTextName.getBottom() + 12;

	iLabel.setText("Notes:", juce::NotificationType::dontSendNotification);
	addAndMakeVisible(iLabel);
	iLabel.setBounds(10, yofs, wl, 24);
	
	iTextEditor.setMultiLine(true);
	iTextEditor.addKeyListener(this);
	iTextEditor.setReturnKeyStartsNewLine(true);
	iTextEditor.setScrollbarsShown(true);	
	iTextEditor.setText( aOwner.getNotes() );
	addAndMakeVisible(iTextEditor);
	iTextEditor.setBounds(10 + wl + 10, yofs, w - wl - 10, 24 + 24 * 3);
	iTextEditor.onTextChange = [this, &aOwner]
	{
		aOwner.setNotes(this->iTextEditor.getText());
	};

	yofs = iTextEditor.getBottom() + 12;

	iLabelUrl.setText("Url:", juce::NotificationType::dontSendNotification);
	addAndMakeVisible(iLabelUrl);
	iLabelUrl.setBounds(10, yofs, wl, 24);

	addAndMakeVisible(iTextUrl);
	iTextUrl.addKeyListener(this);
	iTextUrl.setBounds(10 + wl + 10, yofs, w - wl - 10, 24);
	//iTextUrl.setSelectAllWhenFocused(true);
	iTextUrl.setText(aOwner.getProperties()["url"]);
	iTextUrl.onTextChange = [this]
	{
		this->changesApply();
	};

	yofs = iTextUrl.getBottom() + 12;

	iLabelTags.setText("Tags:", juce::NotificationType::dontSendNotification);
	addAndMakeVisible(iLabelTags);
	iLabelTags.setBounds(10, yofs, wl, 24);

	addAndMakeVisible(iTextTags);
	iTextTags.addKeyListener(this);
	iTextTags.setBounds(10 + wl + 10, yofs, w - wl - 10, 24);
	//iTextTags.setSelectAllWhenFocused(true);
	iTextTags.setText(aOwner.getProperties()["tags"]);
	iTextTags.onTextChange = [this]
	{
		this->changesApply();
	};

	yofs = iTextTags.getBottom() + 12;

	iLabelAssigne.setText("Assignee:", juce::NotificationType::dontSendNotification);
	addAndMakeVisible(iLabelAssigne);
	iLabelAssigne.setBounds(10, yofs, wl + 10, 24);

	addAndMakeVisible(iTextAssigne);
	iTextAssigne.addKeyListener(this);
	iTextAssigne.setBounds(10 + wl + 10, yofs, w - wl - 10, 24);
	iTextAssigne.setText(aOwner.getAssigne());
	iTextAssigne.onTextChange = [this]
	{
		this->changesApply();
	};

	yofs = iTextAssigne.getBottom() + 12;

	iLabelSlider.setBounds(10, yofs, 160, 24);
	addAndMakeVisible(iLabelSlider);

	iSliderDueDate.setRange(-1, 365, 1);
	iSliderDueDate.setSliderStyle(Slider::IncDecButtons);
	iSliderDueDate.setIncDecButtonsMode(Slider::incDecButtonsDraggable_AutoDirection);
	//iSliderDueDate.setTextBoxStyle(Slider::TextBoxLeft, true, 50, 20);
	iSliderDueDate.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
//	iSliderDueDate.setBounds(10, yofs, w - 10, 24);
	iSliderDueDate.setBounds(10 + 160 + 10, yofs - 3, w - 160 - 10, 30);
	//iSliderDueDate.setVelocityBasedMode(true);
	//iSliderDueDate.setVelocityModeParameters(2, 1, 0);
	iSliderDueDate.setTextValueSuffix(" days");
	iSliderDueDate.onValueChange = [this]
	{
		auto sv = this->iSliderDueDate.getValue();
		this->setDueDate(sv);		
	};
	addAndMakeVisible(iSliderDueDate);
	if (!aOwner.isDueDateSet()) iSliderDueDate.setValue(-1);
	else
	{
		iSliderDueDate.setValue((aOwner.getDueDate().toMilliseconds() - aOwner.getCreationDate().toMilliseconds()) / (24 * 3600000), dontSendNotification);
		iLabelSlider.setText(aOwner.getDueDate().formatted("Due date: %d.%m.%Y"), dontSendNotification);
	}
	//this->setDueDate(-1);
	//iSliderDueDate.setValue()

	yofs = iSliderDueDate.getBottom() + 12;

	iColours.reset(new ColoursComponent( 6, 1, CConfiguration::getColourPalette(), CConfiguration::getColourPalette().getColourIndex( aOwner.getColour() )));
	addAndMakeVisible(*iColours);
	iColours->setTopLeftPosition( w + wm - iColours->getWidth() - 10, yofs);
	iColours->setListener(this);

	yofs = iColours->getBottom() + 12;

	iLabelCreationDate.setBounds(10, yofs, w - 10, 20);
	//iLabelCreationDate.setText(aOwner.getCreationDate().toISO8601(true) + "  " + aOwner.getLastUpdateDate().toISO8601(true), dontSendNotification);
	iLabelCreationDate.setText(aOwner.getCreationDate().formatted("Created: %d.%m.%Y %H:%M    ") + aOwner.getLastUpdateDate().formatted("Updated: %d.%m.%Y %H:%M"), dontSendNotification);
	iLabelCreationDate.setColour(Label::textColourId, getLookAndFeel().findColour(Label::textColourId).darker());
	addAndMakeVisible(iLabelCreationDate);

	yofs = iLabelCreationDate.getBottom() + 12;

	setSize(w + wm, yofs);
}

CKanbanCardPropertiesComponent::~CKanbanCardPropertiesComponent()
{
	iOwner.deselect();
}

void CKanbanCardPropertiesComponent::paint (Graphics& g)
{
}

void CKanbanCardPropertiesComponent::resized()
{
}

void CKanbanCardPropertiesComponent::mouseDown(const MouseEvent& event)
{
}

void CKanbanCardPropertiesComponent::mouseUp(const MouseEvent& event)
{
}

void CKanbanCardPropertiesComponent::ColorChanged(int aSelectedColorIdx)
{
	iOwner.setColour(CConfiguration::getColourPalette().getColor(aSelectedColorIdx));
}

bool CKanbanCardPropertiesComponent::keyPressed(const KeyPress & key, Component * originatingComponent)
{
	if (key == key.escapeKey )
	{
		getParentComponent()->exitModalState(0);
		return true;
	}
	else if (key == key.returnKey && originatingComponent != &iTextEditor)
	{
		getParentComponent()->exitModalState(0);
		return true;
	}
	return false;
}

void CKanbanCardPropertiesComponent::changesApply()
{
	/*iBar->getProperties().set("name", iTextName.getText());
	iBar->setColour(this->iColours->getSelectedColourIdx());
	iBar->repaint();*/

	iOwner.setTags(iTextTags.getText());
	iOwner.setUrl(iTextUrl.getText());
	iOwner.setText(iTextName.getText());
	iOwner.setAssigne(iTextAssigne.getText());
	iOwner.repaint();
}

void CKanbanCardPropertiesComponent::setDueDate(double aSliderVal)
{
	if (aSliderVal < 0)
	{
		this->iLabelSlider.setText("Due date: -", dontSendNotification);
		this->iOwner.setDueDate(false, Time::getCurrentTime() - RelativeTime(24 * 3600));
	}
	else
	{
		auto d = this->iOwner.getCreationDate();
		RelativeTime rt(aSliderVal * 24 * 3600);
		d += rt;
		this->iLabelSlider.setText(d.formatted("Due date: %d.%m.%Y"), dontSendNotification);
		this->iOwner.setDueDate(true, d);
	}
}

